all: usb server

usb: usb.c
	gcc -Wall -I/. usb.c libjsmn.a -ludev -o usb

server: server.c
	gcc -Wall server.c -o server -lmicrohttpd

