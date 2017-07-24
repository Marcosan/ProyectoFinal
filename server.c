#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <microhttpd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h> 

#define PORT            8888
#define PORT_USB        1234
#define POSTBUFFERSIZE  512
#define MAXNAMESIZE     20
#define MAXANSWERSIZE   512

#define GET             0
#define POST            1

struct connection_info_struct
{
  int connectiontype;
  char *answerstring;
  struct MHD_PostProcessor *postprocessor;
};


//const char *greetingpage =  "<html><body><h1>Welcome, %s!</center></h1></body></html>";

const char *greetingpage = "{\"valor\": \"hola\", \"valor2\": \"%s\", }";

const char *errorpage = "<html><body>This doesn't seem to be right.</body></html>";
char buffer[10000];
void error(char *msg)
{
    perror(msg);
    exit(0);
}

void connection_usb(char *str_json){
    int sockfd, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    

    //portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname("localhost");

    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(PORT_USB);

    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    //printf("Please enter the message: ");
    bzero(buffer,10000);
    //fgets(buffer,255,stdin);

    n = write(sockfd,str_json,strlen(str_json));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,10000);
    printf("Mensaje enviado\n");
    n = read(sockfd,buffer,10000);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n",buffer);

}


static int send_page (struct MHD_Connection *connection, const char *page){
    int ret;
    struct MHD_Response *response;


    response = MHD_create_response_from_buffer (strlen (page), (void *) page,
             MHD_RESPMEM_PERSISTENT);
    if (!response)
        return MHD_NO;

    ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
    MHD_destroy_response (response);

    return ret;
}


static int iterate_post (void *coninfo_cls, enum MHD_ValueKind kind, const char *key,
              const char *filename, const char *content_type,
              const char *transfer_encoding, const char *data, uint64_t off,
              size_t size){
    struct connection_info_struct *con_info = coninfo_cls;

    if (0 == strcmp (key, "name")){
        if ((size > 0) && (size <= MAXNAMESIZE)){
            char *answerstring;
            answerstring = malloc (MAXANSWERSIZE);
            if (!answerstring)
                return MHD_NO;

            snprintf (answerstring, MAXANSWERSIZE, greetingpage, data);
            con_info->answerstring = answerstring;
        }
        else
            con_info->answerstring = NULL;

        return MHD_NO;
    }

    return MHD_YES;
}


static int answer_to_connection (void *cls, struct MHD_Connection *connection,
                      const char *url, const char *method,
                      const char *version, const char *upload_data,
                      size_t *upload_data_size, void **con_cls){
    printf("%s\n", url);
    const char *fmt = cls;
    const char *val;
    const char *nombre;
    const char *nombre_archivo;
    char *me;
    struct MHD_Response *response;
    if (NULL == *con_cls){
        //printf("con_cls null\n");
        struct connection_info_struct *con_info;

        con_info = malloc (sizeof (struct connection_info_struct));
        if (NULL == con_info)
            return MHD_NO;
        con_info->answerstring = NULL;

        if (0 == strcmp (method, "POST")){
            con_info->postprocessor = MHD_create_post_processor (connection, POSTBUFFERSIZE,
                                     iterate_post, (void *) con_info);

            if (NULL == con_info->postprocessor){
                free (con_info);
                return MHD_NO;
            }

            con_info->connectiontype = POST;
        }
        else
            con_info->connectiontype = GET;

        *con_cls = (void *) con_info;

        return MHD_YES;
    }

    /* EVENTO SI ES UN GET - LISTAR DISPOSITIVOS Y LEER ARCHIVOS */
    if (0 == strcmp (method, "GET")){
        printf("GET:\n");

        *con_cls = NULL;                  /* reset when done */
        val = MHD_lookup_connection_value (connection, MHD_GET_ARGUMENT_KIND, "name");
        me = malloc (snprintf (NULL, 0, fmt, "name", val) + 1);
        if (me == NULL)
          return MHD_NO;
        sprintf (me, fmt, "name", val);
        response = MHD_create_response_from_buffer (strlen (me), me,
                      MHD_RESPMEM_MUST_FREE);
        if (response == NULL){
          free (me);
          return MHD_NO;
        }


        /* SI ES LISTAR DISPOSITIVOS */
        if (0 == strcmp (url, "/listar_dispositivos")){
            printf("Se ejecuta listar_dispositivos:\n");
            connection_usb("{\"solicitud\": \"listar_dispositivos\"}");
            if(val != NULL){
                printf("parametro: %s\n\n", val);
            }

        }


        /* SI ES LEER ARCHIVO */
        if (0 == strcmp (url, "/leer_archivo")){
            printf("Se ejecuta leer_archivo:\n");

            *con_cls = NULL;                  /* reset when done */
            nombre = MHD_lookup_connection_value (connection, MHD_GET_ARGUMENT_KIND, "nombre");
            nombre_archivo = MHD_lookup_connection_value (connection, MHD_GET_ARGUMENT_KIND, "nombre_archivo");
            me = malloc (snprintf (NULL, 0, fmt, "name", val) + 1);
            if (me == NULL)
              return MHD_NO;
            sprintf (me, fmt, "name", val);
            response = MHD_create_response_from_buffer (strlen (me), me,
                          MHD_RESPMEM_MUST_FREE);
            if (response == NULL){
              free (me);
              return MHD_NO;
            }
            char *json_solicitud;
            json_solicitud = (char *) malloc(sizeof(json_solicitud)*10000);
            strcpy(json_solicitud, "{\"solicitud\": \"leer_archivo\",\"nombre\":\"");

            strcat(json_solicitud, nombre);
            strcat(json_solicitud, "\",\"nombre_archivo\":\"");
            strcat(json_solicitud, nombre_archivo);
            strcat(json_solicitud, "\"}");
            printf("%s\n", json_solicitud);
            connection_usb(json_solicitud);
            if(val != NULL){
                printf("parametro: %s\n\n", val);
            }

        }
        
        return send_page (connection, buffer);
        
    }

    if (0 == strcmp (method, "POST")){
        printf("POST:\n");
        struct connection_info_struct *con_info = *con_cls;

        if (*upload_data_size != 0){
          //printf("%s\n", upload_data);
            MHD_post_process (con_info->postprocessor, upload_data,
                          *upload_data_size);
            *upload_data_size = 0;

            return MHD_YES;
        }
        else if (NULL != con_info->answerstring){
            printf("%s\n", con_info->answerstring);
            return send_page (connection, con_info->answerstring);
        }
    }

    return send_page (connection, errorpage);
}

static void request_completed (void *cls, struct MHD_Connection *connection,
                   void **con_cls, enum MHD_RequestTerminationCode toe){
    struct connection_info_struct *con_info = *con_cls;

    if (NULL == con_info)
        return;

    if (con_info->connectiontype == POST){
        MHD_destroy_post_processor (con_info->postprocessor);
        if (con_info->answerstring)
            free (con_info->answerstring);
    }

    free (con_info);
    *con_cls = NULL;
}

int main (){
    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon (MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
                             &answer_to_connection, NULL,
                             MHD_OPTION_NOTIFY_COMPLETED, request_completed,
                             NULL, MHD_OPTION_END);
    if (NULL == daemon)
        return 1;

    getchar ();

    MHD_stop_daemon (daemon);

    return 0;
}
