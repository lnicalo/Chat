 
/*
===============================================================================
Autores:        Daniel G�mez Frontela
		Luis Fernando Nicol�s Alonso
Compilacion:    gcc -c climess.c
Uso:            climess <direccionIP> <puerto> <buffer> <login> 
Funcion:        Cliente sencillo de un servicio de mensajer�a que permitir� el env�o
		de mensajes a otros destinatarios.
Plataforma:     Compilado y probado en Linux 2.6.8
Fecha:		19-12-2008	
=================================================================================
*/
#include "ttg.h"
extern int errno;

int main(int argc, char **argv)	{
	int			sockfd, n, nleft, tambuffer;
	char			* inputline, *ptr, *endptr;
	char			login[8];
	struct sockaddr_in	servaddr;
	long			puerto_l, tambuffer_l;
	in_port_t		puerto;

	/*Comprobacion de argumentos de entrada*/
	if (argc != 5)	{
		fprintf(stderr,"Uso: climess <direccionIP> <puerto> <buffer> <login>\n");
		exit(1);
	}
	
        /*Comprobacion de la validez del puerto introducido
	(usando strtol, si hay algun caracter no numerico, endptr apunta al primero de ellos,
	lo cual implica que si la cadena apuntada por endptr no tiene longitud 0
        es porque se ha introducido un caracter no numerico)*/
	puerto_l = strtol(argv[2],&endptr,10);
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
	/*Comprobaci�n de la validez del login*/
	if (strlen(argv[4]) !=6 ) {
		fprintf(stderr,"Longitud del login debe ser 6 caracteres\n");
		exit(1);
	}
	strcpy(login,argv[4]);
	login[6] = '\n';
	login[7] = 0;

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
	inputline = malloc(tambuffer);
	if(errno!=0)	{
		fprintf(stderr,"Error en la reserva de memoria");
		perror("climess");
		exit(1);
	}
	/*Creacion de nuevo socket*/
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)	{
		fprintf(stderr,"Error de Socket\n");
		perror("climess");
		exit(1);
	}
	
	/*Inicializacion de estructura de direccion*/
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(puerto);	
	/*Conversion dotted-decimal a 32 bits*/
	if ( (servaddr.sin_addr.s_addr = inet_addr(argv[1])) == -1)	{
		fprintf(stderr,"Error de inet_addr() para %s\n",argv[1]);
		exit(1);
	}
	/*Establecimiento de conexion*/
	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)	{
		fprintf(stderr,"Error de connect()\n");
		perror("climess");
		exit(1);
	}
	
	/*Escribir en el socket el login*/
	ptr = login;
	nleft = strlen(login);
	n = 0;
	while ( n < nleft ) {
		ptr += n;
		nleft -=n;
		n = write(sockfd, ptr, nleft > tambuffer ? tambuffer : nleft);
		if(n <= 0) {
			/* Consideramos como error cuando write() devuelve 0 */
			fprintf(stderr,"Error de write()\n");
			perror("climess");
			exit(1);
		}
	}
	/* Llamar a sendmessage() y comprobar si da error*/
	 if( sendmessage(sockfd, inputline, tambuffer) < 0 ) {
		fprintf(stderr,"Error en sendmessage()\n");
		//exit(1);
	}
	
	/*Cierre parcial de socket*/
	if (shutdown(sockfd, SHUT_WR) < 0 ) {
		fprintf(stderr,"Error en cierre parcial\n");
		perror("climess");
		exit(1);
	}
	
	/* Lectura de los datos que env�a el servidor respecto del resultado de la operaci�n*/
	while( (n=read(sockfd, inputline, tambuffer)) != 0)	{
			if(n<1)	{
				printf("Error en lectura de socket...Saliendo!\n");
				perror("climess");
				exit(1);
			} else	{
				/* Es necesario a�adir un fin de l�nea para mostrarlos con printf() */
				inputline[n]=0;
				printf("%s",inputline);
			}
	}
	
	free(inputline);
	if ( close(sockfd) < 0 ) {
		fprintf(stderr,"Error en cierre del socket\n");
		perror("climess");
		exit(1);
	}
	fprintf(stdout,"Conexion cerrada\n\n");
	
	exit(0);
}
	

