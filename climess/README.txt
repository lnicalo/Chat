Cliente de servicio de mensajer�a
Cliente que env�a un mensaje a un destinatario. El remitente se toma como par�metro del programa. El nombre del destinatario se pide en la ejecuci�n del programa. Si el login del destinatario es correcto se lee de la entrada est�ndar el mensaje que se desea enviar. El usuario indica que ha terminado de escribir el mensaje con CTRL+D.

Dise�o e implementaci�n 
Uno de los principales problemas ha sido la escritura en el socket de tambuffer en tambuffer. Es necesario tener en cuenta que lo que hay que escribir no es siempre tambuffer. Puede ser que lo que nos quede por escribir sea menor que tambuffer. Por esta raz�n se hace el m�nimo entre tambuffer y la cantidad de bytes que nos quedan por escribir.

Para comprobar la validez del login se comprueba que la longitud de la cadena leida es de siete bytes y que en el �ltimo caracter hay un retorno de carro. Tambi�n es
considerado login incorrecto que se introduzca un CTRL+D antes de los siete bytes del login.

Para imprimir el mensaje de error correspondiente al valor actual de la variable global errno tras una llamada a una rutina de biblioteca se ha usado la funci�n perror con argumento el nombre del programa. Y para mostrar un mensaje m�s claro al usuario tambi�n se usa la funci�n fprintf. 
Se podr�a haber hecho de otra manera. Es decir, usar como argumento de perror en lugar del nombre del programa el mensaje que se escribe con fprintf. Se ha optado por no hacerlo as� ya que por convenci�n habitualmente se prefiere que aparezca el nombre del programa que produjo el mensaje de error.

Errores
No se ha encontrado ning�n error.
