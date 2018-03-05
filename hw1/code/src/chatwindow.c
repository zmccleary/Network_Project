#include <stdio.h>
#include <stdlib.h>
#include <client.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <wrappers.h>

window ** windows = NULL;
int max_windows = 0;
int num_windows = 0;
window * tail = NULL;

int main(int argc, char* argv[]){

    char * name = argv[1];
    printf("\x1B[36mYou are now chatting with %s:\n", name);
    if(argv[2] != NULL)
        printf("\n>%s\n>", argv[2]);
    fflush(stdout);
    rs_buf * buf = (rs_buf *)malloc(sizeof(rs_buf));
    init_rsbuf(buf, 2048);
    //Loop over reading stdin
    fd_set readset;
    struct timeval tv;
    while(1)
    {
        tv.tv_sec = 30;
        FD_ZERO(&readset);
        FD_SET(3, &readset);
        FD_SET(0, &readset);
        Select(4, &readset, NULL, NULL, &tv);
        if(FD_ISSET(3, &readset))
        {
        
            flush_rsbuf(buf);
            int size = 0;
            
readp:
            size = buf->size - (buf->head - buf->buffer);
            if(read(3, buf->buffer, size) == size)
            {
                realloc_rsbuf(buf, buf->size * 2);
                goto readp;
            }
            //printf("totalbuf:%s\n", totalbuf);
            //strcpy(totalbuf, buf->buffer);
            char * header = strtok(buf->buffer, " ");
            if(strcmp(header, "UOFF") == 0)
            {
                printf("\nUser has logged off.\n");
                cleanup_rsbuf(buf);
                exit(0);
            }
            else if (strcmp(header, "MSG") == 0)
            {
                char * message = strtok(NULL, "\r");
                printf("\x1B[32m\n> %s", message);
                printf("\x1B[36m\n< ");
                fflush(stdout);
            }
            


        }
        if(FD_ISSET(0, &readset))
        {
            flush_rsbuf(buf);
            int size = 0;
reads:
            size = buf->size - (buf->head - buf->buffer);
            if(read(0, buf->buffer, size) == size)
            {
                realloc_rsbuf(buf, buf->size * 2);
                goto reads;
            }
            if(buf->buffer != NULL)
            {
                if(strcmp(buf->buffer, "/close\n") == 0)
                {
                    cleanup_rsbuf(buf);
                    exit(0);
                }
                write(4, buf->buffer, strlen(buf->buffer));
                printf("< ");
                fflush(stdout);
            } 
        }

    }
}
