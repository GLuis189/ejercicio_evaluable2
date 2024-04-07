#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include "claves.h"
#include "lines.h"

int init()
{
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;
    int32_t op;
    int err;
    int32_t res;
    sd = socket(AF_INET, SOCK_STREAM, 0);

    if (sd == -1)
    {
        perror("socket");
        return -1;
    }
    memset(&server_addr, 0, sizeof(server_addr));
    hp = gethostbyname(getenv("IP_TUPLAS"));
    if (hp == NULL)
    {
        printf("Error en gethostbyname\n");
        return -1;
    };
    int port = atoi(getenv("PORT_TUPLAS"));
    memcpy(&(server_addr.sin_addr), hp->h_addr_list[0], hp->h_length);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // se establece la conexión
    err = connect(sd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err == -1)
    {
        printf("Error en connect init\n");
        return -1;
    }
    op = 0;
    op = htonl(op);
    err = sendMessage(sd, (char *)&op, sizeof(int32_t)); // envía la operacion
    if (err == -1)
    {
        printf("Error en envio init\n");
        return -1;
    };

    err = recvMessage(sd, (char *)&res, sizeof(int32_t)); // recibe la respuesta
    if (err == -1)
    {
        printf("Error en recepcion init\n");
        return -1;
    };
    close(sd);
    return (int)res;
}

int set_value(int key, char *value, int N_value, double *V_value)
{
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;
    int32_t op;
    int err;
    int32_t res;
    sd = socket(AF_INET, SOCK_STREAM, 0);

    if (sd == -1)
    {
        perror("socket");
        return -1;
    }
    memset(&server_addr, 0, sizeof(server_addr));
    hp = gethostbyname(getenv("IP_TUPLAS"));
    if (hp == NULL)
    {
        printf("Error en gethostbyname\n");
        return -1;
    };
    int port = atoi(getenv("PORT_TUPLAS"));
    memcpy(&(server_addr.sin_addr), hp->h_addr_list[0], hp->h_length);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // se establece la conexión
    err = connect(sd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err == -1)
    {
        printf("Error en connect set\n");
        return -1;
    }
    op = 1;
    op = htonl(op);
    err = sendMessage(sd, (char *)&op, sizeof(int32_t)); // envía la operacion
    if (err == -1)
    {
        printf("Error en envio set\n");
        return -1;
    };
    int32_t key32 = (int32_t)key;
    key32 = htonl(key32);
    err = sendMessage(sd, (char *)&key32, sizeof(int32_t)); // envía la clave
    if (err == -1)
    {
        printf("Error en envio key\n");
        return -1;
    };

    err = sendMessage(sd, value, 256); // envía el valor 1
    if (err == -1)
    {
        printf("Error en envio value\n");
        return -1;
    };

    N_value = htonl((int32_t)N_value);
    err = sendMessage(sd, (char *)&N_value, sizeof(int32_t)); // envía la dimensión del vector
    if (err == -1)
    {
        printf("Error en envio N\n");
        return -1;
    };

    for (uint32_t i = 0; i < ntohl(N_value); i++)
    {
        err = sendMessage(sd, (char *)&V_value[i], sizeof(double));
        if (err == -1)
        {
            printf("Error en envio %d \n", i);
            return -1;
        }
    }

    err = recvMessage(sd, (char *)&res, sizeof(int32_t)); // recibe la respuesta
    if (err == -1)
    {
        printf("Error en recepcion\n");
        return -1;
    };
    close(sd);
    return (int)res;
}

int get_value(int key, char *value, int *N_value, double *V_value)
{
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;
    int32_t op;
    int err;
    int32_t res;
    sd = socket(AF_INET, SOCK_STREAM, 0);

    if (sd == -1)
    {
        perror("socket");
        return -1;
    }
    memset(&server_addr, 0, sizeof(server_addr));
    hp = gethostbyname(getenv("IP_TUPLAS"));
    if (hp == NULL)
    {
        printf("Error en gethostbyname\n");
        return -1;
    };
    int port = atoi(getenv("PORT_TUPLAS"));
    memcpy(&(server_addr.sin_addr), hp->h_addr_list[0], hp->h_length);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // se establece la conexión
    err = connect(sd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err == -1)
    {
        printf("Error en connect get\n");
        return -1;
    }

    op = 2;
    op = htonl(op);
    err = sendMessage(sd, (char *)&op, sizeof(int32_t)); // envía la operacion
    if (err == -1)
    {
        printf("Error en envio get\n");
        return -1;
    };
    int32_t key32 = (int32_t)key;
    key32 = htonl(key32);
    err = sendMessage(sd, (char *)&key32, sizeof(int32_t)); // envía la clave
    if (err == -1)
    {
        printf("Error en envio key\n");
        return -1;
    };
    err = recvMessage(sd, (char *)&res, sizeof(int32_t)); // recibe la respuesta
    if (err == -1)
    {
        printf("Error en recepcion\n");
        return -1;
    };
    err = recvMessage(sd, value, 256); // recibe el valor 1
    if (err == -1)
    {
        printf("Error en recepcion value\n");
        return -1;
    };
    err = recvMessage(sd, (char *)N_value, sizeof(int32_t)); // recibe la dimensión del vector
    *N_value = ntohl(*N_value);
    if (err == -1)
    {
        printf("Error en recepcion N\n");
        return -1;
    };
    for (int i = 0; i < *N_value; i++)
    {
        err = recvMessage(sd, (char *)&V_value[i], sizeof(double));
        if (err == -1)
        {
            printf("Error en recepcion %d \n", i);
            return -1;
        }
    }
    close(sd);
    return (int)res;
}

int modify_value(int key, char *value, int N_value, double *V_value)
{
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;
    int32_t op;
    int err;
    int32_t res;
    sd = socket(AF_INET, SOCK_STREAM, 0);

    if (sd == -1)
    {
        perror("socket");
        return -1;
    }
    memset(&server_addr, 0, sizeof(server_addr));
    hp = gethostbyname(getenv("IP_TUPLAS"));
    if (hp == NULL)
    {
        printf("Error en gethostbyname\n");
        return -1;
    };
    int port = atoi(getenv("PORT_TUPLAS"));
    memcpy(&(server_addr.sin_addr), hp->h_addr_list[0], hp->h_length);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // se establece la conexión
    err = connect(sd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err == -1)
    {
        printf("Error en connect modify\n");
        return -1;
    }

    op = 3;
    op = htonl(op);
    err = sendMessage(sd, (char *)&op, sizeof(int32_t)); // envía la operacion
    if (err == -1)
    {
        printf("Error en envio modify\n");
        return -1;
    };
    int32_t key32 = (int32_t)key;
    key32 = htonl(key32);
    err = sendMessage(sd, (char *)&key32, sizeof(int32_t)); // envía la clave
    if (err == -1)
    {
        printf("Error en envio key\n");
        return -1;
    };
    err = sendMessage(sd, value, 256); // envía el valor 1
    if (err == -1)
    {
        printf("Error en envio value\n");
        return -1;
    };
    N_value = htonl((int32_t)N_value);
    err = sendMessage(sd, (char *)&N_value, sizeof(int32_t)); // envía la dimensión del vector
    if (err == -1)
    {
        printf("Error en envio N\n");
        return -1;
    };
    for (uint32_t i = 0; i < ntohl(N_value); i++)
    {
        err = sendMessage(sd, (char *)&V_value[i], sizeof(double));
        if (err == -1)
        {
            printf("Error en envio %d \n", i);
            return -1;
        }
    }
    err = recvMessage(sd, (char *)&res, sizeof(int32_t)); // recibe la respuesta
    if (err == -1)
    {
        printf("Error en recepcion\n");
        return -1;
    };
    close(sd);
    return (int)res;
}

int delete_key(int key)
{
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;
    int32_t op;
    int err;
    int32_t res;
    sd = socket(AF_INET, SOCK_STREAM, 0);

    if (sd == -1)
    {
        perror("socket");
        return -1;
    }
    memset(&server_addr, 0, sizeof(server_addr));
    hp = gethostbyname(getenv("IP_TUPLAS"));
    if (hp == NULL)
    {
        printf("Error en gethostbyname\n");
        return -1;
    };
    int port = atoi(getenv("PORT_TUPLAS"));
    memcpy(&(server_addr.sin_addr), hp->h_addr_list[0], hp->h_length);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // se establece la conexión
    err = connect(sd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err == -1)
    {
        printf("Error en connect delete\n");
        return -1;
    }

    op = 4;
    op = htonl(op);
    err = sendMessage(sd, (char *)&op, sizeof(int32_t)); // envía la operacion
    if (err == -1)
    {
        printf("Error en envio delete\n");
        return -1;
    };
    int32_t key32 = (int32_t)key;
    key32 = htonl(key32);
    err = sendMessage(sd, (char *)&key32, sizeof(int32_t)); // envía la clave
    if (err == -1)
    {
        printf("Error en envio key\n");
        return -1;
    };
    err = recvMessage(sd, (char *)&res, sizeof(int32_t)); // recibe la respuesta
    if (err == -1)
    {
        printf("Error en recepcion\n");
        return -1;
    };
    close(sd);
    return (int)res;
}

int exist(int key)
{
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;
    int32_t op;
    int err;
    int32_t res;
    sd = socket(AF_INET, SOCK_STREAM, 0);

    if (sd == -1)
    {
        perror("socket");
        return -1;
    }
    memset(&server_addr, 0, sizeof(server_addr));
    hp = gethostbyname(getenv("IP_TUPLAS"));
    if (hp == NULL)
    {
        printf("Error en gethostbyname\n");
        return -1;
    };
    int port = atoi(getenv("PORT_TUPLAS"));
    memcpy(&(server_addr.sin_addr), hp->h_addr_list[0], hp->h_length);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // se establece la conexión
    err = connect(sd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err == -1)
    {
        printf("Error en connect exist\n");
        return -1;
    }

    op = 5;
    op = htonl(op);
    err = sendMessage(sd, (char *)&op, sizeof(int32_t)); // envía la operacion
    if (err == -1)
    {
        printf("Error en envio exist\n");
        return -1;
    };
    int32_t key32 = (int32_t)key;
    key32 = htonl(key32);
    err = sendMessage(sd, (char *)&key32, sizeof(int32_t)); // envía la clave
    if (err == -1)
    {
        printf("Error en envio key\n");
        return -1;
    };
    err = recvMessage(sd, (char *)&res, sizeof(int32_t)); // recibe la respuesta
    if (err == -1)
    {
        printf("Error en recepcion\n");
        return -1;
    };
    close(sd);
    return (int)res;
}