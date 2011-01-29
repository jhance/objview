#include <stdio.h>
#include <stdlib.h>

#include <GL/glut.h>

#include "objload.h"

static struct obj_model *cube;

void handleKeypress(unsigned char key, int x, int y) {
    if(key == 27)
        exit(0);
}

void initRendering(void) {
    glEnable(GL_DEPTH_TEST);
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
/*
    glBegin(GL_QUADS);
    glVertex3f(-0.7f, -1.5f, -5.0f);
    glVertex3f(0.7f, -1.5f, -5.0f);
    glVertex3f(0.4f, -0.5f, -5.0f);
    glVertex3f(-0.4f, -0.5f, -5.0f);
    glEnd();
    */

    draw_model(cube);

    glutSwapBuffers();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(400, 400);

    glutCreateWindow("Model Viewer");
    initRendering();

    glutDisplayFunc(drawScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);

    cube = load_model("cube/cube.obj");

    glutMainLoop();

    return 0;
}
