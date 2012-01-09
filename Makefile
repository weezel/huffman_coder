CC=clang
LINT=lint
CFLAGS=-g -ansi -pedantic -Wall 

all:
	${CC} ${CFLAGS} -o huffman_coder huffman_coder.c memory.c
clean:
	rm -f *.o huffman_coder
lint:
	${LINT} -h -o huffman_coder huffman_coder.c memory.c
