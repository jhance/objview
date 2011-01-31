#include "tga.h"
#include "xmalloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <GL/glut.h>

static struct tga_image load_tga(char *filepath) {
    FILE *fin = fopen(filepath, "rb");

    struct tga_image t;

    fread(&t.image_id_length, sizeof(uint8_t), 1, fin);
    fread(&t.color_map_type, sizeof(uint8_t), 1, fin);
    fread(&t.image_type, sizeof(uint8_t), 1, fin);

    fread(&t.color_map_offset, sizeof(uint16_t), 1, fin);
    fread(&t.color_map_length, sizeof(uint16_t), 1, fin);
    fread(&t.color_map_entry_size, sizeof(uint8_t), 1, fin);

    fread(&t.x_origin, sizeof(uint16_t), 1, fin);
    fread(&t.y_origin, sizeof(uint16_t), 1, fin);
    fread(&t.width, sizeof(uint16_t), 1, fin);
    fread(&t.height, sizeof(uint16_t), 1, fin);

    fread(&t.pixel_depth, sizeof(uint8_t), 1, fin);
    fread(&t.image_desc, sizeof(uint8_t), 1, fin);

    t.image_id = xmalloc(t.image_id_length);
    fread(t.image_id, 1, t.image_id_length, fin);

    if(t.color_map_type != 0) {
        printf("Bleh. Color map in the TGA\n");
        t.color_map = xmalloc(t.color_map_length);

        if(t.color_map_entry_size == 24) {
            fread(t.color_map, sizeof(struct tga_pixel), t.color_map_length, fin);
        }
        else if(t.color_map_entry_size == 32) { /* we now have alpha channel */
            uint8_t buf;
            int i;
            for(i = 0; i < t.color_map_length; i++) {
                fread(t.color_map + i, sizeof(struct tga_pixel), 1, fin);
                fread(&buf, 1, 1, fin); /* discard the alpha channel */
            }
        }
    }

    size_t image_size = t.width * t.height;
    t.image = xmalloc(sizeof(struct tga_pixel) * image_size);

    if(!t.color_map_type) {
        if(t.pixel_depth == 24) {
            fread(t.image, sizeof(struct tga_pixel), image_size, fin);
        }
        else if(t.pixel_depth == 32) { /* we now have alpha channel */
            uint8_t buf;
            int i;
            for(i = 0; i < image_size; i++) {
                fread(t.image + i, sizeof(struct tga_pixel), 1, fin);
                fread(&buf, 1, 1, fin); /* discard the alpha channel */
            }
        }
        else {
            fprintf(stderr, "Error: Unsupported pixel depth");
            exit(1);
        }
    }
    else {
        int i;
        for(i = 0; i < image_size; i++) {
            int pixel_size_bytes = t.pixel_depth / 8;
            int index;

            fread(&index, pixel_size_bytes, 1, fin);
            t.image[i] = t.color_map[index];
        }
    }

    return t;
}

static void free_texture(struct tga_image t) {
    free(t.image);
    free(t.image_id);
}

GLuint load_tga_texture(char *filename) {
    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    struct tga_image t = load_tga(filename);
    uint8_t *data = xmalloc(t.width * t.height * 3);
    int i;
    for(i = 0; i < t.width * t.height; i++) {
        int j = 3 * i;
        data[j++] = t.image[i].r;
        data[j++] = t.image[i].g;
        data[j++] = t.image[i].b;
    }

    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, t.width, t.height, GL_RGB, GL_UNSIGNED_BYTE, data);

    return texture;
}
