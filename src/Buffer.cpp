#include "Buffer.h"

Buffer::Buffer() {} //default constructor

/*  Find and replace tabs with spaces
    to ensure consistency across editors    */
string Buffer::adjTabs(string line) 
{
    //Might be redundant; still worth a check
    int tab = line.find("\t");
    if (tab == line.npos)
        return line;
    else
        return adjTabs(line.replace(tab, 1, "    "));
}

void Buffer::insertLine(string line, int x)
{
    line = adjTabs(line);
    lines.insert(lines.begin() + x, line);
}

void Buffer::appendLine(string line)
{
    line = adjTabs(line);
    lines.push_back(line);
}

void Buffer::removeLine(int i)
{
    lines.erase(lines.begin() + i);
}
