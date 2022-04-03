CC = gcc
CFLAGS = -c -Wall

all:
	@$(CC) $(CFLAGS) main.c
	@$(CC) main.o -o main
	@./main
	@make -s clean

clean:
	@rm -f main *.o