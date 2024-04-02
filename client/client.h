#ifndef SNAKE_CLIENT
#define SNAKE_CLIENT

#include <ncurses.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <pthread.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../src/handler.h"
#include "../src/protocol.h"
#include "../src/engine.h"
#include "../src/render.h"

#define DSERVER_IP "127.0.0.1"
#define DSERVER_PORT 2422

#define BUFSIZE 1024

#endif
