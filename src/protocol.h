#ifndef SNAKE_PROTOCOL
#define SNAKE_PROTOCOL

#include <stdlib.h> 
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>


#define MOVE_UP    0x0
#define MOVE_DOWN  0x1
#define MOVE_RIGTH 0x2
#define MOVE_LEFT  0x3

#define SNAKE_INITIAL_SIZE 5

typedef struct {
    int16_t x;
    int16_t y;
} __attribute__((packed)) vector2;

typedef struct {
    uint8_t magic;
    uint16_t frame_delay;
    vector2 field;
    uint16_t players_count;
    uint16_t user_id;
} __attribute__((packed)) imessage_header;

typedef struct {
    uint8_t direction;
    uint16_t size;
    vector2* body;
} __attribute__((packed)) snake;


typedef struct {
    imessage_header header;
    snake* snakes;
} __attribute__((packed)) imessage;

typedef struct {
    uint8_t magic;
    uint16_t snake_count;
} __attribute__((packed)) send_sheader;

typedef struct {
    uint16_t id;
    snake snake;
} __attribute__((packed)) p_snake;

typedef struct {
    uint8_t magic;
    vector2 position;
} __attribute__((packed)) send_fdata;

typedef struct {
    uint8_t magic;
    uint16_t place;
} __attribute__((packed)) game_end;

typedef struct {
    uint8_t clientfd;
    bool connected;
    p_snake data;
} player;

typedef struct {
    uint8_t magic;
    uint16_t id;
} __attribute__((packed)) snake_inc_msg;

#endif
