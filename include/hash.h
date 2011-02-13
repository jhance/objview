#ifndef HASH_H
#define HASH_H

#include <GL/glut.h>

struct hash_chain {
    char *k;
    GLuint v;

    struct hash_chain *next;
};

struct hash_table {
    size_t size;
    struct hash_chain *chains;
};

struct hash_table hash_init(size_t size);
GLuint hash_lookup(struct hash_table *t, char *k);
void hash_put(struct hash_table *t, char *k, GLuint v);

#endif
