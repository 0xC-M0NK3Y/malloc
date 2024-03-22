#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <malloc/malloc.h>

int main(int argc, char **argv) {

    void *p[1000];
    int index = 0;
    
    for (int i = 0; i < 100; i++) {
        p[index] = malloc(i);
        if (p[index]) {
            for (int j = 0; j < i; i++)
                ((uint8_t *)p[index])[j] = j;
        }
        index++;
    }

    for (int i = 0; i < 100; i++) {
        p[index] = malloc(10);
        if (p[index]) {
            for (int j = 0; j < 10; j++)
                ((char *)p[index])[j] = 'a';
        }
        index++;
    }

    for (int i = 0; i < index; i++) {
        free(p[index]);
    }
    
}
