#ifndef CLIENT
#define CLIENT

#include <unistd.h>

#define BUFSIZE 1024

extern const char* cli_usage;
extern char ** conn_info;

typedef struct{
    char * buffer;
    int size;
}rs_buf;

typedef enum{
    LOGIN1,
    LOGIN2,
    LIST_USER,
    MESSAGE_TO,
    LOGOUT
} ChatState_t;


int handle_read(int sockfd, rs_buf * buf, ChatState_t state);
char parse_args(int argc, char * const argv[], char * conn_info[]);
int login(int sockfd, char *username, rs_buf *buf);
void init_rsbuf(rs_buf * buf, int bufsize);
void realloc_rsbuf(rs_buf *buf, int bufsize);
void cleanup_rsbuf(rs_buf * buf);
<<<<<<< HEAD

int list_u(char * token, int tok_len, int terminator_read);
=======
void flush_rsbuf(rs_buf * buf);
>>>>>>> c3fa81973a98f0c074476211664bf1bd2652fe40
#endif
