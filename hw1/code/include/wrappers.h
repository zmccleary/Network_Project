#ifndef WRAPPERS
#define WRAPPERS

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <netdb.h>


int Socket(int domain, int type, int protocol);

int Select(int nfds, fd_set *readfds, fd_set *writefds,
		   fd_set *exceptfds, struct timeval *timeout);

void client_error(char *s);

int Write(int fd, const void * buf, int count);

<<<<<<< HEAD
int Read(int fd, void * buf, size_t count, ChatState_t state);
=======
int Read(int fd, void * buf, size_t count);
>>>>>>> 4370dc33498fad9e35048df92fb9d434939a9de5

int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

int Getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);
void exit_cleanup();

#endif
