#ifndef CLIENT
#define CLIENT

#include <unistd.h>

extern const char* cli_usage;
extern char ** conn_info;
char parse_args(int argc, char * const argv[], char * conn_info[]);
int login(int sockfd, char *username, char *buf);
#endif
