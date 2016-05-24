#include <cstdio>
#include <cstring>
#include <SDL2/SDL.h>
#include <cstdlib>
#include <unistd.h>
#include "util.h"

using namespace std;

int fd_tobot[2], fd_frombot[2];

int main(int argc, char *argv[]) {

    // Load the bot
    if (argc != 2) {
        fprintf(stderr,"Usage: %s <bot exec>\n",argv[0]);
        return 1;
    }

    pipe(fd_tobot);
    pipe(fd_frombot);
    pid_t pid = fork();
    if (pid==0) {
        // I am the robot
        dup2(fd_tobot[0], STDIN_FILENO);
        dup2(fd_frombot[1], STDOUT_FILENO);
        close(fd_tobot[1]);
        close(fd_frombot[0]);
        execl(argv[1],"./", (char *)NULL);
        exit(1);
    }
    // I am the game
    FILE *rin = fdopen(fd_tobot[1], "w");
    FILE *rout = fdopen(fd_frombot[0], "r");
    close(fd_tobot[0]);
    close(fd_frombot[1]);

    if (rin== NULL or rout==NULL) {
        printf("Couldn't talk to the bot.\n");
        return 1;
    }

    // SDL Init
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error: %s\n",SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Slithereitor", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Error opening the window: %s\n",SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        printf("Error creating the renderer: %s\n",SDL_GetError());
        SDL_DestroyWindow(window);
        return 1;
    }

    // Create an awesome blue little snake
    SL_snake* blue = create_snake(230, 130, 130, 130, 0, 0, 255);

    // main loop
    for (int tt=0;tt<70;tt++) {

        // Tell the bot the game hasn't finished yet
        fprintf(rin,"0\n"); fflush(rin);

        // TODO: Send other infos to the bot

        // Read a move from the bot
        int ang;
        fscanf(rout,"%d",&ang);

        // Walk the snake 10px with the given angle
        walk(blue, 10, ang);

        // White background
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Draw the snake
        draw_snake(renderer, blue);

        // Render and wait
        SDL_RenderPresent(renderer);
        SDL_Delay(100);

    }

    // Free the memory
    fclose(rin);
    fclose(rout);
    free_snake(blue);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
