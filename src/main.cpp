#include <ncurses.h>
#include "Editor.h"

// Initializes the screen
void init()
{
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, true);
}

string fname = "";

int main(int argc, char *argv[])
{
    Editor ed;
    // Initialize filename
    if (argc > 1)
    {
        fname = string(argv[1]);
        ed = Editor(fname);
    }
    else
    {
        ed = Editor();
    }

    init();
    while (ed.getMode() != 'q')
    {
        if (ed.upstatus)
            ed.updateStatus();
        ed.printStatusLine();
        ed.printBuff();
        int input = getch();
        ed.handleInput(input);
    }

    refresh();
    endwin();
    return 0;
}