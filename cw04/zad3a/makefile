CC = gcc
CFLAGS = -c -Wall

all:
	@make -s sender
	@make -s catcher

sender:
	@$(CC) $(CFLAGS) sender.c
	@$(CC) sender.o -o sender

catcher:
	@$(CC) $(CFLAGS) catcher.c
	@$(CC) catcher.o -o catcher

clean:
	@rm -f sender catcher *.o