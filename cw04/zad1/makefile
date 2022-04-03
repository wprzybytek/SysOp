CC = gcc
CFLAGS = -c -Wall

all:
	@make -s fork
	@make -s forktest
	@make -s exec
	@make -s exectest
	@make -s clean

fork:
	@$(CC) $(CFLAGS) forkmain.c
	@$(CC) forkmain.o -o forkmain

forktest:
	@printf "fork ignore\n"
	@./forkmain ignore
	@printf "fork handler\n"
	@./forkmain handler
	@printf "fork mask\n"
	@./forkmain mask
	@printf "fork pending\n"
	@./forkmain pending

exec:
	@$(CC) $(CFLAGS) execmain.c
	@$(CC) execmain.o -o execmain
	@$(CC) $(CFLAGS) execfile.c
	@$(CC) execfile.o -o execfile

exectest:
	@printf "exec ignore\n"
	@./execmain ignore
	@printf "exec mask\n"
	@./execmain mask
	@printf "exec pending\n"
	@./execmain pending		

clean:
	@rm -f forkmain execmain execfile *.o