CC = gcc
CFLAGS = -Wall

static:
	make -C ../zad1 static
	$(CC) $(CFLAGS) test.c -I../zad1 -lwclibrary -L../zad1 -o test

tests:
	@printf "1 small\n" > raport2.txt
	@./test 1 create_table 1 wc_files 1 ./files/1_small remove_block 0 end >> raport2.txt
	@printf "1 medium\n" >> raport2.txt
	@./test 1 create_table 1 wc_files 1 ./files/4_medium remove_block 0 end >> raport2.txt
	@printf "1 big\n" >> raport2.txt
	@./test 1 create_table 1 wc_files 1 ./files/9_big remove_block 0 end >> raport2.txt
	@printf "3 files\n" >> raport2.txt
	@./test 1 create_table 1 wc_files 3 ./files/2_small ./files/5_medium ./files/8_big remove_block 0 end >> raport2.txt
	@printf "6 files\n" >> raport2.txt
	@./test 1 create_table 1 wc_files 6 ./files/2_small ./files/3_small ./files/5_medium ./files/7_medium ./files/10_big ./files/9_big remove_block 0 end >> raport2.txt
	@printf "10 files\n" >> raport2.txt
	@./test 1 create_table 1 wc_files 10 ./files/1_small ./files/2_small ./files/3_small ./files/4_medium ./files/5_medium ./files/6_medium ./files/7_medium ./files/8_big ./files/9_big ./files/10_big remove_block 0 end >> raport2.txt
	@printf "\n" >> raport2.txt
	@./test 1 create_table 50 testing ./files/10_big end >> raport2.txt

clean:
	make -C ../zad1 clean
	rm -f test tmp raport2.txt