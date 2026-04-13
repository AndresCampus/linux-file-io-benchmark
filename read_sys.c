/**
 * read_sys.c
 * 
 * Este programa cuenta saltos de línea utilizando la Llamada al Sistema (Syscall) `read()`.
 * Es el nivel más bajo de acceso a ficheros en espacio de usuario.
 * 
 * CONCEPTOS CLAVE:
 * - Buffer: Un array en memoria (espacio de usuario) donde guardamos temporalmente lo que
 *           leemos del disco duro (vía Kernel) antes de procesarlo.
 * - Context Switch: Cada vez que llamamos a `read()`, la CPU debe cambiar del estado
 *                   "Usuario" al estado "Kernel" (Context Switch). Esto es muy costoso.
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>   // O_RDONLY
#include <unistd.h>  // read(), close()

// Si no se define en compilación (ej: gcc -DBUFFER_SIZE=4096), usamos 1024 por defecto
#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso correct: %s <fichero>\n", argv[0]);
        return EXIT_FAILURE; // 1
    }

    // 1. Abrimos el fichero invocando al Kernel
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) { 
        perror("Error al hacer open()"); 
        return EXIT_FAILURE; 
    }

    // Reservamos nuestro buffer en la pila (espacio de usuario)
    char buffer[BUFFER_SIZE];
    ssize_t bytes_leidos;
    long count = 0;

    // 2. Bucle de lectura
    // read() le pide al Kernel que llene 'buffer' con hasta BUFFER_SIZE bytes.
    // Retorna el número de bytes leídos realmente, o 0 si llegó al final (EOF).
    while ((bytes_leidos = read(fd, buffer, BUFFER_SIZE)) > 0) {
        
        // Espacio de Usuario: Ahora que tenemos los datos, iteramos sobre ellos
        for (int i = 0; i < bytes_leidos; i++) {
            if (buffer[i] == '\n') {
                count++;
            }
        }
    }

    // 3. Comprobación de errores en read()
    if (bytes_leidos == -1) {
        perror("Error crítico durante read()");
    }

    printf("Total líneas (syscall, buffer=%d): %ld\n", BUFFER_SIZE, count);
    
    // Le decimos al Kernel que libere el descriptor de fichero
    close(fd);
    return EXIT_SUCCESS; // 0
}
