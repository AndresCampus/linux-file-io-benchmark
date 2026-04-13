# Práctica: Estrategias de Acceso a Ficheros y Rendimiento en Linux

## 1. El Reto y los Objetivos
En el diseño de Sistemas Operativos, leer un archivo del disco duro no es siempre equivalente en términos de rendimiento. Entender **cómo** llega un byte desde el disco duro hasta la CPU de tu programa es crucial para escribir software de alto rendimiento.

En esta práctica vamos a implementar un programa muy simple: contar cuántos saltos de línea (`\n`) existen en un fichero de gran tamaño (~500MB). Para ello implementaremos **tres estrategias distintas** para evidenciar las enormes diferencias de rendimiento que ocurren por debajo.

### Objetivos Didácticos
- Comprender la frontera entre el **Espacio de Usuario** y el **Espacio de Kernel**.
- Analizar qué es un **Cambio de Contexto** (*Context Switch*) y cómo aniquila el rendimiento.
- Entender la necesidad fundamental de la existencia de la Biblioteca de C (*libc*) y sus buffers internos.
- Explorar cómo manipular las tablas de paginación de la Memoria Virtual (*mmap*).

---

## 2. Los Esqueletos de Código
En este repositorio se proporcionan 3 implementaciones base que debes estudiar, compilar y comparar:

| Estrategia | Archivo | Descripción | 
| :--- | :--- | :--- |
| **Llamada a Sistema (Syscall)** | `read_sys.c` | Acceso "desnudo" usando la syscall `read()`. Aquí gestionamos un buffer manual (con tamaño personalizable `BUFFER_SIZE`). |
| **Biblioteca en C (Libc)** | `read_libc.c` | Uso de `fopen` y la función `fgetc()`. ¡Lee carácter a carácter usando el "Buffering Inteligente" interno oculto de libc! |
| **Memoria Mapeada (Mmap)** | `read_mmap.c` | "Engaña" al proceso cargando todo el contenido del archivo en la RAM virtual como si fuese un simple array continuo de tamaño gigante. |

---

## 3. Preparación del Laboratorio

Primero, asegúrate de estar en un entorno Linux o macOS con `gcc` instalado.

### Paso 3.1: Entorno de Pruebas Empíricas
Vamos a generar un archivo enorme y repetitivo, con un salto de línea por secuencia para poder contarlas visualmente. Ejecuta este comando en tu terminal para crear un archivo llamado `datos.txt` de exactamente 500 MB:

```bash
# Escribe incesantemente "Línea de prueba..." hasta llegar a los 500 MB
yes "Esta es una linea de prueba para contar repeticiones y saltos de linea." | head -c 500M > datos.txt
```

### Paso 3.2: Compilación
Para que la comparativa del método `Llamadas a Sistema` se observe de manera explícita, vamos a compilar generando un ejecutable configurado con el *Peor Escenario Posible* y otro con un *Escenario Óptimo*:

```bash
# 1. Syscalls con buffer ridículo de 1 byte (La Tortura de la CPU)
gcc -DBUFFER_SIZE=1 read_sys.c -o sys_1

# 2. Syscalls con buffer estándar de 4KB (El tamaño típico de página de RAM)
gcc -DBUFFER_SIZE=4096 read_sys.c -o sys_4k

# 3. Biblioteca estándar C (Libc)
gcc read_libc.c -o libc

# 4. Memoria mapeada en RAM (Mmap)
gcc read_mmap.c -o mmap
```

---

## 4. Obtención de Datos

> [!TIP]
> **Antes de empezar**: Si observáis que las segundas ejecuciones son más rápidas que las primeras es porque Linux almacena en la caché de la RAM las páginas de disco leídas recientemente (Page Cache). Esto es completamente normal y una métrica importante en sistemas.

Usa la herramienta `time` de Unix (por ejemplo `time ./sys_4k datos.txt`) para ejecutar los test e inspeccionar tres métricas clave:
- **`real` (Wall time):** Tiempo total que le costó obtener un resultado (como cronometrado por reloj).
- **`user` (User time):** Tiempo que sumó ejecutando **TU código** C en el espacio de usuario.
- **`sys` (System time):** Tiempo de CPU gastado por el Kernel **en su lado**, realizando el trabajo y moviendo punteros y datos que le habías pedido.

Rellena en tu cabeza o en un papel una tabla similar a esta:
| Ejecutable | Tiempo Total (`real`) | Tu Código (`user`) | Tiempo de Kernel (`sys`) |
| :--- | :--- | :--- | :--- |
| `./sys_1` (buffer = 1 byte) | | | |
| `./sys_4k` (buffer = 4096 bytes)| | | |
| `./libc` (`fgetc` en bucle) | | | |
| `./mmap` (mapeo directo) | | | |

---

## 5. El Cuestionario Reflexivo

El entregable final consta de redactar un pequeño informe respondiendo a estas 4 preguntas, justificándolas en base al marco teórico y tus experimentos anteriores.

1. **La Paradoja de los Syscalls**: Teniendo en cuenta y comparado `sys_1` y `sys_4k` ¿A qué se debe semejante brecha colosal en el rendimiento teniendo en cuenta que dentro llevan *literalmente las mismas líneas de código base*?
2. **Magia de la Libc**: En un vistazo rápido, parece que la versión de libc (`fgetc()`) evalúa el fichero **byte a byte** igual que hiciera `sys_1`, pero, sin embargo, su rendimiento se compite e iguala con `sys_4k`. **¿Qué estrategia utiliza la estructura `FILE *` por debajo del telón para optimizar este proceso?**
3. **Mmap y la Desaparición del Kernel**: Revisa el campo **`sys`** obtenido con `./mmap` al mapear el fichero, respecto al conseguido usando `./sys_4k`. Analiza por qué `mmap` arroja datos del sistema tan bajos y justifica en base al concepto teórico de ***Zero-Copy*** (Cero-Copia de Buffers).
4. **La Lupa de `Strace`**: La orden `strace -c` monitoriza como si fuéramos hackers **todas las peticiones y llamadas que un usuario le realiza al kernel**. Ejecuta:
   ```bash
   strace -c ./sys_1 datos.txt
   ```
   Y compáralo frente a esto:
   ```bash
   strace -c ./libc datos.txt
   ```
   Adjunta ambas tablas y expón una conclusión sobre **cuántas veces exactas se interrumpió a la CPU (Context Switch)** para llamar a `read()`. ¿Por qué pasa eso?
