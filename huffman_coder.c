#include <ctype.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "huffman_coder.h"
#include "memory.h"

int
main(int argc, const char *argv[])
{
	size_t	  x = 0;
	size_t	  y = 0;
	char	**t = NULL;
	FILE	 *fp = NULL;
	char	 *fname = "example.txt";
	char *testikoodi = "0111111111\0";

	if ((fp = fopen(fname, "r")) == NULL) {
		warn("%s\n", fname);
		return 1;
	}

	x = longest_line(fp);
	y = line_count(fp);

	fprintf(stdout, "Max line len : %zu\n", x);
	fprintf(stdout, "Line count   : %zu\n", y);
	fprintf(stdout, "Table size   : %zu\n", x * y);

	fprintf(stdout, "t[-1][-1]    : %zu\n", is_valid_cell(-1, -1, x, y));
	fprintf(stdout, "t[-1][0]     : %zu\n", is_valid_cell(-1, 0, x, y));
	fprintf(stdout, "t[0][-1]     : %zu\n", is_valid_cell(0, -1, x, y));
	fprintf(stdout, "t[%2zu][%2zu]    : %zu\n", x, y, is_valid_cell(x, y, x, y));
	fprintf(stdout, "t[0][0]      : %zu\n", is_valid_cell(0, 0, x, y));

	t = table_alloc(x, y);

	read_table2memory(t, x, y, fp);
	add_table_padding(t, x, y, ' ');

	print_table(t, y);
	walk_tree(t, x, y, testikoodi, sizeof(testikoodi));

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
		if (t[0][i] == NODE) {
			p->x = i;
			p->y = 0;
			return 0;
		}
	}

	return 1;
}

int
step_left(char **t, struct point *p, size_t max_x, size_t max_y)
{
	if (is_valid_cell(p->x - 1, p->y + 1, max_x, max_y) == 0) {
		p->x--;
		p->y++;
		return 0;
	}

	return 1;
}

int
step_right(char **t, struct point *p, size_t max_x, size_t max_y)
{
	if (is_valid_cell(p->x + 1, p->y + 1, max_x, max_y) == 0) {
		p->x++;
		p->y++;
		return 0;
	}

	return 1;
}

int
is_node_leaf(char **t, struct point *p, size_t max_x, size_t max_y)
{
	if (is_valid_cell(p->x - 1, p->y, max_x, max_y) == 0 &&
	    is_valid_cell(p->x + 1, p->y, max_y, max_y) == 0) {
		if (t[p->x - 1][p->y] == CHILD[0] &&
		    t[p->x + 1][p->y] == CHILD[1])
			return 0;
	}
	return 1;
}

void
walk_tree(char **t, size_t max_x, size_t max_y, char *code, size_t code_size)
{
	int		 i;
	struct point	*p;

	if ((p = calloc(1, sizeof(struct point))) == NULL)
		err(2, "point calloc");

	/* Get the root node */
	if (getroot(t, p, max_x) > 0) {
		fprintf(stderr, "Terrible things happened.\n");
		return;
	}
	fprintf(stdout, "Root found from the point: %zu,%zu\n", p->x, p->y);

	for (i = 0; i < code_size; i++) {
		int	c;
		c = code[i];

		switch (c) {
		case 0: /* left */
			if (step_left(t, p, max_x, max_y) == 0) { /* stepping to left succeed */
				if (is_node_leaf(t, p, max_x, max_y) == 0) { /* node is leaf node */
					fprintf(stdout, "%d. letter: %c\n", i, t[p->x][p->y]);
				}
			}
			break;
		case 1: /* right */
			if (step_right(t, p, max_x, max_y) == 0) { /* stepping to right succeed */
				if (is_node_leaf(t, p, max_x, max_y) == 0) { /* node is leaf node */
					fprintf(stdout, "%d. letter: %c\n", i, t[p->x][p->y]);
				}
			}
			break;
		default:
			fprintf(stderr, "Problem with the given code (%c).\n", c);
			break;
		}

		/* Remember to return to root node */
		if (getroot(t, p, max_x) > 0) {
			fprintf(stderr, "Terrible things happened.\n");
			return;
		}
	} /* for */


	if (p)
		free(p);
}

int
is_valid_cell(int x, int y, size_t xsize, size_t ysize)
{
	if ((x >= 0 && y >= 0) || (x < xsize && y < ysize))
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

		if (i+1 >= y-1) /* XXX eh... */
			break;
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

size_t
codeline_len(FILE *fp)
{
	int	ch;
	size_t	lc;

	ch = lc = 0;

	lseek(fileno(fp), 0, SEEK_END);
	/*
	while ((ch = getc(fp)) != EOF) {
		if (ch == '\n')
			lc++;
	}
	*/

	ch = getc(fp);
	return ch;
}

void
add_table_padding(char **t, size_t x, size_t y, char padding)
{
	int i, j;

	i = j = 0;

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
print_table(char **t, size_t y)
{
	int	i = 0;

	for (i = 0; i < y; i++)
		fprintf(stdout, "%s\n", t[i]);
}

