#ifndef UTILS_H__
# define UTILS_H__

#include <stdint.h>

#include <sys/mman.h>

#include "defines.h"
#include "structs.h"

static inline uintptr_t find_free_space_in_block(heap_block_t *block, uintptr_t free_space, size_t zone_size) {
    // on cherche vers la droite
    for (uintptr_t p = free_space; p < block->heap_end; p += zone_size+sizeof(size_t)) {
        if (*(uintptr_t *)p == 0)
            return p;
    }
    // sinon on reprends à gauche
    for (uintptr_t p = block->heap; p < free_space; p += zone_size+sizeof(size_t)) {
        if (*(uintptr_t *)p == 0)
            return p;
    }
    return 0;
}

static inline uintptr_t find_free_space_in_more_blocks(heap_t *heap, size_t zone_size) {
    if (heap->more == NULL || heap->more_nb == 0 || heap->more_cap == 0)
        return 0;
    // on cherche dans tous les block more un free space
    for (size_t i = 0; i < heap->more_nb; i++) {
        if (heap->more[i].alloc_nb < heap->more[i].alloc_cap) {
            uintptr_t p = find_free_space_in_block(&heap->more[i], heap->more[i].heap, zone_size);
            if (p) {
                heap->more_index = i;
                return p;
            }
        }
    }
    return 0;
}

static inline int init_heap_block(heap_block_t *block, size_t zone_size, int page_size) {
    // on initalise le block
    void *tmp;
    size_t tmp_size = ALIGN_TO_PAGE_SIZE(
        (zone_size + sizeof(size_t)) * ZONE_NUMBER, page_size);
    tmp = mmap(NULL, tmp_size,
               PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE,
               -1, 0);
    if (tmp == MAP_FAILED)
        return -1;
    inline_zeromem((uint8_t *)tmp, tmp_size);
    
    block->alloc_cap = tmp_size/(zone_size+sizeof(size_t));
    block->heap      = (uintptr_t)tmp;
    block->heap_end  = block->heap + block->alloc_cap*(zone_size+sizeof(size_t));
    block->alloc_nb  = 0;
   
    return 0;
}

static inline int free_heap_block(heap_block_t *block, size_t zone_size, int page_size) {
    return munmap((void *)block->heap, ALIGN_TO_PAGE_SIZE(block->alloc_cap*(zone_size+sizeof(size_t)), page_size));
}

static inline void alloc_more_block(heap_t *heap, size_t zone_size, int page_size) {
    // si on a jamais alloue de more block
    if (heap->more == NULL || heap->more_cap == 0) {
        // d'abord allouer la structure de heap_block_t
        size_t tmp_size =
            ALIGN_TO_PAGE_SIZE((10 * sizeof(*heap->more)), page_size);
        void   *tmp = mmap(NULL, tmp_size,
                           PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE,
                           -1, 0);
        if (tmp == MAP_FAILED)
            return;
        inline_zeromem((uint8_t *)tmp, tmp_size);
        heap->more_nb  = 0;
        heap->more_cap = tmp_size/sizeof(*heap->more);
        heap->more     = (heap_block_t *)tmp;
        // initialiser le premier block
        if (init_heap_block(&heap->more[0], zone_size, page_size) < 0)
            return;
        heap->free_space = heap->more[0].heap;
        heap->more_index = 0;
        heap->more_nb++;
    // si on peut rajouter un more block
    } else if (heap->more_nb < heap->more_cap) {
        // on initialise le block suivant
        if (init_heap_block(&heap->more[heap->more_nb], zone_size, page_size) < 0)
            return;
        heap->free_space = heap->more[heap->more_nb].heap;
        heap->more_index = heap->more_nb;
        heap->more_nb++;
    // sinon faut reallouer le nombre de more block disponible
    } else {
        size_t tmp_size = ALIGN_TO_PAGE_SIZE(
            ((heap->more_cap + 10) * sizeof(*heap->more)), page_size);
        void   *tmp = mmap(NULL, tmp_size,
                           PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE,
                           -1, 0);
        if (tmp == MAP_FAILED)
            return;
        inline_zeromem((uint8_t *)tmp, tmp_size);
        inline_memcpy((uint8_t *)tmp, (uint8_t *)heap->more, heap->more_nb*sizeof(*heap->more));
        munmap(heap->more, ALIGN_TO_PAGE_SIZE((heap->more_cap*sizeof(*heap->more)), page_size));
        heap->more     = (heap_block_t *)tmp;
        heap->more_cap = tmp_size/sizeof(*heap->more);
        // et on initialise le prochain block
        if (init_heap_block(&heap->more[heap->more_nb], zone_size, page_size) < 0)
            return;
        heap->free_space = heap->more[heap->more_nb].heap;
        heap->more_index = heap->more_nb;
        heap->more_nb++;
    }
}

#endif /* utils.h */
