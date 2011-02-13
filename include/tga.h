#ifndef TGA_H
#define TGA_H

#include <stdint.h>
#include <GL/glut.h>

struct tga_pixel {
    uint8_t b;
    uint8_t g;
    uint8_t r;
};

struct tga_image  {
    uint8_t image_id_length;
    uint8_t color_map_type;
    uint8_t image_type;

    uint16_t color_map_offset;
    uint16_t color_map_length;
    uint8_t color_map_entry_size;

    uint16_t x_origin;
    uint16_t y_origin;
    uint16_t width;
    uint16_t height;

    uint8_t pixel_depth;
    uint8_t image_desc;

    char *image_id;
    struct tga_pixel *color_map;
    struct tga_pixel *image;
};

GLuint load_tga_texture(char *filepath);

#endif
