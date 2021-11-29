// Non-blocking port scanner
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
   extern int errno;
   int errnum;
   int sock;
   int status;
   int fd_max;
   int optval;
   int optlen;
   struct sockaddr_in server;
   char server_reply[2000];
   fd_set master, writefds, readfds;

   const int PORT_TOTAL = 4;
   int ports[PORT_TOTAL] = {1,8081,2,8080};
   int len = PORT_TOTAL;
   int fd_array[PORT_TOTAL];
   int ports_to_check = PORT_TOTAL;

   /*
    * For some reason SOCKSTREAM | O_NONBLOCK doesn't work
    * despite what the man page says (https://man7.org/linux/man-pages/man2/socket.2.html).
    * Hence, using fcntl() instead.
    */
   for (int i = 0; i < len; i++) {
      sock = socket(AF_INET, SOCK_STREAM, 0);
      if (sock == -1) {
        perror("socket error");
        return 1;
      }
      fd_array[i] = sock;
      status = fcntl(sock, F_SETFL, O_NONBLOCK);
      if (status < 0) {
         perror("error setting socket to nonblocking");
         return 1;
      }
      //printf("non-blocking socket created\n");

      server.sin_addr.s_addr = inet_addr("127.0.0.1");
      server.sin_family = AF_INET;
      server.sin_port = htons(ports[i]);

      if (connect (sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
         errnum = errno;
         /* check type of error - i.e. in progress and continue if that is the case */
         if (errnum == EINPROGRESS) {
            //printf("operation still in progress to port: %d. continuing...\n", ports[i]);
         } else {
            perror("connect failed\n");
            return 1;
         }
      }
     //printf("connected to port: %d\n", ports[i]);

      /* add to fd set */
      FD_SET(sock, &master);
      fd_max = sock;
   }

   int counter = 0;

   /* check  */
   for (;;) {
      // printf("iteration %d:\n", counter);
      // counter++;

      if (ports_to_check == 0) {
        printf("We are done checking all ports\n");
        return 0;
      }

      /* copy master set */
      writefds = master;
      if (select(fd_max+1, NULL, &writefds, NULL, NULL) == -1) {
         errnum = errno;
         perror("listen");
         fprintf(stderr, "errno: %s\n", strerror(errnum));
         exit(4);
      }

      for (int i = 0; i < len; i++) {
         if (FD_ISSET(fd_array[i], &writefds)) {
            /* check if socket is open or not */
            struct sockaddr_in peeraddr;
            socklen_t peeraddr_len = sizeof(peeraddr);
            if (getpeername(fd_array[i], (struct sockaddr *)&peeraddr, &peeraddr_len) < 0){
                errnum = errno;
                // if (errnum == EINVAL) {
                //    perror("connection failed error");
                // }
                printf("this port is closed: %d\n", ports[i]);
                FD_CLR(fd_array[i],&master);
                close(fd_array[i]);
                ports_to_check--;
           } else {
             printf("this port is open: %d\n", ports[i]);
             FD_CLR(fd_array[i],&master);
             close(fd_array[i]);
             ports_to_check--;
           }
         }
      }
   }

}
