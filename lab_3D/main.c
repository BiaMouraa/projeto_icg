#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "labirinto3D.h"

Labirinto3D *lab3d;

void display(void) {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    desenha_labirinto3d(lab3d);

    glutSwapBuffers();
    usleep(1000);
}

void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_RIGHT:
            viraDireita(lab3d);
            break;
        case GLUT_KEY_DOWN:
            caminhaPraTras(lab3d);
            break;
        case GLUT_KEY_LEFT:
            viraEsquerda(lab3d);
            break;
        case GLUT_KEY_UP:
            caminhaPraFrente(lab3d);
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 27) { // ESC
        exit(0);
    }
}

void init(void) {
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(tamanho, tamanho);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Labirinto 3D com GLUT");

    init();

    srand(time(NULL));
    lab3d = cria_labirinto3D();

    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutSpecialFunc(specialKeys);
    glutKeyboardFunc(keyboard);

    glutMainLoop();

    destroi_labirinto3D(lab3d);
    return 0;
}
