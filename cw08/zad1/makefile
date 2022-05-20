all: test

test: clean main
	@for thread_no in 1 2 4 8 16 ; do \
  		for method in numbers blocks ; do \
			./main $$thread_no $$method mona_lisa.ascii.pgm mona_lisa_negative.ascii.pgm >> Times.txt ; \
  		done ; \
  	done

main:
	@gcc -Wall -o main main.c -lpthread

clean:
	@rm -f main mona_lisa_negative.ascii.pgm Times.txt