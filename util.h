#include <SDL2/SDL.h>
#include <cmath>

#define EPS 1e-6

typedef struct {
    double x, y;
} SL_Point;             // Um ponto com coordenadas double

typedef struct {
    int r,g,b;          // cor da cobra
    double tam;         // tamanho da cobra
    double vx, vy;      // versor diretor para onde a cobra esta indo
    int ncorpo;         // numero de elementos em corpo
    SL_Point *corpo;    // pontos no corpo da cobra. A cabeca da cobra esta em
                        // corpo[0]
    SDL_Point *corpoi;  // corpo com coordenadas inteiras para o SDL
} SL_cobra;

inline int cmpf(double, double );

double dist(SL_Point *, SL_Point *);

SL_cobra *criacobra(int, int, int, int, int, int, int);

void matacobra(SL_cobra *);

void desenha_cobra(SDL_Renderer *, SL_cobra *);

void rotaciona(double &, double &, int);

void walk(SL_cobra *, int, int);
