#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <fichero>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) { perror("fopen"); return 1; }

    int c;
    long count = 0;

    // Aunque leemos byte a byte, la libc carga bloques de 4KB o más internamente
    while ((c = fgetc(f)) != EOF) {
        if (c == '\n') count++;
    }

    printf("Total líneas (libc): %ld\n", count);
    fclose(f);
    return 0;
}
