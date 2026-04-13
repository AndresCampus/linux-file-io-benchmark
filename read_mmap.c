#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <fichero>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) { perror("open"); return 1; }

    // Necesitamos el tamaño del fichero para mapearlo
    struct stat st;
    if (fstat(fd, &st) == -1) { perror("fstat"); close(fd); return 1; }
    size_t size = st.st_size;

    // Mapear el fichero en memoria (Solo lectura, Privado)
    char *ptr = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (ptr == MAP_FAILED) { perror("mmap"); close(fd); return 1; }

    long count = 0;
    for (size_t i = 0; i < size; i++) {
        if (ptr[i] == '\n') count++;
    }

    printf("Total líneas (mmap): %ld\n", count);

    munmap(ptr, size);
    close(fd);
    return 0;
}
