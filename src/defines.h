#ifndef DEFINES_H__
# define DEFINES_H__

#define TINY_SIZE   32
#define SMALL_SIZE  1024

#define ZONE_NUMBER 100

#define ALIGN_TO_PAGE_SIZE(x, pagesize) ((x | (pagesize-1)) + 1)

static inline void inline_zeromem(uint8_t *p, size_t s) {
    // TODO : opti
    for (size_t i = 0; i < s; i++)
        p[i] = 0;
}

static inline void inline_memcpy(uint8_t *dst, uint8_t *src, size_t s) {
    for (size_t i = 0; i < s; i++)
        dst[i] = src[i];
}


//TODO: rm
#include <stdio.h>
#include <stdarg.h>
static inline void malloc_printf(const char *f, ...) {
    puts("[malloc]");
    va_list args;
    va_start(args, f);
    vprintf(f, args);
    va_end(args);
    puts("[/malloc]");
}

#endif /* defines.h */
