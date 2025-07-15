#include <pthread.h>
#include <stdio.h>

#define NUM_HILOS 8

void *funcion(void *ap);

struct data {
    float a;
    char b;
    int id;
};

int main(){
    pthread_t hilo[NUM_HILOS];
    struct data datos[NUM_HILOS];
    int r;
    int *retval;

    // ¡OJO! No unir el primer y el segundo, porque al encolar los hilos en su creación el valor de i ya estaría erróneo
    for(int i = 0; i < NUM_HILOS; i++){
        // Cargar las estructuras. Una estructura por hilo
        datos[i].a = 3.14 + i; 
        datos[i].b = 0x30 + i; 
        datos[i].id = i; 
    }

    for(int i = 0; i < NUM_HILOS; i++){
        // Crear el Hilo
        r = pthread_create(
            &hilo[i], // Apuntador a donde se debe guardar el nuevo hilo creado
            NULL,
            (void *) funcion, // Función a ejecutar
            (void *) &datos[i] // Apuntadores de los argumentos que quiero pasar a la función a ejecutar
        );

        if(r != 0){
            perror("Error al crear el hilo");
            return -1;
        }

    }

    for(int i = 0; i < NUM_HILOS; i++){
        // Join

        r = pthread_join(
            hilo[i],
            (void **) &retval
        );

        if(r != 0){
            perror("Error en el join");
            return -1;
        }

    }

    return 0;
}

void *funcion(void *ap){
    struct data *datos; // Esto si sabe la estructura pero no la dirección
    datos = ap; // Aquí se le asigna la dirección, ya que sabe cómo manejar la estructura

    // Se usa la flechita porque lo que tengo es un apuntador y necesito un dato de la estructrura exacta
    printf("\n\n\nFloat a %f:", datos -> a);
    printf("\nChar b %c:", datos -> b); 
    printf("\nId Thread %i:", datos -> id);
}