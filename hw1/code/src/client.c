#include <client.h>
#include <stdio.h>
#include <string.h>
#include <wrappers.h>
#include <stdlib.h>
#include <chat.h>

const char* cli_usage = "./client [-hv] NAME SERVER_IP SERVER_PORT\n"
                    "-h                         Displays this help menu, and returns EXIT_SUCCESS.\n"
                    "-v                         Verbose print all incoming and outgoing protocol verbs & content.\n"
                    "NAME                       This is the username to display when chatting.\n"
                    "SERVER_IP                  The ip address of the server to connect to.\n"
                    "SERVER_PORT                The port to connect to.\n";


int handle_read(int sockfd, rs_buf * buf, ChatState_t state){

    int terminator_read;
    //Read from the socket into buf, check for garbage message`
top:
    terminator_read = 0;
    flush_rsbuf(buf);
    if (Read(sockfd, buf, buf->size, state) < 0){
        //Garbage termination function here
        client_error("ERROR: unable to recognize message");
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
    char * bufcopy = (char *)calloc(buf->size, sizeof(char));
    strcpy(bufcopy, buf->buffer); 
    char * token = strtok(bufcopy, " ");
    int tok_len = 0;
    if(strcmp(token, "FROM") == 0){
        handle_from(sockfd, buf);
        if(state != DEFAULT)
            goto top;
    }
    else if (strcmp(token, "UOFF") == 0){
        //I dont think you need to do anything here until later
        char * usrname = strtok(NULL, "\r");
        send_uoff(usrname);
        flush_rsbuf(buf);
        if(state != DEFAULT)
            goto top;
    }
    else if (strcmp(token, "EDNE") == 0){
        printf("User does not exist.\n");
        flush_rsbuf(buf);
        free(bufcopy);
        return -1;
    }
    else{
        if(state == LOGIN1 || state == LOGIN2){
        }
        else if (state == LIST_USER){
        	list_u(token, tok_len, &terminator_read);
            //Handle list user
            if(!terminator_read){
            	flush_rsbuf(buf);
            }
        }
        else if (state == MESSAGE_TO){
            //Handle message to
            return 0;
        }
        else if (state == LOGOUT){
            //Handle logoff :: do nothing
        }
        else{
            //Garbage termination function
        }

    }
    
    free(bufcopy);

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

//Performs handshake and logs user into the server
int login(int sockfd, char *username, rs_buf * buf){
	
    char * loginmsg = (char *)calloc(100, sizeof(char));
    sprintf(loginmsg, "ME2U\r\n\r\n");

    //Write to the socket and wait for a read
    Write(sockfd, loginmsg, strlen(loginmsg));

    handle_read(sockfd, buf, LOGIN1);

    if(strcmp(buf->buffer, "U2EM\r\n\r\n")!= 0)
        goto bad;

    //Write name to the server and wait fr ack
    sprintf(loginmsg, "IAM %s\r\n\r\n", username);

    Write(sockfd, loginmsg, strlen(loginmsg));

    handle_read(sockfd, buf, LOGIN2);
    
    if(strcmp(buf->buffer, "ETAKEN\r\n\r\n")){

    }

    
    if(strcmp(buf->buffer, "ETAKEN\r\n\r\n") == 0)
        client_error("ERROR: Username taken\n");
    else if(strcmp(buf->buffer, "MAI\r\n\r\n") != 0)
        goto bad;


    //Try to read message of the day
    handle_read(sockfd, buf, MOTD);

    if(strncmp(buf->buffer, "MOTD", 4) == 0)
        printMOTD(buf);
    else
        goto bad;

    free(loginmsg);
    flush_rsbuf(buf);
	return 0;
bad:
    client_error("ERROR: Login attempt failed");
    free(loginmsg);
    return -1;
}

//Prints the MOTD from the server to client stdout
void printMOTD(rs_buf * buf){
    char del[] = " ";
    char * token = strtok(buf->buffer, del);
    token = strtok(NULL, "\r");
    printf("Message of the day: %s\n\nYou are now connected to the server "
            "(type '/help' for options): \n", token);
}

//Initialize an rs_buf struct
void init_rsbuf(rs_buf * buf, int bufsize){
    buf->buffer = (char *)calloc(bufsize, sizeof(char));
    buf->size = bufsize;
    buf->head = buf->buffer;
}

void realloc_rsbuf(rs_buf * buf, int bufsize){
    buf->head = buf->buffer + buf->size;
    buf->buffer = (char *)realloc(buf->buffer, bufsize);
    buf->size += bufsize - buf->size;
}

void cleanup_rsbuf(rs_buf * buf){
    free(buf->buffer);
}

void read_rsbuf(int fd, rs_buf * buf)
{
    //flush_rsbuf(buf);
    int size = buf->size - (buf->head - buf->buffer);
    if(read(fd, buf->head, size) == size)
    {
        realloc_rsbuf(buf, buf->size * 2);
        read_rsbuf(fd, buf);
    } 
    else 
        return;
}
int list_u(char * token, int tok_len, int * terminator_read){

	rs_buf out_buf;
	out_buf.buffer = calloc(BUFSIZE,sizeof(char));
	out_buf.size = BUFSIZE;
    int out_ind = 0;

    if(strcmp(token, "UTSIL"))
        		return -1; //first value of token read should be UTSIL or else garbage
        	while((token = strtok(NULL, " ")) != NULL){
        		
        		if(*terminator_read == 1)
        			return -1;

        		tok_len = strlen(token);
        		if((tok_len > 10 && strcmp(token + ((tok_len) - 4), "\r\n\r\n")) ||
        		  	(!strcmp(token +((tok_len) - 4), "\r\n\r\n") && tok_len > 14))
        			return -1; //username cannot exceed 10 chars, treat as garbage unless terminator as suffix and >= 14 chars
        		else if (tok_len <= 10 && strcmp(token + (tok_len - 4), "\r\n\r\n"))
        		{
        			/*
        			 *If token is at most 10 chars and token is not terminated:
        			 *add to out_buf+out_ind
        			 *increment out_ind
        			 */	
        			strcpy(out_buf.buffer + out_ind, token);
        			out_ind += tok_len;
        			strcpy(out_buf.buffer + (out_ind++) , "\t");
        		}
        		else if(tok_len <= 14 && !strcmp(token + (tok_len - 4), "\r\n\r\n")){
        			/*
        			 *If token is terminated and at most 14 chars long
        			 */
        			strncpy(out_buf.buffer + out_ind, token, (tok_len - 4));
        			out_ind += tok_len - 4;
        			*terminator_read = 1;
        		}

        	}
			token = strtok(out_buf.buffer, "\t");
        	while(token != NULL){
        		printf("%s ", token);
        		token = strtok(NULL, "\t");
        	}
        	if(terminator_read)
        		printf("\n");
            free(out_buf.buffer);
        	return 0;

}

void flush_rsbuf(rs_buf * buf){
    memset(buf->buffer, 0, buf->size);
    buf->head = 0;
}

//Adds a window to the windows buffer
void add_window(char * name, char * message){
    //create the pipes for IPC
    int pfd1[2];
    int pfd2[2];
    if (pipe(pfd1) < 0 || pipe(pfd2) < 0)
        client_error("Pipe failed");


    //Add a new window to windows using the name and file descriptors
    window * wp = malloc(sizeof(window));
    strcpy(wp->name, name);
    wp->writepipe = pfd1[1];
    wp->readpipe = pfd2[0];
    awl(wp);

    //Now that the window is set, fork and set the child to spawn a terminal and 
    //multiplex over the input
    pid_t pid;
    if ((pid =fork()) < 0)
        client_error("Fork error");
    else if (pid == 0)  //Child
    {
        //close unused file descriptors for the pipes
        close(pfd1[1]);
        close(pfd2[0]);

        //Dup the used pipes to be file descriptors 3 and 4
        if(pfd1[0] != 0){
            if (dup2(pfd1[0], 3) != 3)
                client_error("Failed to dup stdin");
        }
        if(pfd2[1] != 1){
            if(dup2(pfd2[1], 4) != 4)
                client_error("Failed to dup stdout");
        }

        if(execl("/usr/bin/xterm", "/usr/bin/xterm","-T", name, "-n", name, "-hold", 
                    "-e", "./chatwindow", name, message,  NULL) < 0)
            client_error("Error executing chatwindow");
    } 
    else    //Parent
    {
        close(pfd1[0]);
        close(pfd2[1]);
        char * msg = (char *)calloc(100, sizeof(char));
        /*sprintf(msg, "You are now chatting with %s:\n\n", name);
        if(write(wp->writepipe, msg, strlen(msg)) < 0)
            client_error("Failed to write");*/
        free(msg);
    }


}

//Terminates the window which UOFF user is connected to
void remove_window(window * curr)
{
    
    /*for(curr = tail; curr != NULL; curr = curr->prev);
    {
        if(strcmp(name, curr->name) == 0)
        {*/
            close(curr->writepipe);
            close(curr->readpipe);
            rwl(curr);
        /*}
    }*/
}

//Adds window to the list
void awl(window * wp)
{
    wp->prev = tail;
    wp->next = NULL;
    tail = wp;
}

//removes window from the list
void rwl(window * wp)
{
    if(wp->prev != NULL)
        wp->prev->next = wp->next;
    if(wp->next != NULL)
        wp->next->prev = wp->prev;
    if(tail == wp)
        tail = NULL;
    free(wp);
}




