# Makefile para la práctica de acceso a ficheros en Linux

CC = gcc
CFLAGS = -Wall -Wextra -O2

# Objetivos a compilar por defecto
TARGETS = sys_1 sys_4k libc mmap

all: $(TARGETS)

# 1. Syscalls con buffer de 1 byte
sys_1: read_sys.c
	$(CC) $(CFLAGS) -DBUFFER_SIZE=1 $< -o $@

# 2. Syscalls con buffer de 4KB
sys_4k: read_sys.c
	$(CC) $(CFLAGS) -DBUFFER_SIZE=4096 $< -o $@

# 3. Biblioteca estándar (libc)
libc: read_libc.c
	$(CC) $(CFLAGS) $< -o $@

# 4. Memoria mapeada en RAM (mmap)
mmap: read_mmap.c
	$(CC) $(CFLAGS) $< -o $@

# Generar archivo de datos temporal 
datos:
	yes "Esta es una linea de prueba para contar repeticiones y saltos de linea." | head -c 524288000 > /tmp/datos.txt

# Regla de limpieza para borrar los ejecutables y archivos temporales
clean:
	rm -f $(TARGETS) datos.txt

.PHONY: all clean datos
