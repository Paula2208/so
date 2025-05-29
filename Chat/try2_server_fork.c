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

    setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    bzero(server.sin_zero, 8);

    if (bind(serverfd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("❌ Error al hacer bind");
        exit(EXIT_FAILURE);
    }

    if (listen(serverfd, BACKLOG) == -1) {
        perror("❌ Error al poner en escucha");
        exit(EXIT_FAILURE);
    }

    printf("🚀 Servidor escuchando en el puerto %d...\n", PORT);

    while (1) {
        lenclient = sizeof(client);
        int new_client_fd = accept(serverfd, (struct sockaddr *)&client, &lenclient);
        if (new_client_fd == -1) {
            perror("❌ Error al aceptar conexión");
            continue;
        }

        printf("✅ Cliente conectado desde %s:%d\n",
               inet_ntoa(client.sin_addr), ntohs(client.sin_port));

        // Crear un proceso hijo para manejar al cliente
        if (fork() == 0) {
            close(serverfd); // El hijo no necesita el socket del servidor

            char *welcome = "👋 ¡Bienvenido al servidor!";
            send(new_client_fd, welcome, strlen(welcome) + 1, 0);

            while (1) {
                bzero(buffer, MAX_BUFFER_SIZE);

                // Recibir del cliente
                r = recv(new_client_fd, buffer, MAX_BUFFER_SIZE - 1, 0);
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

                // Leer del servidor (teclado del proceso hijo)
                printf("📤 Tú (a %s:%d): ", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
                fflush(stdout);
                bzero(buffer, MAX_BUFFER_SIZE);
                fgets(buffer, MAX_BUFFER_SIZE, stdin);
                buffer[strcspn(buffer, "\n")] = 0;

                r = send(new_client_fd, buffer, strlen(buffer) + 1, 0);
                if (r == -1) {
                    perror("❌ Error al enviar al cliente");
                    break;
                }

                if (strcmp(buffer, "exit") == 0) {
                    printf("👋 Cerrando conexión con el cliente.\n");
                    break;
                }
            }

            close(new_client_fd);
            exit(0); // Terminar proceso hijo
        } else {
            close(new_client_fd); // El padre no usa el nuevo socket
        }
    }

    close(serverfd);
    return 0;
}
