CC = gcc

all:
	@make -s clean
	@make -s server
	@make -s client

server:
	@gcc -Wall server.c -o server -lrt

client:
	@gcc -Wall client.c -o client -lrt

clean:
	@rm -f *.o server client