all:
	@make -s clean
	@make -s main
	@make -s cook
	@make -s deliverer

main:
	@gcc -Wall -o main main.c

cook:
	@gcc -Wall -o cook cook.c

deliverer:
	@gcc -Wall -o deliverer deliverer.c

clean:
	@rm -f main cook deliverer