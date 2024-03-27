#include "handler.h"

void error(char* error_message){
    perror(error_message);
    exit(1);
}

bool vector_cmp(vector2 v1, vector2 v2){
    return v1.x == v2.x && v1.y == v2.y;
}

struct timeval get_time_to_wait(struct timespec end_time){
    struct timespec current_time;
    clock_gettime(CLOCK_REALTIME, &current_time);

    ll ctotal_time = current_time.tv_sec * 1000 + current_time.tv_nsec / 1000000; // in milliseconds
    ll ptotal_time = end_time.tv_sec * 1000 + end_time.tv_nsec / 1000000;  // in milliseconds
    ll diff_time   = ptotal_time - ctotal_time; // in milliseconds

    struct timeval to_wait;
    to_wait.tv_sec = diff_time / 1000;
    to_wait.tv_usec = (diff_time % 1000) * 1000;
    return to_wait;
}

void print_player(player p){
    printf("Player %hu:\n", p.data.id);
    for (size_t j = 0; j < p.data.snake.size; j++){
        printf("\tx%zu:%hu y%zu:%hu\n", j, p.data.snake.body[j].x, j, p.data.snake.body[j].y);
    }
}

// TODO: Implement better spawn logic
snake* generate_snakes(vector2 field, int snakes_count){
    printf("Creating %d snakes...\n", snakes_count);
    if (field.y * 2 < snakes_count) error("Too much snakes to create!\n");
    if (field.x < 20) error("Field is too small!\n");

    snake* snakes = calloc(snakes_count, sizeof(snake));
    for (size_t i = 0; i < snakes_count; i++){
        snakes[i].size = SNAKE_INITIAL_SIZE;
        snakes[i].direction = MOVE_LEFT;
        snakes[i].body = calloc(sizeof(vector2), SNAKE_INITIAL_SIZE);
        int x = ((rand()) % (field.x - 2 * SNAKE_INITIAL_SIZE)) + SNAKE_INITIAL_SIZE;
        for (size_t j = 0; j < SNAKE_INITIAL_SIZE; j++){
            snakes[i].body[j].x = x + j;
            snakes[i].body[j].y = i*4 + 1;
        }
    }
    
    printf("\n");
    return snakes;
}

char* create_imessage(imessage_header header, snake* snakes, int* total_size){
    *total_size = sizeof(imessage_header);
    for (size_t i = 0; i < header.players_count; i++){
        *total_size += snakes[i].size * sizeof(vector2) + sizeof(uint8_t) + sizeof(uint16_t);
    }
    
    char* return_imessage = calloc(*total_size, sizeof(char));
    memcpy(return_imessage, &header, sizeof(imessage_header));

    char* p = return_imessage;
    p += sizeof(imessage_header);
    for (size_t i = 0; i < header.players_count; i++){
        memcpy(p, &(snakes[i].direction), sizeof(uint8_t));
        p += sizeof(uint8_t);

        memcpy(p, &(snakes[i].size), sizeof(uint16_t));
        p += sizeof(uint16_t);

        memcpy(p, snakes[i].body, snakes[i].size * sizeof(vector2));
        p += snakes[i].size * sizeof(vector2);
    }
    
    return return_imessage;
}

char* create_smeesage(player* p, int* total_size){
    *total_size = sizeof(vector2) * (p->data.snake.size) + sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint16_t);
    
    char* return_imessage = calloc(*total_size, sizeof(uint8_t));

    char* ptr = return_imessage;
    memcpy(ptr, &(p->data.id), sizeof(uint16_t));
    ptr += sizeof(uint16_t);

    memcpy(ptr, &(p->data.snake.direction), sizeof(uint8_t));
    ptr += sizeof(uint8_t);

    memcpy(ptr, &(p->data.snake.size), sizeof(uint16_t));
    ptr += sizeof(uint16_t);

    memcpy(ptr, p->data.snake.body, p->data.snake.size * sizeof(vector2));
    ptr += p->data.snake.size * sizeof(vector2);

    
    return return_imessage;
}

char get_last_direction(char* buf, int size){
    for (int i = size - 1; i >= 0; i--){
        printf("Buf[%d]: %d\n", i, buf[i]);
        switch (buf[i]){
            case MOVE_UP:
                return MOVE_UP;
            case MOVE_DOWN:
                return MOVE_DOWN;
            case MOVE_RIGTH:
                return MOVE_RIGTH;
            case MOVE_LEFT:
                return MOVE_LEFT;
        }
    }
    return MOVE_LEFT;
}

player link_player_and_snake(int clientfd, p_snake s){
    player p;
    p.connected = true;
    p.clientfd = clientfd;
    p.data = s;
    return p;
}

player* link_players_and_snakes(int* clientsfd, snake* snakes, int size){
    player* players = calloc(size, sizeof(player));
    for (size_t i = 0; i < size; i++){
        p_snake tmp;
        tmp.id = i;
        tmp.snake = snakes[i];
        players[i] = link_player_and_snake(clientsfd[i], tmp);
    }
    return players;
}

bool is_opposite_directions(uint8_t dir1, uint8_t dir2){
    return ((dir1 == MOVE_UP && dir2 == MOVE_DOWN) || 
            (dir1 == MOVE_DOWN && dir2 == MOVE_UP) ||
            (dir1 == MOVE_LEFT && dir2 == MOVE_RIGTH) ||
            (dir1 == MOVE_RIGTH && dir2 == MOVE_LEFT));
}

vector2 get_fruit(vector2 field){
    vector2 pos;
    pos.x = (rand() % (field.x - 3)) + 1;
    pos.y = (rand() % (field.y - 3)) + 1;
    return pos;
}
