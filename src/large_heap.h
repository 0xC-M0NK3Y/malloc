#ifndef LARGE_HEAP_H__
# define LARGE_HEAP_H__

#include <stdint.h>

#include <sys/mman.h>

#include "defines.h"
#include "structs.h"
#include "large_heap_utils.h"

static inline void *large_heap_alloc(large_heap_t *heap, size_t size, int page_size) {
    void   *ret     = NULL;
    size_t ret_size = ALIGN_TO_PAGE_SIZE((sizeof(uintptr_t)+size+(size_t)(size*0.1)), page_size);

    if (heap->free_space) {
        // on alloue sa memoire
        ret = mmap(NULL, ret_size,
                   PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE,
                   -1, 0);
        if (ret == MAP_FAILED)
            return NULL;
        // on rempli les infos
        *(uintptr_t *)ret = size;
        ret = &((uintptr_t *)ret)[1];
        heap->free_space->ptr  = (uintptr_t)ret;
        heap->free_space->cap  = ret_size;
        heap->free_space->size = size;

        // si on est dans le block d'infos de base
        if (heap->more_index == (size_t)-1) {
            heap->base.alloc_nb++;

            // si on peut encore allouer dans le block d'info de base,
            // sinon dans un block d'info more
            if (heap->base.alloc_nb < heap->base.alloc_cap)
                heap->free_space = large_find_free_block_info(&heap->base);
            else
                heap->free_space = large_find_free_block_info_in_more(heap);
        // sinon
        } else {
            heap->more[heap->more_index].alloc_nb++;

            // si on peut dans un block d'info de base on prefere
            // sinon si dans le block d'info more actuel on prends
            // sinon on cherche dans les autres block d'info more
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

static inline void large_heap_free(large_heap_t *heap, void *ptr) {
    // on regarde dans le block d'info de base
    for (size_t i = 0; i < heap->base.alloc_cap; i++) {
        // si on a trouver le ptr
        if ((uintptr_t)ptr == heap->base.info[i].ptr) {
            munmap(&((uintptr_t *)ptr)[-1], heap->base.info[i].cap);
            heap->base.info[i].ptr = 0;
            heap->base.alloc_nb--;
            return;
        }
    }
    // sinon on regarde dans les block d'info more
    for (size_t i = 0; i < heap->more_nb; i++) {
        for (size_t j = 0; j < heap->more[i].alloc_cap; j++) {
            if ((uintptr_t)ptr == heap->more[i].info[j].ptr) {
                munmap(&((uintptr_t *)ptr)[-1], heap->more[i].info[j].cap);
                heap->more[i].info[j].ptr = 0;
                heap->more[i].alloc_nb--;
                return;
            }
        }
    }
    return;
}

static inline void *large_heap_realloc(large_heap_t *heap, void *ptr, size_t size, int page_size) {
    // on regarde dans le block d'info de base
    for (size_t i = 0; i < heap->base.alloc_cap; i++) {
        // si on a trouver le ptr
        if ((uintptr_t)ptr == heap->base.info[i].ptr) {
            if (size <= heap->base.info[i].cap) {
                heap->base.info[i].size = size;
                return ptr;
            }
            // on alloue puis on free si on a reussi a allouer
            void *tmp = large_heap_alloc(heap, size, page_size);
            if (tmp) {
                inline_memcpy((uint8_t *)tmp, (uint8_t *)ptr, heap->base.info[i].size);
                munmap(&((uintptr_t *)ptr)[-1], heap->base.info[i].cap);
                heap->base.info[i].ptr = 0;
                heap->base.alloc_nb--;
            }
            return tmp;
        }
    }
    // sinon on regarde dans les block d'info more
    for (size_t i = 0; i < heap->more_nb; i++) {
        for (size_t j = 0; j < heap->more[i].alloc_cap; j++) {
            if ((uintptr_t)ptr == heap->more[i].info[j].ptr) {
                if (size <= heap->more[i].info[j].cap) {
                    heap->more[i].info[j].size = size;
                    return ptr;
                }
                void *tmp = large_heap_alloc(heap, size, page_size);
                if (tmp) {
                    inline_memcpy((uint8_t *)tmp, (uint8_t *)ptr, heap->more[i].info[j].size);
                    munmap(&((uintptr_t *)ptr)[-1], heap->more[i].info[j].cap);
                    heap->more[i].info[j].ptr = 0;
                    heap->more[i].alloc_nb--;
                }
                return tmp;
            }
        }
    }
    return NULL;
}

#endif /* large_heap.h */
