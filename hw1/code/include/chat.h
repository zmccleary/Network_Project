#include <client.h>

#ifndef CHAT
#define CHAT

void printhelp();
void listusers(int sockfd, rs_buf * buf);
void logout(int sockfd, rs_buf * buf);
void chat(int sockfd, rs_buf *buf, char * to, char * msg);

#endif
