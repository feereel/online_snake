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

#endif
