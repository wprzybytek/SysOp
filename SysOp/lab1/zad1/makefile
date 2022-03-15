CC = gcc
CFLAGS = -Wall

static:
	$(CC) $(CFLAGS) -c wclibrary.c
	ar -rcs libwclibrary.a wclibrary.o

shared:
	$(CC) $(CFLAGS) -fPIC -c wclibrary.c
	$(CC) $(CFLAGS) -shared -fPIC -o libwclibrary.so wclibrary.o

clean:
	rm -f *.o *.a *.so tmp