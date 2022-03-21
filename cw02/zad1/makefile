CC = gcc
CFLAGS = -c -Wall

all:
	@make lib
	@./libmain file1.txt file2.txt > pomiar_zad_1.txt
	@make clean
	@make sys
	@./sysmain file1.txt file2.txt >> pomiar_zad_1.txt
	@make clean

lib:
	$(CC) $(CFLAGS) libmain.c
	$(CC) libmain.o -o libmain

sys:
	$(CC) $(CFLAGS) sysmain.c
	$(CC) sysmain.o -o sysmain

clean:
	rm -f libmain sysmain *.o