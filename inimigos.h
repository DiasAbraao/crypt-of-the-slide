#ifndef INIMIGOS_H
#define INIMIGOS_H

#include <SDL.h>
#include "globals.h"

typedef enum {
    INIMIGO_NENHUM = 0,
    INIMIGO_NORMAL = 1,
    INIMIGO_CIRCULAR = 2,
    INIMIGO_BOSS = 3
} TipoInimigo;

typedef struct {
    int x, y;  // posição em tiles
    TipoInimigo tipo;
    Uint32 ultimoTiro;
    bool ativo;
} Inimigo;

#define MAX_INIMIGOS 100

extern Inimigo inimigos[MAX_INIMIGOS];

void inicializarInimigos(char mapa[MAX_LINHAS][MAX_COLUNAS], int linhas, int colunas);
void atualizarInimigos(float playerX, float playerY);
void desenharInimigos(SDL_Renderer *renderer, SDL_Texture *textureInimigo);

#endif

