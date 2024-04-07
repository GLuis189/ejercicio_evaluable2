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
#include "lines.h"

#define MAX_TUPLAS 100
#define MAX	256

typedef struct{
    int clave;
    char valor1[MAX];
    int N;
    double *vector;
}Tupla;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int busy = true;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutex_tuplas = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_keys = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_archivo = PTHREAD_MUTEX_INITIALIZER;

// Definición del vector global para almacenar las tuplas
Tupla tuplas[MAX_TUPLAS];
int keys[MAX_TUPLAS]; // Array para almacenar las claves
int numTuplas = 0;    // Variable global para almacenar el número actual de tuplas

// Definición de la variable global para el nombre del archivo
char filename[FILENAME_MAX];

// Función para escribir las tuplas en un archivo de texto
void escribirTuplas()
{
    pthread_mutex_lock(&mutex_archivo);
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        printf("Error al abrir el archivo para escribir.\n");
        pthread_mutex_unlock(&mutex_archivo);
        return;
    }

    for (int i = 0; i < numTuplas; i++)
    {
        fprintf(fp, "%d,%s", tuplas[i].clave, tuplas[i].valor1);
        for (int j = 0; j < tuplas[i].N; j++)
        {
            fprintf(fp, ",%.2f", tuplas[i].vector[j]);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
    pthread_mutex_unlock(&mutex_archivo);
}

// Función para leer las tuplas desde un archivo de texto
void leerTuplas()
{
    pthread_mutex_lock(&mutex_archivo);
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Error al abrir el archivo para leer.\n");
        pthread_mutex_unlock(&mutex_archivo);
        return;
    }

    while (!feof(fp) && numTuplas < MAX_TUPLAS)
    {
        Tupla t;
        int result = fscanf(fp, "%d,%[^,],%d", &t.clave, t.valor1, &t.N);
        if (result == EOF)
        {
            break;
        }
        t.vector = (double *)malloc(t.N * sizeof(double));
        for (int i = 0; i < t.N; i++)
        {
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

int r_init()
{
    printf("Inicializado\n");

    strcpy(filename, "datos");
    remove(filename);
    escribirTuplas();
    leerTuplas();
    return 0;
}

int r_set_value(int key, char *value1, int N_value, double *V_value)
{
    printf("Seteado valor\n");
    Tupla t;
    t.clave = key;
    strcpy(t.valor1, value1);
    t.N = N_value;
    t.vector = (double *)malloc(N_value * sizeof(double));
    for (int i = 0; i < N_value; i++)
    {
        t.vector[i] = V_value[i];
    }

    pthread_mutex_lock(&mutex_tuplas);
    pthread_mutex_lock(&mutex_keys);
    for (int i = 0; i < numTuplas; i++)
    {
        if (keys[i] == key)
        {
            pthread_mutex_unlock(&mutex_keys);
            pthread_mutex_unlock(&mutex_tuplas);
            return -1;
        }
    }
    tuplas[numTuplas] = t;
    keys[numTuplas] = key;
    numTuplas++;
    pthread_mutex_unlock(&mutex_keys);
    pthread_mutex_unlock(&mutex_tuplas);

    escribirTuplas();
    return 0;
}

int r_get_value(int key, char *value1, int *N_value, double *V_value)
{
    printf("Obteniendo valor\n");
    pthread_mutex_lock(&mutex_tuplas);
    pthread_mutex_lock(&mutex_keys);
    for (int i = 0; i < numTuplas; i++)
    {
        if (keys[i] == key)
        {
            strcpy(value1, tuplas[i].valor1);
            *N_value = tuplas[i].N;
            for (int j = 0; j < *N_value; j++)
            {
                V_value[j] = tuplas[i].vector[j];
            }
            pthread_mutex_unlock(&mutex_keys);
            pthread_mutex_unlock(&mutex_tuplas);
            return 0;
        }
    }
    pthread_mutex_unlock(&mutex_keys);
    pthread_mutex_unlock(&mutex_tuplas);
    return -1;
}

int r_modify_value(int key, char *value1, int N_value, double *V_value)
{
    printf("Modificando valor\n");
    pthread_mutex_lock(&mutex_tuplas);
    pthread_mutex_lock(&mutex_keys);
    for (int i = 0; i < numTuplas; i++)
    {
        if (keys[i] == key)
        {
            strcpy(tuplas[i].valor1, value1);
            tuplas[i].N = N_value;
            // if (tuplas[i].vector != NULL) {
            //     free(tuplas[i].vector);  // Libera la memoria previamente asignada
            // }
            tuplas[i].vector = (double *)malloc(N_value * sizeof(double));
            for (int j = 0; j < N_value; j++)
            {
                tuplas[i].vector[j] = V_value[j];
            }
            pthread_mutex_unlock(&mutex_keys);
            pthread_mutex_unlock(&mutex_tuplas);
            escribirTuplas();
            return 0;
        }
    }
    pthread_mutex_unlock(&mutex_keys);
    pthread_mutex_unlock(&mutex_tuplas);
    return -1;
}

int r_delete_key(int key)
{
    printf("Borrando clave\n");
    pthread_mutex_lock(&mutex_tuplas);
    pthread_mutex_lock(&mutex_keys);
    for (int i = 0; i < numTuplas; i++)
    {
        if (keys[i] == key)
        {
            for (int j = i; j < numTuplas - 1; j++)
            {
                tuplas[j] = tuplas[j + 1];
                keys[j] = keys[j + 1];
            }
            numTuplas--;
            pthread_mutex_unlock(&mutex_keys);
            pthread_mutex_unlock(&mutex_tuplas);
            escribirTuplas();
            return 0;
        }
    }
    pthread_mutex_unlock(&mutex_keys);
    pthread_mutex_unlock(&mutex_tuplas);
    return -1;
}

int r_exist(int key)
{
    printf("Comprobando existencia\n");
    pthread_mutex_lock(&mutex_keys);
    for (int i = 0; i < numTuplas; i++)
    {
        if (keys[i] == key)
        {
            pthread_mutex_unlock(&mutex_keys);
            return 0;
        }
    }
    pthread_mutex_unlock(&mutex_keys);
    return -1;
}

void *tratar_peticion(void *sockfd)
{
    int *sockfd_int = (int *)sockfd;
    int err;
    int s_local;

    int32_t op, key, N_value, res;
    char value1[256];

    pthread_mutex_lock(&mutex);
    s_local = (*sockfd_int);
    busy = false;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    err = recvMessage(s_local, (char *)&op, sizeof(int32_t)); // envía la operacion
    if (err == -1)
    {
        printf("Error in reception op\n");
        close(s_local);
        return NULL;
    }
    printf("Operación: %d\n", op);
    op = ntohl(op);
    if (op == 0)
    {
        // INIT

        res = r_init();

        res = htonl(res);

        err = sendMessage(s_local, (char *)&res, sizeof(int32_t)); // envía el resultado
        if (err == -1)
        {
            printf("Error en envio\n");
            close(s_local);
            return NULL;
        }
    }
    else if (op == 1)
    {
        // SET
        err = recvMessage(s_local, (char *)&key, sizeof(int32_t));
        if (err == -1)
        {
            printf("Error in reception\n");
            close(s_local);
            return NULL;
        }
        key = ntohl(key);
        err = recvMessage(s_local, (char *)&value1, 256);
        if (err == -1)
        {
            printf("Error in reception\n");
            close(s_local);
            return NULL;
        }
        err = recvMessage(s_local, (char *)&N_value, sizeof(int32_t));
        N_value = ntohl(N_value);
        if (err == -1)
        {
            printf("Error in reception\n");
            close(s_local);
            return NULL;
        }
        double *V_value = (double *)malloc(N_value * sizeof(double));
        for (int i = 0; i < N_value; i++)
        {
            double temp;
            err = recvMessage(s_local, (char *)&temp, sizeof(double));
            if (err == -1)
            {
                printf("Error in reception\n");
                close(s_local);

                return NULL;
            }
            V_value[i] = temp;
        }

        res = r_set_value(key, value1, N_value, V_value);
        res = htonl(res);
        err = sendMessage(s_local, (char *)&res, sizeof(int32_t)); // envía el resultado
        if (err == -1)
        {
            printf("Error en envio\n");
            close(s_local);
            return NULL;
        }
        free(V_value);
    }
    else if (op == 2)
    {
        // GET
        err = recvMessage(s_local, (char *)&key, sizeof(int32_t));
        if (err == -1)
        {
            printf("Error in reception\n");
            close(s_local);
            return NULL;
        }
        key = ntohl(key);

        char value1[256];
        int32_t N_value, res;
        double *V_value = (double *)malloc(N_value * sizeof(double));

        res = r_get_value(key, value1, &N_value, V_value);

        res = htonl(res);
        err = sendMessage(s_local, (char *)&res, sizeof(int32_t)); // envía el resultado

        if (ntohl(res) == 0)
        {
            err = sendMessage(s_local, (char *)&value1, 256);
            if (err == -1)
            {
                printf("Error in reception\n");
                close(s_local);
                return NULL;
            }
            N_value = htonl(N_value);
            err = sendMessage(s_local, (char *)&N_value, sizeof(int32_t));
            if (err == -1)
            {
                printf("Error in reception\n");
                close(s_local);
                return NULL;
            }
            for (int i = 0; i < N_value; i++)
            {
                err = sendMessage(s_local, (char *)&V_value[i], sizeof(double));
                if (err == -1)
                {
                    printf("Error in reception\n");
                    close(s_local);

                    return NULL;
                }
            }
        }
        free(V_value);
    }
    else if (op == 3)
    {
        // MODIFY
        err = recvMessage(s_local, (char *)&key, sizeof(int32_t));
        if (err == -1)
        {
            printf("Error in reception\n");
            close(s_local);
            return NULL;
        }
        key = ntohl(key);

        err = recvMessage(s_local, (char *)&value1, 256);
        if (err == -1)
        {
            printf("Error in reception\n");
            close(s_local);
            return NULL;
        }

        err = recvMessage(s_local, (char *)&N_value, sizeof(int32_t));
        N_value = ntohl(N_value);
        if (err == -1)
        {
            printf("Error in reception\n");
            close(s_local);
            return NULL;
        }

        double *V_value = (double *)malloc(N_value * sizeof(double));
        for (int i = 0; i < N_value; i++)
        {
            double temp;
            err = recvMessage(s_local, (char *)&temp, sizeof(double));
            if (err == -1)
            {
                printf("Error in reception\n");
                close(s_local);

                return NULL;
            }
            V_value[i] = temp;
        }
        res = r_modify_value(key, value1, N_value, V_value);

        res = htonl(res);
        err = sendMessage(s_local, (char *)&res, sizeof(int32_t)); // envía el resultado
        if (err == -1)
        {
            printf("Error en envio\n");
            close(s_local);
            return NULL;
        }
        free(V_value);
    }
    else if (op == 4)
    {
        // DELETE
        err = recvMessage(s_local, (char *)&key, sizeof(int32_t));
        if (err == -1)
        {
            printf("Error in reception\n");
            close(s_local);
            return NULL;
        }
        key = ntohl(key);

        res = r_delete_key(key);
        res = htonl(res);
        err = sendMessage(s_local, (char *)&res, sizeof(int32_t)); // envía el resultado
        if (err == -1)
        {
            printf("Error en envio\n");
            close(s_local);
            return NULL;
        }
    }
    else if (op == 5)
    {
        // EXIST
        err = recvMessage(s_local, (char *)&key, sizeof(int32_t));
        if (err == -1)
        {
            printf("Error in reception\n");
            close(s_local);
            return NULL;
        }
        key = ntohl(key);

        res = r_exist(key);
        res = htonl(res);
        err = sendMessage(s_local, (char *)&res, sizeof(int32_t)); // envía el resultado
        if (err == -1)
        {
            printf("Error en envio\n");
            close(s_local);
            return NULL;
        }
    }
    else
    {
        printf("Operación no válida\n");
        close(s_local);
        return NULL;
    }
    fflush(stdout);

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
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&val, sizeof(int));

    // Inicializar
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind
    err = bind(sd, (const struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err == -1)
    {
        printf("Error en bind\n");
        return -1;
    }

    // Listen
    err = listen(sd, SOMAXCONN);
    if (err == -1)
    {
        printf("Error en listen\n");
        return -1;
    }

    size = sizeof(client_addr);

    // Aceptar la conexión en sí
    while (1)
    {
        printf("Esperando conexión... \n");
        fflush(stdout);

        // Nuevo socket
        sc = accept(sd, (struct sockaddr *)&client_addr, (socklen_t *)&size);
        if (sc == -1)
        {
            printf("Error en accept\n");
            return -1;
        }
        pthread_create(&thid, &attr, tratar_peticion, (void *)&sc);

        printf("conexión aceptada de IP: %s   Puerto: %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        while (busy)
        {
            pthread_cond_wait(&cond, &mutex);
            busy = true;
            pthread_mutex_unlock(&mutex);
        }
    }
    close(sd);
    return 0;
}