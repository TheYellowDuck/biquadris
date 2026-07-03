// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
// Required Notice: Copyright (c) 2026 George Zhang and the original Biquadris team (CS 246, University of Waterloo) — https://github.com/TheYellowDuck

// board.cc
#include "board.h"
#include "level.h"
#include <iostream>
#include <cstdlib>

using namespace std;

Board::Board(int playerNum, const string& scriptFile, int startLevel, int seed)
    : playerNum(playerNum), score(0), currentLevel(startLevel),
      isBlind(false), heavyCounter(0), lost(false), blocksSinceLastClear(0),
      scriptFile(scriptFile) {
    
    srand(seed);
    
    // Initialize grid
    grid.resize(HEIGHT, vector<char>(WIDTH, ' '));
    
    // Create level
    level = make_unique<Level>(currentLevel, scriptFile);
    
    // Generate first blocks
    currentBlock = make_unique<Block>(level->getNextBlock(), currentLevel);
    generateNextBlock();
}

Board::~Board() {}

void Board::generateNextBlock() {
    char type = level->getNextBlock();
    nextBlock = make_unique<Block>(type, currentLevel);
}

bool Board::canPlace(const Block& block, int x, int y) const {
    auto shape = block.getShape();
    for (size_t i = 0; i < shape.size(); ++i) {
        for (size_t j = 0; j < shape[i].size(); ++j) {
            if (shape[i][j]) {
                int newX = x + j;
                int newY = y + i;
                
                if (newX < 0 || newX >= WIDTH || newY < 0 || newY >= HEIGHT) {
                    return false;
                }
                
                if (grid[newY][newX] != ' ') {
                    return false;
                }
            }
        }
    }
    return true;
}

void Board::placeBlock(const Block& block) {
    auto shape = block.getShape();
    int x = block.getX();
    int y = block.getY();
    char type = block.getType();
    
    for (size_t i = 0; i < shape.size(); ++i) {
        for (size_t j = 0; j < shape[i].size(); ++j) {
            if (shape[i][j]) {
                grid[y + i][x + j] = type;
            }
        }
    }
}

void Board::removeBlock(const Block& block) {
    auto shape = block.getShape();
    int x = block.getX();
    int y = block.getY();
    
    for (size_t i = 0; i < shape.size(); ++i) {
        for (size_t j = 0; j < shape[i].size(); ++j) {
            if (shape[i][j]) {
                grid[y + i][x + j] = ' ';
            }
        }
    }
}

bool Board::moveLeft() {
    int newX = currentBlock->getX() - 1;
    if (canPlace(*currentBlock, newX, currentBlock->getY())) {
        currentBlock->setX(newX);

        // Check if block is resting on something (can't move down)
        if (!canPlace(*currentBlock, currentBlock->getX(), currentBlock->getY() + 1)) {
            drop();
            return false;  // Indicate that drop was triggered
        }

        // Apply heavy effects (cumulative)
        int dropRows = 0;
        if (currentLevel >= 3) dropRows += 1;  // Level 3+ heavy
        if (heavyCounter > 0) dropRows += 2;   // Special action heavy

        // Drop the required number of rows
        for (int i = 0; i < dropRows; ++i) {
            if (!moveDown()) {
                // Couldn't move down, drop the block
                drop();
                return false;  // Indicate that drop was triggered
            }
        }
    }
    return true;  // Normal move, no drop triggered
}

bool Board::moveRight() {
    int newX = currentBlock->getX() + 1;
    if (canPlace(*currentBlock, newX, currentBlock->getY())) {
        currentBlock->setX(newX);

        // Check if block is resting on something (can't move down)
        if (!canPlace(*currentBlock, currentBlock->getX(), currentBlock->getY() + 1)) {
            drop();
            return false;  // Indicate that drop was triggered
        }

        // Apply heavy effects (cumulative)
        int dropRows = 0;
        if (currentLevel >= 3) dropRows += 1;  // Level 3+ heavy
        if (heavyCounter > 0) dropRows += 2;   // Special action heavy

        // Drop the required number of rows
        for (int i = 0; i < dropRows; ++i) {
            if (!moveDown()) {
                // Couldn't move down, drop the block
                drop();
                return false;  // Indicate that drop was triggered
            }
        }
    }
    return true;  // Normal move, no drop triggered
}

bool Board::moveDown() {
    int newY = currentBlock->getY() + 1;
    if (canPlace(*currentBlock, currentBlock->getX(), newY)) {
        currentBlock->setY(newY);
        return true;
    }
    return false;  // Block couldn't move down
}

bool Board::rotateCW() {
    currentBlock->rotateCW();
    if (!canPlace(*currentBlock, currentBlock->getX(), currentBlock->getY())) {
        currentBlock->rotateCCW();
    } else {
        // Check if block is resting on something (can't move down)
        if (!canPlace(*currentBlock, currentBlock->getX(), currentBlock->getY() + 1)) {
            drop();
            return false;  // Indicate that drop was triggered
        }

        // Apply heavy effects (cumulative)
        int dropRows = 0;
        if (currentLevel >= 3) dropRows += 1;  // Level 3+ heavy
        if (heavyCounter > 0) dropRows += 2;   // Special action heavy

        // Drop the required number of rows
        for (int i = 0; i < dropRows; ++i) {
            if (!moveDown()) {
                // Couldn't move down, drop the block
                drop();
                return false;  // Indicate that drop was triggered
            }
        }
    }
    return true;  // Normal rotation, no drop triggered
}

bool Board::rotateCCW() {
    currentBlock->rotateCCW();
    if (!canPlace(*currentBlock, currentBlock->getX(), currentBlock->getY())) {
        currentBlock->rotateCW();
    } else {
        // Check if block is resting on something (can't move down)
        if (!canPlace(*currentBlock, currentBlock->getX(), currentBlock->getY() + 1)) {
            drop();
            return false;  // Indicate that drop was triggered
        }

        // Apply heavy effects (cumulative)
        int dropRows = 0;
        if (currentLevel >= 3) dropRows += 1;  // Level 3+ heavy
        if (heavyCounter > 0) dropRows += 2;   // Special action heavy

        // Drop the required number of rows
        for (int i = 0; i < dropRows; ++i) {
            if (!moveDown()) {
                // Couldn't move down, drop the block
                drop();
                return false;  // Indicate that drop was triggered
            }
        }
    }
    return true;  // Normal rotation, no drop triggered
}

int Board::drop() {
    while (canPlace(*currentBlock, currentBlock->getX(), currentBlock->getY() + 1)) {
        currentBlock->setY(currentBlock->getY() + 1);
    }
    
    placeBlock(*currentBlock);
    
    int linesCleared = 0;
    for (int i = HEIGHT - 1; i >= 0; --i) {
        if (isRowFull(i)) {
            removeRow(i);
            ++linesCleared;
            ++i;
        }
    }
    
    if (linesCleared > 0) {
        score += (currentLevel + linesCleared) * (currentLevel + linesCleared);
        blocksSinceLastClear = 0;
    } else {
        ++blocksSinceLastClear;

        if (currentLevel >= 4 && blocksSinceLastClear > 0 && blocksSinceLastClear % 5 == 0) {
            // Find the lowest empty row in the center column
            int col = WIDTH / 2;
            for (int row = HEIGHT - 1; row >= 0; --row) {
                if (grid[row][col] == ' ') {
                    grid[row][col] = '*';
                    break;
                }
            }
        }
    }
    
    if (heavyCounter > 0) --heavyCounter;

    // Notify effect manager about drop (handles blind expiry, heavy expiry, etc.)
    effectManager.onDrop(*this);

    checkNextBlock();

    return linesCleared;
}

void Board::checkNextBlock() {
    // Check if next block can be placed at its spawn position before making it current
    if (!canPlace(*nextBlock, nextBlock->getX(), nextBlock->getY())) {
        lost = true;
        return; // Don't make it current if it doesn't fit
    }

    currentBlock = std::move(nextBlock);
    generateNextBlock();
}

bool Board::isRowFull(int row) {
    for (int x = 0; x < WIDTH; ++x) {
        if (grid[row][x] == ' ') return false;
    }
    return true;
}

void Board::removeRow(int row) {
    for (int y = row; y > 0; --y) {
        grid[y] = grid[y - 1];
    }
    grid[0] = vector<char>(WIDTH, ' ');
}

void Board::levelUp() {
    if (currentLevel < 4) {
        ++currentLevel;
        level = make_unique<Level>(currentLevel, scriptFile);
    }
}

void Board::levelDown() {
    if (currentLevel > 0) {
        --currentLevel;
        level = make_unique<Level>(currentLevel, scriptFile);
    }
}

void Board::setNonRandom(const string& file) {
    level->setNonRandom(file);
}

void Board::setRandom() {
    level->setRandom();
}

void Board::replaceBlock(char type) {
    currentBlock = make_unique<Block>(type, currentLevel);
}

void Board::forceBlock(char type) {
    currentBlock = make_unique<Block>(type, currentLevel);
}

int Board::getGhostY() const {
    if (!currentBlock) return 0;
    int y = currentBlock->getY();
    while (canPlace(*currentBlock, currentBlock->getX(), y + 1)) ++y;
    return y;
}

void Board::setBlind(bool blind) {
    isBlind = blind;
}

void Board::addHeavy() {
    ++heavyCounter;
}

bool Board::hasLost() const {
    return lost;
}

int Board::getScore() const {
    return score;
}

int Board::getLevel() const {
    return currentLevel;
}

int Board::getPlayerNum() const {
    return playerNum;
}

char Board::getCell(int x, int y) const {
    if (isBlind && x >= 2 && x <= 8 && y >= 5 && y <= 14) {
        return '?';
    }
    return grid[y][x];
}

const Block* Board::getCurrentBlock() const {
    return currentBlock.get();
}

const Block* Board::getNextBlock() const {
    return nextBlock.get();
}

void Board::applyEffect(unique_ptr<Effect> effect) {
    effectManager.addEffect(std::move(effect), *this);
}

vector<string> Board::getActiveEffects() const {
    return effectManager.getActiveEffectNames();
}

void Board::reset() {
    grid.assign(HEIGHT, vector<char>(WIDTH, ' '));
    score = 0;
    isBlind = false;
    heavyCounter = 0;
    lost = false;
    blocksSinceLastClear = 0;
    effectManager.clear();

    level = make_unique<Level>(currentLevel, scriptFile);
    currentBlock = make_unique<Block>(level->getNextBlock(), currentLevel);
    generateNextBlock();
}
