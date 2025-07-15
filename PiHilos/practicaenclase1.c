#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_HILOS 4           // Número de hilos a trabajar
#define ITERACIONES 2e09     // 2e09  // Iteraciones totales para el cálculo de Pi

// Estructura para pasar los datos de inicio y fin a cada hilo
struct DatosChunk{
    long start;
    long end;
    double resultado;        // Resultado parcial de la serie en este chunk
};

// Función que calcula la serie de Leibniz en un intervalo dado
void *leibniz(void *arg) {
    struct DatosChunk *datos;
    datos = arg;
    double sum = 0.0;

    for (long k = datos -> start; k < datos -> end; k++) {
        sum += ((k % 2 == 0) ? 1.0 : -1.0) / (2 * k + 1);
    }

    datos -> resultado = sum;
    pthread_exit(NULL); // Se puede retornar NULL porque el resultado queda en la estructura
}

int main() {
    pthread_t hilo[NUM_HILOS];
    struct DatosChunk datos[NUM_HILOS];
    int r;
    struct timespec inicio, fin;

    double resultadoFinal = 0.0;
    long chunk_size = ITERACIONES / NUM_HILOS;

    clock_gettime(CLOCK_MONOTONIC, &inicio);

    // Asignarle los respectivos chunks a los hilos
    for (int i = 0; i < NUM_HILOS; i++) {
        datos[i].start = i * chunk_size;
        datos[i].end = (i == NUM_HILOS - 1) ? ITERACIONES : (i + 1) * chunk_size;
        datos[i].resultado = 0.0;
    }

    // Crear hilos
    for (int i = 0; i < NUM_HILOS; i++) {
        // Crear el hilo y pasarle los datos del chunk
        r = pthread_create(
            &hilo[i],     // Apuntador al hilo
            NULL,         // Atributos por defecto
            leibniz,      // Función que ejecuta el hilo
            (void *) &datos[i] // Argumento de entrada
        );

        if (r != 0) {
            perror("Error al crear el hilo");
            return -1;
        }
    }

    // Esperar a que todos los hilos terminen y sumar sus resultados
    for (int i = 0; i < NUM_HILOS; i++) {
        r = pthread_join(hilo[i], NULL);
        if (r != 0) {
            perror("Error en el join");
            return -1;
        }

        resultadoFinal += datos[i].resultado;
    }

    // Multiplicar por 4 para obtener el valor aproximado de pi
    resultadoFinal *= 4;

    clock_gettime(CLOCK_MONOTONIC, &fin);  // Tiempo de fin

    // Calcular duración en milisegundos
    long segundos = fin.tv_sec - inicio.tv_sec;
    long nanosegundos = fin.tv_nsec - inicio.tv_nsec;
    double duracion_ms = segundos * 1000.0 + nanosegundos / 1e6;


    printf("\n\nResultados ʕ•́ᴥ•̀ʔっ\n\n");
    printf("Valor aproximado de pi: %.15f\n\n", resultadoFinal);
    printf("Hilos usados: %d\n", NUM_HILOS);
    printf("Iteraciones por hilo: %ld\n", chunk_size);
    printf("Iteraciones totales: %ld\n", (long) ITERACIONES);
    printf("Tiempo de ejecución total: %.3f ms\n", duracion_ms);

    return 0;
}
