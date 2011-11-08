#ifndef _HUFFMAN_CODER
#define _HUFFMAN_CODER_

#define NODE 'X'
#define CHILD "[]"

struct point {
	size_t	x;
	size_t	y;
};

int getroot(char **, struct point *, size_t);
int step_left(char **, struct point *, size_t, size_t);
int step_right(char **, struct point *, size_t, size_t);
int is_node_leaf(char **, struct point *, size_t, size_t);
void walk_tree(char **, size_t, size_t, char *, size_t);
int is_valid_cell(int, int, size_t, size_t);
void read_table2memory(char **, size_t, size_t, FILE *);
size_t longest_line(FILE *);
size_t codeline_len(FILE *);
void add_table_padding(char **, size_t, size_t, char);
size_t line_count(FILE *);
void print_table(char **, size_t);

#endif /* _HUFFMAN_CODER_ */
