#include "globals.h"

Player player = {
    .x = 3.0f,
    .y = 2.0f,
    .vida = 3,
    .vivo = true
};

Uint32 ultimoDano = 0;
const Uint32 tempoInvulneravel = 1500; // 1,5 segundos

int faseAtual = 1; // ou qualquer valor inicial

float posicaoInicialX = 0;
float posicaoInicialY = 0;

Estado estadoAtual = MENU;
