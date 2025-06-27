// player.c
#include "player.h" // Inclui o cabeçalho do jogador
#include "globals.h" // Inclua seu arquivo globals.h aqui para MAX_LINHAS, MAX_COLUNAS, TILE_SIZE
#include <stdio.h>
#include <stdbool.h>

void inicializarJogador(float startX, float startY) {
    player.x = startX;
    player.y = startY;
}

void atualizarMovimentoJogador(const Uint8* keyboardState, float deltaTime) {
    float novaX = player.x;
    float novaY = player.y;
    float velocidade = VELOCIDADE_JOGADOR * deltaTime;

    if (keyboardState[SDL_SCANCODE_W]) novaY -= velocidade;
    if (keyboardState[SDL_SCANCODE_S]) novaY += velocidade;
    if (keyboardState[SDL_SCANCODE_A]) novaX -= velocidade;
    if (keyboardState[SDL_SCANCODE_D]) novaX += velocidade;

    // Limites do mapa (evitar sair da área)
    if (novaX < 0) novaX = 0;
    if (novaY < 0) novaY = 0;
    if (novaX > colunas - PLAYER_SIZE) novaX = colunas - PLAYER_SIZE;
    if (novaY > linhas - PLAYER_SIZE) novaY = linhas - PLAYER_SIZE;

    // Função auxiliar para checar se um retângulo bate em parede
    bool podeMover(float x, float y) {
        // Canto superior esquerdo
        if (mapa[(int)y][(int)x] == '#') return false;
        // Canto superior direito
        if (mapa[(int)y][(int)(x + PLAYER_SIZE)] == '#') return false;
        // Canto inferior esquerdo
        if (mapa[(int)(y + PLAYER_SIZE)][(int)x] == '#') return false;
        // Canto inferior direito
        if (mapa[(int)(y + PLAYER_SIZE)][(int)(x + PLAYER_SIZE)] == '#') return false;

        return true;
    }

    // Tenta mover no eixo X
    if (podeMover(novaX, player.y)) {
        player.x = novaX;
    }

    // Tenta mover no eixo Y
    if (podeMover(player.x, novaY)) {
        player.y = novaY;
    }
}



void desenharJogador(SDL_Renderer *renderer, SDL_Texture *texturaJogador) {
    Uint32 agora = SDL_GetTicks();

    bool invulneravel = agora < ultimoDano + tempoInvulneravel;

    // Piscar a cada 150 ms se estiver invulnerável
    if (invulneravel && ((agora / 150) % 2 == 0)) {
        return; // Pula o desenho neste frame
    }

    SDL_Rect dst = {
        (int)(player.x * TILE_SIZE),
        (int)(player.y * TILE_SIZE),
        TILE_SIZE,
        TILE_SIZE
    };
    SDL_RenderCopy(renderer, texturaJogador, NULL, &dst);
}


bool verificarColisaoSaida(char mapa[MAX_LINHAS][MAX_COLUNAS], int linhas, int colunas, SDL_Window* window) {
    int tilePlayerX = (int)player.x;
    int tilePlayerY = (int)player.y;

    if (tilePlayerY >= 0 && tilePlayerY < linhas && tilePlayerX >= 0 && tilePlayerX < colunas) {
        if (mapa[tilePlayerY][tilePlayerX] == 'E') {
            return true;
            
        }
    }
    return false;
}


