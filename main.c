#include <ncurses.h>

int main() {
    initscr();

    int x = 0, y = 0;
    getmaxyx(stdscr, y ,x);

    int sx = 0, sy = 0;
    int k = 0;
    bool done = FALSE;
    while (!done){
        mvaddch(sy, sx, 'A');
        refresh();
        k = getch();
        mvaddch(sy, sx, ' ');
        switch (k){
            case 'w':
                sy -= 1;
                sy = sy % y;
                break;
            case 's':
                sy += 1;
                sy = sy % y;
                break;
            case 'q':
                done = TRUE;
                break;
        }
    }
    puts("");

    endwin();
    return 0;
}