// textdisplay.h
#ifndef TEXTDISPLAY_H
#define TEXTDISPLAY_H

#include "board.h"

class TextDisplay {
public:
    TextDisplay();
    ~TextDisplay();
    
    void display(const Board& player1, const Board& player2);
private:
    void displayBlock(const Block* block);
};

#endif