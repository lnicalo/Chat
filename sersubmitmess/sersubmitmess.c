/*
 * ===============================================================================
 * Autor:          Daniel Gómez Frontela
 *                 Luis Fernando Nicolás Alonso
 * Compilacion:	   gcc -c sersubmitmess.c
 * Uso:            sersubmitmess <numero puerto> <backlog> <tambuffer>
 * Funcion:        servidor sencillo de envío de mensajes.
 * Plataforma:     Compilado y probado en Linux 2.6.8
 * Fecha:          19-12-2008
 * =================================================================================
 */

#include "ttg.h"
extern int errno;
main(int argc, char **argv)
{
        int                     sockfd, connsockfd;
        int                     backlog, tambuffer;
	int			len;
        char                    *buffer, *endptr;
        struct sockaddr_in      servaddr;
        struct sockaddr_in      clieaddr;
        in_port_t               puerto;
        long                    puerto_l, backlog_l, tambuffer_l;
	char			loginus[8];
	
	// Variables para leer/escribir del socket
	int			leido, n, lencad;
	char			*ptr,*ptrcad;
	// Variable de control de bucles for
	int			i;
	// Cadenas que envía el servidor cuando el login es incorrecto o se da un error en la función sbmitmessages
	char			*cad1 = "sersubmitmess: login de usuario incorrecto\n";
	char			*cad2 = "sersubmitmess: error en el envío de mensajes\n";

        /*Comprobacion de argumentos de entrada*/
        if (argc != 4)  {
                fprintf(stderr,"Uso: sersubmitmess <puerto> <backlog> <tambuffer>\n");
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
        if( (tambuffer_l <= 2) || (tambuffer_l > 100) || (errno==ERANGE) )  {
                fprintf(stderr,"Numero de buffer fuera de rango (2-100)\n");
                exit(1);
        }
        tambuffer = (int) tambuffer_l;
        fprintf(stdout,"Buffer: %d\n",tambuffer);

        /*Reserva de memoria*/
        buffer = malloc(tambuffer);
        if(errno!=0)    {
                fprintf(stderr,"Error al reservar memoria\n");
		perror("sersubmitmess");
                exit(1);
        }
        /*Creacion de nuevo socket*/
        if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)    {
                fprintf(stderr,"Error de Socket\n");
		perror("sersubmitmess");
                exit(1);
        }

        /*Inicializacion de estructura de direccion*/
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;          /*Internet*/
        servaddr.sin_port   = htons(puerto);    /*Puerto de "escucha"*/
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

        /*Binding...*/
        if( bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0) {
                fprintf(stderr, "Error de bind()");
		perror("sersubmitmess");
                exit(1);
        }

        /*Listen...*/
        if( listen(sockfd, backlog) != 0)       {
                fprintf(stderr, "Error de listen()");
		perror("sersubmitmess");
                exit(1);
        }

        fprintf(stdout,"Escuchando peticiones de conexion...\n");
        while(1)        {
                len = sizeof(clieaddr);
                if( (connsockfd = accept(sockfd, (struct sockaddr *) &clieaddr,  &len)) < 0)    {
                        fprintf(stderr, "Error de accept()");
			perror("sersubmitmess");
                        exit(1);
                }
                printf("SOCKET: %d\n",connsockfd);
                fprintf(stdout,"Conexion recibida de: %s, puerto %d\n",
                                inet_ntoa(clieaddr.sin_addr),
                                ntohs(clieaddr.sin_port));

                /*Leemos del socket 7 bytes que contienen el login del usuario más un retorno de carro*/
		leido = 0;
		ptr = loginus;
		while( leido < 7 )  {
			n=read(connsockfd, ptr, tambuffer < (7 - leido) ? tambuffer : (7 - leido));
			if(n<0) {
				printf("Error en la lectura del socket. Saliendo...\n");
				perror("sersubmitmess");
				return -1;
			}
			else if(n==0) {
				/* Si n es 0 es porque se ha leído un final de archivo y por tanto dejamos
				de leer del socket*/
				break;
			}
			leido += n;	
			ptr += n;
		}
		loginus[7] = 0;

		/* Comprobamos que el login leído es correcto*/
		for (i=0;i<8 && loginus[i] != '\n';i++) {
		}
		
		// El login es correcto si solo hay un retorno de carro en loginus[6]
		// y no se ha cerrado la conexion desde el otro extremo antes de 7 bytes
		if (i != 6 || n == 0)
		{
			/*Escritura del mensaje de login incorrecto*/
			ptrcad = cad1;
			lencad = strlen(ptrcad);
			n = 0;
			while ( n < lencad )	{
				ptrcad += n;
				lencad -=n;
				n = write(connsockfd, ptrcad, lencad < tambuffer ? lencad : tambuffer);
				if(n <= 0)	
				{
					fprintf(stderr,"Error de write()\n");
					perror("sersubmitmess");
					exit(1);
				} 
			}
		}
		else
		{	
			if (submitmessages(connsockfd, loginus, buffer, tambuffer)== -1)
			{
				/*Escritura del mensaje de error en submitmessages*/
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
						perror("sersubmitmess");
						exit(1);
					} 
				}
			}
		}

                /*Y se cierra la conexion con el cliente*/
                if (close(connsockfd) < 0) {
			fprintf(stderr,"Error al cerrar la conexión");
			perror("sersubmitmess");
		}
                fprintf(stdout,"Conexion cerrada\n\n");
	}
		
}

