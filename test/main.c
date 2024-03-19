#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <malloc/malloc.h>

int main(int argc, char **argv) {

    void *a = _malloc(10);
    void *b = _malloc(500);
    void *c = _malloc(5000);

    printf("a = %p\n", a);
    printf("b = %p\n", b);
    printf("c = %p\n", c);
    
    _free(a);
    _free(b);
    _free(c);
}
