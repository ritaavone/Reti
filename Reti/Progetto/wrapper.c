//FILE .C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "wrapper.h"


int Socket(int family, int type, int protocol) {
    int s;
    if ((s = socket(family, type, protocol)) < 0) {
        perror("socket");
        exit(1);
    }
    return s;
}

int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    if (connect(sockfd, addr, addrlen) < 0) {
        perror("connect error");
        exit(1);
    }
    return 0;
}

int Listen(int sockfd, int lunghezza_coda) {
    if ((listen(sockfd, lunghezza_coda)) < 0) {
        perror("listen");
        exit(1);
    }
    return 0;
}

int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    if ((bind(sockfd, addr, addrlen)) < 0) {
        perror("bind");
        exit(1);
    }
    return 0;
}

int Accept(int sockfd, struct sockaddr *clientaddr, socklen_t *addrlen) {
    int a;
    if ((a = accept(sockfd, clientaddr, addrlen)) < 0) {
        perror("accept");
        exit(1);
    }
    return a;
}
