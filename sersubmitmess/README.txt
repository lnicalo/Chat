 
Servidor iterativo de env�o de mensajes
Programa que espera conexi�n de clientes en un puerto especificado. Se espera un login que es el usuario que quiere leer sus mensajes. Si el login es correcto se intenta abrir un archivo con el nombre del login. Si existe se leen y se env�an por la conexi�n los mensajes y luego se borra el archivo. Si no existe el fichero se env�a por la conexi�n una indicaci�n de que no el usuario no tiene mensajes.

Dise�o e implementaci�n
Uno de los principales problemas ha sido la lectura del socket de tambuffer en tambuffer. Es necesario tener en cuenta que lo que hay que leer no es siempre tambuffer. Puede ser que lo que nos quede por leer sea menor que tambuffer. Por esta raz�n se hace el m�nimo entre tambuffer y la cantidad de bytes que nos quedan por leer.

Otro problema ha sido la escritura en el socket de tambuffer en tambuffer. Es necesario tener en cuenta que lo que hay que escribir no es siempre tambuffer. Puede ser que lo que nos quede por escribir sea menor que tambuffer. Por esta raz�n se hace el m�nimo entre tambuffer y la cantidad de bytes que nos quedan por escribir.

Para comprobar la validez del login miramos que hay un �nico retorno de carro y que �ste est� en el s�ptimo caracter. Tambi�n consideramos login incorrecto que se introduzca un CTRL+D antes de los siete bytes del login.