#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glut.h>

#include "xmalloc.h"
#include "objload.h"

static struct obj_model *cube;

static float rotation = 0;

void handleKeypress(unsigned char key, int x, int y) {
    if(key == 27)
        exit(0);
}

void initRendering(void) {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
}

void handleResize(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();
    gluPerspective(45, (double) w / (double) h, 1, 200);
}

void drawScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glPushMatrix();
    glTranslatef(0, 0.0f, -5.0f);
    glRotatef(rotation, 1, 1, 1);
    draw_model(cube);
    glPopMatrix();

    glutSwapBuffers();
}

void rotate(int t) {
    rotation += 0.5f;
    drawScene();

    glutTimerFunc(t, rotate, t);
}

int main(int argc, char **argv) {
    char *dir, *fname;

    if(argc < 2) {
        fprintf(stderr, "Error: Not enough args\n");
        return 1;
    }
    dir = argv[1];

    if(argc == 2) {
        fname = xmalloc(strlen(dir) + 5);
        strcpy(fname, dir);
        strcat(fname, ".obj");
    }
    else {
        fname = argv[2];
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(400, 400);

    glutCreateWindow("Model Viewer");
    initRendering();

    glutDisplayFunc(drawScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);

    cube = load_model(dir, fname);

    glutTimerFunc(10, rotate, 10);

    glutMainLoop();

    return 0;
}
