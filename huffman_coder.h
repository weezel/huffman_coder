#ifndef _HUFFMAN_CODER_
#define _HUFFMAN_CODER_

#define CODESIZE 4096
#define NODE 'X'
#define LL '['
#define LR ']'

struct point {
	size_t	x;
	size_t	y;
};

struct fmetrics {
	size_t max_linelen;
	size_t lines;
};

int getroot(char **, struct point *, size_t);
int step_left(char **, struct point *, size_t, size_t);
int step_right(char **, struct point *, size_t, size_t);
int is_node_leaf(char **, struct point *, size_t, size_t);
void walk_tree(char **, size_t, size_t, char *, size_t);
int is_valid_cell(size_t, size_t, size_t, size_t);
void read_table2memory(FILE *, char **, size_t, size_t);
void read_code2memory(FILE *, size_t, char *);
void read_fmetrics(FILE *, struct fmetrics *);
void add_table_padding(char **, size_t, size_t, char);
void print_table(char **, size_t, size_t);
void clear_screen(void);

#endif /* _HUFFMAN_CODER_ */
