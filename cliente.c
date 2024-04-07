// Código del cliente - nada de colas y funciones como si fuesen locales

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "claves.h"


int main(){
    if (init() == 0){
        printf("Inicializado cliente\n");
    };
    // probar el set_value asi int set_value(int key, char *value1, int N_value2, double *V_value_2). 
    if(set_value(1, "hola", 3, (double[]){1.0, 2.0, 3.0}) == 0){
       printf("Seteado valor\n");
    }
    if(set_value(2, "hola2", 4, (double[]){1.0, 2.0, 3.0, 4.0}) == 0){
        printf("Seteado valor 2\n");
    }
    if(set_value(2, "holasdasda2", 3, (double[]){2.0, 3.0, 4.0}) == 0){
        printf("Seteado valor 2 mal\n");
    }
    int n;
    char value_1[256];
    double *v_value_2 = malloc(sizeof(long) * 32);
    if(get_value(2, value_1, &n, v_value_2) == 0){
        printf("Valor obtenido %s \n", value_1);
        for (int i = 0; i < n; i++){ 
            printf("%f\n", v_value_2[i]);
        }
    }
    free(v_value_2);
    char value_3[256] = "nuevo valor";
    double v_value_4[32] = {4.0, 5.0, 6.0, 7.0};
    int a = modify_value(2, value_3, n, v_value_4);
    if(a == 0){
        printf("Valor modificado con éxito.\n");
    } else {
        printf("Error al modificar el valor.\n");
    }
    if(get_value(2, value_1, &n, v_value_2) == 0){
        printf("Valor obtenido %s \n", value_1);
        for (int i = 0; i < n; i++){ 
            printf("%f\n", v_value_2[i]);
        }
    }
    if(delete_key(1) == 0){
        printf("Clave eliminada con éxito.\n");
    } else {
        printf("Error al eliminar la clave.\n");
    }
   
    if(exist(2) == 0){
        printf("La clave existe.\n");
    }else {
        printf("La clave no existe.\n");
    }
    if(exist(1) == 0){
        printf("La clave existe.\n");
    }else {
        printf("La clave no existe.\n");
    }


    return 0;
}