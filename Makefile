# Makefile para compilar cliente, servidor y crear libclaves.so
# Nombres de los archivos binarios
BIN_FILES = cliente cliente2 servidor

# Compilador y opciones
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
INSTALL_PATH = /ruta/a/tu/install/path
CPPFLAGS = -I$(INSTALL_PATH)/include
LDFLAGS = -L$(INSTALL_PATH)/lib/
LDLIBS = -lpthread -lrt

# Regla para compilar todo
all: $(BIN_FILES) libclaves.so
.PHONY: all

# Reglas para compilar cliente y servidor
cliente: cliente.o libclaves.so
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

cliente2: cliente2.o libclaves.so
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

servidor: servidor.o libclaves.so
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

# Regla para compilar archivos fuente a objetos
%.o: %.c 
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

# Regla para crear la biblioteca compartida libclaves.so
libclaves.so: claves.o lines.o
	$(CC) -shared -o $@ $^

# Regla para limpiar archivos generados
clean:
	rm -f $(BIN_FILES) *.o libclaves.so
.PHONY: clean
