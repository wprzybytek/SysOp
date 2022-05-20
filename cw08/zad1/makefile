all: clean main
	./main 2 numbers mona_lisa.ascii.pgm mona_lisa_negative.ascii.pgm

main:
	@gcc -Wall -o main main.c -lpthread

clean:
	@rm -f main mona_lisa_negative.ascii.pgm