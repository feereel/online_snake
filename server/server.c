#include "server.h"

int port = DPORT;
int players_count = DCLIENTS_COUNT;
uint16_t remaining_players_count = DCLIENTS_COUNT;
int start_delay = DSTART_DELAY; // задержка перед началом игры в с
int frame_delay = DFRAME_DELAY; // задержка перед началом игры в мс
vector2 field = {DWIDTH, DHEIGTH};


int init_server(int16_t port, int players_count){
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
    if ((listen(serverfd, players_count)) != 0) error("Listen failed...\n");


    return serverfd;
}

int* wait_clients(int serverfd, int players_count){
    printf("Waiting %d clients connection...\n\n", players_count);

    int connected = 0;
    int clientlen;
    int* clientsfd =  calloc(players_count, sizeof(int));

    while (connected < players_count){
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

int send_imessages(int start_delay, int frame_delay, int players_count, vector2 field, snake* snakes, int* clientsfd){
    imessage_header header;
    header.magic = 0x30;
    header.frame_delay = frame_delay;
    header.players_count = players_count;
    header.field = field;

    printf("Sending IMessages...\n");
    for (size_t i = 0; i < players_count; i++){
        header.user_id = i;

        int total_size;
        char* imessage = create_imessage(header, snakes, &total_size);
        write(clientsfd[i], imessage, total_size);
        printf("IMessage sent to client %zu\n", i);
        free(imessage);
    }
    return players_count;
}

void send_snakes(int player_id, player* players, int players_count){
    send_sheader sh;
    sh.magic = 0x50;
    sh.snake_count = remaining_players_count;
    write(players[player_id].clientfd, &sh, sizeof(send_sheader));
    for (size_t j = 0; j < players_count; j++){
        if(!players[j].connected) continue;

        int total_size;
        char* smessage = create_smeesage(&(players[j]), &total_size);
        write(players[player_id].clientfd, smessage, total_size);
        free(smessage);
    }
}

void send_fruit(player player, vector2 fruit_pos){
    send_fdata fruit;
    fruit.magic = 0xb0;
    fruit.position = fruit_pos;
    write(player.clientfd, &fruit, sizeof(send_fdata));
}

void remove_player(player* p){
    if (!p->connected){
        pthread_mutex_unlock(&mutex_remaing_players_count);
        return;
    }

    game_end end;
    end.magic = 0xff;
    end.place = remaining_players_count;
    write(p->clientfd, &end, sizeof(game_end));

    p->connected = false;
    close(p->clientfd);

    remaining_players_count--;
    printf("Client %hu disconected!\n", p->data.id);

    pthread_mutex_unlock(&mutex_remaing_players_count);
}

void* handle_player(void* args){
    char buf[BUFSIZE];
    player* p = args;
    int readed;
    while((readed = read(p->clientfd, buf, BUFSIZE)) > 0){
        uint8_t new_dir = get_last_direction(buf, readed);
        if (is_opposite_directions(new_dir, p->data.snake.direction)) continue;

        p->data.snake.direction = new_dir;
        
        printf("Snake %hu changed direction to %d\n", p->data.id, p->data.snake.direction);
    }
    remove_player(p);
    printf("Thread is done!\n");
    return NULL;
}

void game_loop(int serverfd, struct timespec timeout, int* clientsfd, player *players, int players_count){
    srand((unsigned int) time(NULL));

    int max_fd = serverfd;
    for (size_t i = 0; i < players_count; i++)
        if (clientsfd[i] > max_fd) max_fd = clientsfd[i];


    // waiting until game starts
    struct timeval to_wait = get_time_to_wait(timeout);
    select(max_fd+1, NULL, NULL, NULL, &to_wait);

    vector2 fruit = get_fruit(field);
    bool eaten = true;
    while (remaining_players_count > 0){
        printf("Current fruit in x:%hu y:%hu\n", fruit.x, fruit.y);

        pthread_mutex_lock(&mutex_remaing_players_count);
        for (size_t i = 0; i < players_count; i++){
            if (!players[i].connected) continue;

            move_snake(&players[i].data.snake);

            if (!eaten && vector_cmp(players[i].data.snake.body[0], fruit)){
                increase_snake(&players[i].data.snake);
                send_fruit_eaten(players, players_count, i);
                eaten = true;
            }

            send_snakes(i, players, players_count);

            print_player(players[i]);

            if(check_collisions(i, players, players_count, field)){
                remove_player(&players[i]);
            }
        }
        pthread_mutex_unlock(&mutex_remaing_players_count);

        if (eaten){
            fruit = get_fruit(field);
            for (size_t i = 0; i < players_count; i++){
                send_fruit(players[i], fruit);
            }
        }

        timeout.tv_nsec += frame_delay * 1000000;
        to_wait = get_time_to_wait(timeout);
        select(max_fd + 1, NULL, NULL, NULL, &to_wait);

        eaten = false;
    }
}

int main(int argc, char* argv[]) {
    int r = 0;

    while((r=getopt(argc, argv, "p:n:x:y:d:t:h")) != -1){
        switch(r){
            case 'p':
                port = atoi(optarg);
                break;
            case 'n':
                players_count = atoi(optarg);
                remaining_players_count = players_count;
                break;
            case 'x':
                field.x = atoi(optarg);
                break;
            case 'y':
                field.y = atoi(optarg);
                break;
            case 't':
                start_delay = atoi(optarg);
                break;
            case 'd':
                frame_delay = atoi(optarg);
                break;
            case 'h':
                printf("%s", usage);
                exit(0);
        }
    }
    
    int serverfd = init_server((int16_t)port, players_count);

    int* clientsfd = wait_clients(serverfd, players_count);

    struct timespec ct;
    clock_gettime(CLOCK_REALTIME, &ct);
    ct.tv_sec += start_delay;

    snake* snakes = generate_snakes(field, players_count);
    send_imessages(start_delay, frame_delay, players_count, field, snakes, clientsfd);

    player *players = link_players_and_snakes(clientsfd, snakes, players_count);

    pthread_mutex_init(&mutex_remaing_players_count, NULL);
    pthread_t* th = malloc(sizeof(pthread_t) * players_count);
    for (size_t i = 0; i < players_count; i++){
        if (pthread_create(&th[i], NULL, &handle_player, &players[i]) != 0) perror("Failed to create the thread");
    }

    game_loop(serverfd, ct, clientsfd, players, players_count);
    
    free(snakes);
    free(players);
    free(clientsfd);
    close(serverfd);
}
