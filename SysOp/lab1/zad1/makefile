CC = gcc
CFLAGS = -Wall
LIBRARY = wclibrary

static:
	$(CC) $(CFLAGS) -c $(LIBRARY).c
	ar -rcs lib$(LIBRARY).a $(LIBRARY).o

shared:
	$(CC) $(CFLAGS) -fPIC -c $(LIBRARY).c
	$(CC) $(CFLAGS) -shared -fPIC -o lib$(LIBRARY).so $(LIBRARY).o

clean:
	rm -f *.o *.a *.so tmp