#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>

// TODO libevent
// TODO add concurrency, ncat listen, while running portscanner

bool FD_IS_ANY_SET(fd_set const *fdset)
{
    static fd_set empty;     // initialized to 0 -> empty
    return memcmp(fdset, &empty, sizeof(fd_set)) != 0;
}

int main() {

    fd_set master;
    fd_set read_fds;
    int fdmax = 81; // one less than largest fd bc index 0

    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    // port scanner
    // for each port try client connection
    // print ports with server-client connection
    int fd_array[1];
    for(int i = 81; i <= fdmax; i++) {
           int sockfd;
           int errnum;
           struct sockaddr_in servaddr;

           sockfd = socket(AF_INET, SOCK_STREAM, 0);
	   printf("trying to connect - port: %d, sockfd: %d\n", i, sockfd);
           fd_array[0] = sockfd;
           // make socket non-blocking
           fcntl(sockfd, F_SETFL, O_NONBLOCK);

           // int socket(int domain, int type, int protocol);
           if(sockfd == -1) {
       	       errnum = errno;
               //printf("Socket init error: %d!!!", errnum);
               fprintf(stderr, "Error opening file: %s\n", strerror( errnum ));
               //goto out;
           }

           // add fd to master set
	   FD_SET(sockfd, &master);

           servaddr.sin_family = AF_INET;
           servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
           servaddr.sin_port = htons(i);

           //int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
           connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    }


    for (;;) {
        // copy master
        read_fds = master;

        if (FD_IS_ANY_SET(&master) == false) {
                break;
        }

        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
	    perror("select");
	    exit(4);
	}

        for(int i = 81; i <= fdmax; i++) {
                int optval;
                socklen_t optlen;
                int errnum;

		//printf("iterating fd = %d\n", fd_array[i]);
                printf("trying to check - port %d, fd %d\n", i, fd_array[0]);
        	if (FD_ISSET(fd_array[0] , &read_fds)) {
                        //printf("fd = %d\n", fd_array[i]);
                        printf("isset - port %d, fd %d\n", i, fd_array[0]);
			if (getsockopt(fd_array[0], SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {

			  	// remove i from master
				//if(errno == )
				errnum = errno;
				fprintf(stderr, "getsockopt error: %s\n", strerror( errnum ));

			} else {
				printf("getsockopt succeeded - fd = %d, optval = %d, optlen = %d\n", 81, optval, optlen);
                                if (optval == EINPROGRESS) {
                                        continue;
                                }
                                if (optval == 0) {
                                        printf("port is open: %d", i);
                                }
                                // TODO: Figure out why optval sometimes isn't ECONNREFUSED
                                if (optval == ECONNREFUSED) {
                                        printf("connection refused. port is closed");
                                        close(fd_array[0]);
                                }
                                FD_CLR(fd_array[0],&master);

			}

		}
        }
    }
    return 0;
}
