CC = gcc
CFLAGS = -Wall -Wextra -Ofast -Wpedantic -I./3rdparty -isystem ./3rdparty -std=c2x
SRC = ./src/

all: buffer lexer build run

test: buffer lexer parser build 
	./bin/dobre ./tests/parser/double_extend_error.dob
	./bin/dobre ./tests/parser/declare_unknown_type.dob | true
	./bin/dobre ./tests/parser/while.dob

build: ensure_bin 
	$(CC) $(CFLAGS) $(SRC)main.c -o dobre
	cp dobre ./bin/dobre

run:
	./bin/dobre ./scripts/valgrind.dob

format:
	@clang-format -i $(SRC)*.h $(SRC)*.c
	@echo "Formatted source code."

valgrind: build
	valgrind --leak-check=full -s  ./bin/dobre ./scripts/valgrind.dob

ensure_bin: 
	@mkdir ./bin 2> /dev/null | true 

buffer: ensure_bin
	$(CC) $(CFLAGS) $(SRC)buffer.c -o ./bin/buffer
	./bin/buffer

lexer: ensure_bin
	$(CC) $(CFLAGS) $(SRC)lexer.c -o ./bin/lexer
	./bin/lexer

parser: ensure_bin
	$(CC) $(CFLAGS) $(SRC)parser.c -o ./bin/parser
	./bin/parser


big.disabled: ensure_bin
	$(CC) $(CFLAGS) $(SRC)big.c -o ./bin/big
	./bin/big
