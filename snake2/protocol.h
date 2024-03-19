#ifndef SNAKE_PROTOCOL
#define SNAKE_PROTOCOL

#include <stdlib.h> 
#include <unistd.h>
#include <time.h>

#define MOVE_UP    0x0
#define MOVE_DOWN  0x1
#define MOVE_RIGTH 0x2
#define MOVE_LEFT  0x3

typedef struct {
    uint16_t x;
    uint16_t y;
} __attribute__((packed)) vector2;

typedef struct {
    char magic;
    struct timespec timestamp;
    uint16_t frame_delay;
    vector2 position;
    uint16_t clients_count;
    uint16_t user_id;
} __attribute__((packed)) imessage_header;

typedef struct {
    char direction;
    vector2 body[5];
} __attribute__((packed)) snake;


typedef struct {
    imessage_header header;
    snake* snakes;
} __attribute__((packed)) imessage;

typedef struct {
    char magic;
    uint16_t snake_count;
} __attribute__((packed)) send_sheader;

typedef struct {
    char s_id ;
    snake s;
} __attribute__((packed)) send_sbody;

#endif
