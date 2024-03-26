#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <malloc/malloc.h>


int main(int argc, char **argv) {

    void *p[1000];
    int index = 0;

    for (int i = 0; i < 100; i++) {
        p[index] = _malloc(i);
        if (p[index]) {
            for (int j = 0; j < i; j++)
                ((uint8_t *)p[index])[j] = 'a';
            index++;
        }
    }

    for (int i = 0; i < 100; i++) {
        p[index] = _malloc(10);
        if (p[index]) {
            for (int j = 0; j < 10; j++)
                ((char *)p[index])[j] = 'a';
        }
        index++;
    }
    
    for (int i = 0; i < index; i++) {
        _free(p[i]);
    }

    show_alloc_mem();
    
}
/*
    for (int i = 0; i < 100; i++) {
        p[index] = _malloc(10);
        if (p[index]) {
            for (int j = 0; j < 10; j++)
                ((char *)p[index])[j] = 'a';
        }
//        printf("alloc at 0x%llX\n", (uintptr_t)p[index]);
        index++;
    }

    show_alloc_mem();

    for (int i = 0; i < index; i++) {
        _free(p[index]);
    }
*/

/*
int main() {
  void *p1 = _malloc(10);
  void *p2 = _malloc(10);
  void *p3 = _malloc(200);
  void *p4 = _malloc(400);
  show_alloc_mem();
  _free(p2);
  _free(p3);

  for (int i = 0; i < 100; i++)
      _malloc(i);
  
  
  show_alloc_mem();
}
*/
