
 
 
/*
===============================================================================
Autores:        Daniel Gómez Frontela
		Luis Fernando Nicolás Alonso
Compilacion:    gcc -c serstoremess_conc.c
Uso:            serstoremess <puerto> <backlog> <tambuffer>  
Funcion:        Servidor de almacenamiento de un servicio de mensajería: permitirá almacenar
		 mensajes de clientes.
Plataforma:     Compilado y probado en Linux 2.6.8
Fecha:		19-12-2008
=================================================================================
*/
#include "ttg.h"
extern int errno;
void tratamiento(int signalid)	{	/*Tratamiento de la señal*/
	int	stat;
	
	if (wait(&stat) < 0) {
		fprintf(stderr,"Error en la espera del proceso hijo\n");
		perror("serstoremess_conc.c");
		exit(1);
	}
	return;
}
int main(int argc, char **argv)	{
	int			sockfd, n, nleft, backlog, tambuffer, connsockfd, len;
	char			*recvline, *endptr;
	char			login[8], *ptr;
	struct sockaddr_in	servaddr;
	struct sockaddr_in	clieaddr;	
	long			puerto_l, tambuffer_l, backlog_l;
	in_port_t		puerto;
	char			*ptrcad;
	int			lencad;
	char			cad1[] = "serstoremess: login de usuario incorrecto\n";
	char			cad2[] = "serstoremess: Error en el almacenamiento del mensaje\n";
	char			cad3[] = "serstoremess: Mensaje almacenado con éxito\n";
	int			pid, i;

	/*Comprobacion de argumentos de entrada*/
	if (argc != 4)	{
		fprintf(stderr,"Uso: serstoremess_conc <puerto> <backlog> <tambuffer>\n");
		exit(1);
	}
	
        /*Comprobacion de la validez del puerto introducido
	(usando strtol, si hay algun caracter no numerico, endptr apunta al primero de ellos,
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
	 *lo cual implica que si la cadena apuntada por endptr no tiene longitud 0
	 *es porque se ha introducido un caracter no numerico)*/
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
	recvline = malloc(tambuffer);
	if(errno!=0)	{
		fprintf(stderr,"Error en la reserva de memoria");
		perror("serstoremess_conc");
		exit(1);
	}
	/*Creacion de nuevo socket*/
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)	{
		fprintf(stderr,"Error de Socket\n");
		perror("serstoremess_conc");
		exit(1);
	}
	
	/*Inicializacion de estructura de direccion*/
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;          /*Internet*/
        servaddr.sin_port   = htons(puerto);    /*Puerto de "escucha"*/
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	/*Binding...*/
	if( bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0)	{
		fprintf(stderr, "Error de bind()");
		perror("serstoremess_conc");
		exit(1);
	}
	
	/*Listen...*/
	if( listen(sockfd, backlog) != 0)	{
		fprintf(stderr, "Error de listen()");
		perror("serstoremess_conc");
		exit(1);
	}
	fprintf(stdout,"Escuchando peticiones de conexion...\n");
	/*Interrupción*/
	if ( (signal(SIGCHLD,tratamiento)) == SIG_ERR ) {
		fprintf(stdout,"Error de signal()\n");
		exit(1);
	}
	
	while(1) { 
		len = sizeof(clieaddr);
                do {
			connsockfd = accept(sockfd, (struct sockaddr *) &clieaddr,  &len);
                } while ((connsockfd < 0) & (errno == EINTR)); /*Repetimos accept() si el error corresponde
								a la recepción de una señal*/
		if ((connsockfd < 0) & (errno != EINTR)) {
			/*Devolvemos error y salimos si el error no
			corresponde a la recepción de una señal*/
			fprintf(stderr, "Error de accept");	
			perror("serstoremess_conc");
                        exit(1);
		}
		pid = fork(); 
		if (pid == -1 ) 
		{
			fprintf(stderr, "Error de fork()\n");
                        exit(1);
		}
		else if (pid == 0 ) 
		{
			/* Codigo del hijo */
			/* Cerramos el socket de escucha */
			if (close(sockfd) < 0) {
				fprintf(stderr,"Error en el cierre del socket de escucha\n");
				perror("serstoremess_conc");
			}
			

			printf("SOCKET: %d\n",connsockfd);
			fprintf(stdout,"Conexion recibida de: %s, puerto %d\n",
					inet_ntoa(clieaddr.sin_addr),
					ntohs(clieaddr.sin_port));
			
			/* Lectura del login del destinatario del mensaje*/
			nleft = 0;
			ptr = login;
			while( nleft < 7 )  {
				n=read(connsockfd, ptr, tambuffer < (7 - nleft) ? tambuffer : (7 - nleft));
				if(n<0) {
					printf("Error en la lectura del socket\n");
					perror("serstoremess_conc");
					exit(1);
				}
				else if(n==0) {
					/* Si n es 0 es porque se ha leído un final de archivo y por tanto dejamos
					de leer del socket*/
					break;
				}
				nleft += n;
				ptr += n;
			}
			
			/* Comprobamos si el login es correcto*/
			for (i=0; i<8 && login[i] != '\n' ;i++) {
			}
			if ( i!=6 || n==0 ) {
				/* Si es incorrecto enviamos mensaje de login incorrecto por la conexión*/
				ptrcad = cad1;
				lencad = strlen(ptrcad);
				
				while ( n < lencad )	{
					ptrcad += n;
					lencad -=n;
					n = write(connsockfd, ptrcad, lencad < tambuffer ? lencad : tambuffer);
					if(n <= 0)	
					{
						fprintf(stderr,"Error de write()\n");
						perror("serstoremess_conc");
						exit(1);
					} 
				}	
			}
			else {	
				login[7]=0;
				/* Llamar a storemessage() y comprobar si da error*/
				if( storemessage(connsockfd, login, recvline, tambuffer) < 0 ) {
					/* Si da error enviamos mensaje de que no se ha podido guardar el mensaje */
					ptrcad = cad2;
					lencad = strlen(ptrcad);
					n = 0;
					while ( n < lencad )	{
						ptrcad += n;
						lencad -=n;
						n = write(connsockfd, ptrcad, lencad < tambuffer ? lencad : tambuffer);
						if(n <= 0)	
						{
							fprintf(stderr,"Error de write()\n");
							perror("serstoremess_conc");
							exit(1);
						}
					}		
				}
				else {
					/* Enviamos mensaje de que se ha podido guardar el mensaje */
					ptrcad = cad3;
					lencad = strlen(ptrcad);
					
					n = 0;
					while ( n < lencad )	{
						ptrcad += n;
						lencad -=n;
						n = write(connsockfd, ptrcad, lencad < tambuffer ? lencad : tambuffer);
						if(n <= 0)	
						{
							fprintf(stderr,"Error de write()\n");
							perror("serstoremess_conc");
							exit(1);
						} 
					}		
				}	
			}
						
			/*Y se cierra la conexion con el cliente*/
			if (close(connsockfd) < 0) {
				fprintf(stderr,"Error en la desconexión\n");
				perror("serstoremess_conc");
			}
			fprintf(stdout,"Conexion cerrada\n\n");
			exit(0);
		}
		else if (pid > 0 )
		{
			/*Codigo del padre */
			/* Cerramos el socket conectado porque en el código del padre solo interesa el
			socket de escucha*/
			if (close(connsockfd) < 0) {
				fprintf(stderr,"Error en la desconexión\n");
				perror("serstoremess_conc");
			}
		}
  }
}
 

