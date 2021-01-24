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

void client(int port) {
    int sockfd;
    int errnum;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0); 

    // make socket non-blocking
    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    // int socket(int domain, int type, int protocol);
    if(sockfd == -1) {
	errnum = errno;
        //printf("Socket init error: %d!!!", errnum);
        fprintf(stderr, "Error opening file: %s\n", strerror( errnum ));
        goto out;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(port);

    //int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
	printf("open port not found\n");
        goto out;
    } else {
        printf("open port %d\n", port);
    }

out:
    close(sockfd);
}

// TODO libevent
// TODO add concurrency, ncat listen, while running portscanner 

int main() {
   
    fd_set master;
    fd_set read_fds;
    int fdmax = 65534; // one less than largest fd bc index 0

    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    // port scanner
    // for each port try client connection
    // print ports with server-client connection
    int n = 65535;
    for(int i = 1; i <= n; i++) {
           int sockfd;
           int errnum;
           struct sockaddr_in servaddr;
       
           sockfd = socket(AF_INET, SOCK_STREAM, 0); 
       
           // make socket non-blocking
           fcntl(sockfd, F_SETFL, O_NONBLOCK);
       
           // int socket(int domain, int type, int protocol);
           if(sockfd == -1) {
       	       errnum = errno;
               //printf("Socket init error: %d!!!", errnum);
               fprintf(stderr, "Error opening file: %s\n", strerror( errnum ));
               goto out;
           }

           // add fd to master set
	   FD_SET(sockfd, &master);

           servaddr.sin_family = AF_INET;
           servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
           servaddr.sin_port = htons(port);
       
           //int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
           connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
       	   
    }


    return 0;
}
