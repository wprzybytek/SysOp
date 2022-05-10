all:
	@make -s clean
	@make -s main
	@make -s cook

main:
	@gcc -Wall -o main main.c

cook:
	@gcc -Wall -o cook cook.c

clean:
	@rm -f main cook