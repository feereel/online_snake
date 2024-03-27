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
#define DFRAME_DELAY 200
#define DWIDTH 80
#define DHEIGTH 24

#define BUFSIZE 1024

#define ll long long

#define SNAKE_INITIAL_SIZE 7

int port = DPORT;
int players_count = DCLIENTS_COUNT;
uint16_t remaining_players_count = DCLIENTS_COUNT;
int start_delay = DSTART_DELAY; // задержка перед началом игры в с
int frame_delay = DFRAME_DELAY; // задержка перед началом игры в мс
vector2 field = {DWIDTH, DHEIGTH};

char usage[] = "Usage: ./server.exe [-p port] [-n players_count] [-d ms] [-t sec] [-x width] [-y height]\n"
                   "\t -h Usage message\n"
                   "\t -p Port\n"
                   "\t -n Players count\n"
                   "\t -d Delay between frames in milliseconds\n"
                   "\t -t Delay in seconds between players connecting and the initialization message being sent\n"
                   "\t -x Field width\n"
                   "\t -y Field heigth\n\n"
                   "Example: ./server.exe -p 2422 -n 3 -d 600 -t 5\n"
                   "This creates a server on port 2422 that is waiting for 3 players.\n"
                   "After all players have connected, the server waits 5 seconds and the game starts.\n"
                   "The server will send new data every 600ms.\n";

pthread_mutex_t mutex_remaing_players_count;

#endif
