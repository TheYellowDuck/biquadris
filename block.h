// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
// Required Notice: Copyright (c) 2026 George Zhang and the original Biquadris team (CS 246, University of Waterloo) — https://github.com/TheYellowDuck

// block.h
#ifndef BLOCK_H
#define BLOCK_H

#include <vector>

class Block {
    char type;
    int x, y;
    int level;
    std::vector<std::vector<bool>> shape;
    
    void initializeShape();

public:
    Block(char type, int level);
    
    char getType() const;
    int getX() const;
    int getY() const;
    int getLevel() const;
    void setX(int newX);
    void setY(int newY);
    
    const std::vector<std::vector<bool>>& getShape() const;
    void rotateCW();
    void rotateCCW();
};

#endif
