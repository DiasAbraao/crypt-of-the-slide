#include "globals.h" // Inclua seu cabe�alho onde as fun��es s�o declaradas
#include <stdio.h> // Se precisar de algo como printf
#include <stdbool.h>
#include "utils.h"

Projetil projeteis[MAX_PROJETEIS]; // A defini��o da vari�vel global, se n�o estiver em outro lugar

void dispararProjetil(int x, int y, float dx, float dy) {
    // Implementa��o da fun��o para disparar um proj�til
    // Percorrer o array de projeteis para encontrar um inativo e ativ�-lo
    int i;
    for (i = 0; i < MAX_PROJETEIS; i++) {
        if (!projeteis[i].ativo) {
            projeteis[i].x = (float)x;
            projeteis[i].y = (float)y;
            projeteis[i].dx = (float)dx;
            projeteis[i].dy = (float)dy;
            projeteis[i].ativo = true;
            break; // Sai do loop ap�s encontrar e ativar um proj�til
        }
    }
}

void atualizarProjeteis(void) {
    player.x = player.x;
    player.y = player.y;

    Uint32 agora = SDL_GetTicks();
	int i;
    for (i = 0; i < MAX_PROJETEIS; i++) {
        if (projeteis[i].ativo) {
            projeteis[i].x += projeteis[i].dx;
            projeteis[i].y += projeteis[i].dy;

            // Verifica colis�o s� se player estiver vivo e n�o estiver invulner�vel
            if (player.vivo &&
                agora > ultimoDano + tempoInvulneravel &&
                colisao(projeteis[i].x, projeteis[i].y, 5, 5,
                        player.x * TILE_SIZE, player.y * TILE_SIZE, TILE_SIZE, TILE_SIZE)) {
                
                projeteis[i].ativo = false;
                player.vida--;
                ultimoDano = agora;

                if (player.vida <= 0) {
                    player.vivo = false;
                    estadoAtual = ESTADO_GAME_OVER;
                } else {
                    printf("ola");
                }
            }

            // Desativa proj�til que saiu da tela
            if (projeteis[i].x < 0 || projeteis[i].x > LARGURA ||
                projeteis[i].y < 0 || projeteis[i].y > ALTURA) {
                projeteis[i].ativo = false;
            }
        }
    }
}


void desenharProjeteis(SDL_Renderer *renderer) {
    // Implementa��o da fun��o para desenhar os proj�teis
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Cor amarela para os proj�teis
    int i;
    for (i = 0; i < MAX_PROJETEIS; i++) {
        if (projeteis[i].ativo) {
            SDL_Rect projRet = {(int)projeteis[i].x, (int)projeteis[i].y, 5, 5}; // Exemplo de tamanho
            SDL_RenderFillRect(renderer, &projRet);
        }
    }
}


