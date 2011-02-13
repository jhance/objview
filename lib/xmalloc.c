#include <xmalloc.h>
#include <stdio.h>
#include <stdlib.h>

void *xmalloc(size_t s) {
    void *p = malloc(s);
    if(p == NULL) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }

    return p;
}
