Cliente de servicio de mensajería
Cliente que envía un mensaje a un destinatario. El remitente se toma como parámetro del programa. El nombre del destinatario se pide en la ejecución del programa. Si el login del destinatario es correcto se lee de la entrada estándar el mensaje que se desea enviar. El usuario indica que ha terminado de escribir el mensaje con CTRL+D.

Diseño e implementación 
Uno de los principales problemas ha sido la escritura en el socket de tambuffer en tambuffer. Es necesario tener en cuenta que lo que hay que escribir no es siempre tambuffer. Puede ser que lo que nos quede por escribir sea menor que tambuffer. Por esta razón se hace el mínimo entre tambuffer y la cantidad de bytes que nos quedan por escribir.

Para comprobar la validez del login se comprueba que la longitud de la cadena leida es de siete bytes y que en el último caracter hay un retorno de carro. También es
considerado login incorrecto que se introduzca un CTRL+D antes de los siete bytes del login.

Para imprimir el mensaje de error correspondiente al valor actual de la variable global errno tras una llamada a una rutina de biblioteca se ha usado la función perror con argumento el nombre del programa. Y para mostrar un mensaje más claro al usuario también se usa la función fprintf. 
Se podría haber hecho de otra manera. Es decir, usar como argumento de perror en lugar del nombre del programa el mensaje que se escribe con fprintf. Se ha optado por no hacerlo así ya que por convención habitualmente se prefiere que aparezca el nombre del programa que produjo el mensaje de error.

Errores
No se ha encontrado ningún error.
