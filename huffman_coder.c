#include <ctype.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "memory.h"
#include "huffman_coder.h"

int
main(int argc, const char *argv[])
{
	size_t	  x = 0;
	size_t	  y = 0;
	char	**t = NULL;
	FILE	 *fp = NULL;
	char	 *bitcode = NULL;
	const char *fname = NULL;

	if (argc < 2)
		fname = "example.txt";
	else
		fname = argv[argc - 1];

	if ((fp = fopen(fname, "r")) == NULL) {
		if (fp)
			fclose(fp);
		err(5, "%s", fname);
		return 1;
	}

	x = longest_line(fp);
	y = line_count(fp);

	t = table_alloc(x, y);
	bitcode = line_alloc(CODESIZE + 1);

	read_table2memory(t, x, y, fp);
	add_table_padding(t, x, y, ' ');
	read_code2memory(fp, x, bitcode);

	print_table(t, x, y);

	fprintf(stdout, "Code is: %s\n", bitcode);
	fprintf(stdout, "Deciphered code: ");
	walk_tree(t, y, x, bitcode, strlen(bitcode));

	table_free(t, x, y);
	if (fp)
		(void)fclose(fp);

	return 0;
}

int
getroot(char **t, struct point *p, size_t x)
{
	size_t	i = 0;

	for (i = 0; i < x; i++) {
		if (t[0][i] == 'X') {
			p->x = i;
			p->y = 0;

			return 0;
		}
	}

	return -1;
}

/* return 0 if next node is achieved, 1 if node is a leaf and -1 otherwise */
int
step_left(char **t, struct point *p, size_t max_x, size_t max_y)
{
	while (is_valid_cell(p->y + 1, p->x - 1, max_x, max_y) == 0) {
		p->y++;
		p->x--;

		switch (t[p->y][p->x]) {
		case '/':
			continue;
		case NODE:
			return 0;
		default:
			if (is_node_leaf(t, p, max_x, max_y) == 0)
				return 1;
		}
	}

	return -1;
}

/* return 0 if next node is achieved, 1 if node is a leaf and -1 otherwise */
int
step_right(char **t, struct point *p, size_t max_x, size_t max_y)
{
	while (is_valid_cell(p->y + 1, p->x + 1, max_x, max_y) == 0) {
		p->x++;
		p->y++;

		switch (t[p->y][p->x]) {
		case '\\':
			continue;
		case NODE:
			return 0;
		default:
			if (is_node_leaf(t, p, max_x, max_y) == 0)
				return 1;
		}
	}

	return -1;
}

int
is_node_leaf(char **t, struct point *p, size_t max_x, size_t max_y)
{
	if ((is_valid_cell(p->y, p->x - 1, max_x, max_y) == 0) &&
	    (is_valid_cell(p->y, p->x + 1, max_x, max_y) == 0)) {
		if ((t[p->y][p->x - 1] == LL) &&
		    (t[p->y][p->x + 1] == LR))

			return 0;
	}
	return -1;
}

void
walk_tree(char **t, size_t max_x, size_t max_y, char *code, size_t code_size)
{
	int		 i;
	struct point	*p;

	if ((p = calloc(1, sizeof(struct point))) == NULL)
		err(2, "point calloc");

	/* Get the root node */
	if (getroot(t, p, max_x) != 0) {
		fprintf(stderr, "Terrible things happened.\n");
		return;
	}

	for (i = 0; i < code_size; i++) {
		int	isleaf;
		char	c;

		c = code[i];

		switch (c) {
		case '0': /* left */
			isleaf = step_left(t, p, max_x, max_y);
			if (isleaf == 1) { /* stepping to left */
				fprintf(stdout, "%c", t[p->y][p->x]);
				getroot(t, p, max_x); /* Remember to return to root node */
			}
			break;
		case '1': /* right */
			isleaf = step_right(t, p, max_x, max_y);
			if (isleaf == 1) { /* stepping to right */
				fprintf(stdout, "%c", t[p->y][p->x]);
				getroot(t, p, max_x); /* Remember to return to root node */
			}
			break;
		default:
			fprintf(stderr, "Problem with the given code (%c, %d).\n", c, c);
			break;
		}
	}
	fprintf(stdout, "\n");

	if (p)
		free(p);
}

/* 0 is valid, 1 is invalid*/
int
is_valid_cell(int y, int x, size_t xsize, size_t ysize)
{
	if ((x >= 0 && y >= 0) && (x < xsize && y < ysize))
		return 0;
	return 1;
}

void
read_table2memory(char **t, size_t x, size_t y, FILE *fp)
{
	size_t	 i = 0;

	rewind(fp);

	while (fgets(t[i], x, fp) != NULL) {
		t[i][y] = '\0';
		i++;
	}
}

void
read_code2memory(FILE *fp, size_t x, char *code)
{
	size_t	 charsread = 0;
	char	 linebuf[CODESIZE];
	char	*ch;
	char	*codech;

	rewind(fp);

	while (fgets(linebuf, CODESIZE, fp) != NULL) {
		if ((strncasecmp(linebuf, "code", 4)) == 0) {
			ch = &linebuf[0];
			codech = &code[0];
			while (*ch != '\0') {
				if (isdigit(*ch) != 0) {
					*codech = *ch;
					codech++;
					charsread++;
				}
				ch++;

			}
			if (charsread + 1 <= CODESIZE)
				code[charsread + 1] = '\0';
			else
				code[CODESIZE - 1] = '\0';
			return;
		}
	}
}

size_t
longest_line(FILE *fp)
{
	int	ch;
	int	prevch;
	int	i;
	int	max;

	rewind(fp);

	ch = prevch = i = max = 0;

	while ((ch = getc(fp)) != EOF) {
		/* do not read last line since it includes "code:" */
		if (prevch == '\n' && ch == '\n')
			break;

		if (ch == '\n') {
			if (i > max)
				max = i;
			i = 0;
		}
		i++;
		prevch = ch;
	}

	return max + 2;
}

void
add_table_padding(char **t, size_t x, size_t y, char padding)
{
	int i, j;

	for (i = 0; i < x; i++) {
		for (j = 0; j < y; j++) {
			if (iscntrl(t[i][j]))
				t[i][j] = padding;
		}
	}
}

size_t
line_count(FILE *fp)
{
	int	ch;
	size_t	lc;

	ch = lc = 0;

	lseek(fileno(fp), 0, SEEK_SET);

	while ((ch = getc(fp)) != EOF) {
		if (ch == '\n')
			lc++;
	}

	return lc;
}

void
print_table(char **t, size_t max_x, size_t max_y)
{
	int	i, j;

	for (i = 0; i < max_x - 5; i++) { /* XXX silly glue, fix this */
		for (j = 0; j < max_y; j++)
			fprintf(stdout, "%c", t[i][j]);
		fprintf(stdout, "\n");
	}
}

