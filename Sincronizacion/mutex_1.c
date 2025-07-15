/*
Verás que cada bloque de impresión (número y guiones) se ejecuta completamente sin interrupciones. Por ejemplo:

0 0 - - 
1 1 1 - - - 
2 2 2 2 - - - - 
...

Pero nunca verás que las salidas de distintos hilos se mezclen, gracias al mutex.
*/

#include <stdio.h>      // Para printf y perror
#include <stdlib.h>     // Para exit()
#include <pthread.h>    // Para hilos y mutex
#include <unistd.h>     // Para sleep()

// Definiciones de constantes
#define MAX_PROCESOS 1  // (No se usa en este código, pero se podría usar si fuera con semáforos)
#define NUM_HILOS    8  // Número de hilos que se van a crear

// Estructura para enviar datos a los hilos
struct datos_tipo {
    int dato;  // Valor que el hilo imprimirá
    int p;     // Cuántas veces se imprimirá el valor y los guiones
};

// Declaración del mutex (exclusión mutua)
pthread_mutex_t mutex;

// Función que ejecutará cada hilo
void *proceso(void *datos)
{
    // Casting del puntero recibido a la estructura esperada
    struct datos_tipo *datos_proceso = (struct datos_tipo *) datos;

    int a, i, p;

    a = datos_proceso->dato;  // Número que imprimirá el hilo (ej: 0, 1, 2...)
    p = datos_proceso->p;     // Cuántas veces se imprime ese número y los guiones

    // Sección crítica protegida por mutex: sólo un hilo puede entrar a la vez
    pthread_mutex_lock(&mutex);

    // Imprime 'a' (dato) p veces
    for(i = 0; i <= p; i++) {
        printf("%i ", a);
    }

    fflush(stdout);  // Vacía el buffer para que la salida se imprima en consola
    sleep(3);        // Simula trabajo, espera 3 segundos

    // Imprime "-" p veces
    for(i = 0; i <= p; i++) {
        printf("- ");
    }

    fflush(stdout);  // Vacía el buffer de nuevo

    pthread_mutex_unlock(&mutex);  // Libera el mutex para que otro hilo entre

    return NULL;  // Fin del hilo
}

int main()
{
    int error, i;
    char *valor_devuelto;  // No se usa realmente en este caso

    // Arreglo de estructuras con los datos de cada hilo
    struct datos_tipo hilo_datos[NUM_HILOS];

    // Arreglo de identificadores de hilos
    pthread_t idhilo[NUM_HILOS];

    // Inicializa los datos para cada hilo
    for(i = 0; i < NUM_HILOS; i++) {
        hilo_datos[i].dato = i;      // Cada hilo imprime su número
        hilo_datos[i].p = i + 1;     // Cada hilo imprime más veces que el anterior
    }

    // Inicializa el mutex con atributos por defecto (NULL)
    pthread_mutex_init(&mutex, NULL);

    // Crea los hilos y les pasa su respectiva estructura de datos
    for(i = 0; i < NUM_HILOS; i++) {
        error = pthread_create(&idhilo[i], NULL, proceso, (void *) &hilo_datos[i]);
        if (error != 0) {
            perror("No puedo crear hilo");
            exit(-1);
        }
    }

    // Espera a que todos los hilos terminen su ejecución
    for(i = 0; i < NUM_HILOS; i++) {
        pthread_join(idhilo[i], (void **) &valor_devuelto);
    }

    // Destruye el mutex después de que todos los hilos terminaron
    pthread_mutex_destroy(&mutex);

    return 0;  // Fin del programa
}
