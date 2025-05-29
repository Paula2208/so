#include <stdio.h>    
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define PORT 3550

int main()
{
    struct sockaddr_in server;
    char buffer[12];
    int fd;
    int r;
   
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("error al crear el socket");
        exit(1);
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    bzero( server.sin_zero, 8);
   
    r = connect(fd, (struct sockaddr*)&server, sizeof(server));
    if (r != 0) {
        perror("error al conectar");
    }
   
    r = recv(fd, buffer, 11, 0);
    if (r == -1) {
        perror("error al recibir");
    }
   
    buffer[r] = 0;
    printf("%s", buffer);
   
    r = send(fd, "hola server", 11,0);
    close(fd);
}
