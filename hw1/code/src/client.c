#include <client.h>

char parse_args(int argc, char * const argv[], char * conn_info[]){
	if(argc == 1)
		return -1; //client can't be called with no args

	int arg_ind = 1;
	char opcode = 0;
	int info_ind = 0;

	if(!strcmp(argv[arg_ind], "-h"))
	{
		printf(usage);
		return 0; //return 0 to tell main function that program is printing usage info and exit 
	}

	if(!strcmp(argv[arg_ind], "-v"))
	{
			opcode = opcode | 0x1 //OR 1 to set verbose flag
			arg_ind++;
	}

	while(arg_ind < argc){

		/**
		 *validate args and choose execution mode
		 *
		 */
		if(opcode == 0){
			if(arg_ind > 3)
				return -1;

		}

		if(arg_ind > 4)
				return -1;
		
		conn_info[info_ind]++ = argv[arg_ind++];

	}
	
	return 0;
}