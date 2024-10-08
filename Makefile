CC = gcc
CFLAGS = -Wall -Wextra -Ofast -Wpedantic -Werror -I./3rdparty -isystem ./3rdparty
SRC = ./src/

all: buffer lexer build run format

test: buffer lexer run

build: ensure_bin 
	$(CC) $(CFLAGS) $(SRC)main.c -o dobre
	cp dobre ./bin/dobre

run:
	./bin/dobre ./scripts/script.dob

format:
	@clang-format -i $(SRC)*.h $(SRC)*.c
	@echo "Formatted source code."

valgrind: build
	valgrind --leak-check=full ./bin/dobre ./scripts/valgrind.dob

ensure_bin: 
	@mkdir ./bin 2> /dev/null | true 

buffer: ensure_bin
	$(CC) $(CFLAGS) $(SRC)buffer.c -o ./bin/buffer
	./bin/buffer

lexer: ensure_bin
	$(CC) $(CFLAGS) $(SRC)lexer.c -o ./bin/lexer
	./bin/lexer

