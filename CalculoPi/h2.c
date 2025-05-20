#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>  // mmap, munmap
#include <sys/types.h>

double leibniz(long start, long end) {
    double sum = 0.0;
    for (long k = start; k < end; k++) {
        sum += ((k % 2 == 0) ? 1.0 : -1.0) / (2 * k + 1);
    }
    return sum;
}

int main() {
    pid_t pids[4];
    long N = 1e9;
    long part = N / 4;

    // Crear memoria compartida
    double *shared_results = mmap(
                                    NULL,                    // Dirección sugerida
                                    4 * sizeof(double),      // Tamaño del segmento de memoria
                                    PROT_READ | PROT_WRITE,  // Permisos (pueden leer y escribir)
                                    MAP_SHARED | MAP_ANONYMOUS, // Opciones de asignación
                                    -1,                      // Descriptor de archivo (no se usa aquí, pero toca ponerlo :/)
                                    0                        // Desplazamiento dentro del archivo (irrelevante aquí, pero toca ponerlo :/)
                                );

    if (shared_results == MAP_FAILED) {
        perror("Error al crear memoria compartida");
        return -1;
    }

    for (int i = 0; i < 4; i++) {
        pids[i] = fork();

        if (pids[i] < 0) {
            fprintf(stderr, "Error al crear el proceso %d\n", i);
            return -1;
        } else if (pids[i] == 0) {
            // Proceso Hijo
            long start = i * part;
            long end = (i == 3) ? N : start + part;
            double partial = leibniz(start, end);

            shared_results[i] = partial;  // Guardar en memoria compartida

            // Liberar la memoria
            if (munmap(shared_results, 4 * sizeof(double)) == -1) {
                perror("Error al liberar la memoria compartida");
                return -1;
            }
            exit(0);
        }

        //Si es un proceso padre, solo sigue al siguiente hijo :)
    }

    // Esperar a que todos los hijos terminen
    for (int i = 0; i < 4; i++) {
        wait(NULL);
    }

    // Sumar resultados desde memoria compartida
    double pi = 0.0;
    for (int i = 0; i < 4; i++) {
        pi += shared_results[i];
    }

    if (munmap(shared_results, 4 * sizeof(double)) == -1) {
        perror("Error al liberar la memoria compartida");
        return -1;
    }

    printf("PI ≈ %.15f\n", 4 * pi);
    return 0;
}
