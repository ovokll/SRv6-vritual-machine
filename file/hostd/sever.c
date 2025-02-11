#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#define PORT 12345
int main() {
    int server_fd, new_socket;
    struct sockaddr_in6 server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[1024] = {0};
    
struct timespec ts1;
struct timespec ts2;

    if ((server_fd = socket(AF_INET6, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin6_family = AF_INET6;
    server_addr.sin6_port = htons(PORT);
    
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    
    if ((new_socket = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }
    
   while(1){
        char buff[1000] = {0};
        int n = recv(new_socket, buff, 1000, 0);
        if(n <= 0)
            break;
        printf("received: %s\n",buff);
   }
   close(new_socket);
   close(server_fd);
   return 0;
}
