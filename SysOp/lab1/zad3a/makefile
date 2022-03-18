CC = gcc
CFLAGS = -Wall

static:
	make -C ../zad1 static
	$(CC) $(CFLAGS) ../zad2/test.c -I../zad1 -lwclibrary -L../zad1 -o test

shared:
	make -C ../zad1 shared
	$(CC) $(CFLAGS) ../zad2/test.c -I../zad1 -lwclibrary -L../zad1 -o test

dynamic:
	make -C ../zad1 shared
	$(CC) $(CFLAGS) ../zad2/test.c -ldl -D DYNAMIC -o test

tests:
	@printf "1 small\n" >> raport3a.txt
	@./test 1 create_table 1 wc_files 1 ../zad2/files/1_small remove_block 0 end >> raport3a.txt
	@printf "1 medium\n" >> raport3a.txt
	@./test 1 create_table 1 wc_files 1 ../zad2/files/4_medium remove_block 0 end >> raport3a.txt
	@printf "1 big\n" >> raport3a.txt
	@./test 1 create_table 1 wc_files 1 ../zad2/files/9_big remove_block 0 end >> raport3a.txt
	@printf "3 files\n" >> raport3a.txt
	@./test 1 create_table 1 wc_files 3 ../zad2/files/2_small ../zad2/files/5_medium ../zad2/files/8_big remove_block 0 end >> raport3a.txt
	@printf "6 files\n" >> raport3a.txt
	@./test 1 create_table 1 wc_files 6 ../zad2/files/2_small ../zad2/files/3_small ../zad2/files/5_medium ../zad2/files/7_medium ../zad2/files/10_big ../zad2/files/9_big remove_block 0 end >> raport3a.txt
	@printf "10 files\n" >> raport3a.txt
	@./test 1 create_table 1 wc_files 10 ../zad2/files/1_small ../zad2/files/2_small ../zad2/files/3_small ../zad2/files/4_medium ../zad2/files/5_medium ../zad2/files/6_medium ../zad2/files/7_medium ../zad2/files/8_big ../zad2/files/9_big ../zad2/files/10_big remove_block 0 end >> raport3a.txt
	@printf "add and remove\n" >> raport3a.txt
	@./test 1 create_table 1 testing ../zad2/files/10_big end >> raport3a.txt

test-all:
	make static
	@printf "static\n" > raport3a.txt
	make tests
	make clean
	make shared
	@printf "\n shared\n" >> raport3a.txt
	make tests
	make clean
	make dynamic
	@printf "\n dynamic\n" >> raport3a.txt
	make tests
	make clean

clean:
	make -C ../zad2 clean
	rm -f test tmp