
 
 
/*
===============================================================================
Autores:        Daniel Gómez Frontela
		Luis Fernando Nicolás Alonso
Compilación:	gcc -c serstoremess_select.c
Uso:            int serstoremess_select(int connsockfd,char *buffer, int tambuffer)
Funcion:        Función que implmenta un servicio de mensajeria
Plataforma:     Compilado y probado en Linux 2.6.8
Fecha:		19-12-2008
=================================================================================
*/
#include "ttg.h"
extern int errno;
int serstoremess_select(int connsockfd,char *buffer, int tambuffer)	{
	int			n,i, nleido;
	char			login[8], *ptr;
	char			*ptrcad;
	int			lencad;
	char			cad1[] = "serstoremess: login de usuario incorrecto\n";
	char			cad2[] = "serstoremess: Error en el almacenamiento del mensaje\n";
	char			cad3[] = "serstoremess: Mensaje almacenado con éxito\n";
	
	/* Lectura del login del remitente del mensaje*/
	nleido = 0;
	ptr = login;
	while( nleido < 7 )  {
		
		n=read(connsockfd, ptr, tambuffer < (7-nleido) ? tambuffer : (7-nleido));
		if(n<0) {
			fprintf(stderr,"Error en la lectura del socket.\n");
			perror("sermess_select");
			return -1;
		}
		else if(n==0) {
			/* Si n es 0 es porque se ha leído un final de archivo y por tanto dejamos
			de leer del socket*/
			break;
		}
		nleido += n;
		ptr += n;
	}

	login[7]=0;
	
	/* Comprobación de que el login es correcto */
	for (i=0;i<8 && login[i] != '\n';i++) {
	}
	
	/* Si el login es incorrecto se envía un mensaje de login incorrecto */
	if (n==0 || i != 6) {
		ptrcad = cad1;
		lencad = strlen(ptrcad);
		
		while ( n < lencad ) {
				ptrcad += n;
				lencad -=n;
				n = write(connsockfd, ptrcad, lencad < tambuffer ? lencad : tambuffer);
				if(n <= 0) {
					fprintf(stderr,"Error de write()\n");
					perror("sermess_select");
					return -1;
				} 
			}	
	}
	else {				
		/* Llamar a storemessage() y comprobar si da error*/
		if( storemessage(connsockfd, login, buffer, tambuffer) < 0 ) {
			/* Si da error enviamos mensaje de error en el almacenamiento */
			ptrcad = cad2;
			lencad = strlen(ptrcad);
			n = 0;
			while ( n < lencad ) {
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
		else {
			/* Si no da error enviamos mensaje de operación realizada correctamente */
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
					perror("sermess_select");
					return -1;
				} 
			}	
		}
	}
	return 0;
}

