#include "save.h"
#include <stdio.h>
#include <stdlib.h>

#define ARQUIVO_SALVAMENTO "progresso.dat"


void salvarProgresso(Progresso progresso) {
    FILE *arquivo = fopen("progresso.dat", "wb");
    if (arquivo) {
        fwrite(&progresso, sizeof(Progresso), 1, arquivo);
        fclose(arquivo);
    }
}

Progresso carregarProgresso() {
    Progresso progresso;
    FILE *arquivo = fopen("progresso.dat", "rb");
    if (arquivo) {
        fread(&progresso, sizeof(Progresso), 1, arquivo);
        fclose(arquivo);
    } else {
        progresso.fase = 1; // valor inicial se não houver arquivo
    }
    return progresso;
}


