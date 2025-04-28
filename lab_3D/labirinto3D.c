#include "labirinto3D.h"
#include "Grafo.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>   
#define M_PI 3.14159265358979323846
#include "stb_image.h"

//texturas
GLuint texID_teto;
GLuint texID_parede;
GLuint texID_chao;

//=========================================================
//constantes de movimento
#define GIRO_VAL 300
#define GIRO_ANG (90.0/GIRO_VAL)
#define MAX_DIST_PAREDE 0.01f
#define incr 0.01
float sizeX = 0.3f;
float sizeY = 1.0f;

//=======================================================
int Nparedes = 4;

/*paredes do labirinto
tipo = 0 -parede horizontal
tipo = 1 - parede vertical*/
struct coord{
    int tipo; //0 horizontal
    double x1, y1, x2, y2;
};

struct lista_paredes{
    int MAX, qtd;
    struct coord *CO;
};

//protótipos
void viewport_topo(Labirinto3D* lab3d);
void viewport_perspectiva(Labirinto3D* lab3d);
void draw_obj();
void draw_topo(struct lista_paredes *paredes);
void draw_perspectiva(struct lista_paredes *paredes);
struct lista_paredes *monta_labirinto();
double distanciaSegmentoReta(double xk, double yk, struct coord *p);
int pontoDentroSegmentoReta(double xk, double yk, struct coord *p, int tipo_parede);

//=======================================================
//variáveis auxiliares de movimento
double posXY[2] = {0.0f,0.0f};
int anguloZTopo[64];
int anguloZPersp[64];
int idx_anguloZ = 0;
int idx_anguloZ_ant = 0;
int giro = 0, giro_dir = 1;

int Npasso = 21, idx_passo = 10;
const double passo[21] = {-0.0167f, -0.0165f, -0.0160f, -0.0153f, -0.0144f, -0.0133f,
                          -0.0123f, -0.0114f, -0.0106f, -0.0102f, -0.0100f, -0.0102f,
                          -0.0106f, -0.0114f, -0.0123f, -0.0133f, -0.0144f, -0.0153f,
                          -0.0160f, -0.0165f, -0.0167};

//=======================================================
//inicializa tabela de ângulos
void inicializaAngulos() {
    for(int i = 0; i < NanguloZ; i++) {
        anguloZTopo[i] = (int)(360.0 * i / NanguloZ) % 360;
        anguloZPersp[i] = (anguloZTopo[i] + 180) % 360;
    }
}

void carregaTextura() {
    //carrega a imagem da textura, guardando parâmetros de altura, largura e canais de cor
    int width, height, nrChannels;
    unsigned char *data = stbi_load("ceu.png", &width, &height, &nrChannels, 0);
    
    if (data) {
        glGenTextures(1, &texID_teto);
        glBindTexture(GL_TEXTURE_2D, texID_teto);
        
        //configura os parâmetros da textura
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        //carrega a imagem baseada no número de canais
        GLenum format = GL_RGB;
        if (nrChannels == 4)
            format = GL_RGBA;
        else if (nrChannels == 1)
            format = GL_RED;
        
        //cria a textura
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        stbi_image_free(data);
    } else {
        printf("Falha ao carregar textura\n");
        exit(1);
    }
}

void carregaTexturaParede() {
    //carrega a imagem da textura, guardando parâmetros de altura, largura e canais de cor
    int width, height, nrChannels;
    const char *fname = "parede.png";

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd))) {
        printf("Diretório de trabalho: %s\n", cwd);
    }
    printf("Tentando stbi_load(\"%s\")...\n", fname);
    
    unsigned char *data = stbi_load(fname, &width, &height, &nrChannels, 0);
    if (!data) {
        fprintf(stderr, "Falha ao carregar textura de parede: %s\n",
                stbi_failure_reason());
        exit(1);
    }
    printf("Carregou %dx%d, %d canais\n", width, height, nrChannels);

    //configura os parâmetros da textura
    glGenTextures(1, &texID_parede);
    glBindTexture(GL_TEXTURE_2D, texID_parede);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //carrega a imagem baseada no número de canais
    GLenum format = (nrChannels == 4 ? GL_RGBA :
                     nrChannels == 1 ? GL_RED :
                     GL_RGB);

    //cria a textura
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void carregaTexturaChao() {
    //carrega a imagem da textura, guardando parâmetros de altura, largura e canais de cor
    int width, height, nrChannels;
    unsigned char *data = stbi_load("chao.png", &width, &height, &nrChannels, 0);
    if (!data) {
        fprintf(stderr, "Falha ao carregar textura de chao: %s\n", stbi_failure_reason());
        exit(1);
    }

     //configura os parâmetros da textura
    glGenTextures(1, &texID_chao);
    glBindTexture(GL_TEXTURE_2D, texID_chao);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //carrega a imagem baseada no número de canais
    GLenum fmt = (nrChannels == 4 ? GL_RGBA :
                  nrChannels == 1 ? GL_RED :
                  GL_RGB);

    //cria a textura
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, width, height, 0, fmt, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Labirinto3D* cria_labirinto3D(){
    Labirinto3D* lab3d = malloc(sizeof(Labirinto3D));
    if(lab3d != NULL){
        //valores inciais para posição, ângulo e estado de movimento
        lab3d->posX = 1.0 - TAM/2;
        lab3d->posY = -1.0 + TAM/2;
        lab3d->idx_anguloZ = 32;
        lab3d->idx_anguloZ_ant =0;
        lab3d->giro = 0;
        lab3d->giro_dir = 1;
        lab3d->idx_passo = 10;
        lab3d->girandoEsquerda = 0;
        lab3d->girandoDireita = 0;
        lab3d->anguloGiro = 0.0f;
        lab3d->velocidadeGiro = 0.01f;

        //valores inciais para âmgulos e paredes
        inicializaAngulos();
        lab3d->paredes = monta_labirinto();

        //valores inciais para movimentação e estamina
        lab3d->movendoFrente = lab3d->movendoTras = 0;
        lab3d->maxStamina = 10.0f;       
        lab3d->stamina = lab3d->maxStamina;
        lab3d->staminaRegenRate = 2.0f; 
        lab3d->staminaDrainRate = 2.0f; 

    }
    return lab3d;
}


//libera as estruturas do labirinto
void destroi_labirinto3D(Labirinto3D* lab3d){
    free(lab3d->paredes->CO);
    free(lab3d->paredes);
    free(lab3d);
    glDeleteTextures(1, &texID_teto); 
}

void viraEsquerda(Labirinto3D* lab3d){
    if(lab3d == NULL)
        return;

    //verifica giro e inicia rotação para esquerda
    if(lab3d->giro == 0){
        lab3d->giro = GIRO_VAL;
        lab3d->giro_dir = -1;
        lab3d->idx_anguloZ_ant = lab3d->idx_anguloZ;

        //atualiza índice do ângulo (circular)
        lab3d->idx_anguloZ--;
        if(lab3d->idx_anguloZ < 0)
            lab3d->idx_anguloZ = NanguloZ - 1;
    }
}

void viraDireita(Labirinto3D* lab3d){
    if(lab3d == NULL)
        return;

    //verifica giro e inicia rotação para direita
    if(lab3d->giro == 0){
        lab3d->giro = GIRO_VAL;
        lab3d->giro_dir = +1;
        lab3d->idx_anguloZ_ant = lab3d->idx_anguloZ;

        //atualiza índice do ângulo (circular)
        lab3d->idx_anguloZ = (lab3d->idx_anguloZ + 1) % NanguloZ;
    }
}

double distanciaSegmentoReta(double xk, double yk, struct coord *p){
    //produto vetorial ao quadrado
    double numerador = (xk - p->x1) * (p->y2 - p->y1) -
                       (yk - p->y1) * (p->x2 - p->x1);
    numerador = numerador * numerador;

    //quadrado do comprimento do segmento
    double denominador = (p->x1 - p->x2) * (p->x1 - p->x2) +
                         (p->y1 - p->y2) * (p->y1 - p->y2);

    //distância da reta ao ponto
    double t = fabs(numerador / denominador);
    return t;
}

int pontoDentroSegmentoReta(double xk, double yk, struct coord *p, int tipo_parede){
    //verifica se o ponto está dentro dos limites do segmento
    if(tipo_parede == 0){
        ///parede horizontal
        if((p->x1 <= xk && xk <= p->x2) || (p->x2 <= xk && xk <= p->x1))
           return 1;
    }else{
        ///parede vertical
        if((p->y1 <= yk && yk <= p->y2) || (p->y2 <= yk && yk <= p->y1))
            return 1;
    }

    return 0;
}

void caminhaXY(Labirinto3D* lab3d, double dx, double dy, int frente){
    if(lab3d == NULL)
        return;

    //calcula a nova posição após translação de dx e dy
    double xk = lab3d->posX + dx;
    double yk = lab3d->posY + dy;
    double dist_antes, dist_depois;

    int i, caminha = 1;
    int tipo_parede = (fabs(dx) > fabs(dy)) ? 1 : 0; //1 = vertical, 0 = horizontal

    struct lista_paredes *paredes = lab3d->paredes;

    //verifica colisão
    for(i = 0; i < paredes->qtd; i++){
        if(paredes->CO[i].tipo == tipo_parede){

            if(!pontoDentroSegmentoReta(xk,yk,&paredes->CO[i],tipo_parede)){
                continue;
            }

            dist_antes = distanciaSegmentoReta(lab3d->posX,lab3d->posY,&paredes->CO[i]);
            dist_depois = distanciaSegmentoReta(xk,yk,&paredes->CO[i]);

            if(dist_depois < dist_antes && dist_depois < MAX_DIST_PAREDE){
                caminha = 0;
                break;
            }
        }
    }

    //atualiza posição
    if(caminha){
        lab3d->posX = xk;
        lab3d->posY = yk;
        if(frente)
            lab3d->idx_passo = (lab3d->idx_passo + 1) % Npasso;
        else{
            lab3d->idx_passo--;
            if(lab3d->idx_passo < 0)
                lab3d->idx_passo = Npasso - 1;
        }
    }
}


void caminhaPraFrente(Labirinto3D* lab3d){
    if(lab3d == NULL) return;
    if(lab3d->giro > 0) return;

    //calcula deslocamento baseado no ângulo atual e atualiza a posição
    double angle = anguloZTopo[lab3d->idx_anguloZ] * M_PI / 180.0;
    double dx = -incr * sin(angle);
    double dy = -incr * cos(angle);
    caminhaXY(lab3d, dx, dy, 1);
}

void caminhaPraTras(Labirinto3D* lab3d){
    if(lab3d == NULL) return;
    if(lab3d->giro > 0) return;

    //calcula deslocamento baseado no ângulo atual e atualiza a posição
    double angle = anguloZTopo[lab3d->idx_anguloZ] * M_PI / 180.0;
    double dx = incr * sin(angle);
    double dy = incr * cos(angle);
    caminhaXY(lab3d, dx, dy, 0);
}

void desenha_labirinto3d(Labirinto3D* lab3d){
    if(lab3d == NULL)
        return;

    //se a tecla X estiver na opção de desenhar o topo, renderiza essa visão
    if (mostrarTopo) {
        viewport_topo(lab3d);
    }
    viewport_perspectiva(lab3d); //renderiza visão em perspectiva
}

//=======================================================
void viewport_perspectiva(Labirinto3D* lab3d){
    if(lab3d == NULL)
        return;

    //configura viewport e projeção de perspectiva
    glViewport(0,0,tamanho,tamanho);
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective(45.0,1.0,0.01,200.0);

    //prepara o modelview para a câmera
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glRotatef(-90, 1.0f, 0.0f, 0.0f); //ajusta a visao

    //aplica a rotação de giro suave
    if(lab3d->giro > 0) {
        lab3d->giro--;
        float angle_step = 360.0f / NanguloZ;
        float var_ang = lab3d->giro_dir * (GIRO_VAL - lab3d->giro) * (angle_step/GIRO_VAL);
        glRotatef(var_ang + anguloZPersp[lab3d->idx_anguloZ_ant], 0.0f, 0.0f, 1.0f);
    } else {
        glRotatef(anguloZPersp[lab3d->idx_anguloZ], 0.0f, 0.0f, 1.0f);
    }

    glPushMatrix();
    glTranslatef(-lab3d->posX,-lab3d->posY, passo[lab3d->idx_passo]-0.03f);
    draw_perspectiva(lab3d->paredes);
    glPopMatrix();
}

void draw_perspectiva(struct lista_paredes *paredes) {
    //desenha chão texturizado
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID_chao);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
      //repete N×N tiles (uma célula por tile)
      glTexCoord2f(0.0f, 0.0f);        glVertex3f( 1.0f,  1.0f, 0.0f);
      glTexCoord2f((float)N, 0.0f);    glVertex3f( 1.0f, -1.0f, 0.0f);
      glTexCoord2f((float)N, (float)N);glVertex3f(-1.0f, -1.0f, 0.0f);
      glTexCoord2f(0.0f,   (float)N);  glVertex3f(-1.0f,  1.0f, 0.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    //desenha teto texturizado
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID_teto);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f,  1.0f, 0.1f);
      glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, 0.1f);
      glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, 0.1f);
      glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, 0.1f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    // parâmetros de espessura e altura das paredes
    const float t = 0.05f;   // meia-espessura no plano XY
    const float h = 0.3f;   // altura em Z

    //desenha paredes texturizadas
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID_parede);
    glColor3f(1.0f, 1.0f, 1.0f);

    for(int i = 0; i < paredes->qtd; i++) {
        struct coord *p = &paredes->CO[i];
        float dx = p->x2 - p->x1;
        float dy = p->y2 - p->y1;
        float len = sqrtf(dx*dx + dy*dy);
        if (len < 1e-6f) continue;

        //deslocamento perpendicular ao segmento
        float ox =  dy / len * t;
        float oy = -dx / len * t;

        //face frontal da parede
        glBegin(GL_QUADS);
          glTexCoord2f(0.0f, 0.0f); glVertex3f(p->x1 + ox, p->y1 + oy, 0.0f);
          glTexCoord2f(1.0f, 0.0f); glVertex3f(p->x2 + ox, p->y2 + oy, 0.0f);
          glTexCoord2f(1.0f, 1.0f); glVertex3f(p->x2 + ox, p->y2 + oy,    h);
          glTexCoord2f(0.0f, 1.0f); glVertex3f(p->x1 + ox, p->y1 + oy,    h);
        glEnd();

        //face traseira da parede
        glBegin(GL_QUADS);
          glTexCoord2f(0.0f, 0.0f); glVertex3f(p->x2 - ox, p->y2 - oy, 0.0f);
          glTexCoord2f(1.0f, 0.0f); glVertex3f(p->x1 - ox, p->y1 - oy, 0.0f);
          glTexCoord2f(1.0f, 1.0f); glVertex3f(p->x1 - ox, p->y1 - oy,    h);
          glTexCoord2f(0.0f, 1.0f); glVertex3f(p->x2 - ox, p->y2 - oy,    h);
        glEnd();

        //topo da parede
        glBegin(GL_QUADS);
          glTexCoord2f(0.0f, 0.0f); glVertex3f(p->x1 + ox, p->y1 + oy,    h);
          glTexCoord2f(1.0f, 0.0f); glVertex3f(p->x2 + ox, p->y2 + oy,    h);
          glTexCoord2f(1.0f, 1.0f); glVertex3f(p->x2 - ox, p->y2 - oy,    h);
          glTexCoord2f(0.0f, 1.0f); glVertex3f(p->x1 - ox, p->y1 - oy,    h);
        glEnd();
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

//=======================================================
void viewport_topo(Labirinto3D* lab3d){
    if(lab3d == NULL)
        return;

    //configura viewport e projeção de perspectiva
    glViewport(3*tamanho/4.0 - 20,5,tamanho/4,tamanho/4);
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    //prepara o modelview para desenha o topo
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    //desenha o mapa
    glPushMatrix();
        draw_topo(lab3d->paredes);
    glPopMatrix();

    //desenha o jogador no mapa
    glPushMatrix();
    glTranslatef(lab3d->posX,lab3d->posY,0.0f);

    //aplica a rotação de giro suave
    if(lab3d->giro > 0) {
        lab3d->giro--;
        float angle_step = 360.0f / NanguloZ;
        float var_ang = lab3d->giro_dir * (GIRO_VAL - lab3d->giro) * (angle_step/GIRO_VAL);
        glRotatef(-(var_ang + anguloZTopo[lab3d->idx_anguloZ_ant]), 0.0f, 0.0f, 1.0f);
    } else {
        glRotatef(-anguloZTopo[lab3d->idx_anguloZ], 0.0f, 0.0f, 1.0f);
    }
        draw_obj(); //triângulo representando o jogador
    glPopMatrix();
}

void draw_obj(){
    glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f);     // triangulo vermelho
        glVertex3f( 0.05f, 0.05f, -0.05f);
        glVertex3f( -0.05f,0.05f, -0.05f);
        glVertex3f( 0.0f,-2*0.05f, -0.05f);
    glEnd();
}

void draw_topo(struct lista_paredes *paredes){
    //desenha o contorno
    glLineWidth(2);
    glColor3f(0.0f, 0.0f, 0.0f);    
    glBegin(GL_LINE_LOOP);
        glVertex3f( 1.0f, 1.0f, 0.0f);
        glVertex3f( 1.0f,-1.0f, 0.0f);
        glVertex3f(-1.0f,-1.0f, 0.0f);
        glVertex3f(-1.0f, 1.0f, 0.0f);
    glEnd();

    //desenha paredes internas (azul)
    int i;
    glColor3f(0.0,0.0,1.0);
    glLineWidth(2);

    for(i=0; i< paredes->qtd; i++){
        glBegin(GL_LINES);
            glVertex3f(paredes->CO[i].x1, paredes->CO[i].y1, 0.00f);
            glVertex3f(paredes->CO[i].x2, paredes->CO[i].y2, 0.00f);
        glEnd();
    }

    //desenha plano de fundo (cinza claro)
    glBegin(GL_QUADS);
        glColor3f(0.95f, 0.95f, 0.95f);    
        glVertex3f( 1.0f, 1.0f, 0.0f);
        glVertex3f( 1.0f,-1.0f, 0.0f);
        glVertex3f(-1.0f,-1.0f, 0.0f);
        glVertex3f(-1.0f, 1.0f, 0.0f);
    glEnd();

}

void insere_parede(struct lista_paredes *li, double x1, double y1, double x2, double y2, int tipo){
    if(li->qtd == li->MAX){ //aumenta o tamanho da lista quando necessario
        li->MAX = li->MAX * 1.5;
        struct coord *CO = realloc(li->CO,li->MAX * sizeof(struct coord));
        if(CO == NULL){
            printf("ERRO REALLOC\n");
            system("pause");
            exit(1);
        }

        li->CO = CO;
    }
    //insere a parede na lista
    li->CO[li->qtd].tipo = tipo;
    li->CO[li->qtd].x1 = x1;
    li->CO[li->qtd].y1 = y1;
    li->CO[li->qtd].x2 = x2;
    li->CO[li->qtd].y2 = y2;
    li->qtd++;
}

struct lista_paredes *monta_labirinto(){
    int i,j;

    //grafo NxN vértices para gerar o labirinto
    Grafo* gr = cria_Grafo(N*N, 4);

    //insere as arestas no grafo
    for(i=0; i<N; i++){
        for(j=0; j<N; j++){
            int v1 = i * N + j;
            if(j+1 < N){
                int v2 = i * N + (j+1);
                if(!insereAresta(gr,v1,v2,0,rand()%9 + 1))
                    printf("Erro 1: %d %d",i,j);
            }

            if(i+1 < N){
                int v2 = (i+1) * N + j;
                if(!insereAresta(gr,v1,v2,0,rand()%9 + 1))
                    printf("Erro 2: %d %d",i,j);
            }
        }
    }

    
    //calcula a árvore geradora
    int *arv = malloc(N*N*sizeof(int));
    arvoreGeradoraMinimaPRIM_Grafo(gr, 0, arv);
    libera_Grafo(gr);

    //inicia a lista de paredes
    struct lista_paredes *li;
    li = malloc(sizeof(struct lista_paredes));
    li->qtd = 0;
    li->MAX = 500;
    li->CO = malloc(500*sizeof(struct coord));

    //calcula as paredes
    for(i=0; i<N; i++){
        for(j=0; j<N; j++){
            int v1 = i * N + j;
            if(j+1 < N){
                int v2 = i * N + (j+1);
                if(arv[v2] != v1 && arv[v1] != v2){
                    //parede vertical
                    insere_parede(li, MAT2X(j+1), MAT2Y(i), MAT2X(j+1), MAT2Y(i+1), 1);
                }
            }
            if(i+1 < N){
                int v2 = (i+1) * N + j;
                if(arv[v2] != v1 && arv[v1] != v2){
                    //parede horizontal
                    insere_parede(li, MAT2X(j), MAT2Y(i+1), MAT2X(j+1), MAT2Y(i+1), 0);
                }
            }
        }
    }
    //insere as paredes de fora
    insere_parede(li, MAT2X(0.025), MAT2Y(0.025),MAT2X(0.025), MAT2Y(N-0.025),1);
    insere_parede(li, MAT2X(0.025), MAT2Y(N-0.025),MAT2X(N-0.025), MAT2Y(N-0.025),0);
    insere_parede(li, MAT2X(N-0.025), MAT2Y(0.025),MAT2X(N-0.025), MAT2Y(N-0.025),1);
    insere_parede(li, MAT2X(0.025), MAT2Y(0.025),MAT2X(N-0.025), MAT2Y(0.025),0);

    free(arv);
    return li;
}
