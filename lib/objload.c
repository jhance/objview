#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>

#include <objload.h>
#include <xmalloc.h>
#include <tga.h>
#include <hash.h>

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
void *buf_expand(void *buf, size_t unit, size_t *cs, size_t rs) {
    if(rs > *cs) {
        rs += 4;
        buf = realloc(buf, rs * unit);
        *cs = rs; /* update the current size */
    }

    return buf;
}

/* loads a line of data into the ld and m */
static void parse_line(char *l, struct obj_load_data *ld, struct obj_model *m) {
    if(l[0] == 'v') {
        if(l[1] == 't') { struct vertex_uv vuv;
            sscanf(l, "vt %f %f", &vuv.u, &vuv.v);

            m->num_uvs++;
            m->uvs = buf_expand(m->uvs, sizeof(struct vertex_uv),
                    &ld->bufsize_uvs, m->num_uvs);
            m->uvs[m->num_uvs - 1] = vuv;
        }
        else {
            struct vertex_coord vc;
            sscanf(l, "v %f %f %f", &vc.x, &vc.y, &vc.z);
       
            m->num_coords++;
            m->coords = buf_expand(m->coords, sizeof(struct vertex_coord),
                    &ld->bufsize_coords, m->num_coords);

            m->coords[m->num_coords - 1] = vc;
        }
    }
    else if(l[0] == 'f') {
        struct face f;
        size_t bufsize_vertices = 0;

        l+=2;

        f.num_vertices = 0;
        f.indices = NULL;
        f.texture_id = ld->cur_texture;

        while(1) {
            int coordIndex;
            int uvIndex;
            int r = sscanf(l, "%d/%d", &coordIndex, &uvIndex);

            if(r != 2) {
                break;
            }
            
            f.num_vertices++;
            
            f.indices = 
                buf_expand(f.indices, sizeof(struct vertex_index),
                        &bufsize_vertices, f.num_vertices);

            /* obj format indexes based on 1, we index based on 0 */
            f.indices[f.num_vertices - 1].coord = coordIndex - 1;
            f.indices[f.num_vertices - 1].uv = uvIndex - 1;

            while(*l != ' ' && *l != '\n' && l) {
                l++;
            }
            l++;
        }

        m->num_faces++;
        m->faces = buf_expand(m->faces, sizeof(struct face),
                &ld->bufsize_faces, m->num_faces);
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
    char *filepath;
    FILE *fin;
    char *line;
    struct obj_load_data ld;
    struct obj_model *model;
    
    /* remember the null byte! */
    filepath = xmalloc(strlen(directory) + strlen(rfilepath) + 2);
    strcpy(filepath, directory);
    strcat(filepath, "/");
    strcat(filepath, rfilepath);
    fin = fopen(filepath, "r");

    line = xmalloc(255);

    ld.bufsize_coords = 0;
    ld.bufsize_uvs = 0;
    ld.bufsize_faces = 0;
    ld.material_table = hash_init(10);
    ld.directory = directory;
    
    model = xmalloc(sizeof(struct obj_model));
    model->num_coords = 0;
    model->num_uvs = 0;
    model->num_faces = 0;
    model->coords = NULL;
    model->uvs = NULL;
    model->faces = NULL;

    while((line = fgets(line, 255, fin))) {
        /* empty line */
        if(strlen(line) == 0) {
            continue;
        }

        /* ignore comments */
        if(line[0] == '#') {
            continue;
        }

        parse_line(line, &ld, model);
    }

    return model;
}

void draw_model(struct obj_model *model) {
    int i;
    for(i = 0; i < model->num_faces; i++) {
        int j;
        struct face f = model->faces[i];

        glBindTexture(GL_TEXTURE_2D, f.texture_id);
        glBegin(GL_POLYGON);

        for(j = 0; j < f.num_vertices; j++) {
            struct vertex_coord coord = model->coords[f.indices[j].coord];
            struct vertex_uv uv = model->uvs[f.indices[j].uv];

            glTexCoord2d(uv.u, uv.v);
            glVertex3f(coord.x, coord.y, coord.z);
        }
        glEnd();
    }
}
