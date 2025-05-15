#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int main() {
    int fd[2]; // fd[0] para leer, fd[1] para escribir
    pid_t pid;
    int suma_hijo = 0, suma_padre = 0, suma_total = 0;

    // Crear el pipe
    if (pipe(fd) == -1) {
        perror("Error al crear el pipe");
        return 1;
    }

    pid = fork();

    if (pid < 0) {
        perror("Error al crear el proceso");
        return 1;
    } else if (pid == 0) {
        // Proceso hijo
        close(fd[0]); // Cerrar la lecturs

        for (int i = 0; i <= 499; i++) {
            suma_hijo += i;
        }

        if (write(fd[1], &suma_hijo, sizeof(int)) == -1) {
            perror("Error al escribir en el pipe");
            close(fd[1]);
            exit(1);
        }

        printf("Hijo: suma de 0 a 499 = %d\n", suma_hijo);

        // Cerrar escritura
        close(fd[1]);
    } else {
        // Proceso padre
        close(fd[1]); // Cerrar escritura

        for (int i = 500; i <= 999; i++) {
            suma_padre += i;
        }

        // Espera a que el hijo termine
        wait(NULL);

        if (read(fd[0], &suma_hijo, sizeof(int)) == -1) {
            perror("Error al leer del pipe");
            close(fd[0]);
            return 1;
        }

        close(fd[0]);

        suma_total = suma_padre + suma_hijo;

        printf("Padre: suma de 500 a 999 = %d\n", suma_padre);
        printf("Padre: suma total de 0 a 999 = %d\n", suma_total);
    }
    
    exit(0);
    return 0;
}

// gcc h3.c -o h3
// ./h3