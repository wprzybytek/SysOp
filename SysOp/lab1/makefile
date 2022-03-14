CC = gcc
CFLAGS = -Wall

static:
	$(CC) $(CFLAGS) -c wclibrary.c
	ar $(CFLAGS) -rcs libwclibrary.a wclibrary.o
	$(CC) $(CFLAGS) main.c -o main -L. -lwclibrary

shared:
	$(CC) $(CFLAGS) -fPIC -c wclibrary.c
	$(CC) $(CFLAGS) -shared -fPIC -o libwclibrary.so wclibrary.o
	$(CC) $(CFLAGS) main.c -o main -L. -lwclibrary
	export LD_LIBRARY_PATH=$(shell pwd)

clean:
	rm -f *.o *.a *.so temp main