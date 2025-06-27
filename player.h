// player.h
#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <stdbool.h>
#include "globals.h"

// Defini��es, se houver
#define VELOCIDADE_JOGADOR 5.0f // Pixels por segundo, pode ser diferente da VELOCIDADE geral
#define PLAYER_SIZE 0.8f


// Fun��es do jogador
void inicializarJogador(float startX, float startY);
void atualizarMovimentoJogador(const Uint8* keyboardState, float deltaTime);
void desenharJogador(SDL_Renderer* renderer, SDL_Texture* playerTexture);
bool verificarColisaoSaida(char mapa[MAX_LINHAS][MAX_COLUNAS], int linhas, int colunas, SDL_Window* window); // Adicionei isso pensando na sua l�gica de mapa

#endif // PLAYER_H
