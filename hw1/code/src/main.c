#include <stdlib.h>
#include <wrappers.h>
#include <client.h>

int main(int argc, char **argv){
	
	char **conn_info = calloc(3, sizeof(char *));

	if(parse_args(argc, argv, conn_info) == -1){
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
