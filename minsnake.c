#include <curses.h>
#include <stdlib.h>

enum item
{
    ITEM_EMPTY = 0,
    ITEM_HEAD,
    ITEM_TAIL,
};

enum direction
{
    DIR_LEFT = 0,
    DIR_DOWN,
    DIR_RIGHT,
    DIR_UP
};

struct pos
{
    int row;
    int col;
};

static int rows;
static int cols;
static enum item *board;
static enum direction dir;
static struct pos *snake;
static struct pos *first;
static struct pos *last;
static int growcounter = 10;

int main(void)
{
    /* initialize curses
     * *****************/
    
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    timeout(80);
    getmaxyx(stdscr, rows, cols);

    /* create snake
     * ************/

restartgame:
    snake = calloc(rows * cols, sizeof(struct pos));
    first = snake + rows * cols - 1;
    last = first;

    /* initialize board
     * ****************/

    board = calloc(rows * cols, sizeof(enum item));
    board[0] = ITEM_HEAD;

    /* main game loop
     * **************/

    for (;;)
    {
        /* draw the current board
         * **********************/

        for (int i = 0; i < rows; ++i)
        {
            for (int j = 0; j < cols; ++j)
            {
                switch (board[i * cols + j])
                {
                case ITEM_EMPTY:
                    mvaddch(i, j, ' ');
                    break;
                case ITEM_HEAD:
                    mvaddch(i, j, '@');
                    break;
                case ITEM_TAIL:
                    mvaddch(i, j, '#');
                    break;
                }
            }
        }
        refresh();

        /* react on user input
         * *******************/
        int c = getch();
        switch (c)
        {
        case 'q':
        case 'Q':
            goto exitgame;
        }

        /* move snake
         * **********/

        board[first->row * cols + first->col] = ITEM_TAIL;

        struct pos *next = (first == snake)
            ? snake + rows * cols - 1
            : first - 1;
        switch (dir)
        {
        case DIR_LEFT:
            next->col = first->col + 1;
            next->row = first->row;
            break;
        case DIR_DOWN:
            next->col = first->col;
            next->row = first->row + 1;
            break;
        case DIR_RIGHT:
            next->col = first->col - 1;
            next->row = first->row;
            break;
        case DIR_UP:
            next->col = first->col;
            next->row = first->row - 1;
            break;
        }

        if (next->col < 0 || next->col >= cols
                || next->row < 0 || next->row >= rows
                || board[next->row * cols + next->col] != ITEM_EMPTY)
        {
            // hit something, restart:
            free(board);
            free(snake);
            dir = DIR_LEFT;
            growcounter = 10;
            goto restartgame;
        }

        first = next;
        board[first->row * cols + first->col] = ITEM_HEAD;

        if (!--growcounter)
        {
            growcounter = 80;
        }
        else
        {
            board[last->row * cols + last->col] = ITEM_EMPTY;
            last = (last == snake)
                ? snake + rows * cols - 1
                : last - 1;
        }
    }

exitgame:
    endwin();
    free(board);
    return EXIT_SUCCESS;
}
