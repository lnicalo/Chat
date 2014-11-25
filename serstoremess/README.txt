Servidor iterativo de almacenamiento de mensajes
Programa que espera conexi�n de clientes como climess en el puerto especificado. Se espera un login que es del remitente. Luego otro que es el destinatario. A continuaci�n se lee el mensaje y se guarda en un fichero con el nombre del destinatario. Para terminar informa del resultado de la operaci�n.

Dise�o e implementaci�n
Uno de los principales problemas ha sido la lectura del socket de tambuffer en tambuffer. Es necesario tener en cuenta que lo que hay que leer no es siempre tambuffer. Puede ser que lo que nos quede por leer sea menor que tambuffer. Por esta raz�n se hace el m�nimo entre tambuffer y la cantidad de bytes que nos quedan por leer.

Otro problema ha sido la escritura en el socket de tambuffer en tambuffer. Es necesario tener en cuenta que lo que hay que escribir no es siempre tambuffer. Puede ser que lo que nos quede por escribir sea menor que tambuffer. Por esta raz�n se hace el m�nimo entre tambuffer y la cantidad de bytes que nos quedan por escribir.

Para comprobar la validez del login miramos que hay un �nico retorno de carro y que �ste est� en el s�ptimo caracter. Tambi�n consideramos login incorrecto que se introduzca un CTRL+D antes de los siete bytes del login.


