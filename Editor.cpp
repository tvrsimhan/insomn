#include "Editor.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
//#define lno log10(LINES - 1)

//Default Constructor - no file name mentioned
Editor::Editor()
{
    //Set cursor at the beginning
    x = 0;
    y = 0;
    mode = 'n';
    cmd = "";
    upstatus = true;
    raiseflag = false;
    status = "Normal";
    lowerbound = 0;
    filename = "";
    buff = new Buffer();
    buff->appendLine("");
}

//Parameterised constructor - File name is passed as parameter
Editor::Editor(string fname)
{
    x = 0;
    y = 0;
    mode = 'n';
    cmd = "";
    lowerbound = 0;
    raiseflag = false;
    upstatus = true;
    status = "Normal";
    buff = new Buffer();
    filename = fname;

    //Read from file if exists
    ifstream infile(fname.c_str());
    if (infile.is_open())
    {
        while (!infile.eof())
        {
            string temp;
            getline(infile, temp);
            buff->appendLine(temp);
        }
    }
    else
    {
        cerr << "Cannot open file: '" << fname << endl;
        buff->appendLine("");
    }
    infile.close();
}

void Editor::updateStatus()
{
    switch (mode)
    {
    case 'n':
        if (cmd.empty())
            status = "Insomn v0";
        else
            status = cmd;
        break;
    case 'i':
        status = "Insert";
        break;
    case 'q':
        status = "Quit";
        break;
    }
    status += "\tCOL: " + toStr(x) + "\tROW: " + toStr(y + 1);
}

void Editor::handleInput(int c)
{
    upstatus = true;
    switch (mode)
    {
    case 'n':
        switch (c)
        {
        case KEY_LEFT:
            moveLeft();
            break;
        case KEY_RIGHT:
            moveRight();
            break;
        case KEY_UP:
            moveUp();
            break;
        case KEY_DOWN:
            moveDown();
            break;
        case KEY_ENTER:
        case 10:
            // Execute the command
            exeCmd();
            break;
        case 27:
            // Clear the command
            cmd.clear();
            break;
        case 127:
        case KEY_BACKSPACE:
        case KEY_DC:
            // Removes last character
            if (!cmd.empty())
                cmd.erase(cmd.length() - 1, 1);
            break;
        default:
            // Add character to command
            cmd += string(1, char(c));
            break;
        }
        break;
    case 'i':
        switch (c)
        {
        case 27:
            // Escape/Alt key
            mode = 'n';
            break;
        case 127:
        case KEY_BACKSPACE:
            // Backspace
            if (x == 0 && y > 0)
            {
                x = buff->lines[y - 1].length();
                // Bring line down
                buff->lines[y - 1] += buff->lines[y];
                // Delete the line
                deleteLine();
                mvdelch(y, 0);
                mvdelch(y, 0);
                mvdelch(y, 0);
                clrtobot();
                moveUp();
            }
            else if (x > 0)
            {
                buff->lines[y].erase(--x, 1);
            }
            break;
        case KEY_DC:
            // Delete key
            if (x == buff->lines[y].length() && y != buff->lines.size() - 1)
            {
                // Bring line down
                buff->lines[y] += buff->lines[y + 1];
                // Delete the line
                deleteLine(y + 1);
                clrtobot();
            }
            else
            {
                buff->lines[y].erase(x, 1);
            }
            break;
        case KEY_LEFT:
            moveLeft();
            break;
        case KEY_RIGHT:
            moveRight();
            break;
        case KEY_UP:
            moveUp();
            break;
        case KEY_DOWN:
            moveDown();
            break;
        case KEY_ENTER:
        case 10:
            // Bring rest of line down
            if (x < buff->lines[y + lowerbound].length() - 1)
            {
                // Put rest of line on new line
                buff->insertLine(buff->lines[y + lowerbound].substr(x, buff->lines[y + lowerbound].length() - x), y + 1);
                // Remove that part of the line
                buff->lines[y + lowerbound].erase(x, buff->lines[y + lowerbound].length() - x);
            }
            else
                buff->insertLine("", y + lowerbound + 1);
            moveDown();
            break;
        case KEY_BTAB:
        case KEY_CTAB:
        case KEY_STAB:
        case KEY_CATAB:
        case 9:
            // Tab
            buff->lines[y + lowerbound].insert(x, 4, ' ');
            x += 4;
            break;
        default:
            buff->lines[y + lowerbound].insert(x, 1, char(c));
            x++;
            break;
        }
        break;
    default:;
    }
}

void Editor::deleteLine()
{
    buff->removeLine(y);
}

void Editor::deleteLine(int i)
{
    buff->removeLine(i);
}

void Editor::moveLeft()
{
    if (x - 1 >= 0)
    {
        x--;
        move(y, x);
    }
    if (x - 1 < 0)
    {
        if (y - 1 < 0)
        {
            beep();
        }
        else
        {
            y--;
            x = buff->lines[y].length();
            move(y, x);
        }
    }
}

void Editor::moveRight()
{
    if (x + 1 < COLS && x + 1 <= buff->lines[y].length())
    {
        x++;
        move(y, x);
    }
    if (x + 1 < COLS && x + 1 > buff->lines[y].length() && y + 1 < buff->lines.size())
    {
        x = 0;
        y++;
        move(y, x);
    }
}

void Editor::moveUp()
{
    if (y - 1 < 0)
        beep();
    if (y - 1 >= 0)
    {
        y--;
    }
    else if (y - 1 < 0 && lowerbound > 0)
    {
        lowerbound--;
    }
    if (x >= buff->lines[y].length())
        x = buff->lines[y].length();
    move(y, x);
}

void Editor::moveDown()
{
    if (y + 1 < LINES - 1 && y + 1 < buff->lines.size())
    {
        y++;
    }
    else if (lowerbound + y < buff->lines.size())
    {
        raiseflag = true;
        lowerbound++;
    }
    if (x >= buff->lines[y].length())
        x = buff->lines[y].length();
    move(y, x);
}

void Editor::printBuff()
{
    int lc = 0; // Line count
    for (int i = lowerbound; lc < LINES - 1; i++)
    {
        if (i >= buff->lines.size())
        {
        }
        else
        {
            /*int padding = lno;
            if (lc < 100)
                padding -= 1;
            else if (lc < 1000)
                padding -= 2;
            else if (lc < 10000)
                padding -= 3;
            else if (lc < 100000)
                padding -= 4;*/
            attron(A_STANDOUT);
            mvprintw(lc, 0, "%*d", 3, lc + 1);
            attroff(A_STANDOUT);
            mvprintw(lc, 4, buff->lines[i].c_str());
        }
        clrtoeol();
        lc++;
    }
    move(y, x + 4);
}

void Editor::printStatusLine()
{
    if (raiseflag)
        attron(A_BOLD);
    attron(A_REVERSE);
    mvprintw(LINES - 1, 0, status.c_str());
    clrtoeol();
    if (raiseflag)
        attroff(A_BOLD);
    attroff(A_REVERSE);
}

void Editor::saveFile()
{
    if (filename == "")
    {
        // Set filename to untitled if not mentioned
        filename = "untitled";
    }

    ofstream f(filename.c_str());
    if (f.is_open())
    {
        for (int i = 0; i < buff->lines.size(); i++)
        {
            f << buff->lines[i] << endl;
        }
        status = "Saved to file";
    }
    else //Not enough permissions
    {
        status = "Error: Cannot open file for writing!";
    }
    f.close();
}

string Editor::toStr(int n)
{
    stringstream ss;
    ss << n;
    return ss.str();
}

bool Editor::exeCmd()
{
    if (cmd == "i")
        mode = 'i';
    else if (cmd == "q")
        mode = 'q';
    else if (cmd == "s")
    {
        upstatus = false;
        saveFile();
    }

    cmd = "";    // Reset command buffer
    return true; // Returns if command has executed successfully
}