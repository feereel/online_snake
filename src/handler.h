#ifndef SNAKE_HANDLER
#define SNAKE_HANDLER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "protocol.h"

#define ll long long

void error(char* error_message);

bool vector_cmp(vector2 v1, vector2 v2);

struct timeval get_time_to_wait(struct timespec end_time);

void print_player(player p);
void print_snake(snake s);

snake* generate_snakes(vector2 field, int snakes_count);

char* create_imessage(imessage_header header, snake* snakes, int* total_size);
char* create_smeesage(player* p, int* total_size);

uint8_t input_direction(void);
vector2 get_direction(uint8_t direction);
uint8_t get_last_direction(char* buf, int size);

player* link_players_and_snakes(int* clientsfd, snake* snakes, int size);

void* handle_player(void* args);

bool is_opposite_directions(uint8_t dir1, uint8_t dir2);

vector2 get_fruit(vector2 field);

void send_fruit_eaten(player* players, int players_count, uint16_t index);

#endif
