// BIBLIOTECAS

#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL_image.h>
#include "globals.h"
#include "player.h"
#include "inimigos.h"
#include "save.h"
#include <SDL_ttf.h>


// ------- variavreis globais ----------//

char mapa[MAX_LINHAS][MAX_COLUNAS];
int linhas = 0;
int colunas = 0;



// --------------------  FUNÇÕES --------------------------------- //



void desenharHUD(SDL_Renderer *renderer) {
    SDL_Rect vidaRect;
    vidaRect.w = 20;
    vidaRect.h = 20;
    vidaRect.y = 10;

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // vermelho
	int i;
    for (i = 0; i < player.vida; i++) {
        vidaRect.x = 10 + i * (vidaRect.w + 5);
        SDL_RenderFillRect(renderer, &vidaRect);
    }
}

void desenharGameOver(SDL_Renderer *renderer, SDL_Texture *textureGameOver) {    
	
	SDL_Rect game_over;
			game_over.x = 0;
			game_over.y = 0 ;
			game_over.w = 800;
			game_over.h = 600;
			
			SDL_RenderCopy(renderer, textureGameOver, NULL, &game_over);
}


bool carregarMapa(const char *caminhoMapa) {
    FILE *arq = fopen(caminhoMapa, "r");
    if (!arq) {
        printf("Erro ao abrir o arquivo %s\n", caminhoMapa);
        return false;
    }

    linhas = 0;
    char linha[MAX_COLUNAS + 2];
    while (fgets(linha, sizeof(linha), arq) && linhas < MAX_LINHAS) {
        int i = 0;
        while (linha[i] != '\n' && linha[i] != '\0' && i < MAX_COLUNAS) {
            mapa[linhas][i] = linha[i];
            if (linha[i] == 'P') {
		        player.x = i;
		        player.y = linhas;
		        
	            posicaoInicialX = i;
				posicaoInicialY = linhas;
				
		        mapa[linhas][i] = '.'; // torna a célula um chão comum
		    }


            i++;
        }
        mapa[linhas][i] = '\0';
        if (linhas == 0) colunas = i;
        linhas++;
    }
    fclose(arq);
    return true;
}

void desenharMapa(SDL_Renderer *renderer, SDL_Texture *texturaParede) {
	int y, x;
    SDL_Rect rect;
    rect.w = TILE_SIZE;
    rect.h = TILE_SIZE;
    

    for (y = 0; y < linhas; y++) {
        for (x = 0; x < colunas; x++) {
            rect.x = x * TILE_SIZE;
            rect.y = y * TILE_SIZE;

            switch (mapa[y][x]) {
                case '#':
                    SDL_RenderCopy(renderer, texturaParede, NULL, &rect);
                    break;
                case '.':
                    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // claro chão
                    SDL_RenderFillRect(renderer, &rect);
                    break;
                case 'E':
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // verde saída
                    SDL_RenderFillRect(renderer, &rect);
                    break;
                case 'I':
                	SDL_SetRenderDrawColor(renderer, 200, 200, 100, 255);  // amarelo visível
    				SDL_RenderFillRect(renderer, &rect);
                	break;
                default:
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // preto vazio
                    SDL_RenderFillRect(renderer, &rect);
                    break;
            }

        }
    }
}

void jogar() {
    inicializarInimigos(mapa, linhas, colunas);
    estadoAtual = JOGO;
}

void reiniciarJogo() {
    player.vida = 3;
    player.vivo = true;
    ultimoDano = 0;
    
    char caminho[50];
    sprintf(caminho, "mapas/mapa-%d.txt", faseAtual);
    carregarMapa(caminho);
    
    player.x = posicaoInicialX;
    player.y = posicaoInicialY;

    int i;
	for (i = 0; i < MAX_PROJETEIS; i++) {
        projeteis[i].ativo = false;
    }

    inicializarInimigos(mapa, linhas, colunas);
    estadoAtual = JOGO;
}

void desenharTexto(SDL_Renderer* renderer, TTF_Font* font, const char* texto, int x, int y, SDL_Color cor) {
    SDL_Surface* surfaceText = TTF_RenderText_Solid(font, texto, cor);
    SDL_Texture* textureText = SDL_CreateTextureFromSurface(renderer, surfaceText);

    SDL_Rect destRect = { x, y, surfaceText->w, surfaceText->h };
    SDL_RenderCopy(renderer, textureText, NULL, &destRect);

    SDL_FreeSurface(surfaceText);
    SDL_DestroyTexture(textureText);
}


// ----- pontos onde o clique do mouse pega ----//

bool ponto_clique (clique ponto, int x, int y) {
	int limite_x, limite_y;
	limite_x = ponto.x + ponto.w;
	limite_y = ponto.y + ponto.h;
	
	return(y >= ponto.y && y <= limite_y && x >= ponto.x && x <= limite_x);
}


// --------------- CODIGO PRINCIPAL -------------------//

int main(int argc, char* argv[]) {
	
	// ---------------- TESTES DE INICIALIZAÇÃO ---------------//
	
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erro ao iniciar SDL: %s\n", SDL_GetError());
        return 1;
    }
    
    if (TTF_Init() == -1) {
	    printf("Erro ao iniciar SDL_ttf: %s\n", TTF_GetError());
	    // trate o erro aqui
	}

    
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
	    printf("Erro ao iniciar SDL_image: %s\n", IMG_GetError());
	    SDL_Quit();
	    return 1;
	}

    SDL_Window* window = SDL_CreateWindow("Crypt of the slide",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        LARGURA, ALTURA, SDL_WINDOW_SHOWN);
        
    if (!window) {
        printf("Erro ao criar janela: %s\n", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
	    

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Erro ao criar renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    
    TTF_Font* font = TTF_OpenFont("fonts/FreeSans.ttf", 50);
	if (!font) {
	    printf("Erro ao carregar fonte: %s\n", TTF_GetError());
	    // trate o erro
	}

    
	
	// Inicializa todos os projéteis como inativos
	int i;
	for (i = 0; i < MAX_PROJETEIS; i++) {
	    projeteis[i].ativo = false;
	}

    
    // ------- IMAGENS ---------//

    SDL_Surface* surface = IMG_Load("imagens/menu.jpg"); 
    SDL_Surface* surface2 = IMG_Load("imagens/confi.png");
    SDL_Surface* surface3 = IMG_Load("imagens/recorde.jpg"); 
    SDL_Surface* surface5 = IMG_Load("imagens/parede.jpg");
    SDL_Surface* surface10 = IMG_Load("imagens/player6.png");
    SDL_Surface* surfaceInimigo = IMG_Load("imagens/inimigo.jpg");
    SDL_Surface* surfaceGameOver = IMG_Load("imagens/game_over.jpg");
    SDL_Surface* surfaceFases = IMG_Load("imagens/fases.jpg");
    SDL_Surface* surfaceParabens = IMG_Load("imagens/parabens.png");
    
    
    // -------- TESTES DO CARREGAMENTO DAS IMAGENS ---------- //
    
    if (!surface || !surface2 || !surface3 || !surface5 || !surface10 || !surfaceInimigo|| !surfaceGameOver || !surfaceFases) {
        printf("Erro ao carregar imagem: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    
    // ---------- CRIA TEXTURA A PARTIR DAS IMAGENS ---------//
	
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    SDL_Texture* texture2 = SDL_CreateTextureFromSurface(renderer, surface2);
    SDL_FreeSurface(surface2);
    SDL_Texture* texture3 = SDL_CreateTextureFromSurface(renderer, surface3);
    SDL_FreeSurface(surface3);
    SDL_Texture* texturaParede = SDL_CreateTextureFromSurface(renderer, surface5);
	SDL_FreeSurface(surface5);
	SDL_Texture* texture10 = SDL_CreateTextureFromSurface(renderer, surface10);
	SDL_FreeSurface(surface10);
	SDL_Texture* textureInimigo = SDL_CreateTextureFromSurface(renderer, surfaceInimigo);
	SDL_FreeSurface(surfaceInimigo);
    SDL_Texture* textureGameOver = SDL_CreateTextureFromSurface(renderer, surfaceGameOver);
	SDL_FreeSurface(surfaceGameOver);
	SDL_Texture* textureFases = SDL_CreateTextureFromSurface(renderer, surfaceFases);
	SDL_FreeSurface(surfaceFases);
	SDL_Texture* textureParabens = SDL_CreateTextureFromSurface(renderer, surfaceParabens);
	SDL_FreeSurface(surfaceParabens);
		
	// ------- SDL_RECT POSICIONA AS TEXTURAS DO MENU -----------//
	
	Progresso progresso = carregarProgresso();
	faseAtual = progresso.fase;
    
    SDL_Rect destino;
    destino.x = 0;    // posição X na janela
	destino.y = 0;     // posição Y na janela
	destino.w = 800;    // largura desejada da imagem
	destino.h = 600;    // altura desejada da imagem
	
	SDL_Rect destino_confi;
	destino_confi.x = 750;
	destino_confi.y = 550 ;
	destino_confi.w = 50;
	destino_confi.h = 50;
    
    // ------- PONTOS DE CLIQUE ------// 
    
    clique iniciar_jogo = {170, 280, 460, 115};
    clique estatistica = {170, 430, 460, 100};
    clique configurar = {750, 550, 50, 50};
    clique fase1 = {130, 100, 205, 150};
    clique fase2 = {470, 100, 205, 150};
    clique fase3 = {55, 310, 205, 150};
    clique fase4 = {300, 310, 205, 150};
    clique fase5 = {540, 310, 205, 150};
    clique reiniciar = {170, 280, 460, 115};
    clique menu = {170, 430, 460, 100};
    clique continuar = {170, 280, 460, 115};
    clique menu2 = {170, 430, 460, 100};
    clique voltar_status = {20, 510, 240, 50};
    clique voltar_fases = {300, 495, 190, 150};

    
	// --------------------LOOP PRINCIPAL ----------------------//
	
    SDL_Event evento;
    bool rodando = 1;
    Uint32 tempoAnterior = SDL_GetTicks();

	while (rodando) {
		
		Uint32 tempoAtual = SDL_GetTicks();
		float deltaTime = (tempoAtual - tempoAnterior) / 1000.0f;
		tempoAnterior = tempoAtual;
		const Uint8 *state = SDL_GetKeyboardState(NULL);


	    while (SDL_PollEvent(&evento)) {
	        if (evento.type == SDL_QUIT) {
	            rodando = false;
	        }
	        
	        // ------- EVENTOS DE CLIQUE DO TECLADO ----//
	        
	        if (evento.type == SDL_KEYDOWN) {
			    if (evento.key.keysym.sym == SDLK_ESCAPE) {
			        estadoAtual = MENU;
				}
						        
			}
			
			// ----- AÇÕES DO CLIQUE DO MOUSE -------//
			
			if (evento.type == SDL_MOUSEBUTTONDOWN && evento.button.button == SDL_BUTTON_LEFT) {
			    int x = evento.button.x;
			    int y = evento.button.y;
			
			    if (estadoAtual == MENU) {
			        if (ponto_clique(iniciar_jogo ,x, y)) {
			            estadoAtual = FASES;
			        } else if (ponto_clique(estatistica, x, y)) {
			            estadoAtual = ESTATISTICAS;
			        } else if (ponto_clique(configurar, x, y)) {
			            estadoAtual = CONFIGURAR;
			        }
			    } else if (estadoAtual == FASES) {
			        if (ponto_clique(fase1, x, y)) {
			            faseAtual = 1;
					    Progresso novoProgresso = { .fase = faseAtual };
					    salvarProgresso(novoProgresso);
					    carregarMapa("mapas/mapa-1.txt");
			            jogar();
			        }
			        else if (ponto_clique(fase2, x, y)) {
			            faseAtual = 2;
					    Progresso novoProgresso = { .fase = faseAtual };
					    salvarProgresso(novoProgresso);
					    carregarMapa("mapas/mapa-2.txt");
			            jogar();
			        }
			        else if (ponto_clique(fase3, x, y)){
			        	faseAtual = 3;
					    Progresso novoProgresso = { .fase = faseAtual };
					    salvarProgresso(novoProgresso);
					    carregarMapa("mapas/mapa-3.txt");
			        	jogar();
					}
					else if(ponto_clique(fase4, x, y)){
						faseAtual = 4;
					    Progresso novoProgresso = { .fase = faseAtual };
					    salvarProgresso(novoProgresso);
					    carregarMapa("mapas/mapa-4.txt");
						jogar();
					}
					else if(ponto_clique(fase5, x, y)){
						faseAtual = 5;
					    Progresso novoProgresso = { .fase = faseAtual };
					    salvarProgresso(novoProgresso);
					    carregarMapa("mapas/mapa-5.txt");
						jogar();
					} else if (ponto_clique(voltar_fases, x, y)) {
						estadoAtual = MENU;
					}
					
				} else if (estadoAtual == ESTADO_GAME_OVER) {
					if (ponto_clique(reiniciar, x, y)){
						char caminho[50];
						sprintf(caminho, "mapas/mapa-%d.txt", faseAtual);
						carregarMapa(caminho);
						reiniciarJogo();
					} else if (ponto_clique(menu, x, y)){
						estadoAtual = MENU;
						player.vida = 3;
						player.vivo = true;
						ultimoDano = 0;
					}
				} else if (estadoAtual == PARABENS) {
					if(ponto_clique(continuar, x, y)){
						faseAtual++;
						 Progresso novoProgresso = { .fase = faseAtual };
					    salvarProgresso(novoProgresso);
					
					    char caminho[50];
					    sprintf(caminho, "mapas/mapa-%d.txt", faseAtual);
					
					    if (!carregarMapa(caminho)) {
					        estadoAtual = MENU; // ou fim do jogo
					    } else {
					        jogar();
					    }
					} else if(ponto_clique(menu, x, y)){
						estadoAtual = MENU;
					}
				} else if (estadoAtual == ESTATISTICAS) {
					if (ponto_clique(voltar_status, x, y)) {
						estadoAtual = MENU;	
					}
				} else if(estadoAtual == CONFIGURAR) {
					if (ponto_clique(voltar_status, x, y)) {
						estadoAtual = MENU;
					}
				}
			}
		}
		
		// ---------------- AREAS DO JOGO ---------------//
		
		if (estadoAtual == MENU) {
	    	SDL_RenderClear(renderer);
	        SDL_RenderCopy(renderer, texture, NULL, &destino);
	        SDL_RenderCopy(renderer, texture2, NULL, &destino_confi);
	        
	        
	    } else if (estadoAtual == JOGO) {
	    	SDL_RenderClear(renderer);
        	desenharMapa(renderer, texturaParede);
        	
        	// Atualiza a posição do jogador usando a função externa
            atualizarMovimentoJogador(state, deltaTime); 

            // Verifica colisão com a saída
            if (verificarColisaoSaida(mapa, linhas, colunas, window)) {
            	player.vida = 3;
            	estadoAtual = PARABENS;
            	
            	SDL_RenderClear(renderer);
            	
            	SDL_Rect proximo;
				proximo.x = 0;
				proximo.y = 0;
				proximo.w = 800;
				proximo.h = 599;
				
				
				SDL_RenderCopy(renderer, textureParabens, NULL, &proximo);
				
			    
			   
			}

        	
       	    atualizarInimigos(player.x, player.y);
        	
			desenharInimigos(renderer, textureInimigo);

			atualizarProjeteis();
			desenharProjeteis(renderer);
			desenharHUD(renderer);
			desenharJogador(renderer, texture10);   
					    	
		} else if (estadoAtual == FASES) {
			SDL_RenderClear(renderer);
    		
    		SDL_Rect fases;
			fases.x = 0;
			fases.y = 0 ;
			fases.w = 800;
			fases.h = 600;
			SDL_RenderCopy(renderer, textureFases, NULL, &fases);
			
		}
		else if (estadoAtual == ESTATISTICAS) {
	    	
	    	SDL_RenderClear(renderer);
    		
    		SDL_Rect destino_estatistica;
			destino_estatistica.x = 0;
			destino_estatistica.y = 0 ;
			destino_estatistica.w = 800;
			destino_estatistica.h = 600;
			
			SDL_RenderCopy(renderer, texture3, NULL, &destino_estatistica);
			char textoProgresso[100];
		    sprintf(textoProgresso, "Fase atual: %d", faseAtual);
		    SDL_Color corBranca = {255, 255, 255, 255};
		    desenharTexto(renderer, font, textoProgresso, 250, 300, corBranca);
			
	    } else if (estadoAtual == CONFIGURAR) {
	    	SDL_RenderClear(renderer);
	    	SDL_Rect configuracao;
	    	configuracao.x = 0;
	    	configuracao.y = 0;
	    	configuracao.w = 800;
	    	configuracao.h = 600;
	    	
	    	SDL_RenderCopy(renderer, texture3, NULL, &configuracao);
	    	
		} else if (estadoAtual == ESTADO_GAME_OVER) {
		    desenharGameOver(renderer, textureGameOver);
		    
		}else if (estadoAtual == PARABENS) {
		    SDL_RenderClear(renderer);
		    SDL_Rect proximo;
			proximo.x = 0;
			proximo.y = 0;
			proximo.w = 800;
			proximo.h = 600;

		    SDL_RenderCopy(renderer, textureParabens, NULL, &proximo);
		}
			
	    SDL_RenderPresent(renderer);
	}
	// Antes de sair do programa, zera o progresso
	Progresso progressoZerado = { .fase = 1 };
	salvarProgresso(progressoZerado);

	
	// DESTRUINDO TEXTURAS
	
	TTF_CloseFont(font);
	SDL_DestroyTexture(texture);
	SDL_DestroyTexture(texture2);
    SDL_DestroyTexture(texture3);
    SDL_DestroyTexture(texturaParede);
    SDL_DestroyTexture(textureInimigo);
    SDL_DestroyTexture(texture10);
	SDL_DestroyTexture(textureGameOver);
	SDL_DestroyTexture(textureFases);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
