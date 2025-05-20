#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

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
    key_t ky = 123;

    // Crear segmento de memoria compartida
    int shmid = shmget(
        ky,// Llave para encontrar la memoria compartida
        4 * sizeof(double), // Espacio a guardar
        IPC_CREAT | 0666 // Permisos 0666 en octal sería (rwx | uga) - 110 110 110 (solo leer y escribir)
    );
    if (shmid == -1) {
        perror("Error al crear la memoria compartida");
        return -1;
    }

    // Asociar la memoria al espacio del proceso padre
    double *ap_memory = (double *)shmat(shmid, NULL, 0);
    if (ap_memory == (void *)-1) {
        perror("Error al asociar la memoria compartida");
        return -1;
    }

    for (int i = 0; i < 4; i++) {
        pids[i] = fork();

        if (pids[i] < 0) {
            fprintf(stderr, "Error al crear el proceso %d\n", i);
            return -1;
        } else if (pids[i] == 0) {
            // Proceso hijo

            long start = i * part;
            long end = (i == 3) ? N : start + part;
            ap_memory[i] = leibniz(start, end);

            if (shmdt(ap_memory) == -1) {
                perror("Hijo: error al desasociar la memoria compartida");
                exit(1);
            }

            exit(0);
        }
        //Si es un proceso padre, solo sigue al siguiente hijo :)
    }

    // Esperar a todos los hijos
    for (int i = 0; i < 4; i++) {
        wait(NULL);
    }

    // Sumar los resultados
    double pi = 0.0;
    for (int i = 0; i < 4; i++) {
        pi += ap_memory[i];
    }

    // Desasociar y eliminar memoria compartida
    if (shmdt(ap_memory) == -1) {
        perror("Padre: error al desasociar la memoria compartida");
        return -1;
    }

    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        perror("Padre: error al eliminar la memoria compartida");
        return -1;
    }

    printf("PI ≈ %.15f\n", 4 * pi);
    return 0;
}
