#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include <unistd.h>
#include <sys/mman.h>

#include "structs.h"

extern malloc_data_t malloc_data;

__attribute__((constructor))
int init_ft_malloc(void) {
    void   *tmp;
    int    pagesize   = getpagesize();
    size_t tiny_size  = ALIGN_TO_PAGE_SIZE(((TINY_SIZE+sizeof(size_t))*ZONE_NUMBER), pagesize);
    size_t small_size = ALIGN_TO_PAGE_SIZE(((SMALL_SIZE+sizeof(size_t))*ZONE_NUMBER), pagesize);
    size_t total_size = tiny_size + small_size;

    inline_zeromem((uint8_t *)&malloc_data, sizeof(malloc_data));
    
    malloc_data.pagesize = pagesize;
    
    tmp = mmap(NULL, total_size,
               PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE,
               -1, 0);
    if (tmp == MAP_FAILED)
        return -1;
    inline_zeromem(tmp, total_size);

    malloc_data.tiny.base.alloc_cap  = tiny_size/(TINY_SIZE+sizeof(size_t));
    malloc_data.tiny.base.heap       = (uintptr_t)tmp;
    malloc_data.tiny.base.heap_end   = malloc_data.tiny.base.heap + malloc_data.tiny.base.alloc_cap*(TINY_SIZE+sizeof(size_t));
    malloc_data.tiny.base.alloc_nb   = 0;
    malloc_data.tiny.free_space      = malloc_data.tiny.base.heap;
    malloc_data.tiny.more_index      = (size_t)-1;

    malloc_data.small.base.alloc_cap = small_size/(SMALL_SIZE+sizeof(size_t));
    malloc_data.small.base.heap      = (uintptr_t)tmp + tiny_size;
    malloc_data.small.base.heap_end   = malloc_data.small.base.heap + malloc_data.small.base.alloc_cap*(SMALL_SIZE+sizeof(size_t));
    malloc_data.small.base.alloc_nb  = 0;
    malloc_data.small.free_space     = malloc_data.small.base.heap;
    malloc_data.small.more_index     = (size_t)-1;

    malloc_data.large.base.alloc_nb  = 0;
    malloc_data.large.base.alloc_cap = ZONE_NUMBER;
    malloc_data.large.free_space     = &malloc_data.large.base.info[0];
    malloc_data.large.more_index     = (size_t)-1;
    
    return 0;
}
