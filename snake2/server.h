#ifndef SNAKE_SERVER
#define SNAKE_SERVER

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <pthread.h> 
#include <time.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h>
#include <stdarg.h>
#include <sys/select.h>
#include <sys/time.h>

#include "protocol.h"

#define MAX 80 
#define SA struct sockaddr 
#define SL socklen_t

#define DPORT 2422
#define DCLIENTS_COUNT 2
#define DSTART_DELAY 3
#define DFRAME_DELAY 100
#define DWIDTH 80
#define DHEIGTH 24

#define BUFSIZE 1024

#define ll long long

int port = DPORT;
int players_count = DCLIENTS_COUNT;
int remaining_players_count = DCLIENTS_COUNT;
int start_delay = DSTART_DELAY; // задержка перед началом игры в с
int frame_delay = DFRAME_DELAY; // задержка перед началом игры в мс
vector2 field = {DWIDTH, DHEIGTH};

pthread_mutex_t mutex_remaing_players_count;

#endif
