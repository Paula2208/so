/*
Debido al semáforo, la salida se verá ordenada. Primero uno de los hilos imprimirá todo su bloque (números + guiones), luego el otro.

Sin semáforo, las salidas de ambos hilos se mezclarían (condición de carrera), lo que haría que la consola se viera confusa.
*/

#include <stdio.h>      // Para printf, perror, etc.
#include <stdlib.h>     // Para exit()
#include <fcntl.h>      // Para O_CREAT en sem_open
#include <time.h>       // No se usa en este programa realmente
#include <math.h>       // No se usa tampoco
#include <pthread.h>    // Para trabajar con hilos
#include <semaphore.h>  // Para trabajar con semáforos POSIX
#include <unistd.h>     // Para sleep()

#define MAX_PROCESOS 1  // Número máximo de procesos que pueden entrar a la sección crítica (como un mutex)

sem_t *semaforo;        // Puntero al semáforo POSIX

// Estructura para pasar datos a los hilos
struct datos_tipo {
    int dato;   // Valor que se imprimirá (por ejemplo 1 o 2)
    int p;      // Cantidad de veces que se imprime ese valor y los guiones
};

// Función que ejecuta cada hilo
void *proceso(void *datos)
{
    // Se hace cast del puntero void al tipo de datos adecuado
    struct datos_tipo *datos_proceso = (struct datos_tipo *) datos;

    int a, i, p;

    // Espera a que el semáforo esté disponible (bloquea si ya hay un hilo dentro)
    sem_wait(semaforo);

    a = datos_proceso->dato;  // Obtiene el número a imprimir (1 o 2)
    p = datos_proceso->p;     // Número de repeticiones

    // Imprime 'a' p veces (por ejemplo, 1 1 1 1...)
    for(i = 0; i <= p; i++) {
        printf("%i ", a);
    }

    fflush(stdout);  // Fuerza a imprimir en consola lo que haya en el buffer
    sleep(2);        // Espera 2 segundos (simula procesamiento)

    // Imprime guiones p veces (- - - ...)
    for(i = 0; i <= p; i++) {
        printf("- ");
    }

    fflush(stdout);  // Asegura que todo lo impreso salga a consola

    // Libera el semáforo (permite que otro hilo entre)
    sem_post(semaforo);

    return NULL;     // Fin del hilo
}

int main()
{
    int error;                   // Para capturar errores al crear hilos
    char *valor_devuelto;        // Valor de retorno de los hilos (no se usa aquí)

    // Datos para los dos hilos
    struct datos_tipo hilo1_datos, hilo2_datos;

    // Identificadores de los hilos
    pthread_t idhilo1, idhilo2;

    // Inicializa los valores que cada hilo imprimirá
    hilo1_datos.dato = 1;  // Primer hilo imprimirá 1
    hilo2_datos.dato = 2;  // Segundo hilo imprimirá 2

    hilo1_datos.p = 10;    // Primer hilo imprime 1, 10 veces (y 10 guiones)
    hilo2_datos.p = 5;     // Segundo hilo imprime 2, 5 veces (y 5 guiones)

    // Crea el semáforo POSIX nombrado con permisos 0700 (sólo dueño puede usarlo) y valor inicial 1
    semaforo = sem_open("semaforo_name", O_CREAT, 0700, MAX_PROCESOS);

    // Crea el primer hilo, pasa la función y los datos como argumento
    error = pthread_create(&idhilo1, NULL, proceso, (void *)&hilo1_datos);
    if (error != 0) {
        perror("No puedo crear hilo");
        exit(-1);
    }

    // Crea el segundo hilo
    error = pthread_create(&idhilo2, NULL, proceso, (void *)&hilo2_datos);
    if (error != 0) {
        perror("No puedo crear hilo");
        exit(-1);
    }

    // Espera a que ambos hilos terminen
    pthread_join(idhilo2, (void **)&valor_devuelto);
    pthread_join(idhilo1, (void **)&valor_devuelto);

    // Cierra y destruye el semáforo
    sem_close(semaforo);               // Cierra el semáforo
    sem_unlink("semaforo_name");       // Borra el semáforo del sistema

    return 0;  // Fin del programa
}
