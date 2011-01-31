#include "objload.h"
#include "xmalloc.h"
#include "tga.h"
#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>

struct obj_load_data {
    /* struct obj_model load data */
    size_t bufsize_coords;
    size_t bufsize_uvs;
    size_t bufsize_faces;

    /* materials */
    struct hash_table material_table;
    GLuint cur_texture;

    char *directory;
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

static GLuint load_raw_texture(const char *filename) {
    GLuint texture;
    int width, height;
    char *data;
    FILE *fin;

    fin = fopen(filename, "rb");
    if(fin == NULL) return 0;

    width = 512;
    height = 512;
    data = xmalloc(width * height * 3);

    fread(data, width * height * 3, 1, fin);
    fclose(fin);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

    free(data);

    return texture;
}

/* loads a line of data into the ld and m */
static void parse_line(char *l, struct obj_load_data *ld, struct obj_model *m) {
    if(l[0] == 'v') {
        if(l[1] == 't') { struct vertex_uv vuv;
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
        f.texture_id = ld->cur_texture;

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

            /* obj format indexes based on 1, we index based on 0 */
            f.coord_indices[f.num_vertices - 1] = coordIndex - 1;
            f.uv_indices[f.num_vertices - 1] = uvIndex - 1;

            while(*l != ' ' && *l != '\n' && l) {
                l++;
            }
            l++;
        }

        m->num_faces++;
        buf_expand(&m->faces, sizeof(struct face), &ld->bufsize_faces, m->num_faces);
        m->faces[m->num_faces - 1] = f;
    }
    else if (strlen(l) >= 6) {
        if(strncmp(l, "usemtl Material_", strlen("usemtl Material_"))) {
            char *material = l + 16;
            char *material_path = xmalloc(strlen(ld->directory) + strlen(material) + 2);
            strcpy(material_path, ld->directory);
            strcat(material_path, "/");
            strncat(material_path, material, strlen(material) - 1);

            ld->cur_texture = hash_lookup(&ld->material_table, material_path);
        }
    }
}

struct obj_model *load_model(char *directory, char *rfilepath) {
    /* remember the null byte! */
    char *filepath = xmalloc(strlen(directory) + strlen(rfilepath) + 2);
    strcpy(filepath, directory);
    strcat(filepath, "/");
    strcat(filepath, rfilepath);
    FILE *fin = fopen(filepath, "r");
    char *line = xmalloc(255);

    struct obj_load_data ld; 
    ld.bufsize_coords = 0;
    ld.bufsize_uvs = 0;
    ld.bufsize_faces = 0;
    ld.material_table = hash_init(10);
    ld.directory = directory;
    
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
        glBindTexture(GL_TEXTURE_2D, f.texture_id);
        glBegin(GL_POLYGON);

        int j;
        for(j = 0; j < f.num_vertices; j++) {
            struct vertex_coord coord = model->coords[f.coord_indices[j]];
            struct vertex_uv uv = model->uvs[f.uv_indices[j]];

            glTexCoord2d(uv.u, uv.v);
            glVertex3f(coord.x, coord.y, coord.z);
        }
        glEnd();
    }
}
