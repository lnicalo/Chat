Servidor de mensajer�a con control de E/S
Programa que espera conexi�n de clientes como climess en el puerto especificado. Se espera un login que es del remitente. Luego otro que es el destinatario. A continuaci�n se lee el mensaje y se guarda en un fichero con el nombre del destinatario. Para terminar informa del resultado de la operaci�n.

Dise�o e implementaci�n
Uno de los principales problemas ha sido la lectura del socket de tambuffer en tambuffer. Es necesario tener en cuenta que lo que hay que leer no es siempre tambuffer. Puede ser que lo que nos quede por leer sea menor que tambuffer. Por esta raz�n se hace el m�nimo entre tambuffer y la cantidad de bytes que nos quedan por leer.

Otro problema ha sido la escritura en el socket de tambuffer en tambuffer. Es necesario tener en cuenta que lo que hay que escribir no es siempre tambuffer. Puede ser que lo que nos quede por escribir sea menor que tambuffer. Por esta raz�n se hace el m�nimo entre tambuffer y la cantidad de bytes que nos quedan por escribir.

Para comprobar la validez del login miramos que hay un �nico retorno de carro y que �ste est� en el s�ptimo caracter. Tambi�n consideramos login incorrecto que se introduzca un CTRL+D antes de los siete bytes del login. 

Errores
Cuando dos clientes se conectan al puerto correspondiente para enviar un mensaje a un mismo destino se conectan a la vez es posible que los mensajes no se guarden correctamente en el archivo. Los fragmentos de los mensajes se pueden intercalar al introducirlos los usuarios. Esto es debido a que dos procesos diferentes est�n escribiendo a la vez en el mismo archivo y lo pueden hacer alternadamente.

Tambi�n puede darse que un usuario se conecte para escribir a un cierto destinatario y a la vez se conecte el destinatario para leer sus mensajes. El proceso hijo que realiza las funciones de submitmess puede borrar el archivo antes de que el otro proceso hijo que est� escribiendo el mensaje acabe. El resultado es que la parte del mensaje que se quiere escribir despu�s de eliminar el archivo se pierde. 

Una soluci�n a estos problemas es escribir el mensaje en el archivo de una vez. Es decir, se guardar�a el mensaje en memoria para luego escribirlo en el archivo.