#ifndef LARGE_HEAP_H__
# define LARGE_HEAP_H__

#include <stdint.h>

#include <sys/mman.h>

#include "defines.h"
#include "structs.h"
#include "large_utils.h"

static inline void *large_heap_alloc(large_heap_t *heap, size_t size, int page_size) {
    void   *ret     = NULL;
    size_t ret_size = ALIGN_TO_PAGE_SIZE((size+(size_t)(size*0.1)), page_size);

    if (heap->free_space) {
        // on alloue sa memoire
        ret = mmap(NULL, ret_size,
                   PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE,
                   -1, 0);
        if (ret == MAP_FAILED)
            return NULL;
        // on rempli les infos
        heap->free_space->ptr  = (uintptr_t)ret;
        heap->free_space->cap  = ret_size;
        heap->free_space->size = size;

        // si on est dans le block d'infos de base
        if (heap->more_index == (size_t)-1) {
            heap->base.alloc_nb++;
            
            if (heap->base.alloc_nb < heap->base.alloc_cap)
                heap->free_space = large_find_free_block_info(&heap->base);
            else
                heap->free_space = large_find_free_block_info_in_more(heap);
        // sinon
        } else {
            heap->more[heap->more_index].alloc_nb++;
            
            if (heap->base.alloc_nb < heap->base.alloc_cap) {
                heap->free_space = large_find_free_block_info(&heap->base);
                heap->more_index = (size_t)-1;
            } else if (heap->more[heap->more_index].alloc_nb < heap->more[heap->more_index].alloc_cap)
                heap->free_space = large_find_free_block_info(&heap->more[heap->more_index]);
            else
                heap->free_space = large_find_free_block_info_in_more(heap);
        }
    }
    if (heap->free_space == NULL)
        large_alloc_more_block_info(heap, page_size);
    
    return ret;
}

#endif /* large_heap.h */
