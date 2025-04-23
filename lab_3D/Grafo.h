//Arquivo Grafo.h

typedef struct grafo Grafo;

Grafo* cria_Grafo(int nro_vertices, int grau_max);

void libera_Grafo(Grafo* gr);

int insereAresta(Grafo* gr, int orig, int dest, int eh_digrafo, int peso);

void arvoreGeradoraMinimaPRIM_Grafo(Grafo *gr, int ini, int *pai);


