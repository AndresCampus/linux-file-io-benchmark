#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <fichero>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) { perror("open"); return 1; }

    char buffer[BUFFER_SIZE];
    ssize_t n;
    long count = 0;

    while ((n = read(fd, buffer, BUFFER_SIZE)) > 0) {
        for (int i = 0; i < n; i++) {
            if (buffer[i] == '\n') count++;
        }
    }

    printf("Total líneas (syscall): %ld\n", count);
    close(fd);
    return 0;
}
