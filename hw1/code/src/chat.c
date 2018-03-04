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

void chat_init(int sockfd, rs_buf *buf, char * to, char * msg)
{
    char * name = (char *)calloc(11, sizeof(char));
    char * mesg = (char *)calloc(buf->size, sizeof(char));
    strcpy(mesg, msg);
    strcpy(name, to);
    if(chat(sockfd, buf, to, msg) > 0)
        add_window(name, mesg);
    free(mesg);
    free(name);
}


int chat(int sockfd, rs_buf *buf, char * to, char * msg)
{
    char * tomsg = (char *)calloc(buf->size, sizeof(char));
    char * tmg = strtok(msg, "\n");
    if(tmg == NULL)
    {
        free(tomsg);
        free(tmg);
        return -1;
    }
    sprintf(tomsg, "TO %s %s\r\n\r\n", to, tmg);
    Write(sockfd, tomsg, strlen(tomsg));
    handle_read(sockfd, buf, MESSAGE_TO);
    free(tomsg);
    if(strcmp(buf->buffer, "EDNE") == 0)
        return -1;
    flush_rsbuf(buf);
    return 1;
}

void handle_from(int sockfd, rs_buf * buf)
{
    char * confirm = (char *)calloc(25, sizeof(char));
    char * user = strtok(buf->buffer, " ");
    user = strtok(NULL, " ");
    sprintf(confirm, "MORF %s\r\n\r\n", user);
    char * message = strtok(NULL, "\n");
    //printf("<%s>: %s\n", user, message);
    write_to_window(user, message);
    flush_rsbuf(buf);
    Write(sockfd, confirm, strlen(confirm));
    free(confirm);
}

void write_to_window(char * user, char * message)
{
    window * wp;
    for(wp = tail; wp != NULL; wp = wp->prev)
    {
        if(strcmp(wp->name, user) == 0)
            break;
    }
    if(wp == NULL)
    {
        add_window(user, NULL);
        wp = tail;
    }
    char * send = (char *)calloc(strlen(message) + 10, sizeof(char));
    sprintf(send, "MSG %s", message);
    if(write(wp->writepipe, send, strlen(send)) < 0)
        client_error("Bad write in write_to_window\n");
    free(send);
}

void send_uoff(char * name)
{
    window *wp;
    char uoff[] = "UOFF";
    for(wp = tail; wp != NULL; wp = wp->prev)
    {
        if(strcmp(wp->name, name) == 0)
        {
            if(write(wp->writepipe, uoff, strlen(uoff)) < 0)
                client_error("Bad write in uoff\n");
            return;
        }
    }
}
