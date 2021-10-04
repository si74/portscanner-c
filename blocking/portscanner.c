// Blocking port scanner
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

void client(int port) {
    int sockfd;
    int errnum;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
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
        errnum = errno;
        printf("error opening port %d\n", port);
        perror("connect");
        goto out;
    } else {
        printf("open port %d\n", port);
    }

out:
    close(sockfd);
}

// TODO reuse sockfd
// TODO add concurrency, ncat listen, while running portscanner
int main() {

    // port scanner
    // for each port try client connection
    // print ports with server-client connection
    int n = 65535;
    for(int i = 1; i <= n; i++) {

        client(i);
    }


    return 0;
}
