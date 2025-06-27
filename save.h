#ifndef SAVE_H
#define SAVE_H

typedef struct {
    int fase;
} Progresso;

void salvarProgresso(Progresso progresso);
Progresso carregarProgresso();

#endif

