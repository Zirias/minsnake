#include <curses.h>
#include <stdlib.h>

int main(void)
{
    initscr();
    getch();
    endwin();
    return EXIT_SUCCESS;
}
