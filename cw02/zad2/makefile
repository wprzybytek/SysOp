CC = gcc
CFLAGS = -c -Wall

all:
	@make lib
	@./libmain a file.txt > pomiar_zad_2.txt
	@make clean
	@make sys
	@./sysmain a file.txt >> pomiar_zad_2.txt
	@make clean

lib:
	$(CC) $(CFLAGS) libmain.c
	$(CC) libmain.o -o libmain

sys:
	$(CC) $(CFLAGS) sysmain.c
	$(CC) sysmain.o -o sysmain

clean:
	rm -f libmain sysmain *.o