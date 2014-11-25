  
/*
===============================================================================
Autores:        Daniel Gómez Frontela
		Luis Fernando Nicolás Alonso
Compilacion:    gcc -c submitmessage.c
Uso:            submitmessage( int sockfd, char *loginus, char *buffer, int tambuffer)
Funcion:        Lee los mensajes de un archivo y los envía por el socket.
Plataforma:     Compilado y probado en Linux 2.6.8
Fecha:		19-12-2008
=================================================================================
*/

#include "ttg.h"

extern int errno;

int submitmessages(int sockfd, char *loginus, char *buffer, int tambuffer)
{
	int fd;	// Descriptor del archivo con nombre loginus
	char path[] = "/home/labs/ttg/ttgx22/Practicas_Sockets/messages/123456";
	char cad[]  = "sersubmitmess: No tiene mensajes\n";
	char *ptr;
	// Variables para leer/escribir en el socket
	int			n, lencad, nread;
	char			*ptrcad;

	/*Copiamos el loginus al final de la cadena path. Nótese que hemos dejado espacio justo para 6 bytes y que descartamos el \n retorno 
	de carro incluido al final de loginus*/
	ptr = path + (strlen(path)-6);
	strncpy(ptr,loginus,6);

	/*Abrimos el archivo*/
	if ( (fd = open(path, O_RDONLY)) < 0)
	{
		/* No existe el fichero. Enviamos por el socket que no hay mensajes*/ 
		ptrcad = cad;
		lencad = strlen(ptrcad);
		n = 0;
		while ( n < lencad )	{
			ptrcad += n;
			lencad -=n;
			n = write(sockfd, ptrcad, lencad < tambuffer ? lencad : tambuffer);
			if(n <= 0)	
			{
				fprintf(stderr,"Error de write()\n");
				perror("sermess_select");
				return -1;
			} 
		}
	}
	
	/* Leemos los mensajes almacenados en el archivo de tambuffer en tambuffer y los enviamos por el socket */
	while( (nread=read(fd, buffer, tambuffer)) != 0)	{
		if(nread<0)	{
			printf("Error en la lectura del archivo %s\n",path);
			perror("sermess_select");
			return -1;
		}
		else	{
				/* Escribimos todo lo leído*/
				ptrcad = buffer;
				n = 0;
				while ( n < nread )	{
					ptrcad += n;
					nread -=n;
					n = write(sockfd, ptrcad, nread);
					if(n <= 0)	
					{
						fprintf(stderr,"Error de write()\n");
						perror("sermess_select");
						return -1;
					} 
				}
			}
	}	
	
	/* Cerramos el archivo*/
	if (close(fd) < 0 ) {
		fprintf(stderr,"Error al cerrar el archivo\n");
		perror("sersubmmitmess");
		return -1;
	}
	
	/* Borramos el archivo*/
	if (unlink(path) < 0) {
		fprintf(stderr,"Error al eliminar el archivo\n");
		perror("sermess_select");
		return -1;
	}

	return 0;
}
