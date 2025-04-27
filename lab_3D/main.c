#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include "labirinto3D.h"

double posX_ant = 0.0, posY_ant = 0.0;
struct timeval tempoInicial;
struct timeval prevFrameTime;

int chegou = 0;
double tempoDecorridoAtual = 0.0;

int movendoFrente = 0, movendoTras = 0;
int shiftDown = 0;

Labirinto3D *lab3d;

void renderBitmapString(float x, float y, void *font, const char *string) {
    const char *c;
    glRasterPos2f(x, y);
    for (c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

void checaCheckpoint(Labirinto3D *lab3d) {
    if (chegou) return; // já marcou, não precisa checar de novo

    struct timeval agora;
    gettimeofday(&agora, NULL);

    tempoDecorridoAtual = (agora.tv_sec - tempoInicial.tv_sec) + (agora.tv_usec - tempoInicial.tv_usec) / 1e6;

    // Definimos a posição da meta no centro da primeira célula
    double metaX = MAT2X(0) + TAM/2;
    double metaY = MAT2Y(0) - TAM/2;
    double raioCheckpoint = TAM/2; // raio = metade do tamanho de uma célula

    double dx = lab3d->posX - metaX;
    double dy = lab3d->posY - metaY;

    if (dx*dx + dy*dy <= raioCheckpoint * raioCheckpoint) {
        chegou = 1;
        printf("Checkpoint: %.2fs\n", tempoDecorridoAtual);
    }
}

void display(void) {
    struct timeval now;
    gettimeofday(&now, NULL);
    double dt = (now.tv_sec - prevFrameTime.tv_sec)
              + (now.tv_usec - prevFrameTime.tv_usec) / 1e6;
    prevFrameTime = now;

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (lab3d->girandoEsquerda) {
        lab3d->idx_anguloZ = (lab3d->idx_anguloZ - 1 + NanguloZ) % NanguloZ;
    }
    if (lab3d->girandoDireita) {
        lab3d->idx_anguloZ = (lab3d->idx_anguloZ + 1) % NanguloZ;
    }

    int correndo = shiftDown && lab3d->stamina > 0.0f;

    if ((movendoFrente || movendoTras) && correndo) {
        lab3d->stamina -= lab3d->staminaDrainRate * dt;
        if (lab3d->stamina < 0) lab3d->stamina = 0;
    } else {
        lab3d->stamina += lab3d->staminaRegenRate * dt;
        if (lab3d->stamina > lab3d->maxStamina)
            lab3d->stamina = lab3d->maxStamina;
    }

    if (movendoFrente) {
        caminhaPraFrente(lab3d);
        if (correndo) caminhaPraFrente(lab3d);
    }
    if (movendoTras) {
        caminhaPraTras(lab3d);
        if (correndo) caminhaPraTras(lab3d);
    }

    desenha_labirinto3d(lab3d);

    checaCheckpoint(lab3d);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
      glLoadIdentity();
      gluOrtho2D(0, tamanho, 0, tamanho);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
      glLoadIdentity();
      glColor3f(1,1,1);

      char buf[64];
      snprintf(buf, sizeof(buf), "Tempo: %.2f s", tempoDecorridoAtual);
      renderBitmapString(10, tamanho - 20, GLUT_BITMAP_HELVETICA_18, buf);

      snprintf(buf, sizeof(buf), "Stamina: %.1f / %.1f",
               lab3d->stamina, lab3d->maxStamina);
      renderBitmapString(10, tamanho - 40, GLUT_BITMAP_HELVETICA_18, buf);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glutSwapBuffers();
    usleep(100000);
}

void specialKeys(int key, int x, int y) {
    int mods = glutGetModifiers();
    shiftDown = (mods & GLUT_ACTIVE_SHIFT) ? 1 : 0;

    switch (key) {
        case GLUT_KEY_RIGHT:
            lab3d->girandoDireita  = 1;
            lab3d->girandoEsquerda = 0;
            break;
        case GLUT_KEY_LEFT:
            lab3d->girandoEsquerda = 1;
            lab3d->girandoDireita  = 0;
            break;
        case GLUT_KEY_UP:
            movendoFrente = 1;
            break;
        case GLUT_KEY_DOWN:
            movendoTras = 1;
            break;
    }
    glutPostRedisplay();
}

void specialKeysUp(int key, int x, int y) {
    int mods = glutGetModifiers();
    shiftDown = (mods & GLUT_ACTIVE_SHIFT) ? 1 : 0;

    switch (key) {
        case GLUT_KEY_RIGHT:
            lab3d->girandoDireita = 0;
            break;
        case GLUT_KEY_LEFT:
            lab3d->girandoEsquerda = 0;
            break;
        case GLUT_KEY_UP:
            movendoFrente = 0;
            break;
        case GLUT_KEY_DOWN:
            movendoTras = 0;
            break;
    }
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 27) exit(0); 
}

void init(void) {
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_TEXTURE_2D);

    carregaTextura();
    carregaTexturaParede();
    carregaTexturaChao();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(tamanho, tamanho);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Labirinto 3D com GLUT");

    init();

    srand((unsigned)time(NULL));
    lab3d = cria_labirinto3D();

    gettimeofday(&tempoInicial, NULL);
    gettimeofday(&prevFrameTime, NULL);

    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutSpecialFunc(specialKeys);
    glutSpecialUpFunc(specialKeysUp);
    glutKeyboardFunc(keyboard);

    glutMainLoop();

    destroi_labirinto3D(lab3d);
    return 0;
}
