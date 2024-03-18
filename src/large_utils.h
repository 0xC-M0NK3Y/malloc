#ifndef LARGE_UTILS_H__
# define LARGE_UTILS_H__

#include <stdint.h>

#include "structs.h"

static inline large_alloc_info_t *large_find_free_block_info(large_heap_block_t *block) {
    for (size_t i = 0; i < block->alloc_cap; i++) {
        if (block->info[i].ptr == 0)
            return &block->info[i];
    }
    return NULL;
}

static inline large_alloc_info_t *large_find_free_block_info_in_more(large_heap_t *heap) {
    if (heap->more == NULL ||  heap->more_nb == 0)
        return NULL;
    for (size_t i = 0; i < heap->more_nb; i++) {
        if (heap->more[i].alloc_nb < heap->more[i].alloc_cap) {
            large_alloc_info_t *p = large_find_free_block_info(&heap->more[i]);
            
            if (p) {
                heap->more_index = i;
                return p;
            }
        }
    }
    return NULL;
}

static inline void large_alloc_more_block_info(large_heap_t *heap, int page_size) {
    if (heap->more == NULL || heap->more_nb == 0) {
        size_t tmp_size = ALIGN_TO_PAGE_SIZE((10*sizeof(*heap->more)), page_size);
        void   *tmp     = mmap(NULL, tmp_size,
                               PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE,
                               -1, 0);
        if (tmp == MAP_FAILED)
            return;
        inline_zeromem(tmp, tmp_size);
        
        heap->more       = tmp;
        heap->more_nb    = tmp_size/sizeof(*heap->more);
        
        for (size_t i = 0; i < heap->more_nb; i++)
            heap->more[i].alloc_cap = ZONE_NUMBER;

        heap->free_space = &heap->more[0].info[0];
        heap->more_index = 0;
        
    } else {
        size_t tmp_size = ALIGN_TO_PAGE_SIZE(((heap->more_nb+10)*sizeof(*heap->more)), page_size);
        void   *tmp     = mmap(NULL, tmp_size,
                               PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE,
                               -1, 0);
        if (tmp == MAP_FAILED)
            return;
        inline_zeromem(tmp, tmp_size);
        inline_memcpy(tmp, (uint8_t *)heap->more, heap->more_nb*sizeof(*heap->more));
        munmap(heap->more, ALIGN_TO_PAGE_SIZE(heap->more_nb*sizeof(*heap->more), page_size));
        size_t tmp_nb = heap->more_nb;
        heap->more    = tmp;
        heap->more_nb = tmp_size/sizeof(*heap->more);

        for (size_t i = tmp_nb; i < heap->more_nb; i++)
            heap->more[i].alloc_cap = ZONE_NUMBER;

        heap->free_space = &heap->more[tmp_nb].info[0];
        heap->more_index = tmp_nb;
    }

}

#endif /* large_utils.h */
