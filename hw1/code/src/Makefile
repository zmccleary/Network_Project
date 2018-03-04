all: client

client:
	gcc -Wall -Werror main.c wrappers.c client.c chat.c -o $@ -I../include/
	gcc -Wall -Werror chatwindow.c client.c chat.c wrappers.c -o chatwindow -I../include/

clean:
	rm -f client debug *.out

debug:
	gcc -Wall -Werror -g -O2 main.c wrappers.c client.c chat.c -o $@ -I../include/
	gcc -Wall -Werror -g chatwindow.c client.c chat.c wrappers.c -o chatwindow -I../include/
