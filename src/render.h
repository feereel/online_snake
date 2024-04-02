#ifndef SNAKE_RENDER
#define SNAKE_RENDER

#include "handler.h"
#include "ncurses.h"

#define BORDER '#'
#define SNAKE_HEAD '0'
#define SNAKE_BODY 'o'
#define FRUIT '%'

void render_boundary(vector2 field);
void render_snake(snake s, bool highligth);
void render_fruit(vector2 position);

#endif
