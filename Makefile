all: usb #server

usb: usb.c
	gcc -Wall usb.c -o usb -ludev

#usb: server.c
#	gcc -Wall server.c -o server -lmicrohttpd