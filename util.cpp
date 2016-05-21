#include <SDL2/SDL.h>
#include <cmath>

#include "util.h"

inline int cmpf(double a, double b) {
    if (fabs(a-b) < EPS) return 0;
    return a < b ? -1 : 1;
}

double dist(SL_Point *p1, SL_Point *p2) {
    return sqrt( (p1->x - p2->x)*(p1->x - p2->x) + (p1->y - p2->y)*(p1->y - p2->y) );
}

// cria uma cobra com cabeca em (x1,y1), rabo em (x2,y2) e cor (r,g,b)
SL_cobra *criacobra(int x1, int y1, int x2, int y2, int r, int g, int b) {
    SL_cobra *c = (SL_cobra *)malloc(sizeof(SL_cobra));
    c->r = r; c->g = g; c->b = b;
    c->ncorpo = 2;
    c->corpo = (SL_Point *)malloc(2*sizeof(SL_Point));
    c->corpoi = (SDL_Point *)malloc(2*sizeof(SDL_Point));
    c->corpo[0] = {(double)x1, (double)y1}; c->corpo[1] = {(double)x2, (double)y2};
    c->corpoi[0] = {x1, y1}; c->corpoi[1] = {x2, y2};
    c->tam = dist(&(c->corpo[0]), &(c->corpo[1]));
    c->vx = (double)(x1-x2)/c->tam; c->vy = (double)(y1-y2)/c->tam;
    return c;
}

// mata a cobra (e mostra o pau)
void matacobra(SL_cobra *cobra) {
    free(cobra->corpo);
    free(cobra->corpoi);
    free(cobra);
}

// desenha uma cobra na tela
void desenha_cobra(SDL_Renderer *renderer, SL_cobra *cobra) {
    SDL_SetRenderDrawColor(renderer, cobra->r, cobra->g, cobra->b, 255);
    SDL_RenderDrawLines(renderer, cobra->corpoi, cobra->ncorpo);
}

// rotaciona o vetor (x,y) ang graus horario na tela
void rotaciona(double &x, double &y, int ang) {
    double r = M_PI*(ang/180.0);
    double xr = x*cos(r) - y*sin(r);
    double yr = x*sin(r) + y*cos(r);
    x = xr; y = yr;
}

// anda off pixels para frente na cobra, girando ang graus horario
void walk(SL_cobra *cobra, int off, int ang) {

    if (ang==0) { // para frente

        // adiciona off pixels no primeiro segmento
        cobra->corpo[0].x += cobra->vx*off;
        cobra->corpo[0].y += cobra->vy*off;
    } else {

        // atualiza o versor diretor
        rotaciona(cobra->vx, cobra->vy, ang);

        // se for inteiro, coloca inteiro (xunxo para precisao)
        if (cmpf(lround(cobra->vx),cobra->vx)==0) cobra->vx = lround(cobra->vx);
        if (cmpf(lround(cobra->vy),cobra->vy)==0) cobra->vy = lround(cobra->vy);

        // cria um segmento de tamanho off com a orientacao do versor
        // TODO: ver se eh melhor usar uma deque para nao fazer este shift
        cobra->ncorpo++;
        cobra->corpo = (SL_Point *)realloc(cobra->corpo, cobra->ncorpo*sizeof(SL_Point));
        cobra->corpoi = (SDL_Point *)realloc(cobra->corpoi, cobra->ncorpo*sizeof(SDL_Point));
        for (int i=cobra->ncorpo-1;i>0;i--) {
            cobra->corpo[i] = cobra->corpo[i-1];
            cobra->corpoi[i] = cobra->corpoi[i-1];
        }
        cobra->corpo[0].x = cobra->corpo[1].x + cobra->vx*off;
        cobra->corpo[0].y = cobra->corpo[1].y + cobra->vy*off;
    }

    cobra->corpoi[0].x = cobra->corpo[0].x;
    cobra->corpoi[0].y = cobra->corpo[0].y;
    cobra->tam += off;

    // determina quantos segmentos remover da cobra
    // TODO: manter vetor de soma?
    int &n = cobra->ncorpo;
    double soma = 0.0;
    int qts = 0;
    while (cmpf(soma + dist(&(cobra->corpo[n-1-qts]), &(cobra->corpo[n-2-qts])), off)<=0) {
        soma += dist(&(cobra->corpo[n-1-qts]), &(cobra->corpo[n-2-qts]));
        qts++;
    }

    // retira qts segmentos da cobra
    if (n > 0) {
        n -= qts;
        cobra->corpo = (SL_Point *)realloc(cobra->corpo, n*sizeof(SL_Point));
        cobra->corpoi = (SDL_Point *)realloc(cobra->corpoi, n*sizeof(SDL_Point));
        cobra->tam -= soma;
    }

    // retira f=off-soma do ultimo segmento
    double f= off-soma;
    if (cmpf(f,0)>0) {
        double dultseg = dist(&(cobra->corpo[n-1]), &(cobra->corpo[n-2]));
        SL_Point v = { cobra->corpo[n-1].x - cobra->corpo[n-2].x, cobra->corpo[n-1].y - cobra->corpo[n-2].y };
        v.x = (v.x*(dultseg-f))/dultseg; v.y = (v.y*(dultseg-f))/dultseg;
        cobra->corpo[n-1] = { cobra->corpo[n-2].x + v.x, cobra->corpo[n-2].y + v.y };
        cobra->corpoi[n-1].x = cobra->corpo[n-1].x; cobra->corpoi[n-1].y = cobra->corpo[n-1].y;
        cobra->tam -= f;
    }

}
