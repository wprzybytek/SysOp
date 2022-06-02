all: clean main
	@./main
	@make -s clean

main:
	@gcc -Wall -o main main.c -lpthread

clean:
	@rm -f main