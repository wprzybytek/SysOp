CC = gcc
CFLAGS = -c -Wall

all:
	@make stat
	@./statmain files
	@make clean
	@make nftw
	@./nftwmain files
	@make clean

stat:
	$(CC) $(CFLAGS) statmain.c
	$(CC) statmain.o -o statmain

nftw:
	$(CC) $(CFLAGS) nftwmain.c
	$(CC) nftwmain.o -o nftwmain

clean:
	rm -f statmain nftwmain *.o