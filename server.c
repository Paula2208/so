#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

#define PORT 3550
#define BACKLOG 8

void manejador(int sig) {
    printf("\nSeñal SIGINT recibida, cerrando servidor.\n");
    exit(0);
}

int main()
{
    int serverfd;
    struct sockaddr_in server, client;
    int clientfd;
    socklen_t lenclient;
    char buffer[10];
    int r;
    int opt = 1;

    signal(SIGINT, manejador);

    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd == -1) {
        perror("error al crear socket del servidor");
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    bzero(server.sin_zero, 8);

    r = setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt));
    if (r == -1) {
        perror("error en setsockopt");
    }

    r = bind(serverfd, (struct sockaddr*)&server, sizeof(server));
    if (r == -1) {
        perror("error en bind");
    }

    r = listen(serverfd, BACKLOG);
    if (r == -1) {
        perror("error en listen");
    }

    lenclient = sizeof(client);
    clientfd = accept(serverfd, (struct sockaddr*)&client, &lenclient);
    if (clientfd == -1) {
        perror("error en accept");
    }

    r = send(clientfd, "hello world", 11, 0);
    if (r != 11) {
        perror("error al enviar");
    }

    r = recv(clientfd, buffer, sizeof(buffer) - 1, 0);
    if (r == -1) {
        perror("error al recibir");
    }
    printf("%s", buffer);
    close(serverfd);
}
