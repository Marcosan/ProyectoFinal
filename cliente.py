
#!/usr/bin/python
# -*- coding: utf-8 -*-
 

import json
import requests
import os
 
def menu():
	#os.system('clear') # NOTA para windows tienes que cambiar clear por cls
	print ("Selecciona una opcion")
	print ("\t1 - listar_dispositivos (GET)")
	print ("\t2 - nombrar_dispositivo (POST)")
	print ("\t3 - leer_archivo (GET)")
	print ("\t4 - escribir_archivo (POST)")
	print ("\t9 - salir")
 
 
while True:
	menu()
 
	opcionMenu = input("inserta un numero valor >> ")
 
	if opcionMenu==1:
		print ("\nListar dispositivos:")
		requesito = requests.get("http://127.0.0.1:8888/listar_dispositivos")
		temp = len(requesito.text)
		cadena1 = requesito.text[:temp - 2]
		cadena1 = cadena1 + "}"
		j = json.loads(cadena1)
		if requesito.status_code == 200:
			print("Status: exitoso")
		if cadena1.find('/dev/sdb') != -1:
			if j["/dev/sdb"] != None:
				print("Nodo: " + j["/dev/sdb"]["Nodo"])
				print("Montaje: " + j["/dev/sdb"]["Montaje"])
				print("Id Vendor: " + j["/dev/sdb"]["idVendor"])
				print("Id Product: " + j["/dev/sdb"]["idProduct"])
				print("")
		if cadena1.find('/dev/sdc') != -1:
			if j["/dev/sdc"] != None:
				print("Nodo: " + j["/dev/sdc"]["Nodo"])
				print("Montaje: " + j["/dev/sdc"]["Montaje"])
				print("Id Vendor: " + j["/dev/sdc"]["idVendor"])
				print("Id Product: " + j["/dev/sdc"]["idProduct"])
				print("")
		if cadena1.find('/dev/sdd') != -1:
			if j["/dev/sdd"] != None:
				print("Nodo: " + j["/dev/sdd"]["Nodo"])
				print("Montaje: " + j["/dev/sdd"]["Montaje"])
				print("Id Vendor: " + j["/dev/sdd"]["idVendor"])
				print("Id Product: " + j["/dev/sdd"]["idProduct"])
				print("")

	elif opcionMenu==2:
		print ("")
		input("Has pulsado la opcion 2...\npulsa una tecla para continuar")
	elif opcionMenu==3:
		print ("\nLeer archivo:")
		requesito = requests.get("http://127.0.0.1:8888/listar_dispositivos", params = {"name":"774508"})
		if requesito.status_code == 200:
			print("exitoso")
	elif opcionMenu==4:
		print ("")
		input("Has pulsado la opcion 3...\npulsa una tecla para continuar")
	elif opcionMenu==9:
		break
	else:
		print ("")
		input("No has pulsado ninguna opcion correcta...\npulsa una tecla para continuar")




d = {"sitio": "Recursos Python", "url": "recursospython.com"}
#res = requests.post("http://127.0.0.1:8888/", data=json.dumps(d))
'''if res.status_code == 200:
    print (res.text)
    print (res.url)
    print (res.headers)
    print (res.status_code)
    print (res.encoding)
    print (res.json)
'''