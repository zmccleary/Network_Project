#ifndef CLIENT
#define CLIENT

#include <unistd.h>

extern const char* cli_usage;

char parse_args(int argc, char * const argv[], char * conn_info[]);
#endif
