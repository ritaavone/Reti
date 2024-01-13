#ifndef WRAPPER_H
#define WRAPPER_H

int Socket(int family, int type, int protocol);
int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int Listen(int sockfd, int lunghezza_coda);
int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int Accept(int sockfd, struct sockaddr *clientaddr, socklen_t *addrlen);

#endif /* WRAPPER_H */

