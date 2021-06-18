#ifndef BUFFER_H
#define BUFFER_H

#include <ncurses.h>
#include <vector>
#include <string>

using namespace std;

//The buffer holds the contents of the file to be written
class Buffer
{
public:
    Buffer();
    vector<string> lines;
    //Functions to manipulate the contents of buffer
    void insertLine(string, int);
    void appendLine(string);
    void removeLine(int);
    string adjTabs(string);
};

#endif