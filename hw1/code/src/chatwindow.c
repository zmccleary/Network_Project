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
    printf("You are now chatting with %s:\n", name);
    if(argv[2] != NULL)
        printf("%s\n", argv[2]);
    //int nread, totalread;
    rs_buf * buf = (rs_buf *)malloc(sizeof(rs_buf));
    init_rsbuf(buf, 1024);
    char *totalbuf = (char *)calloc(buf->size, sizeof(char)); 
    //printf("hello\n"); 
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
            //printf("SOMETHING TO READ\n");
            flush_rsbuf(buf);
            memset(totalbuf, 0, strlen(totalbuf));
            /*while((nread = read(3, buf->buffer, buf->size)) > 0)
            {
                if(nread > 0)
                {
                    strcat(totalbuf, buf->buffer);
                    //printf("totalbuf:%s\n", totalbuf);

                    totalread += nread;
                    if(nread == buf->size){
                        realloc_rsbuf(buf, buf->size*2);
                        totalbuf = realloc(totalbuf, buf->size);
                    }
                }           
            }*/
            read(3, buf->buffer, buf->size);
            //printf("totalbuf:%s\n", totalbuf);
            strcpy(totalbuf, buf->buffer);
            char * header = strtok(totalbuf, " ");
            if(strcmp(header, "UOFF") == 0)
            {
                printf("\nUser has logged off.\n");
                free(totalbuf);
                cleanup_rsbuf(buf);
                exit(0);
            }
            else if (strcmp(header, "MSG") == 0)
            {
                char * message = strtok(NULL, "\r");
                printf("<%s>: %s\n", name, message);
            }
            /*else
            {
                printf("%s\n", buf->buffer);
            }*/


        }
        if(FD_ISSET(0, &readset))
        {
            //printf("SOMETHING TO READ: stdin\n");
            //flush_rsbuf(buf);
            //memset(totalbuf, 0, strlen(totalbuf));
            /*while((nread = read(3, buf->buffer, buf->size)) > 0)
            {
                strcat(totalbuf, buf->buffer);
                totalread += nread;
                if(totalread == buf->size){
                    realloc_rsbuf(buf, buf->size*2);
                    totalbuf = realloc(totalbuf, buf->size);
                }           
            }*/
            read(0, buf->buffer, buf->size);
            //printf("Read in: %s\n", buf->buffer);
            write(4, buf->buffer, strlen(buf->buffer)); 
            //printf("Wrote out: %s\n", buf->buffer);
            flush_rsbuf(buf);
        }

    }
}
