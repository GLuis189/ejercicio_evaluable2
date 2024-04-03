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


/*

int init(){
    int sockfd;
    struct sockaddr_in servaddr;

    // Crear el socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Asignar dirección IP y puerto
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    // Conectar el cliente al servidor
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("connection failed");
        exit(EXIT_FAILURE);
    }

    struct peticion p;
    p.op = INIT;
    sprintf(p.q_name,  "/Cola-%d", getpid());

    // Enviar la petición
    write(sockfd, &p, sizeof(p));

    int r;
    read(sockfd, &r, sizeof(int));

    close(sockfd);

    return r;
}

int set_value(int key, char *value, int N_value, double *V_value){
    int sockfd;
    struct sockaddr_in servaddr;

    if(N_value > 32 || N_value < 0){
        return -1;
    }

    // Crear el socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Asignar dirección IP y puerto
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    // Conectar el cliente al servidor
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("connection failed");
        exit(EXIT_FAILURE);
    }

    struct peticion p;
    p.op = SET;
    p.key = key;
    strcpy(p.value1, value);
    p.N_value = N_value;
    memcpy(p.V_value, V_value, sizeof(double) * N_value);

    // Enviar la petición
    write(sockfd, &p, sizeof(p));

    int r;
    read(sockfd, &r, sizeof(int));

    close(sockfd);

    return r;
}

*/


int init(){
    printf("Iniciando cliente\n");
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;
	char op;
    int err;
    int res;
    printf("1");
    sd = socket(AF_INET, SOCK_STREAM, 0);

    if (sd == -1) {
        perror("socket");
        return -1;
    }
    printf("2");
    bzero((char *)&server_addr, sizeof(server_addr));
    printf("3");
   	hp = gethostbyname(getenv("IP_TUPLAS"));
    printf("4");
	if (hp == NULL) {
		printf("Error en gethostbyname\n");
		return -1;
	};
    int port = atoi(getenv("PORT_TUPLAS"));
    printf("Puerto: %d\n", port);
    memcpy(&(server_addr.sin_addr), hp->h_addr_list[0], hp->h_length);
   	server_addr.sin_family  = AF_INET;
   	server_addr.sin_port    = htons(port);
   	
	// se establece la conexión
   	err = connect(sd, (struct sockaddr *) &server_addr,  sizeof(server_addr));
	if (err == -1) {
		printf("Error en connect\n");
		return -1;
	}
    op = htonl(INIT);
    err = sendMessage(sd, (char *) &op, sizeof(char));  // envía la operacion
	if (err == -1){
		printf("Error en envio\n");
		return -1;
	};
    err = recvMessage(sd, (char *) &res, sizeof(int32_t));     // recibe la respuesta
	if (err == -1){
		printf("Error en recepcion\n");
		return -1;
	};
    close(sd);
    return res;
}
    

    // Crear el socket
//     if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
//         perror("socket creation failed");
//         exit(EXIT_FAILURE);
//     }

//     memset(&servaddr, 0, sizeof(servaddr));

//     // Asignar dirección IP y puerto
//     servaddr.sin_family = AF_INET;
//     servaddr.sin_port = htons(PORT);
//     servaddr.sin_addr.s_addr = INADDR_ANY;

//     // Conectar el cliente al servidor
//     if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
//         perror("connection failed");
//         exit(EXIT_FAILURE);
//     }

//     close(sockfd);

//     return r;
// }

// int set_value(int key, char *value, int N_value, double *V_value){
//     int sockfd;
//     struct sockaddr_in servaddr;

//     if(N_value > 32 || N_value < 0){
//         return -1;
//     }

//     // Crear el socket
//     if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
//         perror("socket creation failed");
//         exit(EXIT_FAILURE);
//     }

//     memset(&servaddr, 0, sizeof(servaddr));

//     // Asignar dirección IP y puerto
//     servaddr.sin_family = AF_INET;
//     servaddr.sin_port = htons(PORT);
//     servaddr.sin_addr.s_addr = INADDR_ANY;

//     // Conectar el cliente al servidor
//     if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
//         perror("connection failed");
//         exit(EXIT_FAILURE);
//     }

//     struct peticion p;
//     p.op = SET;
//     p.key = key;
//     strcpy(p.value1, value);
//     p.N_value = N_value;
//     memcpy(p.V_value, V_value, sizeof(double) * N_value);

//     // Enviar la petición
//     write(sockfd, &p, sizeof(p));

//     int r;
//     read(sockfd, &r, sizeof(int));

//     close(sockfd);

//     return r;
// }