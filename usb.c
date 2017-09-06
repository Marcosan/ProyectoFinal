//libdev.h
//mntent.h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libudev.h>
#include <errno.h>
#include <mntent.h>	/* for getmntent(), et al. */
#include <unistd.h>	/* for getopt() */
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "jsmn.h"


#define BLOCK_SIZE 1000
#define TAM_RESULT 10000
#define PORT 1234

void process(const char *filename);
void print_mount(const struct mntent *fs);

char *myname;
char *buff_response;
char buffer[256] = "";
int sockfd, newsockfd, portno;
socklen_t clilen;
struct sockaddr_in serv_addr, cli_addr;
int n_read_socket;
int r;
jsmn_parser p;
jsmntok_t t[BLOCK_SIZE];
int flag_file = 1;

char *obtenerNombre(char *name){
	char *tmp = (char *) malloc(sizeof(tmp)*20);

	return tmp = "hi";
}

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

void error(char *msg){
    perror(msg);
    //exit(1);
}

void connection_socket(){
	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0) 
		error("ERROR on accept");

	bzero(buffer,1000);

	n_read_socket = read(newsockfd,buffer,1000);

	if (n_read_socket < 0) error("ERROR reading from socket");

	
}

/* main --- process options */
/*
[{"Fruta": {"Nombre":"Manzana","Cantidad":10}}, {"Verdura": {"Nombre":"Lechuga","Cantidad":80}}]
*/
char * json_token_tostr(char *js, jsmntok_t *t)
{
    js[t->end] = '\0';
    return js + t->start;
}

int getMounts(const char *devNode,const  char *devName,const  char *idVendor,const  char *idProduct){
	char *filename = "/etc/mtab";	/* default file to read */
	char *nameUsb;
	//char *mountAdr;

	FILE *fp;
	struct mntent *fs;

	
	//int i;
	
	
	char *solicitud;

	
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}

	/* Assume the top-level element is an object */
	if (r < 1 || t[0].type != JSMN_OBJECT) {
		printf("Object expected\n");
		return 1;
	}

	fp = setmntent(filename, "r");	/* read only */
	if (fp == NULL) {
		fprintf(stderr, "%s: %s: could not open: %s\n",
			myname, filename, strerror(errno));
		exit(1);
	}

	/* PARSEAR DATOS */
	for (int i = 1; i < r; i++) {
		if (jsoneq(buffer, &t[i], "solicitud") == 0) {
			/* We may use strndup() to fetch string value */
			printf("Solicitud: %.*s\n", t[i+1].end-t[i+1].start,
					buffer + t[i+1].start);
			solicitud = json_token_tostr(buffer, &t[i+1]);
			printf("%s\n", solicitud);
			i++;
		}
	}
	
	/* LISTAR DISPOSITIVOS */
    if (0 == strcmp (solicitud, "listar_dispositivos")){  //Listar dispositivos
    	
	    while ((fs = getmntent(fp)) != NULL){
			if(strstr(fs->mnt_fsname, devName) != NULL && strstr(fs->mnt_fsname, "sda") == NULL) {
	    	//if(strstr(fs->mnt_fsname, devName) != NULL) {
	    		if (idVendor == NULL || idProduct == NULL){
	    			idProduct = "no hay";
	    			idVendor = "no hay";
	    		}
	    		nameUsb = obtenerNombre(fs->mnt_dir);
	    		printf("Nodo: %s\nPunto montaje: %s\nIdVendor: %s\nIdProduct: %s\nNombre: %s\n\n",
					devNode,
					//fs->mnt_fsname,
					fs->mnt_dir,
					idVendor,
					idProduct,
					nameUsb);
	    		strcat(buff_response, "\"");
	    		strcat(buff_response, devNode);
	    		strcat(buff_response, "\":{\"Nodo\":\"");
	    		strcat(buff_response, devNode);
	    		strcat(buff_response, "\",\"Montaje\":\"");
	    		strcat(buff_response, fs->mnt_dir);
	    		strcat(buff_response, "\",\"idVendor\":\"");
	    		strcat(buff_response, idVendor);
	    		strcat(buff_response, "\",\"idProduct\":\"");
	    		strcat(buff_response, idProduct);
	    		strcat(buff_response, "\"},");
			}
		}
		return 1;
	}

	/* LEER ARCHIVO */
    if (0 == strcmp (solicitud, "leer_archivo")){
    	char *nombre_disp;
    	char *nombre_file;
    	/* PARSEAR DATOS */
		for (int i = 1; i < r; i++) {
			if (jsoneq(buffer, &t[i], "nombre") == 0) {
				nombre_disp = json_token_tostr(buffer, &t[i+1]);
				i++;
			}
			if (jsoneq(buffer, &t[i], "nombre_archivo") == 0) {
				nombre_file = json_token_tostr(buffer, &t[i+1]);
				i++;
			}
		}
	    while ((fs = getmntent(fp)) != NULL){
			if(strstr(fs->mnt_dir, nombre_disp) != NULL && flag_file) {
				char *contenido = (char *) malloc(sizeof(contenido)*10000);
				char *new_contenido = (char *) malloc(sizeof(new_contenido)*10000);

				char *filename;
	            filename = (char *) malloc(sizeof(filename)*20);
	            strcpy(filename, nombre_disp);
	            strcat(filename, "/");
				strcat(filename, nombre_file);


				// inicio para leer el archivo
				long length;
				FILE * f = fopen (filename, "rb");

				if (f){
					fseek (f, 0, SEEK_END);
					length = ftell (f);
					fseek (f, 0, SEEK_SET);
					fread (contenido, 1, length, f);
					if (contenido){
						
					}
					fclose (f);
				}

				// fin para leer el archivo

				//Inicio quitar saltos de linea
				int c = 0;
				while(*(contenido+c)){
					if ( *(contenido+c) != '\n' ){
						
						strcat(new_contenido, (contenido+c));
						//printf("%s\n", (new_contenido+c));
						c++;
					}else{
						strcat(new_contenido, "\\");
						//strcat(new_contenido, "n");
						//*(contenido+c) = '\\';
						//*(contenido+c+1) = 'n';
						c++;
					}
				}
				*(new_contenido+c+1) = '\0';

				printf("contenido nuevo: %s\n", new_contenido);
				//fin saltos linea
	    		if (idVendor == NULL || idProduct == NULL){
	    			idProduct = "no hay";
	    			idVendor = "no hay";
	    		}
	    		
	    		nameUsb = obtenerNombre(fs->mnt_dir);

	    		strcat(buff_response, "\"solicitud\":\"leer_archivo\",\"nombre\":\"");
	    		strcat(buff_response, nombre_disp);
	    		strcat(buff_response, "\",\"nombre_archivo\":\"");
	    		strcat(buff_response, nombre_file);
	    		strcat(buff_response, "\",\"contenido\":\"");
	    		strcat(buff_response, new_contenido);
	    		strcat(buff_response, "\",\"str_error\":\"");
	    		strcat(buff_response, "no hay error");
	    		strcat(buff_response, "\"");
	    		flag_file = 0;
	    		return 1;
			}
		}
		
	}
	
	endmntent(fp);

	return 0;
}

int main(){
	struct udev *udev;
	struct udev_device *dev;
	struct udev_enumerate *enumerate;
	struct udev_list_entry *devices, *dev_list_entry;
	struct udev_device* usb;
	const char *idVendor;
	const char *idProduct;
	const char *devNode;
	const char *devName;

	/* CONNECTION SOCKET */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");

	bzero((char *) &serv_addr, sizeof(serv_addr));

	//portno = PORT;//atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(PORT);

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR on binding");
	
	while(1){
		/* create udev object */
		buff_response = (char *) malloc(sizeof(buff_response)*TAM_RESULT);
		strcpy(buff_response, "{");
		udev = udev_new();
		if (!udev) {
			fprintf(stderr, "Cannot create udev context.\n");
			return 1;
		}

		/* create enumerate object */
		enumerate = udev_enumerate_new(udev);
		if (!enumerate) {
			fprintf(stderr, "Cannot create enumerate context.\n");
			return 1;
		}

		udev_enumerate_add_match_subsystem(enumerate, "block");
		udev_enumerate_scan_devices(enumerate);

		/* fillup device list */
		devices = udev_enumerate_get_list_entry(enumerate);
		if (!devices) {
			fprintf(stderr, "Failed to get device list.\n");
			return 1;
		}

		connection_socket();

		printf("Here is the message: %s\n",buffer);
		jsmn_init(&p);
		r = jsmn_parse(&p, buffer, strlen(buffer), t, sizeof(t)/sizeof(t[0]));
		udev_list_entry_foreach(dev_list_entry, devices) {
			const char *path;//, *tmp;
			//unsigned long long disk_size = 0;
			//unsigned short int block_size = BLOCK_SIZE;

			path = udev_list_entry_get_name(dev_list_entry);
			dev = udev_device_new_from_syspath(udev, path);
			usb = udev_device_get_parent_with_subsystem_devtype(dev, "usb", "usb_device");

			/* skip if device/disk is a partition or loop device */
			if (strncmp(udev_device_get_devtype(dev), "partition", 9) != 0 &&
			    strncmp(udev_device_get_sysname(dev), "loop", 4) != 0) {
				devNode = udev_device_get_devnode(dev);
				devName = udev_device_get_sysname(dev);
				idVendor = udev_device_get_sysattr_value(usb, "idVendor");
				idProduct = udev_device_get_sysattr_value(usb, "idProduct");
				getMounts(devNode, devName, idVendor, idProduct);
				
			}

			/* free dev */
			udev_device_unref(dev);
		}
		flag_file = 1;
		strcat(buff_response, "}");
		printf("%s\n", buff_response);
		n_read_socket = write(newsockfd,buff_response,sizeof(buff_response)*TAM_RESULT);

		if (n_read_socket < 0) error("ERROR writing to socket");
		/* free enumerate */
		udev_enumerate_unref(enumerate);
		/* free udev */
		udev_unref(udev);

		free(buff_response);
	}
	return 0;
}