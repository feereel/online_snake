#include "render.h"

void render_boundary(vector2 field){
    printf("%d %d", field.x, field.y);
    for (size_t i = 0; i < field.x; i++){
        mvaddch(0, i, BORDER);
        mvaddch(field.y-1, i, BORDER);
    }
    for (size_t i = 0; i < field.y; i++){
        mvaddch(i, 0, BORDER);
        mvaddch(i, field.x-1, BORDER);
    }
}

void render_snake(snake s, bool highligth){
    if (highligth) attron(COLOR_PAIR(1));

    mvaddch(s.body[0].y, s.body[0].x, SNAKE_HEAD);
    for (size_t i = 1; i < s.size; i++){
        mvaddch(s.body[i].y, s.body[i].x, SNAKE_BODY);
    }
    attroff(COLOR_PAIR(2));
}

void render_fruit(vector2 position){
    mvaddch(position.y, position.x, FRUIT);
}
