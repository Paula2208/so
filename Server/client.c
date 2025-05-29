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
#define MAX_BUFFER_SIZE 64

int clientfd = -1; // Descriptor del socket global para cerrar desde el manejador

// --------------------- Manejador de SIGINT (Ctrl+C) ---------------------
void manejador(int sig) {
    printf("\n🔴 Señal SIGINT recibida. Cerrando conexión del cliente...\n");

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
    struct sockaddr_in server; // Estructura para dirección del servidor
    int r;
    char buffer[MAX_BUFFER_SIZE];

    // Registrar manejador de señales
    if (signal(SIGINT, manejador) == SIG_ERR) {
        perror("❌ Error registrando manejador de señales");
        exit(EXIT_FAILURE);
    }

    // --------------------- socket() ---------------------
    // int socket(int domain, int type, int protocol)
    // AF_INET: IPv4
    // SOCK_STREAM: TCP
    // 0: protocolo por defecto
    clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1) {
        perror("❌ Error creando el socket del cliente");
        exit(EXIT_FAILURE);
    }
    printf("🟢 Socket del cliente creado correctamente.\n");

    // --------------------- Configuración de la estructura 'server' ---------------------
    server.sin_family = AF_INET;               // Dirección IPv4
    server.sin_port = htons(PORT);             // Puerto en orden de red
    server.sin_addr.s_addr = inet_addr(HOST);  // Dirección IP del servidor
    bzero(server.sin_zero, 8);                 // Relleno para compatibilidad

    // --------------------- connect() ---------------------
    // int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
    // sockfd: descriptor del socket del cliente
    // addr: dirección del servidor (cast a struct sockaddr)
    // addrlen: tamaño de la estructura
    r = connect(clientfd, (struct sockaddr *)&server, sizeof(server));
    if (r == -1) {
        perror("❌ Error al conectarse al servidor");
        close(clientfd);
        exit(EXIT_FAILURE);
    }
    printf("✅ Conectado al servidor %s:%d\n", HOST, PORT);

    // --------------------- recv() ---------------------
    // ssize_t recv(int sockfd, void *buf, size_t len, int flags)
    // sockfd: socket desde el cual recibir
    // buf: buffer donde guardar los datos
    // len: tamaño máximo del buffer
    // flags: 0 para operación normal
    r = recv(clientfd, buffer, MAX_BUFFER_SIZE - 1, 0);
    if (r == -1) {
        perror("❌ Error al recibir datos del servidor");
        close(clientfd);
        exit(EXIT_FAILURE);
    }

    buffer[r] = '\0'; // Asegurar terminación de cadena
    printf("📥 Mensaje recibido del servidor: %s\n", buffer);

    // --------------------- send() ---------------------
    // ssize_t send(int sockfd, const void *buf, size_t len, int flags)
    // sockfd: descriptor del socket
    // buf: datos a enviar
    // len: tamaño de los datos (strlen si es string)
    // flags: 0 para operación normal
    const char *msg = "Hey! Nice to meet you server :3";
    r = send(clientfd, msg, strlen(msg) + 1, 0); // +1 para enviar '\0' también
    if (r == -1) {
        perror("❌ Error al enviar datos al servidor");
        close(clientfd);
        exit(EXIT_FAILURE);
    }
    printf("📤 Mensaje enviado al servidor.\n");

    // --------------------- close() ---------------------
    close(clientfd);
    printf("🛑 Conexión cerrada. Cliente finalizado.\n");

    return 0;
}
