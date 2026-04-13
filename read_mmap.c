/**
 * read_mmap.c
 * 
 * Este programa cuenta saltos de línea utilizando Memoria Mapeada (Memory Mapping).
 * En lugar de usar buffers explícitos, "engañamos" al proceso haciéndole creer que
 * todo el fichero de disco está cargado en su espacio de memoria RAM (arrays).
 * 
 * CONCEPTOS CLAVE:
 * - Page Faults: El kernel NO carga todo el archivo de golpe. Cuando intentamos
 *   acceder a un byte `ptr[i]` que aún no está en RAM, se produce una "falta de página" (Page Fault).
 *   La CPU pausa nuestro proceso, el Kernel carga el bloque (4KB) de disco a RAM física,
 *   y luego nuestro proceso sigue como si nada.
 * - Zero-Copy: Esta técnica es muy eficiente porque el Kernel mapea directamente sus
 *   páginas de memoria al espacio del usuario, ¡evitando así realizar copias extras de datos
 *   entre el espacio de Kernel y el de Usuario!
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>      // O_RDONLY
#include <sys/mman.h>   // mmap(), munmap()
#include <sys/stat.h>   // struct stat, fstat()
#include <unistd.h>     // close()

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso correcto: %s <fichero>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) { 
        perror("Error al hacer open()"); 
        return EXIT_FAILURE; 
    }

    // 1. Obtener tamaño del fichero
    // Para mapear un fichero en memoria, necesitamos saber cuántos bytes ocupa exactamente.
    struct stat st;
    if (fstat(fd, &st) == -1) { 
        perror("Error en fstat()"); 
        close(fd); 
        return EXIT_FAILURE; 
    }
    size_t size = st.st_size;

    // 2. Mapear el fichero usando mmap()
    char *ptr = mmap(NULL,           // Dirección sugerida (NULL = que el Kernel decida)
                     size,           // Longitud del mapeo a crear
                     PROT_READ,      // Permisos: Solo lectura
                     MAP_PRIVATE,    // Visibilidad: Privada (sin reflejar en otros procesos)
                     fd,             // Descriptor del fichero que vamos a mapear
                     0);             // Desplazamiento desde el principio (offset = 0)
                     
    if (ptr == MAP_FAILED) { 
        perror("Error crítico en mmap()"); 
        close(fd); 
        return EXIT_FAILURE; 
    }

    // 3. Procesamiento (La ilusión de que está en RAM)
    // Tratamos el archivo completo como un simple array de chars. ¡Así de limpio!
    long count = 0;
    for (size_t i = 0; i < size; i++) {
        // En la primera pasada de un bloque, esto provocará un Page Fault.
        // En los siguientes 4095 bytes, será tan rápido como leer la memoria RAM.
        if (ptr[i] == '\n') {
            count++;
        }
    }

    printf("Total líneas (mmap): %ld\n", count);

    // 4. Limpieza
    munmap(ptr, size); // "Desenganchamos" la memoria mapeada
    close(fd);
    
    return EXIT_SUCCESS;
}
