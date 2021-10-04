// Non-blocking port scanner
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const int PORT = 8080;

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

   int ports[2] = {1,2};
   int len = 2;
   int fd_array[2];
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
      printf("non-blocking socket created\n");

      server.sin_addr.s_addr = inet_addr("127.0.0.1");
      server.sin_family = AF_INET;
      server.sin_port = htons(ports[i]);

      if (connect (sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
         errnum = errno;
         /* check type of error - i.e. in progress and continue if that is the case */
         if (errno == EINPROGRESS) {
            printf("operation still in progress to port: %d. continuing...\n", ports[i]);
         } else {
            perror("connect failed\n");
            return 1;
         }
      }
      printf("connected to port: %d\n", ports[i]);

      /* add to fd set */
      FD_SET(sock, &master);
      fd_max = sock;

   }

   int counter = 0;

   /* check  */
   for (;;) {
      counter++;
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
            /* check if socket has error */
            if (getsockopt(fd_array[i], SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
				   errnum = errno;
				   fprintf(stderr, "getsockopt error: %s\n", strerror( errnum ));
			   } else {
				   printf("getsockopt succeeded - port %d, fd = %d, optval = %d, optlen = %d\n", i, fd_array[i], optval, optlen);
               if (optval == EINPROGRESS) {
                  printf("it's in progress\n");
                  continue;
               }
               if (optval == 0) {
                  printf("port is open: %d\n", i);
               }
               // TODO: Figure out why optval sometimes isn't ECONNREFUSED
               if (optval == ECONNREFUSED) {
                  printf("connection refused. port is closed\n");
               }
               FD_CLR(fd_array[i],&master);
               close(fd_array[i]);
            }
         }

      }
   }


   close(fd_array[0]);
   close(fd_array[1]);
   return 0;

}
