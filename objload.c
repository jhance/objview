#include "objload.h"
#include "xmalloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>

struct obj_load_data {
    /* struct obj_model load data */
    size_t bufsize_coords;
    size_t bufsize_uvs;
    size_t bufsize_faces;
};

/* allocates room for 4 more of an object at a time */
static void buf_expand(void **buf, size_t unit, size_t *cs, size_t rs) {
    if(rs > *cs) {
        rs += 4;
        *buf = realloc(*buf, rs * unit);
        *cs = rs; /* update the current size */
    }
}

/* expands a parallel array */
static void buf_expand2(void **buf1, void **buf2, size_t unit1, size_t unit2, size_t *cs, size_t rs) {
    if(rs > *cs) {
        rs += 4;
        *buf1 = realloc(*buf1, rs * unit1);
        *buf2 = realloc(*buf2, rs * unit2);
        *cs = rs;
    }
}

/* loads a line of data into the ld and m */
static void parse_line(char *l, struct obj_load_data *ld, struct obj_model *m) {
    if(l[0] == 'v') {
        if(l[1] == 't') {
            struct vertex_uv vuv;
            sscanf(l, "vt %f %f", &vuv.u, &vuv.v);

            m->num_uvs++;
            buf_expand(&m->uvs, sizeof(struct vertex_uv), &ld->bufsize_uvs, m->num_uvs);
            m->uvs[m->num_uvs - 1] = vuv;
        }
        else {
            struct vertex_coord vc;
            sscanf(l, "v %f %f %f", &vc.x, &vc.y, &vc.z);
       
            m->num_coords++;
            buf_expand(&m->coords, sizeof(struct vertex_coord), &ld->bufsize_coords, m->num_coords);
            m->coords[m->num_coords - 1] = vc;
        }
    }
    else if(l[0] == 'f') {
        l+=2;

        struct face f;
        f.num_vertices = 0;
        f.coord_indices = NULL;
        f.uv_indices = NULL;

        size_t bufsize_vertices = 0;

        while(1) {
            int coordIndex;
            int uvIndex;
            int r = sscanf(l, "%d/%d", &coordIndex, &uvIndex);

            if(r != 2) {
                break;
            }
            
            f.num_vertices++;
            
            buf_expand2(&f.coord_indices, &f.uv_indices, sizeof(int), sizeof(int), &bufsize_vertices, f.num_vertices);
            f.coord_indices[f.num_vertices - 1] = coordIndex;
            f.uv_indices[f.num_vertices - 1] = uvIndex;

            while(*l != ' ' && *l != '\n' && l) {
                l++;
            }
            l++;
        }

        m->num_faces++;
        buf_expand(&m->faces, sizeof(struct face), &ld->bufsize_faces, m->num_faces);
        m->faces[m->num_faces - 1] = f;
    }
}

struct obj_model *load_model(char *filepath) {
    FILE *fin = fopen(filepath, "r");
    char *line = xmalloc(255);

    struct obj_load_data ld; 
    ld.bufsize_coords = 0;
    ld.bufsize_uvs = 0;
    ld.bufsize_faces = 0;
    
    struct obj_model *model = xmalloc(sizeof(struct obj_model));
    model->num_coords = 0;
    model->num_uvs = 0;
    model->num_faces = 0;
    model->coords = NULL;
    model->uvs = NULL;
    model->faces = NULL;

    while(line = fgets(line, 255, fin)) {
        /* empty line */
        if(strlen(line) == 0) {
            continue;
        }

        /* ignore comments */
        if(line[0] == '#') {
            continue;
        }

        printf("Parse line: %s", line);

        parse_line(line, &ld, model);
    }

    return model;
}

void draw_model(struct obj_model *model) {
    int i;
    for(i = 0; i < model->num_faces; i++) {
        struct face f = model->faces[i];
        /* ignore uvs for now */

        glBegin(GL_POLYGON);

        int j;
        for(j = 0; j < f.num_vertices; j++) {
            struct vertex_coord coord = model->coords[f.coord_indices[j]];
            glVertex3f(coord.x, coord.y, coord.z);
        }
        glEnd();
    }
}
