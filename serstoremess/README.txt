Servidor iterativo de almacenamiento de mensajes
Programa que espera conexión de clientes como climess en el puerto especificado. Se espera un login que es del remitente. Luego otro que es el destinatario. A continuación se lee el mensaje y se guarda en un fichero con el nombre del destinatario. Para terminar informa del resultado de la operación.

Diseño e implementación
Uno de los principales problemas ha sido la lectura del socket de tambuffer en tambuffer. Es necesario tener en cuenta que lo que hay que leer no es siempre tambuffer. Puede ser que lo que nos quede por leer sea menor que tambuffer. Por esta razón se hace el mínimo entre tambuffer y la cantidad de bytes que nos quedan por leer.

Otro problema ha sido la escritura en el socket de tambuffer en tambuffer. Es necesario tener en cuenta que lo que hay que escribir no es siempre tambuffer. Puede ser que lo que nos quede por escribir sea menor que tambuffer. Por esta razón se hace el mínimo entre tambuffer y la cantidad de bytes que nos quedan por escribir.

Para comprobar la validez del login miramos que hay un único retorno de carro y que éste está en el séptimo caracter. También consideramos login incorrecto que se introduzca un CTRL+D antes de los siete bytes del login.


