#include <client.h>
#include <stdio.h>
#include <string.h>
#include <wrappers.h>
#include <stdlib.h>

const char* cli_usage = "./client [-hv] NAME SERVER_IP SERVER_PORT\n"
                    "-h                         Displays this help menu, and returns EXIT_SUCCESS.\n"
                    "-v                         Verbose print all incoming and outgoing protocol verbs & content.\n"
                    "NAME                       This is the username to display when chatting.\n"
                    "SERVER_IP                  The ip address of the server to connect to.\n"
                    "SERVER_PORT                The port to connect to.\n";


int handle_read(int sockfd, rs_buf * buf, ChatState_t state){
    
    int terminator_read = 0; //keep track of state when \r\n\r\n is read
    //Read from the socket into buf, check for garbage message`
    flush_rsbuf(buf);
    if (Read(sockfd, buf, buf->size, state) < 0){
        //Garbage termination function here
        
        return -1;
    }
    
    /*
     * Different cases here:
     * 1) Read returns the ACK message corresponding to state
     * 2) Read returns a FROM message from another client
     * 3) Read returns a UOFF from another client
     * 4) Read returns garbage
     *
     * We parse the input and then handle the options accordingly
     */
    char * token = strtok(buf->buffer, " ");
    int tok_len = 0;
    if(strcmp(token, "FROM")){

    }
    else if (strcmp(token, "UOFF")){
    
    }
    else{
        if(state == LOGIN1){
            //If the ack is not recieved, quit
            if(!strcmp(token, "U2EM\r\n\r\n"))
                return -1;
        }
        else if (state == LOGIN2){

        }
        else if (state == LIST_USER){
        	list_u(token, tok_len, terminator_read);
            //Handle list user
        }
        else if (state == MESSAGE_TO){
            //Handle message to
        }
        else if (state == LOGOUT){
            //Handle logoff
        }
        else{
            //Garbage termination function
        }

    }

    return 0;

     
}

char parse_args(int argc, char * const argv[], char * conn_info[]){
	if(argc == 1)
		return -1; //client can't be called with no args

	int arg_ind = 1;
	char opcode = 0;
	int info_ind = 0;

	if(!strcmp(argv[arg_ind], "-h"))
	{
		printf("%s", cli_usage);
		return 2; //return 2 to tell main function that program is printing usage info and exit 
	}

	if(!strcmp(argv[arg_ind], "-v"))
	{
			opcode = opcode | 0x1; //OR 1 to set verbose flag
			arg_ind++;
	}

    //Checks for the correct number of arguments
    if(argc-opcode != 4)
        return -1;

	while(arg_ind < argc){
		
        //Fill conn_info with NAME, SERVER_IP, SERVER_PORT 
		conn_info[info_ind++] = argv[arg_ind++];

	}
	
	return opcode;
}
int login(int sockfd, char *username, rs_buf * buf){
	
    char * loginmsg = "ME2U\r\n\r\n";

    //Write to the socket and wait for a read
    Write(sockfd, loginmsg, strlen(loginmsg));

    handle_read(sockfd, buf, LOGIN1);

    if(!strcmp(buf->buffer, "U2EM\r\n\r\n"))
        client_error("Login attempt failed.\n");

    loginmsg = "IAM ";
    strcat(loginmsg, username);
    strcat(loginmsg, "\r\n\r\n");

    Write(sockfd, loginmsg, strlen(loginmsg));

    handle_read(sockfd, buf, LOGIN2);
    
    if(strcmp(buf->buffer, "ETAKEN\r\n\r\n")){
    }

    







	return 0;

}

//Initialize an rs_buf struct
void init_rsbuf(rs_buf * buf, int bufsize){
    buf->buffer = (char *)calloc(bufsize, sizeof(char));
    buf->size = bufsize;
}

void realloc_rsbuf(rs_buf * buf, int bufsize){
    buf->buffer = (char *)realloc(buf->buffer, bufsize);
    buf->size += bufsize - buf->size;
}

void cleanup_rsbuf(rs_buf * buf){
    free(buf->buffer);
}

<<<<<<< HEAD
int list_u(char * token, int tok_len, int terminator_read){
	if(strcmp(token, "UTSIL"))
        		return -1; //first value of token read should be UTSIL or else garbage
        	while((token = strtok(NULL, " ")) != NULL){
        		tok_len = strlen(token);
        		if((tok_len > 10 && strcmp(token + ((tok_len) - 4), "\r\n\r\n")) ||
        		  	(!strcmp(token +((tok_len) - 4), "\r\n\r\n") && tok_len > 14))
        			return -1; //username cannot exceed 10 chars, treat as garbage unless terminator as suffix and >= 14 chars
        		else if (tok_len <= 10 && strcmp(token + (tok_len - 4), "\r\n\r\n"))
        		{
        			/*
        			 *If token is at most 10 chars and token is not terminated:
        			 * 		 
        			 */	



        		}
        		else if(tok_len <= 14 && !strcmp(token + (tok_len - 4), "\r\n\r\n")){
        			/*
        			 *If token is terminated and at most 14 chars long
        			 */

        			terminator_read = 1;
        		}

        	}

        	if(!terminator_read){
        		//flush read buffer and call read again
        	}

        	return 0;

}
=======
void flush_rsbuf(rs_buf * buf){
    memset(buf->buffer, 0, buf->size);
}

>>>>>>> c3fa81973a98f0c074476211664bf1bd2652fe40
