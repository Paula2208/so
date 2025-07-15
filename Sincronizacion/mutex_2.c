/*
Este es un ejemplo clásico de sincronización de productor-consumidor usando mutex y variables de condición (pthread_cond_t) en C. Vamos a desglosarlo completamente.
✅ 1. Explicación detallada del código

Este programa ilustra un escenario con dos hilos:

    Un productor que genera datos y los coloca en un buffer circular.

    Un consumidor que extrae esos datos.

🧠 Problema que resuelve

El problema del productor-consumidor ocurre cuando:

    El productor debe esperar si el buffer está lleno.

    El consumidor debe esperar si el buffer está vacío.

Este código usa:

    Un mutex (pthread_mutex_t mutex) para evitar condiciones de carrera al acceder al buffer.

    Dos variables de condición:

        vacio: para que el consumidor espere si no hay datos.

        lleno: para que el productor espere si no hay espacio.

✅ Variables importantes

    buffer[MAX_BUFFER]: el buffer compartido (circular) donde se almacenan los datos.

    num_datos: cuántos elementos hay actualmente en el buffer.

    posicion: índice para insertar o consumir (avanza circularmente).

✅ Flujo general:

    El productor inserta MAX_DATOS datos en el buffer.

        Espera si el buffer está lleno.

        Notifica al consumidor cuando inserta el primer dato.

    El consumidor consume esos MAX_DATOS datos.

        Espera si el buffer está vacío.

        Notifica al productor cuando saca un dato y deja un espacio libre.
*/

/*
El programa imprimirá alternadamente mensajes como:

se ha producido el dato: 0
se ha consumido el dato: 0
se ha producido el dato: 1
se ha consumido el dato: 1
...

Gracias al uso del mutex y las condiciones, nunca se consume un dato que no se ha producido, ni se sobreescribe uno sin consumir.
*/

// Este es un ejemplo que se puede contextualizar cuando uno envía un audio o video.

#include <stdio.h>      // printf, perror, etc.
#include <termios.h>    // (No se usa realmente aquí)
#include <sys/mman.h>   // (Tampoco se usa)
#include <stdlib.h>     // malloc, exit, etc.
#include <sys/types.h>  // Tipos de sistema
#include <sys/stat.h>   // Para open
#include <fcntl.h>      // Para O_CREAT, O_RDONLY, etc.
#include <time.h>       // (No usado directamente aquí)
#include <math.h>       // (Tampoco se usa)
#include <pthread.h>    // Hilos, mutex, condiciones
#include <unistd.h>     // sleep

#define MAX_BUFFER 16   // Tamaño máximo del buffer compartido
#define MAX_DATOS 32    // Cantidad de datos que se producirán/consumirán

// Recursos de sincronización globales
pthread_mutex_t mutex;        // Mutex para proteger el acceso al buffer
pthread_cond_t vacio, lleno;  // Condiciones para controlar el flujo de productor/consumidor

int buffer[MAX_BUFFER];  // Buffer circular compartido
int num_datos = 0;        // Número actual de datos en el buffer

// Función que ejecuta el hilo consumidor
void *consumidor()
{
    int i, dato, posicion = 0;

    // Consumir MAX_DATOS elementos
    for(i = 0; i < MAX_DATOS; i++)
    {
        pthread_mutex_lock(&mutex);  // Entra a la sección crítica

        // Espera mientras el buffer esté vacío
        while(num_datos == 0)
            pthread_cond_wait(&vacio, &mutex);  // Libera el mutex y espera señal

        // Extrae dato desde la posición actual
        dato = buffer[posicion];

        // Avanza circularmente en el buffer
        if(posicion == MAX_BUFFER - 1)
            posicion = 0;
        else
            posicion++;

        // Disminuye la cuenta de elementos en el buffer
        num_datos--;

        // Si antes el buffer estaba lleno, ahora hay espacio -> despierta productor
        if(num_datos == MAX_BUFFER - 1)
            pthread_cond_signal(&lleno);

        pthread_mutex_unlock(&mutex);  // Sale de la sección crítica

        // Muestra el dato consumido
        printf("\nse ha consumido el dato: %d", dato);
        fflush(stdout);
        sleep(1);  // Simula tiempo de procesamiento
    }

    pthread_exit(0);  // Termina el hilo consumidor
}

// Función que ejecuta el hilo productor
void *productor()
{
    int i, dato, posicion = 0;

    // Produce MAX_DATOS elementos
    for(i = 0; i < MAX_DATOS; i++)
    {
        pthread_mutex_lock(&mutex);  // Entra a la sección crítica

        // Espera mientras el buffer esté lleno
        while(num_datos == MAX_BUFFER)
            pthread_cond_wait(&lleno, &mutex);  // Libera el mutex y espera señal

        // Escribe el dato en la posición actual
        buffer[posicion] = i;
        dato = i;

        // Avanza circularmente
        if(posicion == MAX_BUFFER - 1)
            posicion = 0;
        else
            posicion++;

        // Aumenta la cuenta de elementos en el buffer
        num_datos++;

        // Si antes el buffer estaba vacío, ahora hay un dato -> despierta consumidor
        if(num_datos == 1)
            pthread_cond_signal(&vacio);

        pthread_mutex_unlock(&mutex);  // Sale de la sección crítica

        // Muestra el dato producido
        printf("\nse ha producido el dato: %d", dato);
        fflush(stdout);
        // sleep(1); // Si se quiere ralentizar el productor, descomentar
    }

    pthread_exit(0);  // Termina el hilo productor
}

int main()
{
    int error;
    char *valor_devuelto;

    // Identificadores para los hilos
    pthread_t idhilo1, idhilo2;

    // Inicializa el mutex y las variables de condición
    pthread_mutex_init(&mutex, NULL);    // Mutex sin atributos especiales
    pthread_cond_init(&lleno, NULL);     // Condición para buffer lleno
    pthread_cond_init(&vacio, NULL);     // Condición para buffer vacío

    // Crea hilo productor
    error = pthread_create(&idhilo1, NULL, productor, NULL);
    if (error != 0)
    {
        perror("No puedo crear hilo productor");
        exit(-1);
    }

    // Crea hilo consumidor
    error = pthread_create(&idhilo2, NULL, consumidor, NULL);
    if (error != 0)
    {
        perror("No puedo crear hilo consumidor");
        exit(-1);
    }

    // Espera a que ambos hilos terminen
    pthread_join(idhilo2, (void **)&valor_devuelto);
    pthread_join(idhilo1, (void **)&valor_devuelto);

    // Libera los recursos de sincronización
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&lleno);
    pthread_cond_destroy(&vacio);

    return 0;  // Fin del programa
}
