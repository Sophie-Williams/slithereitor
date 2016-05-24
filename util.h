#include <SDL2/SDL.h>
#include <cmath>

#define EPS 1e-6

typedef struct {
    double x, y;
} SL_Point;             // A point with double coordinates

// A snake
typedef struct {
    int r,g,b;         // Color
    double size;       // lenght
    double vx, vy;     // vector indicating the direction
    int nbody;         // number of points in its body
    SL_Point *body;    // points in its body. The head is body[0]
    SDL_Point *bodyi;  // body[] with integer coordinates (for SDL)
} SL_snake;

inline int cmpf(double, double );

double dist(SL_Point *, SL_Point *);

SL_snake *create_snake(int, int, int, int, int, int, int);

void free_snake(SL_snake *);

void draw_snake(SDL_Renderer *, SL_snake *);

void rotate(double &, double &, int);

void walk(SL_snake *, int, int);
