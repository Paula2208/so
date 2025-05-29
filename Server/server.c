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
#define MAX_BUFFER_SIZE 11

int serverfd = -1;  // Descriptor del socket del servidor
int clientfd = -1;  // Descriptor del socket del cliente

// Manejador de la señal SIGINT (Ctrl+C)
void manejador(int sig) {
    printf("\n🔴 Señal SIGINT recibida, cerrando el servidor correctamente...\n");

    if (clientfd != -1) {
        close(clientfd); // Cierra la conexión con el cliente
        printf("🔌 Cliente desconectado.\n");
    }
    else{
        perror("❌ Error desconectando cliente");
        exit(-1);
    }

    if (serverfd != -1) {
        close(serverfd); // Cierra el socket del servidor
        printf("🔌 Socket del servidor cerrado.\n");
    }
    else{
        perror("❌ Error desconectando servidor");
        exit(-1);
    }

    exit(0);
}

int main() {
    struct sockaddr_in server, client; // Estructuras para dirección del servidor y cliente
    socklen_t lenclient;
    char buffer[MAX_BUFFER_SIZE];
    int opt = 1;
    int r;

    // Registrar el manejador para la señal SIGINT (Ctrl+C)
    if (signal(SIGINT, manejador) == SIG_ERR) {
        perror("❌ Error registrando el manejador de señales");
        exit(EXIT_FAILURE);
    }

    // --------------------- socket() ---------------------
    // int socket(int domain, int type, int protocol)
    // domain: AF_INET (IPv4)
    // type: SOCK_STREAM (TCP)
    // protocol: 0 (usar protocolo predeterminado según domain y type)
    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd == -1) {
        perror("❌ Error creando el socket");
        exit(EXIT_FAILURE);
    }
    printf("🟢 Socket del servidor creado exitosamente.\n");

    // --------------------- setsockopt() ---------------------
    // int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
    // sockfd: descriptor del socket
    // level: SOL_SOCKET (nivel del socket)
    // optname: SO_REUSEADDR (permite reutilizar el puerto)
    // optval: puntero al valor de la opción (1 para habilitar)
    // optlen: tamaño del valor (sizeof(opt))
    r = setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (r == -1) {
        perror("❌ Error en setsockopt");
        close(serverfd);
        exit(EXIT_FAILURE);
    }

    // --------------------- bind() ---------------------
    // int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
    // sockfd: descriptor del socket
    // addr: puntero a estructura sockaddr (cast de sockaddr_in)
    // addrlen: tamaño de la estructura
    server.sin_family = AF_INET;              // Dirección IPv4
    server.sin_port = htons(PORT);            // Puerto en orden de red
    server.sin_addr.s_addr = INADDR_ANY;      // Acepta conexiones desde cualquier IP
    bzero(server.sin_zero, 8);                // Relleno para compatibilidad

    r = bind(serverfd, (struct sockaddr *)&server, sizeof(server));
    if (r == -1) {
        perror("❌ Error al hacer bind");
        close(serverfd);
        exit(EXIT_FAILURE);
    }
    printf("🟢 Dirección asociada al socket.\n");

    // --------------------- listen() ---------------------
    // int listen(int sockfd, int backlog)
    // sockfd: descriptor del socket
    // backlog: número máximo de conexiones pendientes en cola
    r = listen(serverfd, BACKLOG);
    if (r == -1) {
        perror("❌ Error al poner en escucha");
        close(serverfd);
        exit(EXIT_FAILURE);
    }
    printf("🟢 Servidor escuchando en el puerto %d...\n", PORT);

    // --------------------- accept() ---------------------
    // int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
    // sockfd: descriptor del socket del servidor
    // addr: puntero donde se guardará la dirección del cliente
    // addrlen: puntero al tamaño de la estructura de cliente
    lenclient = sizeof(client);
    clientfd = accept(serverfd, (struct sockaddr *)&client, &lenclient);
    if (clientfd == -1) {
        perror("❌ Error al aceptar conexión");
        close(serverfd);
        exit(EXIT_FAILURE);
    }

    printf("✅ Cliente conectado desde %s:%d\n",
           inet_ntoa(client.sin_addr), ntohs(client.sin_port));

    // --------------------- send() ---------------------
    // ssize_t send(int sockfd, const void *buf, size_t len, int flags)
    // sockfd: descriptor del socket del cliente
    // buf: datos a enviar
    // len: longitud del mensaje (incluyendo '\0' si es string)
    // flags: 0 (sin opciones adicionales)
    const char *msg = "¡Hey! I'm your server";
    r = send(clientfd, msg, strlen(msg) + 1, 0);
    if (r == -1) {
        perror("❌ Error al enviar mensaje");
        close(clientfd);
        close(serverfd);
        exit(EXIT_FAILURE);
    }
    printf("📤 Mensaje enviado al cliente.\n");

    // --------------------- recv() ---------------------
    // ssize_t recv(int sockfd, void *buf, size_t len, int flags)
    // sockfd: descriptor del socket del cliente
    // buf: buffer donde se guardan los datos recibidos
    // len: tamaño máximo del buffer
    // flags: 0 (sin opciones)
    r = recv(clientfd, buffer, MAX_BUFFER_SIZE - 1, 0);
    if (r == -1) {
        perror("❌ Error al recibir datos");
    } else {
        buffer[r] = '\0'; // Agregar fin de cadena
        printf("📥 Mensaje recibido del cliente: %s\n", buffer);
    }

    // --------------------- close() ---------------------
    // Cierra ambos sockets
    close(clientfd);
    close(serverfd);
    printf("🛑 Servidor finalizado correctamente.\n");

    return 0;
}
