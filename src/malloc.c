#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

#include "defines.h"
#include "structs.h"
#include "init.h"
#include "heap.h"
#include "large_heap.h"

malloc_data_t malloc_data = {0};

void *_malloc(size_t size) {

    void *ret = NULL;

    if (!malloc_data.ready && init_ft_malloc() < 0)
        return NULL;
    
    if (size == 0)
        size++;
    if (size <= TINY_SIZE) {
        ret = heap_alloc(&malloc_data.tiny, size, TINY_SIZE, malloc_data.pagesize);

    }
    else if (size <= SMALL_SIZE) {
        ret = heap_alloc(&malloc_data.small, size, SMALL_SIZE, malloc_data.pagesize);

    }
    else {
        ret = large_heap_alloc(&malloc_data.large, size, malloc_data.pagesize);

    }
    return ret;
}

void _free(void *ptr) {

    
    if (ptr == NULL)
        return;
    // on regarde les metadonnées
    if (((uintptr_t *)ptr)[-1] <= TINY_SIZE) {
        heap_free(&malloc_data.tiny, ptr, TINY_SIZE, malloc_data.pagesize);

    }
    else if (((uintptr_t *)ptr)[-1] <= SMALL_SIZE) {
        heap_free(&malloc_data.small, ptr, SMALL_SIZE, malloc_data.pagesize);

    } else {

        large_heap_free(&malloc_data.large, ptr);

    }
}


