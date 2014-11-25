 
/*
===============================================================================
Autores:        Daniel Gómez Frontela
		Luis Fernando Nicolás Alonso
Compilacion:    gcc -c storemessage.c
Uso:            storemessage( int socket, char *login, char *puntero_buffer, int buffer) 
Funcion:        Guarda en un archivo un mensaje leido de un socket 
Plataforma:     Compilado y probado en Linux 2.6.8
Fecha:		19-12-2008
=================================================================================
*/
#include "ttg.h"
extern int errno;
int storemessage (int sockfd, char *loginus, char *buffer, int tambuffer)
{
	char logindest[8];
	/* Reservamos 6 bytes más para el nombre del archivo */
	char archivo[56] = "/home/labs/ttg/ttgx22/Practicas_Sockets/messages/";
	char cadena[] = "\nMensaje de <123456>:\n"; // Dejamos 6 caracteres de espacio para el login
	int n, i, fde;
	char *ptr;
	int leido;
	
	leido = 0;
	ptr = logindest;
	/* Leemos 7 bytes del socket conectado y comprobamos que el 7 caracter 
	es un retorno de carro y los anteriores no*/
	while( leido < 7 )  {
		n=read(sockfd, ptr, tambuffer < (7 - leido) ? tambuffer : (7 - leido));
		if(n<0) {
			fprintf(stderr,"Error en la lectura del socket. Saliendo...\n");
			perror("sermess_select");
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
	
	for (i=0; i<8 && logindest[i] != '\n';i++) {
	}

	if (i != 6 || n == 0) {
		return -1;
	}

	/* Concatenamos el nombre del destinatario para formar el path del archivo a abrir o crear*/
	strncat(archivo,logindest,6);
	if ((fde = open(archivo, O_WRONLY | O_CREAT | O_APPEND, S_IRWXU |  S_IRWXG | S_IRWXO)) < 0){
		fprintf(stderr,"Error en la apertura del fichero\n");
		perror("sermess_select");
		return -1;
	}

	/* Escribimos una cabecera del mensaje en el archivo abierto*/
	strncpy(&cadena[13],loginus,6);
	if (n=write(fde,cadena,strlen(cadena)) < 0){
		fprintf(stderr,"Error al escribir en el fichero\n");
		perror("sermess_select");
		return -1;
	}

	/* Leemos el mensaje del socket conectado de tambuffer en tambuffer bytes y lo escribimos al final del archivo*/
	while( (n=read(sockfd, buffer, tambuffer)) != 0)	{
		if(n<1)	{
			fprintf(stderr,"Error en lectura de socket...Saliendo!\n");
			perror("sermess_select");
			return -1;
		} 
		else	{
			if (write(fde,buffer,n)<0){
				fprintf(stderr,"Error al escribir en el fichero\n");
				perror("sermess_select");
				return -1;
			}
		}
	}
	if (close(fde)<0){
		fprintf(stderr,"Error en el cierre del fichero\n");
		perror("sermess_select");
	}

	return 0;
}

