#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

extern GLuint texID_ceu;
void carregaTextura();

extern float sizeX;
extern float sizeY;
#define tamanho 512
#define incr 0.01

typedef struct labirinto3D Labirinto3D;

void viraEsquerda(Labirinto3D* lab3d);
void viraDireita(Labirinto3D* lab3d);

void caminhaPraFrente(Labirinto3D* lab3d);
void caminhaPraTras(Labirinto3D* lab3d);

void desenha_labirinto3d(Labirinto3D* lab3d);
Labirinto3D* cria_labirinto3D();
void destroi_labirinto3D(Labirinto3D* lab3d);
