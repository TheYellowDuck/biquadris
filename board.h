// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
// Required Notice: Copyright (c) 2026 George Zhang and the original Biquadris team (CS 246, University of Waterloo) — https://github.com/TheYellowDuck

// board.h
#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <memory>
#include <string>
#include "block.h"
#include "level.h"
#include "effect.h"

class Board {
    static const int WIDTH = 11;
    static const int HEIGHT = 18;
    static const int VISIBLE_HEIGHT = 15;
    
    int playerNum;
    std::vector<std::vector<char>> grid;
    std::unique_ptr<Block> currentBlock;
    std::unique_ptr<Block> nextBlock;
    std::unique_ptr<Level> level;
    int score;
    int currentLevel;
    bool isBlind;
    int heavyCounter;
    bool lost;
    int blocksSinceLastClear;
    std::string scriptFile;
    EffectManager effectManager;

    bool isRowFull(int row);
    void removeRow(int row);
    bool canPlace(const Block& block, int x, int y) const;
    void placeBlock(const Block& block);
    void removeBlock(const Block& block);
    void generateNextBlock();

public:
    Board(int playerNum, const std::string& scriptFile, int startLevel, int seed);
    ~Board();
    
    bool moveLeft();   // Returns false if heavy drop was triggered
    bool moveRight();  // Returns false if heavy drop was triggered
    bool moveDown();   // Returns false if block couldn't move down
    bool rotateCW();   // Returns false if heavy drop was triggered
    bool rotateCCW();  // Returns false if heavy drop was triggered
    int drop();
    void checkNextBlock();
    
    void levelUp();
    void levelDown();
    void setNonRandom(const std::string& file);
    void setRandom();
    void replaceBlock(char type);
    void forceBlock(char type);
    
    void setBlind(bool blind);
    void addHeavy();
    void applyEffect(std::unique_ptr<Effect> effect);

    bool hasLost() const;
    int getScore() const;
    int getLevel() const;
    int getPlayerNum() const;
    char getCell(int x, int y) const;
    int  getGhostY() const;
    const Block* getCurrentBlock() const;
    const Block* getNextBlock() const;
    std::vector<std::string> getActiveEffects() const;

    void reset();
};

#endif
