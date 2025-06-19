#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> // Para pipe(), write(), read()

// SOLO COMO EJERCICIO

#define NUM_HILOS 8
#define ITERACIONES 2e09

double resultadoFinal = 0.0;

struct DatosChunk {
    long start;
    long end;
    int fd[2]; // Pipe compartido
    int error;
};

// Tubería como entre bloqueante
void *leibniz(void *arg) {
    struct DatosChunk *datos = (struct DatosChunk *)arg;
    double sum = 0.0;

    for (long k = datos->start; k < datos->end; k++) {
        sum += ((k % 2 == 0) ? 1.0 : -1.0) / (2 * k + 1);
    }

    int testigo;

    // Leer del pipe
    if (read(datos->fd[0], &testigo, sizeof(int)) == -1) {
        perror("Error al leer del pipe");
        datos->error = 1;
        pthread_exit(NULL);
    }

    // Sumar el valor leído
    resultadoFinal += sum;

    // Escribir el testigo
    if (write(datos->fd[1], &testigo, sizeof(int)) == -1) {
        perror("Error al escribir en el pipe");
        datos->error = 1;
        pthread_exit(NULL);
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t hilo[NUM_HILOS];
    struct DatosChunk datos[NUM_HILOS];
    int fd[2]; // fd[0] leer, fd[1] escribir
    int r;
    struct timespec inicio, fin;
    int testigo = 1;
    long chunk_size = ITERACIONES / NUM_HILOS;

    clock_gettime(CLOCK_MONOTONIC, &inicio);

    if (pipe(fd) == -1) {
        perror("Error al crear el pipe");
        return -1;
    }

    // Escribir el valor inicial (semilla) en el pipe
    if (write(fd[1], &testigo, sizeof(int)) == -1) {
        perror("Error al escribir en el pipe");
        close(fd[1]);
        close(fd[0]);
        return -1;
    }

    // Asignar rangos y pipe a los hilos
    for (int i = 0; i < NUM_HILOS; i++) {
        datos[i].start = i * chunk_size;
        datos[i].end = (i == NUM_HILOS - 1) ? ITERACIONES : (i + 1) * chunk_size;
        datos[i].fd[0] = fd[0];
        datos[i].fd[1] = fd[1];
        datos[i].error = 0;
    }

    // Crear hilos
    for (int i = 0; i < NUM_HILOS; i++) {
        r = pthread_create(&hilo[i], NULL, leibniz, (void *)&datos[i]);
        if (r != 0) {
            perror("Error al crear el hilo");
            return -1;
        }
    }

    // Esperar a que terminen
    for (int i = 0; i < NUM_HILOS; i++) {
        r = pthread_join(hilo[i], NULL);
        if (r != 0) {
            perror("Error en el join");
            return -1;
        }

        if (datos[i].error) {
            perror("Error al procesar el hilo join");
            return -1;
        }
    }

    resultadoFinal *= 4;

    close(fd[1]);
    close(fd[0]);

    clock_gettime(CLOCK_MONOTONIC, &fin);

    long segundos = fin.tv_sec - inicio.tv_sec;
    long nanosegundos = fin.tv_nsec - inicio.tv_nsec;
    double duracion_ms = segundos * 1000.0 + nanosegundos / 1e6;

    printf("\n\nResultados ʕ•́ᴥ•̀ʔっ\n\n");
    printf("Valor aproximado de pi: %.15f\n\n", resultadoFinal);
    printf("Hilos usados: %d\n", NUM_HILOS);
    printf("Iteraciones por hilo: %ld\n", chunk_size);
    printf("Iteraciones totales: %ld\n", (long)ITERACIONES);
    printf("Tiempo de ejecución total: %.3f ms\n", duracion_ms);

    return 0;
}
