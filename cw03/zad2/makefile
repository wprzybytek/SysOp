CC = gcc
CFLAGS = -c -Wall

all:
	@$(CC) $(CFLAGS) main.c
	@$(CC) main.o -o main
	@make -s test
	@make -s clean

test:
	printf "" > raport2.txt
	for size in "0.0001" "0.00001" "0.000001"; do \
		for proc in 1 10 100 1000; do \
			./main $$size $$proc >> raport2.txt ; \
		done ; \
	done

clean:
	@rm -f *.o main files/*.txt