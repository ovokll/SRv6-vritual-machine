#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define PORT 12345
int main() {
   int sock = 0;
   struct sockaddr_in6 serv_addr;
   char *data[] ={"hellooo world"};
   if ((sock = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
       printf("\n Socket creation error \n");
       return -1;
   }
   
   memset(&serv_addr, '0', sizeof(serv_addr));
   serv_addr.sin6_family = AF_INET6;
   serv_addr.sin6_port = htons(PORT);
   
   if (inet_pton(AF_INET6, "2024:cd::2", &serv_addr.sin6_addr) <= 0) {
       printf("\nInvalid address/ Address not supported \n");
       return -1;
   }
   
   if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
       printf("\nConnection Failed \n");
       return -1;
   }
   
   int i = 1;
   for(; i <= 10; i++){
      send(sock , data[0] , strlen(data[0]) , 0 );
      printf("send: %s\n", data[0]);
      usleep(100000);//100ms
   }
  sleep(1);
  close(sock);
  return 0;
}
