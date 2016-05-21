#include <cstdio>
#include <cstring>
#include <SDL2/SDL.h>
#include <cstdlib>
#include <unistd.h>
#include "util.h"

using namespace std;

int fd_torobo[2], fd_fromrobo[2];

int main(int argc, char *argv[]) {

    //Abre o robo
    if (argc != 2) {
        fprintf(stderr,"Uso: %s <executavel_do_robo_1>\n",argv[0]);
        return 1;
    }

    pipe(fd_torobo);
    pipe(fd_fromrobo);
    pid_t pid = fork();
    if (pid==0) { // sou o robo
        dup2(fd_torobo[0], STDIN_FILENO);
        dup2(fd_fromrobo[1], STDOUT_FILENO);
        close(fd_torobo[1]);
        close(fd_fromrobo[0]);
        execl(argv[1],"./", (char *)NULL);
        exit(1);
    }
    // sou o jogo

    FILE *rin = fdopen(fd_torobo[1], "w");
    FILE *rout = fdopen(fd_fromrobo[0], "r");
    close(fd_torobo[0]);
    close(fd_fromrobo[1]);

    if (rin== NULL or rout==NULL) {
        printf("fodeu\n");
        return 1;
    }

    // Inicializa coisas da SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erro: %s\n",SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Slithereitor", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Erro ao criar janela: %s\n",SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        printf("Erro ao criar renderizador: %s\n",SDL_GetError());
        SDL_DestroyWindow(window);
        return 1;
    }

    // Cria uma cobrinha azul marota
    SL_cobra* blue = criacobra(230, 130, 130, 130, 0, 0, 255);

    // laco principal
    for (int tt=0;tt<70;tt++) {

        // Avisa o robo que o jogo ainda nao acabou
        fprintf(rin,"0\n"); fflush(rin);

        // TODO: Enviar outras informacoes ao robo

        // Le a jogada do robo
        int ang;
        fscanf(rout,"%d",&ang);

        // Faz a cobrinha andar para frente em 10px com o angulo dado
        walk(blue, 10, ang);

        // Fundo branco
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        desenha_cobra(renderer, blue);    // desenha a cobra

        // Mostra na tela e aguarda
        SDL_RenderPresent(renderer);
        SDL_Delay(100);

    }

    // destroi memoria
    fclose(rin);
    fclose(rout);
    matacobra(blue);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
