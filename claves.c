#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "mensajes.h"
#include "claves.h"

#define PORT 8080

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