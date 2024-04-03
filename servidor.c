#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include "mensajes.h"
#include "lines.h"

#define PORT 8080

#define MAX_TUPLAS 100 

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int busy = true;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutex_tuplas = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_keys = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_archivo = PTHREAD_MUTEX_INITIALIZER;

/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include "mensajes.h"
#include "claves.h"

#define PORT 8080

// Estructura para representar una tupla <clave-valor1-valor2>
typedef struct {
    int clave;
    char valor1[256];
    int N;
    double *vector;
} Tupla;

// Definición del vector global para almacenar las tuplas
Tupla tuplas[MAX_TUPLAS];
int keys[MAX_TUPLAS]; // Array para almacenar las claves
int numTuplas = 0; // Variable global para almacenar el número actual de tuplas

// Definición de la variable global para el nombre del archivo
char filename[FILENAME_MAX];

pthread_mutex_t mutex_tuplas = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_keys = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_archivo = PTHREAD_MUTEX_INITIALIZER;

// Función para escribir las tuplas en un archivo de texto
void escribirTuplas() {
    pthread_mutex_lock(&mutex_archivo);
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error al abrir el archivo para escribir.\n");
        pthread_mutex_unlock(&mutex_archivo);
        return;
    }

    for (int i = 0; i < numTuplas; i++) {
        fprintf(fp, "%d,%s", tuplas[i].clave, tuplas[i].valor1);
        for (int j = 0; j < tuplas[i].N; j++) {
            fprintf(fp, ",%.2f", tuplas[i].vector[j]);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
    pthread_mutex_unlock(&mutex_archivo);
}

// Función para leer las tuplas desde un archivo de texto
void leerTuplas() {
    pthread_mutex_lock(&mutex_archivo);
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error al abrir el archivo para leer.\n");
        pthread_mutex_unlock(&mutex_archivo);
        return;
    }

    while (!feof(fp) && numTuplas < MAX_TUPLAS) {
        Tupla t;
        int result = fscanf(fp, "%d,%[^,],%d", &t.clave, t.valor1, &t.N);
        if (result == EOF) {
            break;
        }
        t.vector = (double *)malloc(t.N * sizeof(double));
        for (int i = 0; i < t.N; i++) {
            fscanf(fp, ",%lf", &t.vector[i]);
        }
        pthread_mutex_lock(&mutex_tuplas);
        pthread_mutex_lock(&mutex_keys);
        tuplas[numTuplas] = t;
        keys[numTuplas] = t.clave;
        numTuplas++;
        pthread_mutex_unlock(&mutex_keys);
        pthread_mutex_unlock(&mutex_tuplas);
    }

    fclose(fp);
    pthread_mutex_unlock(&mutex_archivo);
}

int r_init(){   
    printf("Inicializado\n");

    strcpy(filename, "datos.txt");
    remove(filename);
    escribirTuplas();
    leerTuplas();    
    return 0;
}

int r_set_value(int key, char *value1, int N_value, double *V_value){
    pthread_mutex_lock(&mutex_tuplas);
    pthread_mutex_lock(&mutex_keys);

    // Buscar si la clave ya existe
    for (int i = 0; i < numTuplas; i++) {
        if (keys[i] == key) {
            // Si la clave ya existe, imprimir un mensaje de error y devolver un código de error
            printf("Error set_value: Ya existe una tupla con la clave %d\n", key);
            pthread_mutex_unlock(&mutex_tuplas);
            pthread_mutex_unlock(&mutex_keys);
            return -1;
        }
    }

    // Si la clave no existe, añadir una nueva tupla
    Tupla t;
    t.clave = key;
    strcpy(t.valor1, value1);
    t.N = N_value;
    t.vector = (double *)malloc(N_value * sizeof(double));
    for (int i = 0; i < N_value; i++) {
        t.vector[i] = V_value[i];
    }
    tuplas[numTuplas] = t;
    keys[numTuplas] = key; // Añadir la clave al array de claves
    numTuplas++;
    escribirTuplas();

    pthread_mutex_unlock(&mutex_tuplas);
    pthread_mutex_unlock(&mutex_keys);
    
    return 0;
}

int r_get_value(int key, char *value1, int *N_value, double *V_value){
    printf("Get value\n");

    pthread_mutex_lock(&mutex_tuplas);
    pthread_mutex_lock(&mutex_keys);
    
    for (int i = 0; i < numTuplas; i++) {
        if (keys[i] == key) {
            strcpy(value1, tuplas[i].valor1);
            *N_value = tuplas[i].N;
            for (int j = 0; j < tuplas[i].N; j++) {
                V_value[j] = tuplas[i].vector[j];
            }

            pthread_mutex_unlock(&mutex_tuplas);
            pthread_mutex_unlock(&mutex_keys);
    
            return 0;
        }
    }
    printf("Error get_value: No existe una tupla con la clave %d\n", key);
    
    return -1;
}

int r_modify_value(int key, char *value1, int N_value, double *V_value){
    printf("Modify value\n");

    pthread_mutex_lock(&mutex_tuplas);
    pthread_mutex_lock(&mutex_keys);
    
    for (int i = 0; i < numTuplas; i++) {
        if (keys[i] == key) {
            strcpy(tuplas[i].valor1, value1);
            tuplas[i].N = N_value;
            for (int j = 0; j < N_value; j++) {
                tuplas[i].vector[j] = V_value[j];
            }
            escribirTuplas();

            pthread_mutex_unlock(&mutex_tuplas);
            pthread_mutex_unlock(&mutex_keys);

            return 0;
        }
    }
    printf("Error modify_value: No existe una tupla con la clave %d\n", key);

    return -1;
}

int r_delete_key(int key){
    printf("Delete key\n");

    pthread_mutex_lock(&mutex_tuplas);
    pthread_mutex_lock(&mutex_keys);

    for (int i = 0; i < numTuplas; i++) {
        if (keys[i] == key) {
            for (int j = i; j < numTuplas - 1; j++) {
                tuplas[j] = tuplas[j + 1];
                keys[j] = keys[j + 1];
            }
            numTuplas--;
            escribirTuplas();

            pthread_mutex_unlock(&mutex_tuplas);
            pthread_mutex_unlock(&mutex_keys);

            return 0;
        }
    }
    printf("Error delete_key: No existe una tupla con la clave %d\n", key);
    return -1;
}

int r_exist(int key){

    pthread_mutex_lock(&mutex_tuplas);
    pthread_mutex_lock(&mutex_keys);
    
    printf("Exist\n");
    for (int i = 0; i < numTuplas; i++) {
        if (keys[i] == key) {
            printf("si existe");

            pthread_mutex_unlock(&mutex_tuplas);
            pthread_mutex_unlock(&mutex_keys);

            return 0;
        }
    }
    printf("No existe\n");

    pthread_mutex_unlock(&mutex_tuplas);
    pthread_mutex_unlock(&mutex_keys);

    return -1;
}

void tratar_peticion(int sockfd){
    struct peticion p;
    int r;

    // Leer la petición del socket
    read(sockfd, &p, sizeof(struct peticion));

    switch (p.op){
        case INIT:
            r = r_init();
            break;
        case SET:
            r = r_set_value(p.key, p.value1, p.N_value, p.V_value);
            break;
        case GET:
            r = r_get_value(p.key, p.value1, &p.N_value, p.V_value);
            break;
        case MODIFY:
            r = r_modify_value(p.key, p.value1, p.N_value, p.V_value);
            break;
        case DELETE:
            r = r_delete_key(p.key);
            break;
        case EXIST:
            r = r_exist(p.key);
            break;
        default:
            r = -1;
            break;
    }

    // Enviar la respuesta al cliente
    write(sockfd, &r, sizeof(int));
}

void *thread_func(void *arg){
    int newsockfd = *(int *)arg;
    tratar_peticion(newsockfd);
    close(newsockfd);
    return NULL;
}

int main(){
    int sockfd, newsockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t clilen;
    pthread_t thread;

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

    // Enlazar el socket con la dirección IP
    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Poner el servidor a escuchar
    if (listen(sockfd, 5) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    while (1){
        clilen = sizeof(cliaddr);
        // Aceptar una nueva conexión
        if ((newsockfd = accept(sockfd, (struct sockaddr*)&cliaddr, &clilen)) < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        // Crear un nuevo hilo para tratar la petición
        if (pthread_create(&thread, NULL, thread_func, &newsockfd) != 0){
            perror("pthread_create");
        }
    }

    close(sockfd);
    return 0;
}


*/


// Definición del vector global para almacenar las tuplas
Tupla tuplas[MAX_TUPLAS];
int keys[MAX_TUPLAS]; // Array para almacenar las claves
int numTuplas = 0; // Variable global para almacenar el número actual de tuplas

// Definición de la variable global para el nombre del archivo
char filename[FILENAME_MAX];


// Función para escribir las tuplas en un archivo de texto
void escribirTuplas() {
    pthread_mutex_lock(&mutex_archivo);
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error al abrir el archivo para escribir.\n");
        pthread_mutex_unlock(&mutex_archivo);
        return;
    }

    for (int i = 0; i < numTuplas; i++) {
        fprintf(fp, "%d,%s", tuplas[i].clave, tuplas[i].valor1);
        for (int j = 0; j < tuplas[i].N; j++) {
            fprintf(fp, ",%.2f", tuplas[i].vector[j]);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
    pthread_mutex_unlock(&mutex_archivo);
}

// Función para leer las tuplas desde un archivo de texto
void leerTuplas() {
    pthread_mutex_lock(&mutex_archivo);
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error al abrir el archivo para leer.\n");
        pthread_mutex_unlock(&mutex_archivo);
        return;
    }

    while (!feof(fp) && numTuplas < MAX_TUPLAS) {
        Tupla t;
        int result = fscanf(fp, "%d,%[^,],%d", &t.clave, t.valor1, &t.N);
        if (result == EOF) {
            break;
        }
        t.vector = (double *)malloc(t.N * sizeof(double));
        for (int i = 0; i < t.N; i++) {
            fscanf(fp, ",%lf", &t.vector[i]);
        }
        pthread_mutex_lock(&mutex_tuplas);
        pthread_mutex_lock(&mutex_keys);
        tuplas[numTuplas] = t;
        keys[numTuplas] = t.clave;
        numTuplas++;
        pthread_mutex_unlock(&mutex_keys);
        pthread_mutex_unlock(&mutex_tuplas);
    }

    fclose(fp);
    pthread_mutex_unlock(&mutex_archivo);
}

int r_init(){   
    printf("Inicializado\n");

    escribirTuplas();
    leerTuplas();    
    return 0;
}

void tratar_peticion(int * sockfd){
    char op;
    int err;

    int32_t key, N_value, res;
    char value1[MAX];
    
    pthread_mutex_lock(&mutex);
    busy = false;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    err = recvMessage(sockfd, (char *)&op, sizeof(char));   // receive the operation
    if (err == -1) {
        printf("Error in reception\n");
        close(sockfd);
        return;
    }
    err = recvMessage(sockfd, (char *)&key, sizeof(char));
    if (err == -1) {
        printf("Error in reception\n");
        close(sockfd);
        return;
    } 
    err = recvMessage(sockfd, (char *)&value1, sizeof(char)*MAX);
    if (err == -1) {
        printf("Error in reception\n");
        close(sockfd);
        return;
    }
    err = recvMessage(sockfd, (char *)&N_value, sizeof(int));
    if (err == -1) {
        printf("Error in reception\n");
        close(sockfd);
        return;
    }
    int vector = (double *)malloc(N_value * sizeof(double));
    err = recvMessage(sockfd, (char *)&vector, sizeof(double)*N_value);
    if (err == -1) {
        printf("Error in reception\n");
        close(sockfd);
        return;
    }
    int r;
    switch (op){
        case INIT:
            r = r_init();
            break;
        // case SET:
        //     r = r_set_value(key, value1, N_value, V_value);
        //     break;
        // case GET:
        //     r = r_get_value(key, value1, &N_value, V_value);
        //     break;
        // case MODIFY:
        //     r = r_modify_value(key, value1, N_value, V_value);
        //     break;
        // case DELETE:
        //     r = r_delete_key(key);
        //     break;
        // case EXIST:
        //     r = r_exist(key);
        //     break;
        // default:
        //     r = -1;
        //     break;
    }
    
    // if (err == -1) {
    //     printf("Error in reception\n");
    //     close(sockfd);
    //     return;
    // }
    // err = recvMessage(sockfd, (char *)&a, sizeof(int32_t)); // receive a
    // if (err == -1) {
    //     printf("Error in reception\n");
    //     close(sockfd);
    //     return;
    // }
    // err = recvMessage(sockfd, (char *)&b, sizeof(int32_t)); // receive b
    // if (err == -1) {
    //     printf("Error in reception\n");
    //     close(sockfd);
    //     return;
    // }
    // a = ntohl(a);
    // b = ntohl(b);

    // if (op == 0) // process the request
    //     res = a + b;
    // else
    //     res = a - b;

    // res = htonl(res);
    // err = sendMessage(sockfd, (char *)&res, sizeof(int32_t));  // send the result
    // if (err == -1) {
    //     printf("Error in sending\n");
    //     close(sockfd);
    //     return;
    // }
    res = htonl(res);
    err = sendMessage(sockfd, (char *)&res, sizeof(int32_t));  // envía el resultado
		if (err == -1) {
			printf("Error en envi�o\n");
			close(sockfd);
		}
    close(sockfd); // close the connection
    pthread_exit(NULL);
}


int main(int argc, char *argv[])
{
    struct sockaddr_in server_addr, client_addr;
    socklen_t size;
    int sd, sc;
    int err;
    int op;
    pthread_t thid;
    pthread_attr_t attr;

    // Inicializar el atributo
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);


    // Crear el socket
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("SERVER: Error en el socket");
        return (0);
    }

    // Permite que se reuse el socket
    int val = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(int));

    // Inicializar
	bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(argv[1]);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind
    err = bind(sd, (const struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err == -1) {
		printf("Error en bind\n");
		return -1;
	}

    // Listen
    err = listen(sd, SOMAXCONN);
	if (err == -1) {
		printf("Error en listen\n");
		return -1;
	}
    
    size = sizeof(client_addr);

    // Aceptar la conexión en sí
    while(1){
        printf("Esperando conexión...");

        //Nuevo socket
    	sc = accept(sd, (struct sockaddr *)&client_addr, (socklen_t *)&size);
        if (sc == -1) {
			printf("Error en accept\n");
			return -1;
		}
        pthread_create(&thid, &attr, tratar_peticion, (int *)&sc);
		printf("conexión aceptada de IP: %s   Puerto: %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port)); 
        while(busy){
            pthread_cond_wait(&cond, &mutex);
            busy = true;
            pthread_mutex_unlock(&mutex);
        }
        close(sc);
    }
    close(sd);
    return 0;
}