#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main() {
    pid_t pid;
    int suma_padre = 0, suma_hijo = 0, suma_total = 0;
    FILE *archivo;

    pid = fork();

    if (pid == -1) {
        perror("Error al crear el proceso");
        return -1;
    } else if (pid == 0) {
        // Hijo
        for (int i = 0; i <= 499; i++) {
            suma_hijo += i;
        }

        archivo = fopen("resultado_hijo.txt", "w");
        if (archivo == NULL) {
            perror("Error al abrir el archivo para escribir");
            return -1;
        }
        fprintf(archivo, "%d", suma_hijo);
        fclose(archivo);

        printf("Hijo: suma de 0 a 499 = %d\n", suma_hijo);
    } else {
        // Padre
        for (int i = 500; i <= 999; i++) {
            suma_padre += i;
        }

        // Esperar a que el hijo termine
        wait(NULL);

        archivo = fopen("resultado_hijo.txt", "r");
        if (archivo == NULL) {
            perror("Error al abrir el archivo para leer");
            return -1;
        }
        fscanf(archivo, "%d", &suma_hijo);
        fclose(archivo);

        suma_total = suma_padre + suma_hijo;

        printf("Padre: suma de 500 a 999 = %d\n", suma_padre);
        printf("Padre: suma total de 0 a 999 = %d\n", suma_total);
    }

    return 0;
}

// gcc h2.c -o h2
// ./h2