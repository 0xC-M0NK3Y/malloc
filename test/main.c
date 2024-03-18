#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <malloc/malloc.h>

int main(int argc, char **argv) {

    void *ptrs[10000] = {0};

    for (int i = 0; i < 100+1000+100; i++) {
        ptrs[i] = _malloc(7000);
    }
    
}
