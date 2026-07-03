// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
// Required Notice: Copyright (c) 2026 George Zhang and the original Biquadris team (CS 246, University of Waterloo) — https://github.com/TheYellowDuck

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
