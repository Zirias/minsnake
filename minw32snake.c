#include <stdlib.h>
#include <windows.h>

#define ITEM_EMPTY " "
#define ITEM_HEAD "@"
#define ITEM_TAIL "#"

enum direction
{
    DIR_RIGHT = 0,
    DIR_DOWN,
    DIR_LEFT,
    DIR_UP,
};

static HANDLE consoleInput;
static DWORD consoleInputMode;
static HANDLE consoleOutput;
static CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
static INPUT_RECORD inputBuffer[32];
static DWORD number;

static int rows;
static int cols;
static enum direction dir;
static COORD *snake;
static COORD *first;
static COORD *last;
static int growcounter = 10;

#define putConsoleYX(y,x,c) \
    WriteConsoleOutputCharacterA(consoleOutput, (c), 1, \
    (COORD){(x)+consoleInfo.srWindow.Left, (y)+consoleInfo.srWindow.Top}, \
    &number)

static char getConsoleYX(SHORT y, SHORT x)
{
    char c;
    ReadConsoleOutputCharacterA(consoleOutput, &c, 1,
            (COORD){x+consoleInfo.srWindow.Left, y+consoleInfo.srWindow.Top},
            &number);
    return c;
}

int main(void)
{
    /* initialize console
     * ******************/

    consoleInput = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(consoleInput, &consoleInputMode);
    SetConsoleMode(consoleInput, 0);

restartgame:
    consoleOutput = CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE, 0, 0,
            CONSOLE_TEXTMODE_BUFFER, 0);
    GetConsoleScreenBufferInfo(consoleOutput, &consoleInfo);
    rows = consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top;
    cols = consoleInfo.srWindow.Right - consoleInfo.srWindow.Left;
    SetConsoleActiveScreenBuffer(consoleOutput);
    CONSOLE_CURSOR_INFO cursor = { 20, 0 };
    SetConsoleCursorInfo(consoleOutput, &cursor);

    /* create snake
     * ************/

    snake = calloc(rows * cols, sizeof(COORD));
    first = snake + rows * cols - 1;
    last = first;
    putConsoleYX(0, 0, ITEM_HEAD);

    /* main game loop
     * **************/

    for (;;)
    {
        if (WaitForSingleObject(consoleInput, 80) == WAIT_OBJECT_0)
        {
            ReadConsoleInput(consoleInput, inputBuffer, 32, &number);
            for (DWORD i = 0; i < number; ++i)
            {
                if (inputBuffer[i].EventType != KEY_EVENT) continue;
                if (!inputBuffer[i].Event.KeyEvent.bKeyDown) continue;
                switch (inputBuffer[i].Event.KeyEvent.wVirtualKeyCode)
                {
                    case 0x51: // Q
                        goto exitgame;
                    case VK_LEFT:
                        dir = DIR_LEFT;
                        break;
                    case VK_DOWN:
                        dir = DIR_DOWN;
                        break;
                    case VK_RIGHT:
                        dir = DIR_RIGHT;
                        break;
                    case VK_UP:
                        dir = DIR_UP;
                        break;
                }
            }
        }

        putConsoleYX(first->Y, first->X, ITEM_TAIL);

        COORD *next = (first == snake)
            ? snake + rows * cols - 1
            : first - 1;
        switch (dir)
        {
        case DIR_RIGHT:
            next->X = first->X + 1;
            next->Y = first->Y;
            break;
        case DIR_DOWN:
            next->X = first->X;
            next->Y = first->Y + 1;
            break;
        case DIR_LEFT:
            next->X = first->X - 1;
            next->Y = first->Y;
            break;
        case DIR_UP:
            next->X = first->X;
            next->Y = first->Y - 1;
            break;
        }

        if (next->X < 0 || next->X >= cols
                || next->Y < 0 || next->Y >= rows
                || getConsoleYX(next->Y, next->X) != ' ')
        {
            // hit something, restart:
            free(snake);
            dir = DIR_RIGHT;
            growcounter = 10;
            SetConsoleActiveScreenBuffer(GetStdHandle(STD_OUTPUT_HANDLE));
            CloseHandle(consoleOutput);
            goto restartgame;
        }

        first = next;
        putConsoleYX(first->Y, first->X, ITEM_HEAD);

        if (!--growcounter)
        {
            growcounter = 25;
        }
        else
        {
            putConsoleYX(last->Y, last->X, ITEM_EMPTY);
            last = (last == snake)
                ? snake + rows * cols - 1
                : last - 1;
        }
    }

exitgame:
    //endwin();
    //free(board);
    free(snake);
    SetConsoleActiveScreenBuffer(GetStdHandle(STD_OUTPUT_HANDLE));
    SetConsoleMode(consoleInput, consoleInputMode);
    CloseHandle(consoleOutput);
    return EXIT_SUCCESS;
}
