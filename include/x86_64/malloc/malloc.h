#ifndef FT_MALLOC_H__
# define FT_MALLOC_H__

#include <stddef.h>

void *_malloc(size_t size);
void _free(void *ptr);
int show_alloc_mem();
#endif /* ft_malloc.h */
