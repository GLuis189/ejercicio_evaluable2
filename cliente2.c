// Código del cliente

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "claves.h"


int main(){
    if(set_value(3, "hola c2", 3, (double[]){1.0, 2.0, 3.0}) == 0){
        printf("Seteado valor 3\n");
    }
    else{
        printf("Error al setear valor 3\n");
    }
    if(set_value(4, "hola2 c2", 4, (double[]){1.0, 2.0, 3.0, 4.0}) == 0){
        printf("Seteado valor 4\n");
    }
    else{
        printf("Error al setear valor 4\n");
    }
    if(exist(2) == 0){
        printf("La clave 2 existe.\n");
    }
    if(exist(3) == 0){
        printf("La clave 3 existe.\n");
    }
    if(delete_key(3) == 0){
        printf("Clave 3 eliminada con éxito.\n");
    }
    if(exist(3) == 0){
        printf("La clave 3 existe.\n");
    }
    else
    {
        printf("La clave 3 no existe.\n");
    }
    return 0;
}