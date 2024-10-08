CC = gcc
CFLAGS = -Wall -Wextra -Ofast -Wpedantic -Werror
SRC = ./src/

all: buffer lexer build run

build: 
	$(CC) $(CFLAGS) $(SRC)main.c -o dobre
	cp dobre ./bin/dobre

run:
	./dobre ./test/script.dob

format:
	clang-format -i $(SRC)*.h $(SRC)*.c

valgrind: build
	valgrind --leak-check=full ./bin/dobre ./test/valgrind.dob

ensure_bin: 
	@mkdir ./bin 2> /dev/null | true 

buffer: ensure_bin
	$(CC) $(CFLAGS) $(SRC)buffer.c -o ./bin/buffer
	./bin/buffer

lexer: ensure_bin
	$(CC) $(CFLAGS) $(SRC)lexer.c -o ./bin/lexer
	./bin/lexer

