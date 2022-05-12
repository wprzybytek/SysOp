all:
	@make -s clean
	@make -s main
	@make -s cook
	@make -s deliverer

main:
	@gcc -Wall -o main main.c -lpthread -lrt

cook:
	@gcc -Wall -o cook cook.c -lpthread -lrt

deliverer:
	@gcc -Wall -o deliverer deliverer.c -lpthread -lrt

clean:
	@rm -f main cook deliverer