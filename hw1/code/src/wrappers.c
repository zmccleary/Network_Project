#include "wrappers.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
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
		if((n_written = write(fd, buf, count)) == -1){
			if(errno == EINTR){
				/*write returns EINTR, restart the read*/
				goto write_loop; 
			}
			else{
				client_error("Write failure.");
			}
		}
		count -= n_written;
		
	}
	return 0;
}

int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen){
	int result = 0;
	force_connect:
		if((result = connect(sockfd, addr, addrlen)) == -1){
			if(errno == EINTR)
				goto force_connect;
			client_error("Failure to connect to server.");
		}

		return result;
}

int Getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res)
{
	int result = 0;

	if((result = getaddrinfo(node, service, hints, res)) == -1){
		client_error("ERROR: getaddrinfo\n");
	}

	return result;
}

int Read(int fd, void *buf, size_t count, ChatState_t state)
{
	int nread = 0;
	int total_read = 0;
	char *template;

	int temp_len;
	int buf_len;
	int min = 0;

	switch(state){
		case LOGIN:
			template = "U2EM\r\n\r\n"; //read should only be called for server response
			break;
		case MAI:
			template = "MAI\r\n\r\n";
		case LIST_USER:
			template = "UTSIL *. \r\n\r\n"; //this may work better with a regex
			break;
		case MESSAGE_TO:
			template = "OT {1-10}/d \r\n\r\n"; //this too
			break;
		case LOGOUT:
			template = "EYB \r\n\r\n";
			break;
		default:
			client_error("Not in a valid state!");
	}

	temp_len = strlen(template);

	while(count > 0)
	{
		if((nread = read(fd, buf, count)) == -1)
		{
			if(errno == EINTR)
				continue;
			else
				client_error("Unable to read bytes from buffer.");
		}

		if(nread == 0) //EOF
			return total_read;

		/*At this point, we have some bytes in buf. 
		*Parse the buffer to make sure that it starts with template.
		*/
		buf_len = strlen(buf);

		min = buf_len < temp_len ? buf_len : temp_len;

		if(!strncmp(buf, template, min)){
			return -1; //buffer is not prefixed in valid way, return error
		}

		count -= nread;
		total_read += nread;
	}

	return total_read;
}
