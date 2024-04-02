#include "client.h"

int serverfd;
int server_port = DSERVER_PORT;
char* server_ip = DSERVER_IP;

imessage_header init_data;
snake* snakes;
bool* snake_alive;
vector2 fruit_pos;


void handle_server_disconnect(void){
    clear();
    printw("Game over!\nServer disconnected\n");
    refresh();
    exit(0);
}

void recieve_dirs(void){
    uint16_t snakes_count;

    int readed = read(serverfd, &snakes_count, sizeof(uint16_t));
    if (readed <= 0) handle_server_disconnect();

    memset(snake_alive, false, sizeof(bool) * init_data.players_count);
    for (size_t i = 0; i < snakes_count; i++){
        uint16_t snake_id;
        recv(serverfd, &snake_id, sizeof(uint16_t), 0);
        recv(serverfd, &snakes[snake_id].direction, sizeof(uint8_t), 0);
        snake_alive[snake_id] = true;
    }
}

void recieve_fruit(void){
    int readed = read(serverfd, &fruit_pos.x, 2);
    if (readed <= 0) handle_server_disconnect();

    readed = read(serverfd, &fruit_pos.y, 2);
    if (readed <= 0) handle_server_disconnect();
}

void recieve_inc_snake(void){
    uint16_t snake_id;
    int readed = read(serverfd, &snake_id, 2);
    if (readed <= 0) handle_server_disconnect();
    increase_snake(&snakes[snake_id]);
}

void recieve_win(void){
    uint16_t position;
    int readed = read(serverfd, &position, 2);
    if (readed <= 0) handle_server_disconnect();
    
    clear();
    printw("Game over!\nYour position is: %hu", position);
    refresh();
    exit(0);
}


bool handle_game_messages(void){
    uint8_t magic;
    int readed = read(serverfd, &magic, 1);
    if (readed <= 0) handle_server_disconnect();

    switch (magic){
        case 0x50:
            recieve_dirs();
            return true;
        case 0xb0:
            recieve_fruit();
            return false;
        case 0xea:
            recieve_inc_snake();
            return false;
        case 0xff:
            recieve_win();
            return false;
    }
    return false;
}

void handle_init_message(void){
    int readed = read(serverfd, &init_data, sizeof(imessage_header));
    if (readed <= 0 || init_data.magic != 0x30) handle_server_disconnect();

    snakes = malloc(init_data.players_count * sizeof(snake));
    snake_alive = malloc(init_data.players_count * sizeof(bool));
    for (size_t i = 0; i < init_data.players_count; i++){
        snake_alive[i] = true;

        readed = read(serverfd, &snakes[i].direction, 1);
        readed = read(serverfd, &snakes[i].size, 2);

        int body_size = sizeof(vector2) * snakes[i].size;
        snakes[i].body = malloc(body_size);
        readed = read(serverfd, snakes[i].body, body_size);

        if (readed != body_size)
            error("Recieved uncorrect imessage (body_size) from server");
    }

    printf("Init message recived successfully\n");
    sleep(1);
}

void* handle_input(void* args){
    uint8_t dir;
    while (true){
        dir = input_direction();
        if (dir == ' ') continue;

        if (write(serverfd, &dir, 1) != 1) error("Can't send direction to server...\n");

        snakes[init_data.user_id].direction = dir;
    }
    return NULL;
}

void connect_to_server(void){
    struct sockaddr_in servaddr;
 
    if ((serverfd = socket(AF_INET, SOCK_STREAM, 0)) <= 0) error("Can't create socket...\n");
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(server_ip);
    servaddr.sin_port = htons(server_port);

    printf("Connecting to server...\n");
    if (connect(serverfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) error("Can't connect to server...\n");
    printf("Connected successfully ...\n");
}

int main(int argc, char* argv[]) {
    int r = 0;

    while((r=getopt(argc, argv, "p:s:h")) != -1){
        switch(r){
            case 'p':
                server_port = atoi(optarg);
                break;
            case 's':
                server_ip = optarg;
                break;
            case 'h':
                printf("%s", "USAGE");
                exit(0);
        }
    }

    connect_to_server();
    handle_init_message();

    initscr();
    noecho();
    curs_set(0);
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);

    pthread_t thread_kb_input;
    pthread_create(&thread_kb_input, NULL, handle_input, NULL);

    while (true){
        bool move = handle_game_messages();
        clear();

        for (size_t i = 0; i < init_data.players_count; i++){
            if (!snake_alive[i]) continue;
            if (move) {move_snake(&snakes[i]);}

            render_snake(snakes[i], i == init_data.user_id);
        }

        render_fruit(fruit_pos);
        render_boundary(init_data.field);

        refresh();
    }

    pthread_exit(thread_kb_input);
    close(serverfd);
}
