  
/*
===============================================================================
Autores:        Daniel G�mez Frontela
		Luis Fernando Nicol�s Alonso
Compilacion:    gcc -c sendmessage.c
Uso:            sendmessage( int socket, char *puntero_buffer, int buffer)
Funcion:        Env�a un mensaje a trav�s de socket a un destinatario.
Plataforma:     Compilado y probado en Linux 2.6.8
Fecha:		19-12-2008
=================================================================================
*/
#include "ttg.h"
extern int errno;
int sendmessage (int sockfd, char *buffer, int tambuffer)
{
	char login[8];
	int n,i;
	char *ptr;
	int nleft;
	
	fprintf(stdout,"Introduce el login del destinatario del mensaje: \n");
	/* Leemos 7 bytes como maximo de la entrada estandar. Y comprobamos que el 7 caracter 
	es un retorno de carro y los anteriores no*/
	if(fgets(login,8,stdin) == NULL)
	{
		return -1;
	}

	if (!(strlen(login) == 7 && login[6] == '\n') ) {
		fprintf(stderr,"El login del destinatario debe tener 6 caracteres\n");
		return -1;
	}

	/*Escribir el login en el socket */
	ptr = login;
	nleft = strlen(login);
	n = 0;
	while ( n < nleft ) {
		ptr += n;
		nleft -= n;
		n = write(sockfd, ptr,nleft > tambuffer ? tambuffer : nleft );
		if(n <= 0) {
			fprintf(stderr,"Error de write()\n");
			perror("climess");
			return -1;
		}
	}

	fprintf(stdout,"Introduce tu mensaje (teclea CTRL-D para terminar): \n");
	while(1)	{
		/* Leemos de stdin como m�ximo tambuffer bytes*/ 
		if( (fgets(buffer,tambuffer+1,stdin)) == NULL)	{
			/*fgets devuelve NULL cuando se produce alg�n error
			 * pero tambi�n cuando se lee EOF (Ctrl-D)*/
			return 0;
		}
	
		/*Escritura de Mensaje*/
		ptr = buffer;
		nleft = strlen(buffer);
		n = 0;
		while ( n < nleft) {
			/* nleft es siempre menor o igual que tambuffer*/
			n = write(sockfd, ptr,nleft);
			if(n < 0)	{
				fprintf(stderr,"Error de write()\n");
				perror("climess");
				return -1;
			} else if(n < nleft) {
				/* Si no fue posible enviar todo el mensaje 
				 * de una vez, se transmite el resto */
				ptr += n;
				nleft -=n;
			}
		}
	}
}

