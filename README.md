# Práctica: Estrategias de Acceso a Ficheros y Rendimiento en Linux

## 1. Objetivos
- Comprender la diferencia entre **Llamadas al Sistema**, **Funciones de Biblioteca** y **Memoria Mapeada**.
- Analizar el impacto de los cambios de contexto (context switches) y el buffering en el rendimiento del software.
- Aprender a utilizar herramientas de diagnóstico de Linux como `time` y `strace`.

## 2. El Problema
Implementar tres variantes de un programa que cuente las apariciones del carácter salto de línea (`\n`) en un fichero de gran tamaño (mínimo 100MB). El programa debe ser capaz de procesar el archivo utilizando tres mecanismos distintos de Linux.

## 3. Ficheros del Repositorio
- `read_sys.c`: Acceso mediante syscalls `read()`. Permite configurar el tamaño del buffer en compilación.
- `read_libc.c`: Acceso mediante `fgetc()` de la biblioteca estándar (libc).
- `read_mmap.c`: Acceso mediante mapeo de memoria `mmap()`.

## 4. Ejercicio Práctico

### Paso 1: Preparación del entorno
Crea un fichero de datos aleatorios de 500MB:
```bash
base64 /dev/urandom | head -c 500M > datos.bin
```

### Paso 2: Análisis de rendimiento
Compila las versiones y utiliza el comando `time` para medir el desempeño. Para la versión de syscalls, compila con diferentes tamaños de buffer:

```bash
# Syscalls con buffer de 1 byte
gcc -DBUFFER_SIZE=1 read_sys.c -o sys_1

# Syscalls con buffer de 4KB
gcc -DBUFFER_SIZE=4096 read_sys.c -o sys_4k

# Biblioteca estándar
gcc read_libc.c -o libc

# Memoria mapeada
gcc read_mmap.c -o mmap
```

Ejecuta cada uno con `time ./nombre_ejecutable datos.bin`.

### Paso 3: Informe de resultados
Responde a las siguientes cuestiones basándose en tus medidas:

1.  **Comparativa de Syscalls**: Compara `sys_1` frente a `sys_4k`. ¿A qué se debe la diferencia masiva de tiempo si el código hace exactamente lo mismo?
2.  **Transparencia**: ¿Por qué la versión `libc` (que usa `fgetc` carácter a carácter) es mucho más rápida que `sys_1`? ¿Dónde reside la "magia"?
3.  **Análisis de Mmap**: Compara el "System Time" (tiempo en kernel) de `mmap` frente a `sys_4k`. ¿Por qué `mmap` suele ser más eficiente en el uso de CPU?
4.  **Uso de Strace**: Ejecuta `strace -c ./libc datos.bin` y `strace -c ./mmap datos.bin`. Adjunta el resumen de llamadas al sistema y explica las diferencias encontradas.

---
**Notas:**
- Con `BUFFER_SIZE=1`, el sistema operativo realiza un cambio de contexto (Usuario -> Kernel -> Usuario) por cada byte, lo que penaliza enormemente el rendimiento.
- Con `mmap`, se evita la copia de datos entre el buffer del kernel y el espacio de usuario.
- Para evitar efectos de la caché (opcional): `sync; echo 3 | sudo tee /proc/sys/vm/drop_caches`.
