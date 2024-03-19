#include "server.h"

#include <limits.h>
#include <stdarg.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>

void error(char* error_message){
    perror(error_message);
    exit(1);
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

int init_server(int16_t port, int clients_count){
    int serverfd; 
    struct sockaddr_in servaddr; 
   
    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd == -1) error("socket creation failed...\n");


   /* setsockopt: Handy debugging trick that lets 
    * us rerun the server immediately after we kill it; 
    * otherwise we have to wait about 20 secs.
    * Eliminates "ERROR on binding: Address already in use" error. 
    */
    int optval = 1;
    setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

    bzero(&servaddr, sizeof(servaddr)); 

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(port); 
   
    if ((bind(serverfd, (SA*)&servaddr, sizeof(servaddr))) != 0) error("Cannot bind server socket\n");
    if ((listen(serverfd, clients_count)) != 0) error("Listen failed...\n");


    return serverfd;
}

int* wait_clients(int serverfd, int clients_count){
    printf("Waiting %d clients connection...\n\n", clients_count);

    int connected = 0;
    int clientlen;
    int* clientsfd =  calloc(clients_count, sizeof(int));

    while (connected < clients_count){
        struct sockaddr_in client;
        clientlen = sizeof(client);
        clientsfd[connected] = accept(serverfd, (SA*)&client, (SL*) &clientlen);
        if (clientsfd[connected] < 0)
            error("Client connection error\n");

        printf("Client %d connected from %s:%d\n", connected+1, inet_ntoa(client.sin_addr), ntohs(client.sin_port));
        connected++;
    }
    printf("\n");
    return clientsfd;
}

void print_snakes(snake* snakes, int snakes_count){
    for (size_t i = 0; i < snakes_count; i++){
        printf("Snake %zu created in:\n", i);
        for (size_t j = 0; j < 5; j++){
            printf("\tx%zu:%hu y%zu:%hu\n", j, snakes[i].body[j].x, j, snakes[i].body[j].y);
        }
    }
}

// TODO: Implement better spawn logic
snake* generate_snakes(vector2 field, int snakes_count){
    printf("Creating %d snakes...\n", snakes_count);
    if (field.y * 2 < snakes_count) error("Too much snakes to create!\n");
    if (field.x < 20) error("Field is too small!\n");

    snake* snakes = calloc(snakes_count, sizeof(snake));
    for (size_t i = 0; i < snakes_count; i++){
        printf("Snake %zu created in:\n", i);
        for (size_t j = 0; j < 5; j++){
            snakes[i].body[j].x = 10 + j;
            snakes[i].body[j].y = i*2 + 1;
            snakes[i].direction = MOVE_LEFT;
            printf("\tx%zu:%lu y%zu:%lu\n", i, 4+j, i, i*2);
        }
    }
    
    printf("\n");
    return snakes;
}

char* create_message(imessage_header header, snake* snakes, int* total_size){
    *total_size = header.clients_count * sizeof(snake) + sizeof(imessage_header);
    char* return_imessage = calloc(*total_size, sizeof(char));
    memcpy(return_imessage, &header, sizeof(imessage_header));
    memcpy(return_imessage + sizeof(imessage_header), snakes, sizeof(snake) * header.clients_count);
    return return_imessage;
}

int send_imessages(int start_delay, struct timespec timestamp, int frame_delay, int clients_count, vector2 field, snake* snakes, int* clientsfd){
    imessage_header header;
    header.magic = 0x30;
    header.timestamp = timestamp;
    header.frame_delay = frame_delay;
    header.clients_count = clients_count;
    header.position = field;

    printf("Sending IMessages...\n");
    for (size_t i = 0; i < clients_count; i++){
        header.user_id = i;

        int total_size;
        char* imessage = create_message(header, snakes, &total_size);
        write(clientsfd[i], imessage, total_size);
        printf("IMessage sent to client %zu\n", i);
        free(imessage);
    }
    return clients_count;
}

char read_last_direction(char* buf, int size){
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

void send_snakes(snake* snakes, int* clientsfd, int clients_count){
    for (size_t i = 0; i < clients_count; i++){
        send_sheader sh;
        sh.magic = 0x50;
        sh.snake_count = clients_count;
        write(clientsfd[i], &sh, sizeof(send_sheader));
        for (size_t j = 0; j < clients_count; j++){
            send_sbody cs;
            cs.s = snakes[j];
            cs.s_id = j;
            write(clientsfd[i], &cs, sizeof(send_sbody));
        }
    }
}

void move_tail(snake* s, int size){
    int prev_x = s->body[0].x;
    int prev_y = s->body[0].y;
    int dir_x = 0;
    int dir_y = 0;
    switch (s->direction){
        case MOVE_UP:
            dir_y = -1;
            break;
        case MOVE_DOWN:
            dir_y = 1;
            break;
        case MOVE_LEFT:
            dir_x = -1;
            break;
        case MOVE_RIGTH:
            dir_x = 1;
            break;
    }
    s->body[0].x += dir_x;
    s->body[0].y += dir_y;

    for (size_t i = 1; i < size; i++){
        int t_x = s->body[i].x;
        int t_y = s->body[i].y;

        s->body[i].x = prev_x;
        s->body[i].y = prev_y;

        prev_x = t_x;
        prev_y = t_y;
    }
}

void game_loop(int serverfd, struct timespec timeout, ll frame_delay, int* clientsfd, snake *snakes, int clients_count){
    int max_fd = serverfd;
    for (size_t i = 0; i < clients_count; i++)
        if (clientsfd[i] > max_fd) max_fd = clientsfd[i];


    // waiting until game starts
    struct timeval to_wait = get_time_to_wait(timeout);
    select(max_fd+1, NULL, NULL, NULL, &to_wait);

    ll recieve_delay = frame_delay / 2;
    ll send_delay = frame_delay - recieve_delay;

    char buf[BUFSIZE];

    while (1){
        fd_set readfds;
        FD_ZERO(&readfds);
        for (size_t i = 0; i < clients_count; i++){
            FD_SET(clientsfd[i], &readfds);
        }

        timeout.tv_nsec += recieve_delay * 1000000;
        to_wait = get_time_to_wait(timeout);

        if (select(max_fd + 1, &readfds, NULL, NULL, &to_wait) < 0) error("Error with select...\n");
        for (size_t i = 0; i < clients_count; i++){
            if (FD_ISSET(clientsfd[i], &readfds)){
                int readed = read(clientsfd[i], buf, BUFSIZE);
                if (readed > 0){
                    snakes[i].direction = read_last_direction(buf, readed);
                    printf("Snake %zu changed direction to %d\n", i, snakes[i].direction);
                } else {
                    printf("Client %zu disconected!\n", i);
                    exit(0);
                }
            }
            move_tail(&snakes[i], 5);
        }
        printf("All mesages are recieved!\n");

        timeout.tv_nsec += send_delay * 1000000;
        to_wait = get_time_to_wait(timeout);
        select(max_fd+1, NULL, NULL, NULL, &to_wait);

        send_snakes(snakes, clientsfd, clients_count);
        print_snakes(snakes, clients_count);

        printf("All mesages are sended!\n");
    }
}

int main(int argc, char* argv[]) {
    int port = DPORT;
    int clients_count = DCLIENTS_COUNT;
    int start_delay = DSTART_DELAY; // задержка перед началом игры в с
    int frame_delay = DFRAME_DELAY; // задержка перед началом игры в мс
    vector2 field;
    field.x = DWIDTH;
    field.y = DHEIGTH;
    
    int r = 0;
    while((r=getopt(argc, argv, "p:n:h:w:d:t:")) != -1){
        switch(r){
            case 'p':
                port = atoi(optarg);
                break;
            case 'n':
                clients_count = atoi(optarg);
                break;
            case 'w':
                field.x = atoi(optarg);
                break;
            case 'h':
                field.y = atoi(optarg);
                break;
            case 't':
                start_delay = atoi(optarg);
                break;
            case 'd':
                frame_delay = atoi(optarg);
                break;
        }
    }
    
    int serverfd = init_server((int16_t)port, clients_count);

    //TODO: need to close connection
    int* clientsfd = wait_clients(serverfd, clients_count);

    struct timespec ct;
    clock_gettime(CLOCK_REALTIME, &ct);
    ct.tv_sec += start_delay;

    snake* snakes = generate_snakes(field, clients_count);
    send_imessages(start_delay, ct, frame_delay, clients_count, field, snakes, clientsfd);

    game_loop(serverfd, ct, frame_delay, clientsfd, snakes, clients_count);

    free(snakes);
    free(clientsfd);
    close(serverfd);
}
