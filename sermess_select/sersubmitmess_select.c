/*
 * ===============================================================================
 * Autor:          Daniel G�mez Frontela
 *                 Luis Fernando Nicol�s Alonso
 * Compilacion:	   gcc -c sersubmitmess_select.c
 * Uso:            sersubmitmess_select(int connsockfd,char *buffer, int tambuffer)
 * Funcion:        servidor de env�o de mensajes.
 * Plataforma:     Compilado y probado en Linux 2.6.8
 * Fecha:          19-12-2008
 * =================================================================================
 */

#include "ttg.h"
extern int errno;
sersubmitmess_select(int connsockfd,char *buffer, int tambuffer)
{
	char			loginus[8];
	
	// Variables para leer/escribir del socket
	int			leido, n, lencad;
	char			*ptr,*ptrcad;
	// Variable de control de bucles for
	int			i;
	// Cadenas que env�a el servidor cuando el login es incorrecto o se da un error en la funci�n sbmitmessages
	char			*cad1 = "sersubmitmess: login de usuario incorrecto\n";
	char			*cad2 = "sersubmitmess: error en el env�o de mensajes\n";

	/*Leemos del socket 7 bytes que contienen el login del usuario m�s un retorno de carro*/
	leido = 0;
	ptr = loginus;
	while( leido < 7 )  {
		n=read(connsockfd, ptr, tambuffer < (7 - leido) ? tambuffer : (7 - leido));
		if(n<0) {
			fprintf(stderr,"Error en la lectura del socket.\n");
			perror("sermess_select");
			return -1;
		}
		else if(n==0) {
			/* Si n es 0 es porque se ha le�do un final de archivo y por tanto dejamos
			de leer del socket*/
			break;
		}
		leido += n;	
		ptr += n;
	}

	loginus[7] = 0;

	/* Comprobamos que el login le�do es correcto*/
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
				perror("sermess_select");
				return -1;
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
					perror("sermess_select");
					return -1;
				} 
			}
		}
	}

	return 0;
}

