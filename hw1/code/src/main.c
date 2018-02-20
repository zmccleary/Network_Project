#include <stdlib.h>
#include <wrappers.h>
#include <client.h>
#include <chat.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

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

    rs_buf * buf = (rs_buf *)malloc(sizeof(rs_buf));
    init_rsbuf(buf, BUFSIZE);
    
    
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
    int portno = atoi(*(conn_info+2));

    if(strlen(cli_name) > 10)
    	client_error("User name should be maximum 10 characters.");
    

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
    {
        printf("Could not connect to host %s on port %d\n", hostname, portno);
        exit(0);
    }
    printf("Succesful connection! Login is being attempted...\n");
	
    freeaddrinfo(info);

	login(sockfd, cli_name, buf);


    fd_set read, write;
    struct timeval timeout;
    
    
    
    while(1){
   	    //printf("The program gets up to here.\n");
		timeout.tv_sec = 30; //timeout 30 seconds
		FD_ZERO(&read);
 	    FD_ZERO(&write);

    	FD_SET(0,&read); //check stdin for user input
    	FD_SET(sockfd, &read); //check server for incoming data
    	FD_SET(sockfd, &write); //check for data to send to server
	
    	Select(sockfd + 1, &read, &write, NULL, &timeout);

    	if(FD_ISSET(0,&read)){
    		//something to be read from stdin
    		fgets(buf->buffer, BUFSIZE, stdin);
            if(strncmp(buf->buffer, "/help", 5) == 0)
                printhelp();
            else if(strncmp(buf->buffer, "/logout", 7) == 0)
                logout(sockfd, buf);
            else if(strncmp(buf->buffer, "/listu", 6) == 0)
                listusers(sockfd, buf);
            else if(strncmp(buf->buffer, "/chat", 5) == 0){
                //get to and msg, then call:
                char * to = strtok(buf->buffer, " ");
                to = strtok(NULL, " ");
                char * msg = strtok(NULL, "\n");
                chat(sockfd, buf, to, msg);
            }
            else
            {
                printf("Message not recognized\n");
            }
            
            flush_rsbuf(buf);
        }

    	if(FD_ISSET(sockfd, &read)){
    		//something to be read from server socket
    	    handle_read(sockfd, buf, DEFAULT);	
    	}

    	if(FD_ISSET(sockfd,&write)){
    		//something to be written to sockfd
    		Write(sockfd, buf->buffer, strlen(buf->buffer));
    		flush_rsbuf(buf);
    	}
    }

    return 0;
   
}

