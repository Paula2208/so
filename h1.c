#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid;

    // Crear un nuevo proceso
    pid = fork();

    if (pid < 0) {
        // Error al crear el proceso
        perror("Error al crear el proceso");
        return -1;
    } else if (pid == 0) {
        // Este bloque lo ejecuta el proceso hijo
        printf("¡Obi-Wan Me dijo lo suficiente! Dijo que tú le mataste\n");
    } else {
        // Este bloque lo ejecuta el proceso padre
        printf("No. Yo soy tu padre.\n");
    }

    return 0;
}

// gcc h1.c -o h1
// ./h1