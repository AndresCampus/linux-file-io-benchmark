/**
 * read_libc.c
 * 
 * Este programa cuenta saltos de línea utilizando la Biblioteca Estándar de C (libc).
 * Usa la función de alto nivel `fgetc()`.
 * 
 * CONCEPTOS CLAVE:
 * - Buffering transparente: Aunque le pedimos a la libc un solo carácter a la vez (fgetc),
 *                           la libc ES INTELIGENTE. Internamente reserva un bloque grande de memoria
 *                           (suele ser 4KB u 8KB) e invoca al Kernel (read()) solo cuando 
 *                           dicho buffer interno se queda vacío.
 * - Rendimiento: Gracias a este buffer, ahorramos millones de costosos Context Switches.
 */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso correcto: %s <fichero>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // fopen() es parte de libc. Internamente acabará llamando a open().
    FILE *f = fopen(argv[1], "r");
    if (!f) { 
        perror("Error al hacer fopen()"); 
        return EXIT_FAILURE; 
    }

    // IMPORTANTE: fgetc() devuelve un 'int', no un 'char'.
    // ¿Por qué? Porque necesita poder devolver el carácter leído (0 a 255)
    // O devolver una constante especial que indique "Fin de Fichero" o Error.
    // Esta constante es EOF (habitualmente -1). Si usaramos 'char', no podríamos
    // distinguir entre el byte de datos 0xFF (-1) y el código de error EOF (-1).
    int c;
    long count = 0;

    // Bucle transparente. Pedimos a la libc 1 byte cada vez.
    // La mayoría de estas pasadas son RÁPIDAS (solo leemos de RAM, espacio de usuario).
    // Ocasionalmente, la libc detendrá el bucle milisegundos para llamar a read() y rellenar.
    while ((c = fgetc(f)) != EOF) {
        if (c == '\n') {
            count++;
        }
    }

    printf("Total líneas (libc): %ld\n", count);
    
    // fclose() libera el buffer interno de libc y llama a close()
    fclose(f);
    return EXIT_SUCCESS;
}
