#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "lines.h"

int main(int argc, char *argv[])
{
    struct sockaddr_in server_addr, client_addr;
    socklen_t size;
    int sd, sc;
    int err;

    // Crear el socket
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("SERVER: Error en el socket");
        return (0);
    }

    // Permite que se reuse el socket
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &1, sizeof(int));

    // Inicializar
	bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(4200);
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
		printf("conexión aceptada de IP: %s   Puerto: %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port)); 

    }
    
}