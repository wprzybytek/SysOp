CC = gcc
CFLAGS = -c -Wall

all:
	@$(CC) $(CFLAGS) main.c
	@$(CC) main.o -o main
	@./main commands.txt
	@make -s clean

clean:
	@rm -f main *.o