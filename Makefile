CC = gcc
CFLAGS = -Wall -Wextra -Ofast -Wpedantic

all: test_buffer build run

build: 
	$(CC) $(CFLAGS) main.c -o dobre

run:
	./dobre ./script.dob

test_buffer:
	$(CC) $(CFLAGS) buffer.c -o buffer
	./buffer

