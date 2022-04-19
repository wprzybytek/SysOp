CC = gcc
CFLAGS = -c -Wall

all:
	@make -s producer
	@make -s consumer
	@make -s main
	@./main 5 1
	@make -s clean

main:
	@$(CC) $(CFLAGS) main.c
	@$(CC) main.o -o main

consumer:
	@$(CC) $(CFLAGS) consumer.c
	@$(CC) consumer.o -o consumer

producer:
	@$(CC) $(CFLAGS) producer.c
	@$(CC) producer.o -o producer

clean:
	@rm -f consumer producer main *.o