CC = gcc

all:
	@make -s server
	@make -s client

server:
	@gcc -Wall server.c -o server

client:
	@gcc -Wall client.c -o client

clean:
	rm -f *.o server client