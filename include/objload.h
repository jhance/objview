#ifndef OBJLOAD_H
#define OBJLOAD_H

#include <stdlib.h>
#include <GL/glut.h>

struct vertex_index {
    int coord;
    int uv;
};

struct vertex_coord {
    float x;
    float y;
    float z;
};

struct vertex_uv {
    float u;
    float v;
};

struct face {
    size_t num_vertices;
    struct vertex_index *indices;
    GLuint texture_id;
};

struct obj_model {
    size_t num_coords;
    size_t num_uvs;
    size_t num_faces;
    struct vertex_coord *coords;
    struct vertex_uv *uvs;
    struct face *faces;
};

struct obj_model *load_model(char *path, char *tpath);
void draw_model(struct obj_model *model);

#endif
