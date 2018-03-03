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


typedef struct window{
    char name[11];
    int readpipe;
    int writepipe;
    struct window * next;
    struct window * prev;
}window;
extern window * tail;


typedef enum{
    LOGIN1,
    LOGIN2,
    MOTD,
    LIST_USER,
    MESSAGE_TO,
    LOGOUT,
    DEFAULT
} ChatState_t;

//Client based functions
int handle_read(int sockfd, rs_buf * buf, ChatState_t state);
char parse_args(int argc, char * const argv[], char * conn_info[]);
int login(int sockfd, char *username, rs_buf *buf);
void printMOTD(rs_buf *buf); 

//Buffer functions
void init_rsbuf(rs_buf * buf, int bufsize);
void realloc_rsbuf(rs_buf *buf, int bufsize);
void cleanup_rsbuf(rs_buf * buf);


int list_u(char * token, int tok_len, int *terminator_read);

void flush_rsbuf(rs_buf * buf);

void add_window(char * name, char * message);
void awl(window * wp);
void rwl(window * wp);
void remove_window(char * name);

#endif
