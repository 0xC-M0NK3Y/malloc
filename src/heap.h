#ifndef HEAP_H__
# define HEAP_H__

#include <stdint.h>

#include "structs.h"
#include "utils.h"

static inline void *heap_alloc(heap_t *heap, size_t size, size_t zone_size, int page_size) {
    void *ret = NULL;

    // si on a un emplacement dispo
    if (heap->free_space != 0) {
        // on met les metadonnées
        ((size_t *)heap->free_space)[0] = size;
        ret = (void *)(&((size_t *)heap->free_space)[1]);
        // Si le free space est dans le block de base
        if (heap->more_index == (size_t)-1) {
            heap->base.alloc_nb++;
            // si on a de la place dans le block de base on cherche dedans, sinon on cherche dans un block more
            if (heap->base.alloc_nb < heap->base.alloc_cap)
                heap->free_space = find_free_space_in_block(&heap->base, heap->free_space, zone_size);
            else
                heap->free_space = find_free_space_in_more_blocks(heap, zone_size);
        // Sinon dans un block more
        } else {
            heap->more[heap->more_index].alloc_nb++;
            // si on peut trouver un free space dans le block de base tant mieu
            if (heap->base.alloc_nb < heap->base.alloc_cap) {
                heap->free_space = find_free_space_in_block(&heap->base, heap->base.heap, zone_size);
                heap->more_index = (size_t)-1;
            // sinon si on peut on cherche dans le block more actuel
            } else if (heap->more[heap->more_index].alloc_nb < heap->more[heap->more_index].alloc_cap) {
                heap->free_space = find_free_space_in_block(&heap->more[heap->more_index], heap->free_space, zone_size);
            // sinon on cherche dans les autres block more
            } else
                heap->free_space = find_free_space_in_more_blocks(heap, zone_size);
        }
    }
    // si on a pas eu de free space, ca veut dire il faut qu'on allouer plus de block more
    if (heap->free_space == 0)
        alloc_more_block(heap, zone_size, page_size);
    return ret;
}

static inline void heap_free(heap_t *heap, void *ptr, size_t zone_size, int page_size) {
    // si le pointeur vient de la zone de base
    if (heap->base.heap <= (uintptr_t)ptr && (uintptr_t)ptr < heap->base.heap_end) {
        ((size_t *)ptr)[-1] = 0;
        heap->base.alloc_nb--;
        // si le free space actuel est dans la zone de base
        if (heap->base.heap <= heap->free_space && heap->free_space < heap->base.heap_end) {
            // si le le pointeur actuel libre est a gauche du free_space actuel, on le prends en tant que free_space
            if ((uintptr_t)(&((size_t *)ptr)[-1]) < heap->free_space) {
                heap->free_space = (uintptr_t)(&((size_t *)ptr)[-1]);
                heap->more_index = (size_t)-1;
            }
        // sinon le free_space est pas dans la zone de base, on prefere allouer le prochain dans la zone de base
        } else {
            heap->free_space = (uintptr_t)(&((size_t *)ptr)[-1]);
            heap->more_index = (size_t)-1;
        }
    // sinon le pointeur n'est pas dans la zone de base, on le cherche
    } else {
        for (size_t i = 0; i < heap->more_nb; i++) {
            // si on a trouver de quel block more le ptr vient
            if (heap->more[i].heap <=  (uintptr_t)ptr &&  (uintptr_t)ptr < heap->more[i].heap_end) {
                ((size_t *)ptr)[-1] = 0;
                heap->more[i].alloc_nb--;
                // si le free space actuel est pas dans ce block et que y'a plus d'allocation dedans
                // on le libere
                if (heap->more_index != i && heap->more[i].alloc_nb <= 0) {
                    if (free_heap_block(&heap->more[i], zone_size, page_size) == 0) {
                        memmove(&heap->more[i], &heap->more[i+1], heap->more_nb-i-1);
                        if (heap->more_index != (size_t)-1 && heap->more_index > i)
                            heap->more_index--;
                    }
                // sinon si le free_space est dans le block actuel, on prefere prendre un free_space a gauche
                } else if (heap->more_index == i) {
                    if ((uintptr_t)(&((size_t *)ptr)[-1]) < heap->free_space)
                        heap->free_space = (uintptr_t)(&((size_t *)ptr)[-1]);
                }
                break;
            }
        }
    }
}

#endif /* heap.h */
