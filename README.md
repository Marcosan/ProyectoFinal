# ProyectoFinal
### ESPOL

### FACULTAD DE INGENIERIA EN ELECTRICIDAD Y COMPUTACION

#### PROGRAMACION DE SISTEMAS

En este proyecto, crearán un programa que se encargará de monitorear los puertos USB, detectar nuevos dispositivos. El programa usará el formato JSON y un API REST para comunicarse con clientes externos, que darán órdenes a su programa. Su programa ejecutará estas órdenes, y devolverá resultados al cliente. 

1. Un proceso que monitorea los dispositivos USB conectados al PC (como daemon). 
2. Un proceso que funciona como servidor web. 
3. Un cliente escrito en Python.

La premisa es que el cliente pueda enviar comandos al proceso web, usando un API REST + JSONs, para que el proceso USB pueda 
manipular archivos en los dispositivos conectados.
Por ejemplo: 
1. Listar los dispositivos conectados 
2. Escribir un archivo recibido desde cliente y escribirlo en un dispositivo USB 
3. Leer un archivo en uno de los dispositivos USB, y enviárselo al cliente. 

Pre-requisitos:
Para este proyecto se ha considerado usar librerias externas:libudev, libmicrohttpd, Requests
las cuales deberan estar instaladas antes de la ejecucion de este programa.

Comandos para la intalacion de las librerias:
  * sudo apt-get install libudev-dev
  * sudo apt-get install libmicrohttpd*
  * sudo pip install request
  * pip install requests

Ademas se adjunta el achivo make con el cual se creara los archivos ejecutables necesarios, esto se podra realizar con el comando
  * make
  
