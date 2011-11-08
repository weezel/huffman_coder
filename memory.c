#include <stdio.h>
#include <stdlib.h>
#include <err.h>

char *
line_alloc(size_t n)
{
	char	*t = NULL;

	if ((t = calloc(n, sizeof(char))) == NULL)
		err(1, "malloc");

	return t;
}

char **
table_alloc(size_t x, size_t y)
{
	int	  i;
	char	**table = NULL;

	if ((table = calloc(x, sizeof(char *))) == NULL)
		err(1, "malloc");

	for (i = 0; i < x; i++) {
		if ((table[i] = calloc(y, sizeof(char))) == NULL)
			err(1, "malloc");
	}

	return table;
}

void
table_free(char **t, size_t x, size_t y)
{
	int	i;

	for (i = 0; i < x; i++) {
		if (t[i]) {
			(void)free(t[i]);
			t[i] = NULL;
		}
	}
	if (t)
		(void)free(t);
	t = NULL;
}

