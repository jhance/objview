#include <string.h>
#include <GL/glut.h>

#include "tga.h"
#include "hash.h"
#include "xmalloc.h"

static unsigned int hash_alg(size_t size, char *k) {
    unsigned int h = 0;
    char *p;

    for(p = k; *p != '\0'; p++) {
        h = 31 * h + *p;
    }

    return h % size;
}

struct hash_table hash_init(size_t size) {
    struct hash_table h;
    int i;

    h.size = size;
    h.chains = xmalloc(sizeof(struct hash_chain) * size);

    for(i = 0; i < size; i++) {
        h.chains[i].k = 0;
        h.chains[i].v = 0;
        h.chains[i].next = NULL;
    }

    return h;
}

GLuint hash_lookup(struct hash_table *t, char *k) {
    GLuint v;
    int h = hash_alg(t->size, k);

    struct hash_chain *chain = &t->chains[h];

    while(chain->next != NULL) {
        chain = chain->next;

        if(!strcmp(chain->k, k)) {
            return chain->v;
        }
    }

    v = load_tga_texture(k);
    hash_put(t, k, v);
    return v;
}

void hash_put(struct hash_table *t, char *k, GLuint v) {
    int h = hash_alg(t->size, k);
    struct hash_chain *chain = &t->chains[h];
    struct hash_chain *new_chain;

    while(chain->next != NULL) {
        chain = chain->next;
        if(!strcmp(chain->k, k)) {
            chain->v = v;
        }
    }

    new_chain = xmalloc(sizeof(struct hash_chain));
    chain->next = new_chain;
    new_chain->k = k;
    new_chain->v = v;
    new_chain->next = NULL;
}
