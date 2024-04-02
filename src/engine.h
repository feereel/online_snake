#ifndef SNAKE_ENGINE
#define SNAKE_ENGINE

#include "protocol.h"
#include "handler.h"

void move_snake(snake* s);

bool check_collisions(uint8_t player_id, player* players, int players_count, vector2 field);

void increase_snake(snake* s);

#endif
