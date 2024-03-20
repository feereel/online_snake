#ifndef SNAKE_ENGINE
#define SNAKE_ENGINE

#include "protocol.h"
#include "handler.h"

void move_tail(snake* s);

vector2 get_fruit(vector2 field);

bool check_collisions(uint8_t player_id, player* players, int players_count, vector2 field);

void increase_snake(snake* s);

#endif
