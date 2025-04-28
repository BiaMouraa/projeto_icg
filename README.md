# 🌟 Labirinto 3D - Projeto de ICG 🌟

## Sobre o Projeto
Este projeto foi desenvolvido para a disciplina de **Introdução à Computação Gráfica (ICG) - Professor:  Davi Henrique dos Santos**.  
Consiste na criação de um **labirinto 3D** gerado automaticamente a partir de uma **Árvore Geradora Mínima**.  
O objetivo do jogador é percorrer o labirinto no menor tempo possível, utilizando as setas do teclado para se movimentar, com um sistema de **stamina** e **minimapa** para auxiliar na navegação.

O projeto foi desenvolvido utilizando **OpenGL**, **GLUT** e  **Linguagem C** .

---

## 🧩 Funcionalidades
- Geração automática do labirinto por algoritmo de Árvore Geradora Mínima.
- Exibição do labirinto em 3D com texturização de chão, paredes e céu.
- Minimapa em tempo real para orientação.
- Sistema de tempo e stamina para o jogador.
- Menu inicial e tela de instruções interativas.

---

## ⚙️ Compilação

Certifique-se de ter as bibliotecas **OpenGL** e **GLUT** instaladas.  
(Exemplo: `sudo apt install freeglut3-dev` para Linux ou `brew install freeglut` para MacOS.)

Para Windows, configure o **MinGW** com suporte às bibliotecas `freeglut`, `opengl32` e `glu32`.

### Linux

---

## 📁 Estrutura do Projeto

- **Labirinto 2D**: Geração e visualização de um labirinto com Árvore Geradora Mínima.
- **Labirinto 3D**: Imersão em um labirinto 3D, com movimentação, texturas e minimapa.
    - `main.c`: Ponto de entrada da aplicação (gerencia a interface e os estados do jogo).
    - `Labirinto.c`: Funções responsáveis por construir e desenhar o labirinto 3D.
    - `Grafo.c`: Funções de manipulação de grafos, criação da árvore geradora mínima e cálculo do menor caminho.

---

## 📸 Funcionamento e Detalhes

<div align="center">

<h3>Menu Inicial</h3>
<img src="https://drive.google.com/uc?export=view&id=1FApcs9yl1NR2bHsw0erFkphaNuDDe76L" width="300px" style="border: 2px solid #ccc; border-radius: 10px; box-shadow: 2px 2px 10px #aaa;">

<h3>Tela de Instruções</h3>
<img src="https://drive.google.com/uc?export=view&id=1WmwQBuSSzjxZJmpDcJHavPiNQG1eFE2-" width="300px" style="border: 2px solid #ccc; border-radius: 10px; box-shadow: 2px 2px 10px #aaa;">

<h3>Labirinto 3D em Execução</h3>
<img src="https://drive.google.com/uc?export=view&id=1Qf3RnUwbvtWOYpjVKENnHOZk6K9MmJs-" width="300px" style="border: 2px solid #ccc; border-radius: 10px; box-shadow: 2px 2px 10px #aaa; margin: 10px;">
<img src="https://drive.google.com/uc?export=view&id=1CuqGuUcwQ2W1bw6TrmJZIHhvf3JS2y2q" width="300px" style="border: 2px solid #ccc; border-radius: 10px; box-shadow: 2px 2px 10px #aaa; margin: 10px;">

</div>
---

## 🧑‍💻 Integrantes
<div align="center">

<table>
  <tr>
    <td align="center">
      <img src="https://drive.google.com/uc?export=view&id=1c5xlO3lX6pE3SHc--1P-lAoRX8Sv_HYQ" width="160" height="180" style="object-fit: cover;"><br>
      <b>Beatriz Moura</b>
    </td>
    <td align="center">
      <img src="https://drive.google.com/uc?export=view&id=19ffZj7PEelyPrItMqpYavFV9cz4ucy7b" width="160" height="180" style="object-fit: cover;"><br>
      <b>Matheus Ferreira</b>
    </td>
    <td align="center">
      <img src="https://drive.google.com/uc?export=view&id=1QEsoGm4mfOv_hIE1Zm67epH88huBR4Bi" width="160" height="180" style="object-fit: cover;"><br>
      <b>Matheus Mendonça</b>
    </td>
  </tr>
</table>

</div>


---


```bash

