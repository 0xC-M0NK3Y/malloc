#include <stdint.h>

#include "defines.h"
#include "structs.h"

/*
  min = 0
  act = 1000

  if 300 > 0 && 1000 > 100
  100 - 300 // 100
  300 - 600 // 350


 */

#define NO_ZONE_INFO    0
#define TINY_ZONE_INFO  1
#define SMALL_ZONE_INFO 2
#define LARGE_ZONE_INFO 3

typedef struct alloc_info {
    uintptr_t ptr;
    uintptr_t size;
    int       zone;
} alloc_info_t;

static alloc_info_t search_in_heap(const heap_t *heap, size_t zone_size, uintptr_t min,
                         alloc_info_t act) {

    if (heap->base.heap_end > min && act.ptr > heap->base.heap) {
        for (uintptr_t p = heap->base.heap; p < heap->base.heap_end;
             p += zone_size + sizeof(size_t)) {
            if (*(uintptr_t *)p && p > min && p < act.ptr) {
                act.ptr  = p;
                act.size = *(uintptr_t *)p;
            }
        }
    }
    for (size_t i = 0; i < heap->more_nb; i++) {
        if (heap->more[i].heap_end > min && act.ptr > heap->more[i].heap) {
            for (uintptr_t p = heap->more[i].heap; p < heap->more[i].heap_end;
                 p += zone_size + sizeof(size_t)) {
                if (*(uintptr_t *)p && p > min && p < act.ptr) {
                    act.ptr  = p;
                    act.size = *(uintptr_t *)p;
                }
            }
        }
    }
    return act;
}

static alloc_info_t search_in_large_heap(const large_heap_t *heap, uintptr_t min,
                               alloc_info_t act) {

    for (size_t i = 0; i < heap->base.alloc_cap; i++) {
        if (heap->base.info[i].ptr > min && heap->base.info[i].ptr < act.ptr) {
            act.ptr  = heap->base.info[i].ptr;
            act.size = heap->base.info[i].size;
        }
    }
    for (size_t i = 0; i < heap->more_nb; i++) {
        for (size_t j = 0; j < heap->more[i].alloc_cap; j++) {
            if (heap->more[i].info[j].ptr > min &&
                heap->more[i].info[j].ptr < act.ptr) {
                act.ptr  = heap->more[i].info[j].ptr;
                act.size =  heap->more[i].info[j].size;
            }
        }
    }
    return act;
}

static alloc_info_t search_min(const malloc_data_t *data, uintptr_t min) {

    alloc_info_t ret = {0};
    uintptr_t tmp;

    ret.ptr = (uintptr_t)-1;
    tmp     = ret.ptr;
    
    ret.zone = NO_ZONE_INFO;
    
    ret = search_in_heap(&data->tiny, TINY_SIZE, min, ret);
    if (ret.ptr != tmp) {
        ret.zone = TINY_ZONE_INFO;
        tmp = ret.ptr;
    }
    ret = search_in_heap(&data->small, SMALL_SIZE, min, ret);
    if (ret.ptr != tmp) {
        ret.zone = SMALL_ZONE_INFO;
        tmp = ret.ptr;
    }
    ret = search_in_large_heap(&data->large, min, ret);
    if (ret.ptr != tmp) {
        ret.zone = LARGE_ZONE_INFO;
        tmp = ret.ptr;
    }
    return ret;
}

static void print_min(alloc_info_t info) {

    static int zone_act = 0;
    
    switch (info.zone) {
    case TINY_ZONE_INFO:
        if (zone_act != info.zone)
            printf("TINY ZONE\n");
        zone_act = TINY_ZONE_INFO;
        break;
    case SMALL_ZONE_INFO:
        if (zone_act != info.zone)
            printf("SMALL ZONE\n");
        zone_act = SMALL_ZONE_INFO;
        break;
        
    case LARGE_ZONE_INFO:
        if (zone_act != info.zone)
            printf("LARGE ZONE\n");
        zone_act = LARGE_ZONE_INFO;
        break;
    default:
        return;
    }
    printf("  0x%llX - %llu\n", info.ptr, info.size);
}

int show_alloc_mem_internal(const malloc_data_t *data) {
    alloc_info_t info = {0};
    
    while (info.ptr != (uintptr_t)-1) {
        info = search_min(data, info.ptr);
        print_min(info);
    }
    return 0;
}
