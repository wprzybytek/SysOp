CC = gcc
CFLAGS = -Wall
LIBRARY = wclibrary

static:
	$(CC) $(OPT) $(CFLAGS) -c $(LIBRARY).c
	ar -rcs lib$(LIBRARY).a $(LIBRARY).o

shared:
	$(CC) $(OPT) $(CFLAGS) -fPIC -c $(LIBRARY).c
	$(CC) $(OPT) $(CFLAGS) -shared -fPIC -o lib$(LIBRARY).so $(LIBRARY).o

clean:
	rm -f *.o *.a *.so tmp