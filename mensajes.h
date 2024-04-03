#ifndef MENSAJES_H
#define MENSAJES_H

#define MAX	256

#define INIT	0
#define SET     1
#define GET     2
#define MODIFY  3
#define DELETE  4
#define EXIST   5

typedef struct{
    int clave;
    char valor1[MAX];
    int N;
    double *vector;
}Tupla;

#endif