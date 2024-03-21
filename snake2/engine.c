#include "engine.h"

vector2 get_direction(uint8_t direction){
    vector2 dir2;

    switch (direction){
        case MOVE_UP:
            dir2.x = 0;
            dir2.y = -1;
            break;
        case MOVE_DOWN:
            dir2.x = 0;
            dir2.y = 1;
            break;
        case MOVE_LEFT:
            dir2.x = -1;
            dir2.y = 0;
            break;
        case MOVE_RIGTH:
            dir2.x = 1;
            dir2.y = 0;
            break;
    }
    return dir2;
}

void move_tail(snake* s){
    int prev_x = s->body[0].x;
    int prev_y = s->body[0].y;

    vector2 dir =  get_direction(s->direction);

    s->body[0].x += dir.x;
    s->body[0].y += dir.y;

    for (size_t i = 1; i < s->size; i++){
        int t_x = s->body[i].x;
        int t_y = s->body[i].y;

        s->body[i].x = prev_x;
        s->body[i].y = prev_y;

        prev_x = t_x;
        prev_y = t_y;
    }
}

vector2 get_fruit(vector2 field){
    vector2 pos;
    pos.x = (rand() % (field.x - 1)) + 1;
    pos.y = (rand() % (field.y - 1)) + 1;
    return pos;
}

bool check_player_collision(uint8_t player_id, player* players, int players_count){
    for (size_t i = 0; i < players_count; i++){
        for (size_t j = 0; j < players[i].data.snake.size; j++){
            if (i == player_id) continue;
            if (vector_cmp(players[player_id].data.snake.body[0], players[i].data.snake.body[j])){
                return true;
            }
        }
    }
    return false;
}

void increase_snake(snake* s){
    s->size++;
    s->body = realloc(s->body, sizeof(vector2) * s->size);

    vector2 dir = get_direction(s->direction);

    s->body[s->size - 1].x = s->body[s->size - 2].x - dir.x;
    s->body[s->size - 1].y = s->body[s->size - 2].y - dir.y;
}


bool check_wall_collision(player p, vector2 field){
    return (p.data.snake.body[0].x <= 0       ||
            p.data.snake.body[0].x >= field.x ||
            p.data.snake.body[0].y <= 0       ||
            p.data.snake.body[0].y >= field.y );
}

bool check_collisions(uint8_t player_id, player* players, int players_count, vector2 field){
    return check_wall_collision(players[player_id], field) || check_player_collision(player_id, players, players_count);
}