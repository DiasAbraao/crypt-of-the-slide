// globals.h
#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdbool.h>
#include <SDL.h>

#define LARGURA 800
#define ALTURA 599
#define MAX_LINHAS 50
#define MAX_COLUNAS 50
#define TILE_SIZE 26
#define MAX_PROJETEIS 100

typedef struct {
    float x, y; // posição central (ou canto superior esquerdo)
    int vida;
    bool vivo;
} Player;

extern Player player;

typedef enum {
    MENU,
    JOGO,
    FASES,
    ESTATISTICAS,
    CONFIGURAR,
    ESTADO_GAME_OVER,
    PARABENS
} Estado;

typedef struct {
	int x;
	int y;
	int w;
	int h;
} clique;

typedef struct {
    float x, y;
    float dx, dy;
    bool ativo;
} Projetil;


void dispararProjetil(int x, int y, float dx, float dy);
void atualizarProjeteis(void);
void desenharProjeteis(SDL_Renderer *renderer);

extern int faseAtual;


extern Uint32 ultimoDano;
extern const Uint32 tempoInvulneravel;

extern Estado estadoAtual;

extern float posicaoInicialX, posicaoInicialY;


extern char mapa[MAX_LINHAS][MAX_COLUNAS];
extern int linhas, colunas;
extern Projetil projeteis[MAX_PROJETEIS];


#endif


