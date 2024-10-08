CC = gcc
CFLAGS = -Wall -Wextra -Ofast -Wpedantic -Werror

all: buffer lexer build run

build: 
	$(CC) $(CFLAGS) main.c -o dobre
	cp dobre ./bin/dobre

run:
	./dobre ./script.dob

format:
	clang-format -i *.h *.c

valgrind: build
	valgrind --leak-check=full ./bin/dobre ./valgrind.dob

ensure_bin: 
	@mkdir ./bin 2> /dev/null | true 

buffer: ensure_bin
	$(CC) $(CFLAGS) buffer.c -o ./bin/buffer
	./bin/buffer

lexer: ensure_bin
	$(CC) $(CFLAGS) lexer.c -o ./bin/lexer
	./bin/lexer

