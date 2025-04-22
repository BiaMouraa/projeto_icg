#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "Labirinto.h"

int tipo = 0;
LABIRINTO *lab = NULL;

// Função de desenho
void display(void) {
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glPushMatrix();
    desenha_labirinto(lab, tipo);
    glPopMatrix();

    glutSwapBuffers();
}

// Atualização com tempo (opcional se precisar animação)
void timer(int value) {
    glutPostRedisplay();          // força redesenho
    glutTimerFunc(10, timer, 0);  // agenda próxima chamada
}

// Interação com teclado
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27:  // ESC
            destroi_labirinto(lab);
            exit(0);
            break;

        case ' ':  // Espaço
            tipo = (tipo + 1) % 4;
            if (tipo > 3)
                inicia_animacao(lab);
            break;
    }

    glutPostRedisplay();
}

// Inicialização do contexto e labirinto
void init(void) {
    srand(time(NULL));
    lab = monta_labirinto();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1); // define coordenadas de visualização
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(650, 650);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Labirinto com GLUT");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(10, timer, 0);

    glutMainLoop();

    return 0;
}
