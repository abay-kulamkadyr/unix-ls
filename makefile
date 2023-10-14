CFLAGS= -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L 

all:build
build: 
	gcc main.c -o UnixLs
run: build
	./UnixLs
valgrind: build
	valgrind --leak-check=full ./UnixLs
clean:
	rm -f UnixLs