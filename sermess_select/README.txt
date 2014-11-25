Servidor de mensajería con control de E/S
Programa que espera conexión de clientes como climess en el puerto especificado. Se espera un login que es del remitente. Luego otro que es el destinatario. A continuación se lee el mensaje y se guarda en un fichero con el nombre del destinatario. Para terminar informa del resultado de la operación.

Diseño e implementación
Uno de los principales problemas ha sido la lectura del socket de tambuffer en tambuffer. Es necesario tener en cuenta que lo que hay que leer no es siempre tambuffer. Puede ser que lo que nos quede por leer sea menor que tambuffer. Por esta razón se hace el mínimo entre tambuffer y la cantidad de bytes que nos quedan por leer.

Otro problema ha sido la escritura en el socket de tambuffer en tambuffer. Es necesario tener en cuenta que lo que hay que escribir no es siempre tambuffer. Puede ser que lo que nos quede por escribir sea menor que tambuffer. Por esta razón se hace el mínimo entre tambuffer y la cantidad de bytes que nos quedan por escribir.

Para comprobar la validez del login miramos que hay un único retorno de carro y que éste está en el séptimo caracter. También consideramos login incorrecto que se introduzca un CTRL+D antes de los siete bytes del login. 

Errores
Cuando dos clientes se conectan al puerto correspondiente para enviar un mensaje a un mismo destino se conectan a la vez es posible que los mensajes no se guarden correctamente en el archivo. Los fragmentos de los mensajes se pueden intercalar al introducirlos los usuarios. Esto es debido a que dos procesos diferentes están escribiendo a la vez en el mismo archivo y lo pueden hacer alternadamente.

También puede darse que un usuario se conecte para escribir a un cierto destinatario y a la vez se conecte el destinatario para leer sus mensajes. El proceso hijo que realiza las funciones de submitmess puede borrar el archivo antes de que el otro proceso hijo que está escribiendo el mensaje acabe. El resultado es que la parte del mensaje que se quiere escribir después de eliminar el archivo se pierde. 

Una solución a estos problemas es escribir el mensaje en el archivo de una vez. Es decir, se guardaría el mensaje en memoria para luego escribirlo en el archivo.