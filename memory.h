#ifndef _MEMORY_H_
#define _MEMORY_H_

char *line_alloc(size_t);
char **table_alloc(size_t, size_t);
void table_free(char **, size_t, size_t);

#endif /* _MEMORY_H_ */
