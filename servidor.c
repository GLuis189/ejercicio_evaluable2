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


#define MAX_TUPLAS 100 

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int busy = true;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutex_tuplas = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_keys = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_archivo = PTHREAD_MUTEX_INITIALIZER;


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

    strcpy(filename, "datos.txt");
    remove(filename);
    escribirTuplas();
    leerTuplas();    
    return 0;
}

int r_set_value(int key, char *value1, int N_value, double *V_value){
    printf("Seteando valor\n");
    pthread_mutex_lock(&mutex_tuplas);
    pthread_mutex_lock(&mutex_keys);
    for (int i = 0; i < numTuplas; i++) {
        if (keys[i] == key) {
            pthread_mutex_unlock(&mutex_keys);
            pthread_mutex_unlock(&mutex_tuplas);
            return -1;
        }
    }

    Tupla t;
    t.clave = key;
    strcpy(t.valor1, value1);
    t.N = N_value;
    t.vector = (double *)malloc(N_value * sizeof(double));
    for (int i = 0; i < N_value; i++) {
        t.vector[i] = V_value[i];
    }

    tuplas[numTuplas] = t;
    keys[numTuplas] = key;
    numTuplas++;
    pthread_mutex_unlock(&mutex_keys);
    pthread_mutex_unlock(&mutex_tuplas);
    escribirTuplas();
    return 0;
}

void tratar_peticion(int * sockfd){
    char op;
    int err;
    int s_local;

    int32_t key, N_value, res;
    char value1[MAX]; 
    
    pthread_mutex_lock(&mutex);
    s_local = (* (int *)sockfd);
    busy = false;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    err = sendMessage(s_local, (char *) &op, sizeof(char));  // envía la operacion
    if (err == -1) {
        printf("Error in reception op\n");
        close(s_local);
        return;
    }
    if (op == INIT){
        res = r_init();
    }
    else{
        err = recvMessage(s_local, (char *)&key, sizeof(char));
    if (err == -1) {
        printf("Error in reception\n");
        close(s_local);
        return;
    } 
    err = recvMessage(s_local, (char *)&value1, sizeof(char)*MAX);
    if (err == -1) {
        printf("Error in reception\n");
        close(s_local);
        return;
    }
    err = recvMessage(s_local, (char *)&N_value, sizeof(int));
    if (err == -1) {
        printf("Error in reception\n");
        close(s_local);
        return;
    }
    double* V_value = (double *)malloc(N_value * sizeof(double));
    for (int i = 0; i < N_value; i++) {
        err = recvMessage(s_local, (char *)&V_value[i], sizeof(double));
        if (err == -1) {
            printf("Error in reception\n");
            close(s_local);
            return;
        }
    }
    switch (op){
        case SET:
            res = r_set_value(key, value1, N_value, V_value);
            break;
        // case GET:
        //     res = r_get_value(key, value1, &N_value, V_value);
        //     break;
        // case MODIFY:
        //     res = r_modify_value(key, value1, N_value, V_value);
        //     break;
        // case DELETE:
        //     res = r_delete_key(key);
        //     break;
        // case EXIST:
        //     res = r_exist(key);
        //     break;
        // default:
        //     res = -1;
        //     break;
    }
    }
    res = htonl(res);
    err = sendMessage(s_local, (char *)&res, sizeof(int32_t));  // envía el resultado
		if (err == -1) {
			printf("Error en envio\n");
			close(s_local);
		}
    close(s_local); // close the connection
    pthread_exit(NULL);
}


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Error atributos\n");
        return 0;
    }
    struct sockaddr_in server_addr, client_addr;
    socklen_t size;
    int sd, sc;
    int err;
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
    server_addr.sin_port        = htons(atoi(argv[1]));
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
        printf("Esperando conexión... \n");
        fflush(stdout);

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
    }
    close(sc);
    close(sd);
    return 0;
}