CC = gcc
CFLAGS = -c -Wall

consumer:
	@$(CC) $(CFLAGS) consumer.c
	@$(CC) consumer.o -o consumer

producer:
	@$(CC) $(CFLAGS) producer.c
	@$(CC) producer.o -o producer

clean:
	@rm -f consumer producer *.o