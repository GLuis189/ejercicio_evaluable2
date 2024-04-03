#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include "mensajes.h"
#include "claves.h"
#include "lines.h"


int init(){
    printf("Iniciando cliente\n");
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;
	char op;
    int err;
    int res;
    sd = socket(AF_INET, SOCK_STREAM, 0);

    if (sd == -1) {
        perror("socket");
        return -1;
    }
    bzero((char *)&server_addr, sizeof(server_addr));
   	hp = gethostbyname(getenv("IP_TUPLAS"));
	if (hp == NULL) {
		printf("Error en gethostbyname\n");
		return -1;
	};
    printf("IP: %s\n", getenv("IP_TUPLAS"));
    int port = atoi(getenv("PORT_TUPLAS"));
    printf("Puerto: %d\n", port);
    memcpy(&(server_addr.sin_addr), hp->h_addr_list[0], hp->h_length);
   	server_addr.sin_family  = AF_INET;
   	server_addr.sin_port    = htons(port);
   	
	// se establece la conexión
   	err = connect(sd, (struct sockaddr *) &server_addr,  sizeof(server_addr));
	if (err == -1) {
		printf("Error en connect init\n");
		return -1;
	}
    op = htonl(INIT);
    err = sendMessage(sd, (char *) &op, sizeof(char));  // envía la operacion
	if (err == -1){
		printf("Error en envio init\n");
		return -1;
	};
    err = recvMessage(sd, (char *) &res, sizeof(int32_t));     // recibe la respuesta
	if (err == -1){
		printf("Error en recepcion init\n");
		return -1;
	};
    close(sd);
    return res;
}

int set_value(int key, char *value, int N_value, double *V_value){
    printf("Seteando valor\n");
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;
    char op;
    int err;
    int res;
    sd = socket(AF_INET, SOCK_STREAM, 0);

    if (sd == -1) {
        perror("socket");
        return -1;
    }
    bzero((char *)&server_addr, sizeof(server_addr));
    hp = gethostbyname(getenv("IP_TUPLAS"));
    if (hp == NULL) {
        printf("Error en gethostbyname\n");
        return -1;
    };
    printf("IP: %s\n", getenv("IP_TUPLAS"));
    int port = atoi(getenv("PORT_TUPLAS"));
    printf("Puerto: %d\n", port);
    memcpy(&(server_addr.sin_addr), hp->h_addr_list[0], hp->h_length);
    server_addr.sin_family  = AF_INET;
    server_addr.sin_port    = htons(port);

    // se establece la conexión
    err = connect(sd, (struct sockaddr *) &server_addr,  sizeof(server_addr));
    if (err == -1) {
        printf("Error en connect set\n");
        return -1;
    }
    op = htonl(SET);
    err = sendMessage(sd, (char *) &op, sizeof(char));  // envía la operacion
    if (err == -1){
        printf("Error en envio set\n");
        return -1;
    };
    key = htonl(key);
    err = sendMessage(sd, (char *) &key, sizeof(int32_t));  // envía la clave
    if (err == -1){
        printf("Error en envio key\n");
        return -1;
    };
    err = sendMessage(sd, value, 256);  // envía el valor 1
    if (err == -1){
        printf("Error en envio value\n");
        return -1;
    };
    N_value = htonl(N_value);
    err = sendMessage(sd, (char *) &N_value, sizeof(int32_t));  // envía la dimensión del vector
    if (err == -1){
        printf("Error en envio N\n");
        return -1;
    };
    for(int i; i < N_value; i++){
        err = sendMessage(sd, (char *) &V_value[i], sizeof(char));  // envía el vector
        if (err == -1){
            printf("Error en envio %d \n", i);
            return -1;
        };
    }
    fflush(stdout);
    
    err = recvMessage(sd, (char *) &res, sizeof(int32_t));     // recibe la respuesta
    if (err == -1){
        printf("Error en recepcion\n");
        return -1;
    };
    close(sd);
    return res;
}
