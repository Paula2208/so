#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 3550
#define HOST "25.21.155.51"// #define HOST "127.0.0.1"
#define MAX_BUFFER_SIZE 1024

int clientfd = -1;

void manejador(int sig) {
    printf("\n🔴 Señal SIGINT recibida. Cerrando cliente...\n");

    if (clientfd != -1) {
        close(clientfd);
        printf("🔌 Socket del cliente cerrado.\n");
    }
    else{
        perror("❌ Error desconectando cliente");
        exit(-1);
    }

    exit(0);
}

int main() {
    struct sockaddr_in server;
    char buffer[MAX_BUFFER_SIZE];
    int r;

    signal(SIGINT, manejador);

    clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1) {
        perror("❌ Error creando socket");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(HOST);
    bzero(server.sin_zero, 8);

    r = connect(clientfd, (struct sockaddr *)&server, sizeof(server));
    if (r == -1) {
        perror("❌ Error conectando al servidor");
        exit(EXIT_FAILURE);
    }

    printf("✅ Conectado al servidor en %s:%d\n", HOST, PORT);

    // Recibir mensaje de bienvenida
    r = recv(clientfd, buffer, MAX_BUFFER_SIZE - 1, 0);
    if (r > 0) {
        buffer[r] = '\0';
        printf("📥 Servidor: %s\n", buffer);
    }

    // Bucle de chat
    while (1) {
        // Leer del cliente (teclado)
        printf("📤 Tú: ");
        fflush(stdout);
        bzero(buffer, MAX_BUFFER_SIZE);
        fgets(buffer, MAX_BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0; // Quitar salto de línea

        r = send(clientfd, buffer, strlen(buffer) + 1, 0);
        if (r == -1) {
            perror("❌ Error al enviar al servidor");
            break;
        }

        if (strcmp(buffer, "exit") == 0) {
            printf("👋 Cerrando conexión con el servidor.\n");
            break;
        }

        // Recibir respuesta del servidor
        bzero(buffer, MAX_BUFFER_SIZE);
        r = recv(clientfd, buffer, MAX_BUFFER_SIZE - 1, 0);
        if (r <= 0) {
            printf("❌ Servidor desconectado o error.\n");
            break;
        }

        buffer[r] = '\0';
        if (strcmp(buffer, "exit") == 0) {
            printf("👋 El servidor cerró la conexión.\n");
            break;
        }

        printf("📥 Servidor: %s\n", buffer);
    }

    close(clientfd);
    return 0;
}
