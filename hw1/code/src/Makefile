all: client

client:
	gcc -Wall -Werror main.c wrappers.c client.c -o $@ -I../include/

clean:
	rm -f client debug *.out

debug:
	gcc -Wall -Werror -g main.c wrappers.c client.c -o $@ -I../include/
