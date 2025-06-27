// BIBLIOTECAS

#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL_image.h>

// ----- DEEFINES -----//

#define ALTURA 599
#define LARGURA 800
#define MAX_LINHAS 50
#define MAX_COLUNAS 50
#define TILE_SIZE 26
#define MAX_PROJETEIS 100

char mapa[MAX_LINHAS][MAX_COLUNAS];
int linhas = 0;
int colunas = 0;

// --------------------------  STRUCTS ----------------------//

typedef enum {
    MENU,
    JOGO,
    ESTATISTICAS,
    CONFIGURAR
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

Projetil projeteis[MAX_PROJETEIS];


// --------------------  FUNÇÕES --------------------------------- //

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
                	SDL_SetRenderDrawColor(renderer, 200, 200, 100, 100);
                	// atirar()
                default:
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // preto vazio
                    SDL_RenderFillRect(renderer, &rect);
                    break;
            }

        }
    }
}

void dispararProjetil(int x, int y, int dx, int dy) {
	int i;
    for (i = 0; i < MAX_PROJETEIS; i++) {
        if (!projeteis[i].ativo) {
            projeteis[i].x = x;
            projeteis[i].y = y;
            projeteis[i].dx = dx;
            projeteis[i].dy = dy;
            projeteis[i].ativo = true;
            break;
        }
    }
}

void atualizarProjeteis() {
	int i;
    for (i = 0; i < MAX_PROJETEIS; i++) {
        if (projeteis[i].ativo) {
            projeteis[i].x += projeteis[i].dx;
            projeteis[i].y += projeteis[i].dy;

            int col = (int)(projeteis[i].x / TILE_SIZE);
			int lin = (int)(projeteis[i].y / TILE_SIZE);

            if (col < 0 || col >= colunas || lin < 0 || lin >= linhas || mapa[lin][col] == '#') {
                projeteis[i].ativo = false;
            }
        }
    }
}

void desenharProjeteis(SDL_Renderer *renderer) {
    SDL_Rect rect;
    rect.w = TILE_SIZE / 2;
    rect.h = TILE_SIZE / 2;
	int i;
    for (i = 0; i < MAX_PROJETEIS; i++) {
        if (projeteis[i].ativo) {
            rect.x = (int)projeteis[i].x;
			rect.y = (int)projeteis[i].y;
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
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
    
    if (!carregarMapa("mapa-1.txt")) {
    	printf("Falha ao carregar o mapa.\n");
    	
	}
	
	// Inicializa todos os projéteis como inativos
	int i;
	for (i = 0; i < MAX_PROJETEIS; i++) {
	    projeteis[i].ativo = false;
	}

    
    // ------- TEXTURAS ---------//

    SDL_Surface* surface = IMG_Load("imagens/menu.jpg"); 
    SDL_Surface* surface2 = IMG_Load("imagens/confi.png");
    SDL_Surface* surface3 = IMG_Load("imagens/fundo_status.jpg"); 
    SDL_Surface* surface5 = IMG_Load("imagens/parede.jpg");
    SDL_Surface* surface10 = IMG_Load("imagens/player4.jpg");
    
    // -------- TESTES DO CARREGAMENTO DAS TEXTURAS ---------- //
    
    if (!surface || !surface2 || !surface3 || !surface5 || !surface10) {
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
	
	// ------- SDL_RECT POSICIONA AS TEXTURAS -----------//
    
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
    
    //-------- POSIÇÃO DO PLAYER ---------//
    
    float playerX = 3.0f;
	float playerY = 2.0f;

    
	// --------------------LOOP PRINCIPAL ----------------------//
	
    SDL_Event evento;
    bool rodando = 1;
    Estado estadoAtual = MENU;
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
	        	    // ------ EVENTOS DE CLIQUE DO TECLADO NA ABA JOGO ----------//
			
	    if (estadoAtual == JOGO) {
		    float velocidade = 200.0f;
		    
		    // CIMA
		    float novaPosY = playerY - velocidade * deltaTime;
		    int tileAcima = (int)(novaPosY);
		    if (state[SDL_SCANCODE_UP] && tileAcima >= 0 && mapa[tileAcima][(int)playerX] != '#') {
		        playerY = novaPosY;
		    }
		    
		    // BAIXO
		    novaPosY = playerY + velocidade * deltaTime;
		    int tileAbaixo = (int)(novaPosY + 0.99f);
		    if (state[SDL_SCANCODE_DOWN] && tileAbaixo < linhas && mapa[tileAbaixo][(int)playerX] != '#') {
		        playerY = novaPosY;
		    }
		    
		    // ESQUERDA
		    float novaPosX = playerX - velocidade * deltaTime;
		    int tileEsquerda = (int)(novaPosX);
		    if (state[SDL_SCANCODE_LEFT] && tileEsquerda >= 0 && mapa[(int)playerY][tileEsquerda] != '#') {
		        playerX = novaPosX;
		    }
		    
		    // DIREITA
		    novaPosX = playerX + velocidade * deltaTime;
		    int tileDireita = (int)(novaPosX + 0.99f);
		    if (state[SDL_SCANCODE_RIGHT] && tileDireita < colunas && mapa[(int)playerY][tileDireita] != '#') {
		        playerX = novaPosX;
		    }
		}

	        
	        // ------- EVENTOS DE CLIQUE DO TECLADO ----//
	        
	        if (evento.type == SDL_KEYDOWN) {
			    if (evento.key.keysym.sym == SDLK_ESCAPE) {
			        estadoAtual = MENU;
			    

						        
			        //-------------- Verifica se o jogador chegou à saída -------------- //
			        
					if (mapa[(int)playerY][(int)playerX] == 'E') {
					    printf("Vitoria!\n");
					    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Parabéns!", "Voce chegou a saida!", window);
						 // ou qualquer outra ação, como reiniciar o jogo
					    playerX = 3;
					    playerY = 2;
					}

			    }
			}
			
			// ----- AÇÕES DO CLIQUE DO MOUSE -------//
			
			if (evento.type == SDL_MOUSEBUTTONDOWN && evento.button.button == SDL_BUTTON_LEFT) {
			    int x = evento.button.x;
			    int y = evento.button.y;
			
			    if (estadoAtual == MENU) {
			        if (ponto_clique(iniciar_jogo ,x, y)) {
			            estadoAtual = JOGO;
			        } else if (ponto_clique(estatistica, x, y)) {
			            estadoAtual = ESTATISTICAS;
			        } else if (ponto_clique(configurar, x, y)) {
			            estadoAtual = CONFIGURAR;
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
        	
        	static Uint32 ultimoTiro = 0;
			Uint32 agora = SDL_GetTicks();
			int y, x;
			if (agora > ultimoTiro + 1000) {  // 1 segundo
			    for (y = 0; y < linhas; y++) {
			        for (x = 0; x < colunas; x++) {
			            if (mapa[y][x] == 'I') {
			                int origemX = x * TILE_SIZE;
							int origemY = y * TILE_SIZE;
							int destinoX = playerX * TILE_SIZE;
							int destinoY = playerY * TILE_SIZE;
							
							int dx = destinoX - origemX;
							int dy = destinoY - origemY;
							float distancia = SDL_sqrtf(dx * dx + dy * dy);
							if (distancia == 0) distancia = 1;
							
							float dirX = dx / distancia;
							float dirY = dy / distancia;
							float velocidade = 1.5;
							dispararProjetil(origemX, origemY, dirX * velocidade, dirY * velocidade);

			            }
			        }
			    }
			    ultimoTiro = agora;
			}
			
			atualizarProjeteis();
			desenharProjeteis(renderer);
        	
        	// Desenha o jogador como um quadrado azul POR ENQUANTO
        	
			SDL_Rect jogadorRect;
			jogadorRect.x = (int)(playerX * TILE_SIZE);
			jogadorRect.y = (int)(playerY * TILE_SIZE);
			jogadorRect.w = TILE_SIZE;
			jogadorRect.h = TILE_SIZE;
			SDL_RenderCopy(renderer, texture10, NULL, &jogadorRect); 
        	

	    } else if (estadoAtual == ESTATISTICAS) {
	    	
	    	SDL_RenderClear(renderer);
    		
    		SDL_Rect destino_estatistica;
			destino_estatistica.x = 0;
			destino_estatistica.y = 0 ;
			destino_estatistica.w = 800;
			destino_estatistica.h = 600;
			
			SDL_RenderCopy(renderer, texture3, NULL, &destino_estatistica);
			
	    } else if (estadoAtual == CONFIGURAR) {
	    	SDL_RenderClear(renderer);
	    	SDL_Rect configuracao;
	    	configuracao.x = 0;
	    	configuracao.y = 0;
	    	configuracao.w = 800;
	    	configuracao.h = 600;
	    	
	    	SDL_RenderCopy(renderer, texture3, NULL, &configuracao);
		}
	
	    SDL_RenderPresent(renderer);
	}
	
	// DESTRUINDO TEXTURAS
	
	SDL_DestroyTexture(texture);
	SDL_DestroyTexture(texture2);
    SDL_DestroyTexture(texture3);
    SDL_DestroyTexture(texturaParede);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
