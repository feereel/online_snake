#ifndef SNAKE_HANDLER
#define SNAKE_HANDLER

#include <stdio.h>
#include <string.h>

#include "protocol.h"

#define ll long long

void error(char* error_message);

bool vector_cmp(vector2 v1, vector2 v2);

struct timeval get_time_to_wait(struct timespec end_time);

void print_player(player p);

snake* generate_snakes(vector2 field, int snakes_count);

char* create_imessage(imessage_header header, snake* snakes, int* total_size);

char* create_smeesage(player* p, int* total_size);

char get_last_direction(char* buf, int size);

player* link_players_and_snakes(int* clientsfd, snake* snakes, int size);

void* handle_player(void* args);

bool is_opposite_directions(uint8_t dir1, uint8_t dir2);

#endif
