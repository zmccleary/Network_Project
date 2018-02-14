#include <wrappers.h>
#include <stdlib.h>
void client_error(char *s){
	perror(s);
	exit(EXIT_SUCCESS);
}

int Socket(int domain, int type, int protocol){
	int socketfd;
	if((socketfd = socket(domain, type, protocol)) == -1){
			/* Display an error*/
			client_error("Socket creation failed.");
	}	

	return socketfd;
}

int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout){
	int fds;
	if( (fds = select(nfds, readfds, writefds, exceptfds, timeout)) == -1){
		/* Display an error */
		client_error("Failure to select file descriptor.");
	}

	return fds;
}

int Write(int fd, const void *buf, int count){

	/* Check write for EINTR and restart. 
	 * also check return value of write().
	 */
	int n_written = 0;
	write_loop:
	while(count > 0){
		if((n_written = write(fd, const void *buf, int count)) == -1){
			if(errno == EINTR){
				/*write returns EINTR, restart the read*/
				goto write_loop; 
			}
			else{
				client_error("Write failure.");
			}
		}
		count += n_written;
		
	}
	return 0;
}