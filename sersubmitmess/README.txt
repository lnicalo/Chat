 
Servidor iterativo de envío de mensajes
Programa que espera conexión de clientes en un puerto especificado. Se espera un login que es el usuario que quiere leer sus mensajes. Si el login es correcto se intenta abrir un archivo con el nombre del login. Si existe se leen y se envían por la conexión los mensajes y luego se borra el archivo. Si no existe el fichero se envía por la conexión una indicación de que no el usuario no tiene mensajes.

Diseño e implementación
Uno de los principales problemas ha sido la lectura del socket de tambuffer en tambuffer. Es necesario tener en cuenta que lo que hay que leer no es siempre tambuffer. Puede ser que lo que nos quede por leer sea menor que tambuffer. Por esta razón se hace el mínimo entre tambuffer y la cantidad de bytes que nos quedan por leer.

Otro problema ha sido la escritura en el socket de tambuffer en tambuffer. Es necesario tener en cuenta que lo que hay que escribir no es siempre tambuffer. Puede ser que lo que nos quede por escribir sea menor que tambuffer. Por esta razón se hace el mínimo entre tambuffer y la cantidad de bytes que nos quedan por escribir.

Para comprobar la validez del login miramos que hay un único retorno de carro y que éste está en el séptimo caracter. También consideramos login incorrecto que se introduzca un CTRL+D antes de los siete bytes del login.