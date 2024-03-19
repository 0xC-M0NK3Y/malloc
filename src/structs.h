#ifndef STRUCTS_H__
# define STRUCTS_H__

#include <stdint.h>
#include <stddef.h>

#include "defines.h"

typedef struct heap_block {
    uintptr_t heap;
    uintptr_t heap_end;
    size_t    alloc_nb;
    size_t    alloc_cap;
} heap_block_t;

typedef struct {
    heap_block_t base;
    heap_block_t *more;
    size_t       more_nb;
    size_t       more_cap;
    size_t       more_index; // si on est dans un more  
    uintptr_t    free_space;
} heap_t;

typedef struct large_alloc_info {
    uintptr_t ptr;
    size_t    size;
    size_t    cap;
} large_alloc_info_t;

typedef struct large_heap_block {
    large_alloc_info_t info[ZONE_NUMBER];
    size_t             alloc_nb;
    size_t             alloc_cap;
} large_heap_block_t;

typedef struct large_heap {
    large_heap_block_t base;
    large_heap_block_t *more;
    size_t             more_nb;
    size_t             more_index;
    large_alloc_info_t *free_space;
} large_heap_t;

typedef struct malloc_data {
    heap_t       tiny;
    heap_t       small;
    large_heap_t large;
    int          pagesize;
    int          ready;
} malloc_data_t;

#endif /* structs.h */
