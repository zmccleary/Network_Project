#include <client.h>
#include <stdio.h>
#include <string.h>
#include <wrappers.h>

const char* cli_usage = "./client [-hv] NAME SERVER_IP SERVER_PORT\n"
                    "-h                         Displays this help menu, and returns EXIT_SUCCESS.\n"
                    "-v                         Verbose print all incoming and outgoing protocol verbs & content.\n"
                    "NAME                       This is the username to display when chatting.\n"
                    "SERVER_IP                  The ip address of the server to connect to.\n"
                    "SERVER_PORT                The port to connect to.\n";


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
int login(int sockfd, char *username, char * buf){
	
	fd_set read, write;
	struct timeval time;

	//int fds = 0;
	FD_ZERO(&read);
	FD_ZERO(&write);
	FD_SET(sockfd, &read);
	FD_SET(sockfd, &write);
	time.tv_sec = 45;
	//fds = 

	Select(sockfd, &read, &write, NULL, &time);

	if(FD_ISSET(sockfd, &read)){
		//do read operation
	}

	if(FD_ISSET(sockfd, &write)){
		//do write operation
	}










	return 0;

}

