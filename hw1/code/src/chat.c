#include <chat.h>
#include <client.h>
#include <string.h>
#include <stdio.h>
#include <wrappers.h>
#include <stdlib.h>

void printhelp(){
    const char * helpmsg = "\nCommands:\n"
        "/help              Displays all command that the client accepts.\n"
        "/logout            Initiates logout procedure from the server.\n"
        "/listu             Request a list of logged in users from the server\n"
        "/chat <to> <msg>   Sends <msg> to user <to>\n\n";
    printf("%s", helpmsg);
}

void listusers(int sockfd, rs_buf * buf){
    
    const char * listumsg = "LISTU\r\n\r\n";
    Write(sockfd, listumsg, strlen(listumsg));
    handle_read(sockfd, buf, LIST_USER);
}


//Logs out from the server
void logout(int sockfd, rs_buf * buf){
    
    //Print BYE to the server and wait for EYB
    char * logoutmsg = "BYE\r\n\r\n";
    Write(sockfd, logoutmsg, strlen(logoutmsg));
    handle_read(sockfd, buf, LOGOUT);
    cleanup_rsbuf(buf);
    free(buf);
    printf("Logging out of the server.\n");
    exit(EXIT_SUCCESS);


}

void chat(int sockfd, rs_buf *buf, char * to, char * msg)
{
    char * tomsg = (char *)calloc(buf->size, sizeof(char));
    sprintf(tomsg, "TO %s %s\r\n\r\n", to, msg);
    Write(sockfd, tomsg, strlen(tomsg));
    handle_read(sockfd, buf, MESSAGE_TO);
    free(tomsg);
}

void handle_from(int sockfd, rs_buf * buf)
{
    char * confirm = (char *)calloc(25, sizeof(char));
    char * user = strtok(buf->buffer, " ");
    user = strtok(NULL, " ");
    sprintf(confirm, "MORF %s\r\n\r\n", user);
    char * message = strtok(NULL, "\r");
    printf("<%s>: %s\n", user, message);
    flush_rsbuf(buf);
    Write(sockfd, confirm, strlen(confirm));
    free(confirm);
}
