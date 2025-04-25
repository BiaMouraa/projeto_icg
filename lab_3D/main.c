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

    if (lab3d->girandoEsquerda) {
        lab3d->idx_anguloZ = (lab3d->idx_anguloZ - 1) % NanguloZ;
        if (lab3d->idx_anguloZ < 0) lab3d->idx_anguloZ = NanguloZ - 1;
    }
    if (lab3d->girandoDireita) {
        lab3d->idx_anguloZ = (lab3d->idx_anguloZ + 1) % NanguloZ;
    }

    desenha_labirinto3d(lab3d);

    glutSwapBuffers();
    usleep(100000); // Ajuste para controlar a velocidade
}

void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_RIGHT:
            lab3d->girandoDireita = 1;
            lab3d->girandoEsquerda = 0;
            break;
        case GLUT_KEY_LEFT:
            lab3d->girandoEsquerda = 1;
            lab3d->girandoDireita = 0;
            break;
        case GLUT_KEY_UP:
            caminhaPraFrente(lab3d);
            break;
        case GLUT_KEY_DOWN:
            caminhaPraTras(lab3d);
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

void specialKeysUp(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_RIGHT:
            lab3d->girandoDireita = 0;
            break;
        case GLUT_KEY_LEFT:
            lab3d->girandoEsquerda = 0;
            break;
        default:
            break;
    }
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

    glEnable(GL_TEXTURE_2D);
    carregaTextura();
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
    glutSpecialUpFunc(specialKeysUp);
    glutKeyboardFunc(keyboard);

    glutMainLoop();

    destroi_labirinto3D(lab3d);
    return 0;
}
