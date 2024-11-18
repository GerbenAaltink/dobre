CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wpedantic -I./3rdparty -isystem ./3rdparty
SRC = ./src/
BIN = ./bin/
BUILD_CMD = $(CC) $(CFLAGS) $(SRC)main.c -o dobre

all: test build_all run

unit_test: build_fast buffer lexer parser dict 

script_test: build_fast 
	
	-./bin/dobre ./tests/parser/double_class_declaration.dob
	./bin/dobre ./tests/parser/assignment.dob
	-./bin/dobre ./tests/parser/double_extend_error.dob
	-./bin/dobre ./tests/parser/declare_unknown_type.dob
	-./bin/dobre ./tests/parser/while.dob
	./bin/dobre ./tests/parser/for.dob
	./bin/dobre ./tests/parser/operators.dob
	./bin/dobre ./tests/parser/declare_function.dob

test: script_test unit_test

build: ensure_bin 
	$(BUILD_CMD) -o dobre -Ofast
	cp dobre ./bin/dobre

build_all: build
	$(BUILD_CMD) -o  $(BIN)dobre_c99 -std=c99
	$(BUILD_CMD) -o  $(BIN)dobre_gnu99 -std=gnu99
	$(BUILD_CMD) -o  $(BIN)dobre_c11 -std=c11
	$(BUILD_CMD) -o  $(BIN)dobre_gnu11 -std=gnu11
	$(BUILD_CMD) -o  $(BIN)dobre_c17 -std=c17
	$(BUILD_CMD) -o  $(BIN)dobre_gnu17 -std=gnu17
	$(BUILD_CMD) -o  $(BIN)dobre_c2x -std=c2x
	$(BUILD_CMD) -o  $(BIN)dobre_gnu2x -std=gnu2x


build_fast:
	$(CC) $(SRC)main.c -o dobre -Werror -std=c17
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

dict: ensure_bin
	$(CC) $(CFLAGS) $(SRC)dict.c -o ./bin/dict
	./bin/dict


big.disabled: ensure_bin
	$(CC) $(CFLAGS) $(SRC)big.c -o ./bin/big
	./bin/big
