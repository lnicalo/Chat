/*
 * ===============================================================================
 * Autor:          Daniel Gómez Frontela
 *                 Luis Fernando Nicolás Alonso
 * Compilacion:	   gcc -c sermess_select.c
 * Uso:            sermess_select <numero puerto> <backlog> <tambuffer>
 * Funcion:        servidor concurrente que es capaz de aceptar conexiones TCP en los puertos <puerto> y <puerto+1>.
			Si llega una petición de conexión de un cliente dirigida al puerto de escucha <puerto>, crea un proceso 
			?hijo? que almacena un mensaje dirigido a un usuario. Si llega una petición de conexión de un cliente 
			dirigida al puerto de escucha <puerto+1>, crea un proceso ?hijo? que envía los mensajes almacenados dirigidos
			a un usuario.
 * Plataforma:     Compilado y probado en Linux 2.6.8
 * Fecha:          19-12-2008
 * =================================================================================
 */

#include "ttg.h"
extern int errno;

void tratamiento(int signalid) {
	int	stat;

	if ( wait(&stat) < 0 ) {
		fprintf(stderr,"Error en wait()\n");
		perror("sermess_select");
	}
	return;
}

main(int argc, char **argv)
{
        int                     sockfd[2], connsockfd; // Necesitamos tener dos socket de escucha
	int			len;
        int                     backlog, tambuffer;
        char                    *buffer, *endptr;
        struct sockaddr_in      servaddr;
        struct sockaddr_in      clieaddr;
        in_port_t               puerto;
        long                    puerto_l, backlog_l, tambuffer_l;
	fd_set			setfd; 
	int			i,fdmax,pid,ret;
	
        /*Comprobacion de argumentos de entrada*/
        /* Los argumentos de entrada son:
                - Numero de puerto de escucha
                - backlog
                - tambuffer
        */
        if (argc != 4)  {
                fprintf(stderr,"Uso: sermess_select <puerto> <backlog> <tambuffer>\n");
                exit(1);
        }
        /*Comprobacion de la validez del puerto introducido*/
        /*(si hay algun caracter no numerico, endptr apunta al primero de ellos,
        lo cual implica que si la cadena apuntada por endptr no tiene longitud 0
        es porque se ha introducido un caracter no numerico)*/
        puerto_l = strtol(argv[1],&endptr,10);
        if(strlen(endptr)!=0)   {
                fprintf(stderr,"Numero de puerto incorrecto (el puerto ha de ser numerico)\n");
                exit(1);
        }

        if( (puerto_l <= 0) || (puerto_l > 65535) || (errno==ERANGE) )  {
                fprintf(stderr,"Numero de puerto fuera de rango (1-65535)\n");
                exit(1);
        }
        puerto = (in_port_t) puerto_l;
        fprintf(stdout,"Puerto: %d\n",puerto);

        /*Comprobacion de la validez del backlog introducido*/
        /*(si hay algun caracter no numerico, endptr apunta al primero de ellos,
        lo cual implica que si la cadena apuntada por endptr no tiene longitud 0
        es porque se ha introducido un caracter no numerico)*/
        backlog_l = strtol(argv[2],&endptr,10);
        if(strlen(endptr)!=0)   {
                fprintf(stderr,"Numero de backlog incorrecto (el puerto ha de ser numerico)\n");
                exit(1);
        }
        if( (backlog_l < 0) || (backlog_l> 100) || (errno==ERANGE) )  {
                fprintf(stderr,"Tamano de backlog fuera de rango (1-100)\n");
                exit(1);
        }
        backlog = (int) backlog_l;
        fprintf(stdout,"Backlog: %d\n",backlog);
	
        /*Comprobacion de la validez del buffer introducido*/
        /*(si hay algun caracter no numerico, endptr apunta al primero de ellos,
        lo cual implica que si la cadena apuntada por endptr no tiene longitud 0
        es porque se ha introducido un caracter no numerico)*/
        tambuffer_l = strtol(argv[3],&endptr,10);
        if(strlen(endptr)!=0)   {
                fprintf(stderr,"Tamano de buffer incorrecto (el tamano ha de ser numerico)\n");
                exit(1);
        }
        if( (tambuffer_l < 2) || (tambuffer_l > 100) || (errno==ERANGE) )  {
                fprintf(stderr,"Numero de buffer fuera de rango (2-100)\n");
                exit(1);
        }
        tambuffer = (int) tambuffer_l;
        fprintf(stdout,"Buffer: %d\n",tambuffer);

        /*Reserva de memoria*/
        buffer = malloc(tambuffer);
        if(errno!=0)    {
                fprintf(stderr,"Error de malloc en al reservar memoria\n");
		perror("sermess_select");
                exit(1);
        }
        /*Creacion de nuevo socket*/
        if ( (sockfd[0] = socket(AF_INET, SOCK_STREAM, 0)) < 0)    {
                fprintf(stderr,"Error de Socket 1\n");
		perror("sermess_select");
                exit(1);
        }

	if ( (sockfd[1] = socket(AF_INET, SOCK_STREAM, 0)) < 0)    {
                fprintf(stderr,"Error de Socket 1\n");
		perror("sermess_select");
                exit(1);
        }
        /*Inicializacion de estructura de direccion*/
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;          /*Internet*/
        servaddr.sin_port   = htons(puerto);    /*Puerto de "escucha"*/
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

        /*Binding...*/
        if( bind(sockfd[0], (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0) {
                fprintf(stderr, "Error de bind()");
		perror("sermess_select");
                exit(1);
        }
	
	// Cambiamos solo el puerto  de escucha para hacer bind en el segundo socket
	servaddr.sin_port   = htons(puerto+1);    /*Puerto de "escucha"*/

	if( bind(sockfd[1], (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0) {
                fprintf(stderr, "Error de bind()");
		perror("sermess_select");
                exit(1);
        }

        /*Listen...*/
        if( listen(sockfd[0], backlog) != 0)       {
                fprintf(stderr, "Error de listen()");
		perror("sermess_select");
                exit(1);
        }
	
	if( listen(sockfd[1], backlog) != 0)       {
                fprintf(stderr, "Error de listen()");
		perror("sermess_select");
                exit(1);
        }
        fprintf(stdout,"Escuchando peticiones de conexion en los puertos %d %d\n",puerto,puerto+1);

	/* Ejecutamos la función tratamiento cuando termina un hijo para no tener procesos zombies*/
	if ( (signal(SIGCHLD,tratamiento)) == SIG_ERR ) {
		fprintf(stderr,"Error en signal()\n");
		perror("sermess_select");
		exit(1);
	}
	
        while(1) {
		// Esperemos a que suceda un evento en cualquiera de los dos sockets
		do {
			// Bucle que se vuelve a ejecutar si accept() da error y el error es EINTR
			// Inicializo a 0 setfd
			FD_ZERO(&setfd);
			// añadir sockfd[0] y sockfd[1] al conjunto de descriptores
			FD_SET(sockfd[0], &setfd);
			FD_SET(sockfd[1], &setfd);
		
			fdmax = sockfd[0] > sockfd[1] ? sockfd[0] : sockfd[1]; 
			ret = select(fdmax+1, &setfd, NULL, NULL, NULL);
		} while(ret  == -1 &&  errno == EINTR );
			
		if (ret == -1) {
			fprintf(stderr,"Error en select()\n");
			perror("sermess_select");
			exit(1);
		}

		// 
		if (FD_ISSET(sockfd[0], &setfd)) {
			/* El socket de escucha ha recibido nuevo datos en sockfd[0]. Aceptamos la peticion de conexion*/
			len = sizeof(clieaddr);
			do {
				// Bucle que se vuelve a ejecutar si accept() da error y el error es EINTR
				connsockfd = accept(sockfd[0], (struct sockaddr *) &clieaddr,  &len);
			} while(connsockfd  < 0 &&  errno == EINTR );
			
			// tratamos el resto de errores
			if (connsockfd < 0 ) {
				fprintf(stderr, "Error de accept()");
				exit(1);
			}

			printf("SOCKET: %d\n",connsockfd);
			fprintf(stdout,"Conexion recibida de: %s, puerto %d\n",
					inet_ntoa(clieaddr.sin_addr),
					ntohs(clieaddr.sin_port));
			pid = fork();
			if (pid == -1 ) {
				fprintf(stderr,"Error en el fork()\n");
				exit(1);
			}
			else if (pid != 0) {
				// Codigo del padre
				// Cerramos el socket conectado
				close(connsockfd);
			}
			else if (pid == 0) {
				// Codigo del hijo
				// Cerramos el socket de escucha
				if ( close(sockfd[0]) < 0) {
					fprintf(stderr,"Error al cerrar el socket de escucha\n");
					perror("sermess_select");
					exit(1);
				}

				if (serstoremess_select(connsockfd,buffer,tambuffer) == -1) {
					fprintf(stderr,"Error en serstoremess_select()\n");
					exit(1);
				}
				
				/*Y se cierra la conexion con el cliente*/
				if ( close(connsockfd) < 0 ) {
					fprintf(stderr,"Error al cerrar la conexión\n");
					perror("sermess_select");
					exit(1);
				}
				fprintf(stdout,"Conexion cerrada con: %s, puerto %d\n",
					inet_ntoa(clieaddr.sin_addr),
					ntohs(clieaddr.sin_port));
				exit(0);
			}
		}

		else if (FD_ISSET(sockfd[1], &setfd)) {
			/* El socket de escucha ha recibido nuevo datos en sockfd[1]. Aceptamos la peticion de conexion*/
			len = sizeof(clieaddr);
			do {
				// Bucle que se vuelve a ejecutar si accept() de error y el error es EINTR
				connsockfd = accept(sockfd[1], (struct sockaddr *) &clieaddr,  &len);
			} while(connsockfd  < 0 &&  errno == EINTR );
			
			// tratamos el resto de errores
			if (connsockfd < 0 ) {
				fprintf(stderr, "Error de accept()");
				perror("sermess_select");
				exit(1);
			}
			
			printf("SOCKET: %d\n",connsockfd);
			fprintf(stdout,"Conexion recibida de: %s, puerto %d\n",
					inet_ntoa(clieaddr.sin_addr),
					ntohs(clieaddr.sin_port));

			pid = fork();
			if (pid == -1 ) {
				fprintf(stderr,"Error en el fork()\n");
				perror("sermess_select");
				exit(1);
			}
			else if (pid != 0) {
				// Código del padre
				// Cerramos el socket conectado
				if ( close(connsockfd) < 0) {
					fprintf(stderr,"Error al cerrar el socket de escucha\n");
					perror("sermess_select");
					exit(1);
				}
			}
			else if (pid == 0) {
				// Código del hijo
				// Cerramos el socket de escucha
				if ( close(sockfd[1]) < 0) {
					fprintf(stderr,"Error al cerrar el socket de escucha\n");
					perror("sermess_select");
					exit(1);
				}
				
				if (sersubmitmess_select(connsockfd,buffer,tambuffer) == -1) {
					fprintf(stderr,"Error en sersubmitmess_select()\n");
					exit(1);
				}
				/*Y se cierra la conexion con el cliente*/
				if ( close(connsockfd) < 0) {
					fprintf(stderr,"Error al cerrar la conexión\n");
					perror("sermess_select");
					exit(1);
				}
				fprintf(stdout,"Conexion cerrada con: %s, puerto %d\n",
					inet_ntoa(clieaddr.sin_addr),
					ntohs(clieaddr.sin_port));
				exit(0);
			}
		}
	} // fin del while(1)
}
