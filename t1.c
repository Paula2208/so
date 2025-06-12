#include <pthread.h>
#include <stdio.h>

void *funcion(void *ap);

int main(){
    pthread_t hilo;
    float pi = 3.141592;
    int r;
    int *retval;

    // Crear el Hilo
    r = pthread_create(
        &hilo, // Apuntador a donde se debe guardar el nuevo hilo creado
        NULL,
        (void *) funcion, // Función a ejecutar
        (void *) &pi // Apuntadores de los argumentos que quiero pasar a la función a ejecutar
    );

    if(r != 0){
        perror("Error al crear el hilo");
        return -1;
    }

    // Join -> Es como el wait, hay que esperar a unir todos los hilos al final
    // IMPORTANTE: wait espera un cambio de estado, mientras que JOIN espera a que termine

    r = pthread_join(
        hilo,
        (void **) &retval
    );

    if(r != 0){
        perror("Error en el join");
        return -1;
    }

    return 0;
}

void *funcion(void *ap){
    printf("%f", *(float*) ap);
}