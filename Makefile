CC=clang

all:
	${CC} -g -o huffman_coder -ansi -pedantic -Wall huffman_coder.c memory.c
