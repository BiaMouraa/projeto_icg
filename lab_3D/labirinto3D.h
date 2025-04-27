#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

extern GLuint texID_ceu;
void carregaTextura();
void carregaTexturaParede();
void carregaTexturaChao();

extern float sizeX;
extern float sizeY;
#define tamanho 512
#define incr 0.01
#define N 10
#define TAM (2.0f / N)
#define MAT2X(j) ((j)* TAM - 1.0)
#define MAT2Y(i) (1.0 - (i) * TAM)

#define NanguloZ 64
// int girandoEsquerda = 0;
// int girandoDireita = 0;
// float anguloGiro = 0.0f;
// const float velocidadeGiro = 1.0f;

typedef struct labirinto3D{
    double posX, posY;
    int idx_anguloZ;
    int idx_anguloZ_ant;
    int giro;
    int giro_dir;
    int idx_passo;
    struct lista_paredes *paredes;
    int girandoEsquerda;
    int girandoDireita;
    float anguloGiro;
    float velocidadeGiro;
    int movendoFrente, movendoTras;
    float stamina, maxStamina;
    float staminaRegenRate;
    float staminaDrainRate;
}Labirinto3D;

void viraEsquerda(Labirinto3D* lab3d);
void viraDireita(Labirinto3D* lab3d);

void caminhaPraFrente(Labirinto3D* lab3d);
void caminhaPraTras(Labirinto3D* lab3d);

void desenha_labirinto3d(Labirinto3D* lab3d);
Labirinto3D* cria_labirinto3D();
void destroi_labirinto3D(Labirinto3D* lab3d);
