CC = gcc
CFLAGS = -Wall

static:
	make -C ../zad1 static
	$(CC) $(CFLAGS) $(OPT) ../zad2/test.c -I../zad1 -lwclibrary -L../zad1 -o test

shared:
	make -C ../zad1 shared
	$(CC) $(CFLAGS) $(OPT) ../zad2/test.c -I../zad1 -lwclibrary -L../zad1 -o test

dynamic:
	make -C ../zad1 shared
	$(CC) $(CFLAGS) $(OPT) ../zad2/test.c -ldl -D DYNAMIC -o test

tests:
	@printf "1 small\n" >> raport3b.txt
	@./test 1 create_table 1 wc_files 1 ../zad2/files/1_small remove_block 0 end >> raport3b.txt
	@printf "1 medium\n" >> raport3b.txt
	@./test 1 create_table 1 wc_files 1 ../zad2/files/4_medium remove_block 0 end >> raport3b.txt
	@printf "1 big\n" >> raport3b.txt
	@./test 1 create_table 1 wc_files 1 ../zad2/files/9_big remove_block 0 end >> raport3b.txt
	@printf "3 files\n" >> raport3b.txt
	@./test 1 create_table 1 wc_files 3 ../zad2/files/2_small ../zad2/files/5_medium ../zad2/files/8_big remove_block 0 end >> raport3b.txt
	@printf "6 files\n" >> raport3b.txt
	@./test 1 create_table 1 wc_files 6 ../zad2/files/2_small ../zad2/files/3_small ../zad2/files/5_medium ../zad2/files/7_medium ../zad2/files/10_big ../zad2/files/9_big remove_block 0 end >> raport3b.txt
	@printf "10 files\n" >> raport3b.txt
	@./test 1 create_table 1 wc_files 10 ../zad2/files/1_small ../zad2/files/2_small ../zad2/files/3_small ../zad2/files/4_medium ../zad2/files/5_medium ../zad2/files/6_medium ../zad2/files/7_medium ../zad2/files/8_big ../zad2/files/9_big ../zad2/files/10_big remove_block 0 end >> raport3b.txt
	@printf "add and remove\n" >> raport3b.txt
	@./test 1 create_table 1 testing ../zad2/files/10_big end >> raport3b.txt

test-all:
	printf "" > raport3b.txt

	for lib_type in static shared dynamic ; do \
  		for opt in O0 O3 Os ; do \
			printf "\n $$lib_type $$opt\n" >> raport3b.txt ; \
			make $$lib_type OPT=-$$opt ; \
			make tests ; \
  		done ; \
  	done

clean:
	make -C ../zad2 clean
	make -C ../zad3a clean
	rm -f test tmp