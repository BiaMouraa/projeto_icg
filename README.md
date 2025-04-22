# projeto_icg

## lab_2d
- monta um labirinto a partir de uma árvore geradora mínima
    - ao executar é possível ver a árvore geradora, as paredes do labirinto e o menor caminho

## Estrutura do Projeto

- `main.c`: ponto de entrada da aplicação
- `Labirinto.c`: funções responsáveis por construir o labirinto
- `Grafo.c`: funções de manipulação de grafos e geração da árvore

---

## Compilação

Certifique-se de ter as bibliotecas OpenGL e GLUT instaladas (ex: `sudo apt install freeglut3-dev` ou `brew install freeglut
`).

Certifique-se de ter o MinGW configurado corretamente com as bibliotecas freeglut, opengl32 e glu32 para windows.


### Linux

```bash
gcc main.c Labirinto.c Grafo.c -o app -lGL -lGLU -lglut -lm
```
### MacOS

```bash
clang main.c Labirinto.c Grafo.c -framework OpenGL -framework GLUT -lm -o app
```

### Windows

```bash
gcc main.c Labirinto.c Grafo.c -o app.exe -lglu32 -lopengl32 -lfreeglut -lm
```