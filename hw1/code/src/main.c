#include <stdlib.h>
#include <wrappers.h>
#include <client.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFSIZE 1024
char **conn_info;

void exit_cleanup()
{
	free(conn_info);
}


int main(int argc, char **argv)
{
	conn_info = calloc(3, sizeof(char *));
	int sockfd;
	atexit(exit_cleanup);
	char verbose = 0;

    char buf[BUFSIZE];
    
    
    //Read in command line arguments
	if((verbose = parse_args(argc, argv, conn_info)) == -1)
	{
		return EXIT_FAILURE;
	}

	if(verbose == 2){
		return EXIT_SUCCESS;
	}
	char * cli_name = *conn_info;
    char * hostname = *(conn_info+1);
    

    //Get address info using host and port name
    struct addrinfo *info, *curadd;
    Getaddrinfo(hostname, *(conn_info+2), NULL, &info);
    
    //Check returned addresses, try each until a successful connection occurs
    for(curadd = info; curadd != NULL; curadd = curadd->ai_next)
    {
        sockfd = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
        if(sockfd == -1)
            continue;

        if(connect(sockfd, curadd->ai_addr, curadd->ai_addrlen) != -1)
            break;

        close(sockfd);
    }

    

    //If curadd is NULL after this loop, no address found. Send error message
    if(curadd == NULL)
        client_error("Could not connect to host %s on port %d\n");
	
	login(sockfd, cli_name, buf);
    freeaddrinfo(info);

    
}

