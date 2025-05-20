#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

double leibniz(long start, long end) {
    double sum = 0.0;
    for (long k = start; k < end; k++) {
        sum += ((k % 2 == 0) ? 1.0 : -1.0) / (2 * k + 1);
    }
    return sum;
}

int main() {
    int fds[4][2]; // 4 listas de lectura y escritura :3
    pid_t pids[4];
    long N = 1e9;
    long part = N / 4;

    for (int i = 0; i < 4; i++) {

        if (pipe(fds[i]) == -1) {
            fprintf(stderr, "Error al crear el pipe %d\n", i);
            return -1;
        }

        pids[i] = fork();

        if (pids[i] < 0) {
            fprintf(stderr, "Error al crear el proceso %d\n", i);
            return -1;
        }
        else if(pids[i] == 0) {
            // Proceso Hijo

            close(fds[i][0]); // cerrar lectura

            long start = i * part;
            long end = (i == 3) ? N : start + part;
            double partial = leibniz(start, end);

            if (write(fds[i][1], &partial, sizeof(double)) == -1) {
                fprintf(stderr, "Error al escribir en el pipe %d\n", i);
                close(fds[i][1]);
                return -1;
            }

            close(fds[i][1]); // cerrar escritura
            exit(0);
        }

        //Si es un proceso padre, solo sigue al siguiente hijo :)
    }

    // Proceso Padre (Final)
    double pi = 0.0;
    for (int i = 0; i < 4; i++) {
        close(fds[i][1]); // cerrar escritura

        double partial;
        if (read(fds[i][0], &partial, sizeof(double)) == -1) {
            fprintf(stderr, "Error al leer del pipe %d\n", i);
            close(fds[i][0]);
            return -1;
        }

        pi += partial;
        close(fds[i][0]);

        wait(NULL); // Evitar zombies *_*
    }

    printf("PI ≈ %.15f\n", 4 * pi);
    exit(0);
    return 0;
}
