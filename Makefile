CC = gcc
CFLAGS = -Wall -Wextra -Ofast -Wpedantic -Werror

all: buffer lexer build run

build: 
	$(CC) $(CFLAGS) main.c -o dobre

run:
	./dobre ./script.dob

format:
	clang-format -i *.h *.c

valgrind: build
	valgrind --leak-check=full ./dobre ./valgrind.dob

buffer:
	$(CC) $(CFLAGS) buffer.c -o bin/buffer
	./bin/buffer

lexer:
	$(CC) $(CFLAGS) lexer.c -o bin/lexer
	./bin/lexer

