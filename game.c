#include <ncurses.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <pthread.h> 


#define INITIAL_SPEED 100*1000 // 0.1 sec
#define FRUIT_SPAWN_TIME 1000*1000 * 3  // 3 sec

#define MAX_FRUITS 5


int dir_x = 0;
int dir_y = 0;

int width;
int height;

typedef struct {
    int x;
    int y;
} vector2;

vector2 fruits[MAX_FRUITS];
int fruits_count = 0;

void *input(void *vargp) { 
    char k = 0;
    while(TRUE){
        k = getch();
        if (k == 'w' && !dir_y){
            dir_y = -1;
            dir_x = 0;
        } else if (k == 's' && !dir_y){
            dir_y = 1;
            dir_x = 0;
        } else if (k == 'd' && !dir_x){
            dir_y = 0;
            dir_x = 1;
        } else if (k == 'a' && !dir_x){
            dir_y = 0;
            dir_x = -1;
        }
    }
    return NULL; 
}

void *spawn_fruit(void *vargp) { 
    while(TRUE){
        usleep(FRUIT_SPAWN_TIME);
        if (fruits_count >= MAX_FRUITS) continue;

        int fruit_i = -1;
        for (size_t i = 0; i < MAX_FRUITS; i++){
            if (fruits[i].x == -1){
                fruit_i = i;
                break;
            }
        }
        if (fruit_i == -1){
            printf("error with finding fruit free index!");
            exit(111);
        }

        fruits[fruit_i].x = rand() % width;
        fruits[fruit_i].y = rand() % height;
        fruits_count++;
    }
    return NULL; 
}


// return index in fruits if exists else -1
int check_fruitpos(int x, int y){
    for (size_t i = 0; i < MAX_FRUITS; i++){
        if (fruits[i].x == x && fruits[i].y == y)
            return i;
    }
    return -1;
}

bool check_collision(vector2* tail, int tail_size){
    for (size_t i = 1; i < tail_size; i++){
        if (tail[i].x == tail[0].x && tail[i].y == tail[0].y) return TRUE;
    }
    return FALSE;
}

void move_tail(vector2* tail, int tail_size){
    int prev_x = tail[0].x;
    int prev_y = tail[0].y;
    tail[0].x = (tail[0].x < 0) ? width - 1: (tail[0].x + dir_x) % width;
    tail[0].y = (tail[0].y < 0) ? height - 1 : (tail[0].y + dir_y) % height;

    for (size_t i = 1; i < tail_size; i++){
        int t_x = tail[i].x;
        int t_y = tail[i].y;

        tail[i].x = prev_x;
        tail[i].y = prev_y;

        prev_x = t_x;
        prev_y = t_y;
    }
}

void render_tail(vector2* tail, int tail_size){
    mvaddch(tail[0].y, tail[0].x, '0');
    for (size_t i = 1; i < tail_size; i++){
        mvaddch(tail[i].y, tail[i].x, 'o');
    }
}

void render_fruits(){
    for (size_t i = 0; i < MAX_FRUITS; i++){
        if (fruits[i].x == -1) continue;
        mvaddch(fruits[i].y, fruits[i].x, '@');
    }
}

void increase_tail(vector2** tail, int* tail_size){
    (*tail_size)++;
    *tail = realloc(*tail, *tail_size);
    (*tail)[*tail_size - 1].x = (*tail)[*tail_size - 2].x - dir_x;
    (*tail)[*tail_size - 1].y = (*tail)[*tail_size - 2].y - dir_y;
}


int main() {
    initscr();
    noecho();
    curs_set(0);

    getmaxyx(stdscr, height, width);

    int score = 0;

    vector2* tail;
    int tail_size = 1;
    
    tail = calloc(tail_size, sizeof(vector2));
    tail[0].x = width/2;
    tail[0].y = height/2;

    // clear fruits ... TODO: change this to init function
    for (size_t i = 0; i < MAX_FRUITS; i++){
        fruits[i].x = -1;
    }

    pthread_t thread_input; 
    pthread_create(&thread_input, NULL, input, NULL);

    pthread_t thread_fruits; 
    pthread_create(&thread_fruits, NULL, spawn_fruit, NULL);

    bool collided = FALSE;

    while(!collided){
        clear();
        render_tail(tail, tail_size);
        render_fruits();
        int eated_fruit = check_fruitpos(tail[0].x, tail[0].y);
        
        if (eated_fruit >= 0){
            score++;

            increase_tail(&tail, &tail_size);
            fruits_count--;
            fruits[eated_fruit].x = -1;
        }

        usleep(INITIAL_SPEED);

        move_tail(tail, tail_size);
        collided = check_collision(tail, tail_size);

        refresh();
    }

    clear();
    printw("You died! Your score: %d", score);
    getch();

    endwin();
    return 0;
}