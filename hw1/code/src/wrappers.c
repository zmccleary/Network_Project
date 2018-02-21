#include "wrappers.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
void client_error(char *s){
	perror(s);
	exit(0);
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

int Read(int fd, rs_buf *buf, size_t buf_size,  ChatState_t state)
{
	int nread = 0;
	int total_read = 0;
	char *template;
    char * uoff_temp = "UOFF";
    char * from_temp = "FROM";

	int temp_len;
	int min = 0;

	switch(state){
		case LOGIN1:
			template = "U2EM"; //read should only be called for server response
			break;
		case LOGIN2:
			template = "MAI";
            break;
        case MOTD:
            template = "MOTD";
            break;
		case LIST_USER:
			template = "UTSIL"; //this may work better with a regex
			break;
		case MESSAGE_TO:
			template = "OT"; //this too
			break;
		case LOGOUT:
			template = "EYB";
			break;
		default:
			client_error("Not in a valid state!");
	}

	temp_len = strlen(template);
    char * totalbuf = (char *)calloc(buf->size, sizeof(char));
	while(strstr(totalbuf, "\r\n\r\n") == NULL)
	{
		if((nread = read(fd, buf->buffer, 1)) == -1)
		{
			if(errno == EINTR)
				continue;
			else
				client_error("Unable to read bytes from buffer.");
		}

		/*if(nread == 0) //EOF
			return total_read;*/

		/*At this point, we have some bytes in buf. 
		*Parse the buffer to make sure that it starts with template.
		*/
        if(total_read == (4 > temp_len ? 4 : temp_len) && state != MOTD)
        {
		    min =  4 < temp_len ? 4 : temp_len;

            //If it is not the correct template message, or a uoff message, or a from message, it is garbage
		    if(strncmp(totalbuf, template, min) != 0 && strncmp(totalbuf, uoff_temp, 4) != 0 
                    && strncmp(totalbuf, from_temp, 4) != 0)
                return -1;
        }

        strcat(totalbuf, buf->buffer);

		total_read += nread;
        
        /*If the count == 0, then nread = count: which means the whole buffer was filled. To fix, we realloc
         *the buffer and set count = buf_size again
         */ 
        if(total_read == buf->size){
            realloc_rsbuf(buf, buf->size*2);
            totalbuf = realloc(totalbuf, buf->size);
        }
	}
    strcpy(buf->buffer, totalbuf);
    free(totalbuf);
	return total_read;
}
