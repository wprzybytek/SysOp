CC = gcc
CFLAGS = -c -Wall

all:
	@$(CC) $(CFLAGS) main.c
	@$(CC) main.o -o main
	@./main 10
	@make -s clean

clean:
	@rm -f *.o main