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

// create a snake with head at (x1,y1), tail at (x2,y2), and color (r,g,b)
SL_snake *create_snake(int x1, int y1, int x2, int y2, int r, int g, int b) {
    SL_snake *c = (SL_snake *)malloc(sizeof(SL_snake));
    c->r = r; c->g = g; c->b = b;
    c->nbody = 2;
    c->body = (SL_Point *)malloc(2*sizeof(SL_Point));
    c->bodyi = (SDL_Point *)malloc(2*sizeof(SDL_Point));
    c->body[0] = {(double)x1, (double)y1}; c->body[1] = {(double)x2, (double)y2};
    c->bodyi[0] = {x1, y1}; c->bodyi[1] = {x2, y2};
    c->size = dist(&(c->body[0]), &(c->body[1]));
    c->vx = (double)(x1-x2)/c->size; c->vy = (double)(y1-y2)/c->size;
    return c;
}

// free the snake
void free_snake(SL_snake *snake) {
    free(snake->body);
    free(snake->bodyi);
    free(snake);
}

// Draw the snake with its color
void draw_snake(SDL_Renderer *renderer, SL_snake *snake) {
    SDL_SetRenderDrawColor(renderer, snake->r, snake->g, snake->b, 255);
    SDL_RenderDrawLines(renderer, snake->bodyi, snake->nbody);
}

// rotate vector (x,y) ang degrees clockwise
void rotate(double &x, double &y, int ang) {
    double r = M_PI*(ang/180.0);
    double xr = x*cos(r) - y*sin(r);
    double yr = x*sin(r) + y*cos(r);
    x = xr; y = yr;
}

// walk the snake off pixels turning it ang degrees
void walk(SL_snake *snake, int off, int ang) {

    if (ang==0) { // forward
        // increase the first segment off pixels
        snake->body[0].x += snake->vx*off;
        snake->body[0].y += snake->vy*off;
    } else {

        // update the directing vector
        rotate(snake->vx, snake->vy, ang);

        // if its coordinates are integer, round them
        // (workaround for better precision)
        if (cmpf(lround(snake->vx),snake->vx)==0) snake->vx = lround(snake->vx);
        if (cmpf(lround(snake->vy),snake->vy)==0) snake->vy = lround(snake->vy);

        // create a new segment with lenght off
        // TODO: maybe use a deque ?
        snake->nbody++;
        snake->body = (SL_Point *)realloc(snake->body, snake->nbody*sizeof(SL_Point));
        snake->bodyi = (SDL_Point *)realloc(snake->bodyi, snake->nbody*sizeof(SDL_Point));
        for (int i=snake->nbody-1;i>0;i--) {
            snake->body[i] = snake->body[i-1];
            snake->bodyi[i] = snake->bodyi[i-1];
        }
        snake->body[0].x = snake->body[1].x + snake->vx*off;
        snake->body[0].y = snake->body[1].y + snake->vy*off;
    }

    snake->bodyi[0].x = snake->body[0].x;
    snake->bodyi[0].y = snake->body[0].y;
    snake->size += off;

    // determine how many segments will be removed from the tail
    // TODO: maintain the prefix sum array?
    int &n = snake->nbody;
    double sum = 0.0;
    int hmany = 0;
    while (cmpf(sum + dist(&(snake->body[n-1-hmany]), &(snake->body[n-2-hmany])), off)<=0) {
        sum += dist(&(snake->body[n-1-hmany]), &(snake->body[n-2-hmany]));
        hmany++;
    }

    // remove hmany segments
    if (hmany > 0) {
        n -= hmany;
        snake->body = (SL_Point *)realloc(snake->body, n*sizeof(SL_Point));
        snake->bodyi = (SDL_Point *)realloc(snake->bodyi, n*sizeof(SDL_Point));
        snake->size -= sum;
    }

    // decrease f=off-sum pixels from the last segment
    double f= off-sum;
    if (cmpf(f,0)>0) {
        double dlastseg = dist(&(snake->body[n-1]), &(snake->body[n-2]));
        SL_Point v = { snake->body[n-1].x - snake->body[n-2].x, snake->body[n-1].y - snake->body[n-2].y };
        v.x = (v.x*(dlastseg-f))/dlastseg; v.y = (v.y*(dlastseg-f))/dlastseg;
        snake->body[n-1] = { snake->body[n-2].x + v.x, snake->body[n-2].y + v.y };
        snake->bodyi[n-1].x = snake->body[n-1].x; snake->bodyi[n-1].y = snake->body[n-1].y;
        snake->size -= f;
    }

}
