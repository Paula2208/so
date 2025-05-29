#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 3550
#define BACKLOG 8
#define MAX_BUFFER_SIZE 1024

int serverfd = -1;
int clientfd = -1;

// Manejador de Ctrl+C
void manejador(int sig) {
    printf("\n🔴 Señal SIGINT recibida. Cerrando servidor...\n");

    if (clientfd != -1) {
        close(clientfd);
        printf("🔌 Socket del cliente cerrado.\n");
    }
    else{
        perror("❌ Error desconectando cliente");
        exit(-1);
    }

    if (serverfd != -1) {
        close(serverfd);
        printf("🛑 Socket del servidor cerrado.\n");
    }
    else{
        perror("❌ Error desconectando servidor");
        exit(-1);
    }

    exit(0);
}

int main() {
    struct sockaddr_in server, client;
    socklen_t lenclient;
    char buffer[MAX_BUFFER_SIZE];
    int r;
    int opt = 1;

    signal(SIGINT, manejador);

    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd == -1) {
        perror("❌ Error creando socket del servidor");
        exit(EXIT_FAILURE);
    }

    // Permitir reutilizar puerto
    setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    bzero(server.sin_zero, 8);

    r = bind(serverfd, (struct sockaddr *)&server, sizeof(server));
    if (r == -1) {
        perror("❌ Error al hacer bind");
        exit(EXIT_FAILURE);
    }

    r = listen(serverfd, BACKLOG);
    if (r == -1) {
        perror("❌ Error al poner en escucha");
        exit(EXIT_FAILURE);
    }

    printf("🚀 Servidor escuchando en el puerto %d...\n", PORT);

    lenclient = sizeof(client);
    clientfd = accept(serverfd, (struct sockaddr *)&client, &lenclient);
    if (clientfd == -1) {
        perror("❌ Error al aceptar conexión");
        exit(EXIT_FAILURE);
    }

    printf("✅ Cliente conectado desde %s:%d\n",
           inet_ntoa(client.sin_addr), ntohs(client.sin_port));

    // Enviar mensaje de bienvenida
    char *welcome = "👋 ¡Bienvenido al servidor!";
    send(clientfd, welcome, strlen(welcome) + 1, 0);

    // Bucle de chat
    while (1) {
        bzero(buffer, MAX_BUFFER_SIZE);

        // Recibir del cliente
        r = recv(clientfd, buffer, MAX_BUFFER_SIZE - 1, 0);
        if (r <= 0) {
            printf("❌ Cliente desconectado o error.\n");
            break;
        }

        buffer[r] = '\0';
        if (strcmp(buffer, "exit") == 0) {
            printf("👋 Cliente cerró la conexión.\n");
            break;
        }

        printf("📥 Cliente: %s\n", buffer);

        // Leer del servidor (teclado)
        printf("📤 Tú: ");
        fflush(stdout);
        bzero(buffer, MAX_BUFFER_SIZE);
        fgets(buffer, MAX_BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0; // Quitar salto de línea

        r = send(clientfd, buffer, strlen(buffer) + 1, 0);
        if (r == -1) {
            perror("❌ Error al enviar al cliente");
            break;
        }

        if (strcmp(buffer, "exit") == 0) {
            printf("👋 Cerrando conexión con el cliente.\n");
            break;
        }
    }

    close(clientfd);
    close(serverfd);
    return 0;
}
