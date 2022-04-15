CC = gcc
CFLAGS = -c -Wall

all:
	@$(CC) $(CFLAGS) main.c
	@$(CC) main.o -o main
	@./main $$USER topic content
	@printf "Sort by date.\n"
	@./main date
	@printf "Sort by sender.\n"
	@./main sender
	@make -s clean

clean:
	@rm -f main *.o