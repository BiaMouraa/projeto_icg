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

// Implementa o carregamento de imagens com stb_image
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Variáveis globais para posição, tempo e estado
double posX_ant = 0.0, posY_ant = 0.0;
struct timeval tempoInicial;
struct timeval prevFrameTime;
int chegou = 0;
double tempoDecorridoAtual = 0.0;
int movendoFrente = 0, movendoTras = 0;
int shiftDown = 0;
int estadoJogo = 0; // 0: menu, 1: jogando, 2: instruções
int mouseX = 0, mouseY = 0;
GLuint texID_fundoBlur;
Labirinto3D *lab3d;

// Função para desenhar uma string na tela em coordenadas 2D
void renderBitmapString(float x, float y, void *font, const char *string) {
    const char *c;
    glRasterPos2f(x, y);
    for (c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

// Verifica se o jogador chegou no checkpoint 
void checaCheckpoint(Labirinto3D *lab3d) {
    if (chegou) return; 

    struct timeval agora;
    gettimeofday(&agora, NULL);

    tempoDecorridoAtual = (agora.tv_sec - tempoInicial.tv_sec) + (agora.tv_usec - tempoInicial.tv_usec) / 1e6;

    double metaX = MAT2X(0) + TAM/2;
    double metaY = MAT2Y(0) - TAM/2;
    double raioCheckpoint = TAM/2;

    double dx = lab3d->posX - metaX;
    double dy = lab3d->posY - metaY;

    if (dx*dx + dy*dy <= raioCheckpoint * raioCheckpoint) {
        chegou = 1;
        printf("Checkpoint: %.2fs\n", tempoDecorridoAtual);
    }
}

// Tela exibida ao chegar no checkpoint
void telaCheckpoint() {
    glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, tamanho, 0, tamanho);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    float centerX = tamanho / 2.0f;
    glColor3f(1.0f, 0.84f, 0.0f); // Dourado

    char buf[128];
    snprintf(buf, sizeof(buf), "VOCE CHEGOU!");
    renderBitmapString(centerX - 90, tamanho/2 + 50, GLUT_BITMAP_TIMES_ROMAN_24, buf);

    snprintf(buf, sizeof(buf), "%.2f segundos", tempoDecorridoAtual);
    renderBitmapString(centerX - 70, tamanho/2 + 10, GLUT_BITMAP_HELVETICA_18, buf);

    snprintf(buf, sizeof(buf), "Aperte ESPACO para jogar novamente");
    renderBitmapString(centerX - 160, tamanho/2 - 40, GLUT_BITMAP_HELVETICA_18, buf);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glutSwapBuffers();
}

// Menu principal
void telaBoasVindas() {
    glClearColor(0, 0, 0, 1); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, tamanho, 0, tamanho);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(1.0f, 0.84f, 0.0f);

    float centerX = tamanho / 2.0f;
    char buf[128];

    snprintf(buf, sizeof(buf), "Bem-vindo ao Labirinto 3D!");
    renderBitmapString(centerX - 120, tamanho/2 + 100, GLUT_BITMAP_TIMES_ROMAN_24, buf);

    snprintf(buf, sizeof(buf), "Jogar");
    renderBitmapString(centerX - 30, tamanho/2 + 30, GLUT_BITMAP_TIMES_ROMAN_24, buf);

    snprintf(buf, sizeof(buf), "Instrucoes");
    renderBitmapString(centerX - 60, tamanho/2 - 30, GLUT_BITMAP_TIMES_ROMAN_24, buf);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glutSwapBuffers();
}

// Instruções do jogo
void telaInstrucoes() {
    glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, tamanho, 0, tamanho);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    float centerX = tamanho / 2.0f;
    glColor3f(1.0f, 0.84f, 0.0f);

    char buf[256];
    snprintf(buf, sizeof(buf), "Instrucoes do Jogo");
    renderBitmapString(centerX - 120, tamanho/2 + 120, GLUT_BITMAP_TIMES_ROMAN_24, buf);

    glColor3f(1,1,1);
    snprintf(buf, sizeof(buf), "- Movimente-se usando as SETAS do teclado.");
    renderBitmapString(centerX - 180, tamanho/2 + 60, GLUT_BITMAP_HELVETICA_18, buf);

    snprintf(buf, sizeof(buf), "- Seu objetivo e chegar a area de chegada o mais rapido possivel.");
    renderBitmapString(centerX - 250, tamanho/2 + 20, GLUT_BITMAP_HELVETICA_18, buf);

    snprintf(buf, sizeof(buf), "- Para sair do jogo, aperte a tecla ESC.");
    renderBitmapString(centerX - 180, tamanho/2 - 20, GLUT_BITMAP_HELVETICA_18, buf);

    snprintf(buf, sizeof(buf), "Clique com o BOTAO DIREITO para voltar ao menu.");
    renderBitmapString(centerX - 220, tamanho/2 - 80, GLUT_BITMAP_HELVETICA_18, buf);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glutSwapBuffers();
}

// Função de renderização
void display(void) {
    if (estadoJogo == 0) {
        telaBoasVindas();
        return;
    }

    if (estadoJogo == 2) {
        telaInstrucoes();
        return;
    }

    struct timeval now;
    gettimeofday(&now, NULL);
    double dt = (now.tv_sec - prevFrameTime.tv_sec) + (now.tv_usec - prevFrameTime.tv_usec) / 1e6;
    prevFrameTime = now;

    if (chegou) {
        telaCheckpoint();
        return;
    }

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (lab3d->girandoEsquerda)
        lab3d->idx_anguloZ = (lab3d->idx_anguloZ - 1 + NanguloZ) % NanguloZ;
    if (lab3d->girandoDireita)
        lab3d->idx_anguloZ = (lab3d->idx_anguloZ + 1) % NanguloZ;

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

    snprintf(buf, sizeof(buf), "Stamina: %.1f / %.1f", lab3d->stamina, lab3d->maxStamina);
    renderBitmapString(10, tamanho - 40, GLUT_BITMAP_HELVETICA_18, buf);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glutSwapBuffers();
    usleep(100000);
}

// Movimentos de giro 
void specialKeys(int key, int x, int y) {
    int mods = glutGetModifiers();
    shiftDown = (mods & GLUT_ACTIVE_SHIFT) ? 1 : 0;

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
            movendoFrente = 1;
            break;
        case GLUT_KEY_DOWN:
            movendoTras = 1;
            break;
    }
    glutPostRedisplay();
}

// Libera as teclas de giro 
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

// Comandos do teclado
void keyboard(unsigned char key, int x, int y) {
    if (key == 27) exit(0); // ESC
    if (key == 32 && chegou) { // ESPAÇO
        printf("Reiniciando jogo...\n");
        destroi_labirinto3D(lab3d);
        lab3d = cria_labirinto3D();
        carregaTextura();
        carregaTexturaParede();
        carregaTexturaChao();
        chegou = 0;
        gettimeofday(&tempoInicial, NULL);
        tempoDecorridoAtual = 0.0;
    }
}

// Funções de clique do mouse
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        if (estadoJogo == 2) {
            estadoJogo = 0;
            printf("Voltando para o menu inicial...\n");
            return;
        }
    }

    if (estadoJogo == 0 && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        mouseX = x;
        mouseY = tamanho - y;
        float centerX = tamanho / 2.0f;

        if (mouseX >= centerX - 50 && mouseX <= centerX + 50 &&
            mouseY >= tamanho/2 + 20 && mouseY <= tamanho/2 + 50) {
            estadoJogo = 1;
            gettimeofday(&tempoInicial, NULL);
            gettimeofday(&prevFrameTime, NULL);
            printf("Iniciando jogo...\n");
        }

        if (mouseX >= centerX - 70 && mouseX <= centerX + 70 &&
            mouseY >= tamanho/2 - 40 && mouseY <= tamanho/2 - 10) {
            estadoJogo = 2;
            printf("Mostrando instruções...\n");
        }
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
    carregaTexturaParede();
    carregaTexturaChao();
}

// Função principal
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
    glutMouseFunc(mouse);

    glutMainLoop();
    destroi_labirinto3D(lab3d);

    return 0;
}
