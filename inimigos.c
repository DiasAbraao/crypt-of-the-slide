#include "inimigos.h"
#include <math.h>
#include <SDL.h>
#include <stdio.h>

Inimigo inimigos[MAX_INIMIGOS];

// Inicializa inimigos conforme o mapa
void inicializarInimigos(char mapa[MAX_LINHAS][MAX_COLUNAS], int linhas, int colunas) {
	int i;
	for (i = 0; i < MAX_INIMIGOS; i++) {
        inimigos[i].ativo = false;
    }
	
    int count = 0;
    int y, x;
    for (y = 0; y < linhas; y++) {
        for (x = 0; x < colunas; x++) {
            if (count >= MAX_INIMIGOS) return;
            char tile = mapa[y][x];
            if (tile == 'I') {
                inimigos[count].x = x;
                inimigos[count].y = y;
                inimigos[count].tipo = INIMIGO_NORMAL;
                inimigos[count].ultimoTiro = 0;
                inimigos[count].ativo = true;
                count++;
            } else if (tile == 'C') {
                inimigos[count].x = x;
                inimigos[count].y = y;
                inimigos[count].tipo = INIMIGO_CIRCULAR;
                inimigos[count].ultimoTiro = 0;
                inimigos[count].ativo = true;
                count++;
            } else if (tile == 'B') {
                inimigos[count].x = x;
                inimigos[count].y = y;
                inimigos[count].tipo = INIMIGO_BOSS;
                inimigos[count].ultimoTiro = 0;
                inimigos[count].ativo = true;
                count++;
            }
        }
    }
}

static void disparoNormal(Inimigo *inimigo, float playerX, float playerY, Uint32 agora) {
    Uint32 cooldown = 1000; // 1s
    if (agora < inimigo->ultimoTiro + cooldown) return;

    float origemX = inimigo->x * TILE_SIZE + TILE_SIZE / 2;
    float origemY = inimigo->y * TILE_SIZE + TILE_SIZE / 2;
    float destinoX = playerX * TILE_SIZE + TILE_SIZE / 2;
    float destinoY = playerY * TILE_SIZE + TILE_SIZE / 2;

    float dx = destinoX - origemX;
    float dy = destinoY - origemY;
    float dist = sqrtf(dx*dx + dy*dy);

    if (dist > 300) return; // alcance máximo

    if (dist == 0) dist = 1;

    float dirX = dx / dist;
    float dirY = dy / dist;
    float velocidade = 0.1f;

    dispararProjetil(origemX, origemY, dirX * velocidade, dirY * velocidade);
    inimigo->ultimoTiro = agora;
}

static void disparoCircular(Inimigo *inimigo, Uint32 agora) {
    Uint32 cooldown = 3000; // 3s
    if (agora < inimigo->ultimoTiro + cooldown) return;

    float origemX = inimigo->x * TILE_SIZE + TILE_SIZE / 2;
    float origemY = inimigo->y * TILE_SIZE + TILE_SIZE / 2;
    int numProj = 8;
    float velocidade = 1.1f;
	
	int i;
    for (i = 0; i < numProj; i++) {
        float angulo = (2.0f * M_PI / numProj) * i;
        float dx = cosf(angulo);
        float dy = sinf(angulo);
        dispararProjetil(origemX, origemY, dx * velocidade, dy * velocidade);
    }
    inimigo->ultimoTiro = agora;
}

static void disparoBoss(Inimigo *inimigo, float playerX, float playerY, Uint32 agora) {
    Uint32 cooldown = 1500;
    if (agora < inimigo->ultimoTiro + cooldown) return;

    float origemX = inimigo->x * TILE_SIZE + TILE_SIZE / 2;
    float origemY = inimigo->y * TILE_SIZE + TILE_SIZE / 2;

    static bool alterna = false;
    alterna = !alterna;

    if (alterna) {
        // Tiro direto
        float destinoX = playerX * TILE_SIZE + TILE_SIZE / 2;
        float destinoY = playerY * TILE_SIZE + TILE_SIZE / 2;

        float dx = destinoX - origemX;
        float dy = destinoY - origemY;
        float dist = sqrtf(dx*dx + dy*dy);
        if (dist == 0) dist = 1;

        float dirX = dx / dist;
        float dirY = dy / dist;
        float velocidade = 1.5f;
        dispararProjetil(origemX, origemY, dirX * velocidade, dirY * velocidade);
    } else {
        // Disparo circular maior
        int numProj = 16;
        float velocidade = 1.2f;
        int i;
        for (i = 0; i < numProj; i++) {
            float angulo = (2.0f * M_PI / numProj) * i;
            float dx = cosf(angulo);
            float dy = sinf(angulo);
            dispararProjetil(origemX, origemY, dx * velocidade, dy * velocidade);
        }
    }
    inimigo->ultimoTiro = agora;
}

void atualizarInimigos(float playerX, float playerY) {
    Uint32 agora = SDL_GetTicks();
	int i;
    for (i = 0; i < MAX_INIMIGOS; i++) {
        if (!inimigos[i].ativo) continue;

        switch (inimigos[i].tipo) {
            case INIMIGO_NORMAL:
                disparoNormal(&inimigos[i], playerX, playerY, agora);
                break;
            case INIMIGO_CIRCULAR:
                disparoCircular(&inimigos[i], agora);
                break;
            case INIMIGO_BOSS:
                disparoBoss(&inimigos[i], playerX, playerY, agora);
                break;
            default:
                break;
        }
    }
}

void desenharInimigos(SDL_Renderer *renderer, SDL_Texture *textureInimigo) {
    SDL_Rect rect;
    rect.w = TILE_SIZE;
    rect.h = TILE_SIZE;
	int i;
    for (i = 0; i < MAX_INIMIGOS; i++) {
        if (!inimigos[i].ativo) continue;

        rect.x = inimigos[i].x * TILE_SIZE;
        rect.y = inimigos[i].y * TILE_SIZE;
        SDL_RenderCopy(renderer, textureInimigo, NULL, &rect);
    }
}

