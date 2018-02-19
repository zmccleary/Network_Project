#ifndef WRAPPERS
#define WRAPPERS

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>


int Socket(int domain, int type, int protocol);

int Select(int nfds, fd_set *readfds, fd_set *writefds,
		   fd_set *exceptfds, struct timeval *timeout);

void client_error(char *s);

int Write(int fd, const void * buf, int count);

#endif
