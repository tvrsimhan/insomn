#ifndef EDITOR_H
#define EDITOR_H

#include "Buffer.h"
#include <ncurses.h>

class Editor
{
private:
    int x, y;       //Position of cursor
    int lowerbound; //Number of rows filled till cursor position
    char mode;      //Normal, Insert or Quit
    bool raiseflag;
    Buffer *buff;
    string status; //Message to display
    string cmd;
    string filename;

    string toStr(int);

    bool exeCmd();

    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
    void deleteLine();
    void deleteLine(int i);

    void saveFile();

public:
    bool upstatus;

    Editor();
    Editor(string);

    char getMode() { return mode; }

    void handleInput(int);
    void printBuff();
    void printStatusLine();
    void updateStatus();
};

#endif
