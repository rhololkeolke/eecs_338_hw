.PHONY: clean

binaries=hw1
CC=gcc -std=gnu99 -Wall -Wextra

all: hw1

debug: CC += -DDEBUG -g
debug: hw1

hw1: hw1.c
	${CC} -o hw1 hw1.c
clean:
	rm -f $(binaries) *.o
script_record: hw1
	script hw1_script_output.txt -c ./hw1
redirect_record: hw1
	./hw1 > hw1_redirect_output.txt 2>&1
