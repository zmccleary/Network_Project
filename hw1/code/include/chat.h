#include <client.h>

#ifndef CHAT
#define CHAT

extern window * tail;
void printhelp();
void listusers(int sockfd, rs_buf * buf);
void logout(int sockfd, rs_buf * buf);
void chat_init(int sockfd, rs_buf *buf, char * to, char * msg);
void chat(int sockfd, rs_buf *buf, char * to, char * msg);
void handle_from(int sockfd, rs_buf * buf);
void write_to_window(char * user, char * message);
void send_uoff(char * name);

#endif
