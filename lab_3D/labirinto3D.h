#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

extern float sizeX;
extern float sizeY;
#define tamanho 512
#define incr 0.01

void viraEsquerda();
void viraDireita();

void caminhaPraFrente();
void caminhaPraTras();

void viewport_topo();
void viewport_perspectiva();

void draw_perspectiva();
void draw_obj();
void draw_topo();
