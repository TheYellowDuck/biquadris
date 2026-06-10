// block.cc
#include "block.h"

using namespace std;

Block::Block(char type, int level) : type(type), x(0), y(3), level(level) {
    initializeShape();
}

void Block::initializeShape() {
    shape.clear();
    
    switch(type) {
        case 'I':
            shape = {{true, true, true, true}};
            break;
        case 'J':
            shape = {{true, false, false},
                     {true, true, true}};
            break;
        case 'L':
            shape = {{false, false, true},
                     {true, true, true}};
            break;
        case 'O':
            shape = {{true, true},
                     {true, true}};
            break;
        case 'S':
            shape = {{false, true, true},
                     {true, true, false}};
            break;
        case 'Z':
            shape = {{true, true, false},
                     {false, true, true}};
            break;
        case 'T':
            shape = {{true, true, true},
                     {false, true, false}};
            break;
        default:
            shape = {{true}};
            break;
    }
}

char Block::getType() const { return type; }
int Block::getX() const { return x; }
int Block::getY() const { return y; }
int Block::getLevel() const { return level; }
void Block::setX(int newX) { x = newX; }
void Block::setY(int newY) { y = newY; }

const vector<vector<bool>>& Block::getShape() const {
    return shape;
}

void Block::rotateCW() {
    int oldRows = shape.size();
    int oldCols = shape[0].size();

    // Save the lower-left corner position (in grid coordinates)
    int lowerLeftX = x;
    int lowerLeftY = y + oldRows - 1;

    // Perform rotation: new[j][rows-1-i] = old[i][j]
    vector<vector<bool>> rotated(oldCols, vector<bool>(oldRows));

    for (int i = 0; i < oldRows; ++i) {
        for (int j = 0; j < oldCols; ++j) {
            rotated[j][oldRows - 1 - i] = shape[i][j];
        }
    }

    shape = rotated;

    // New dimensions after rotation
    int newRows = shape.size();

    // Keep lower-left corner position fixed
    // Lower-left corner is at (x, y + oldRows - 1) before rotation
    // After rotation, we want it to still be at (lowerLeftX, lowerLeftY)
    // So: x = lowerLeftX
    //     y = lowerLeftY - (newRows - 1)
    x = lowerLeftX;
    y = lowerLeftY - (newRows - 1);
}

void Block::rotateCCW() {
    int oldRows = shape.size();
    int oldCols = shape[0].size();

    // Save the lower-left corner position (in grid coordinates)
    int lowerLeftX = x;
    int lowerLeftY = y + oldRows - 1;

    // Perform rotation: new[cols-1-j][i] = old[i][j]
    vector<vector<bool>> rotated(oldCols, vector<bool>(oldRows));

    for (int i = 0; i < oldRows; ++i) {
        for (int j = 0; j < oldCols; ++j) {
            rotated[oldCols - 1 - j][i] = shape[i][j];
        }
    }

    shape = rotated;

    // New dimensions after rotation
    int newRows = shape.size();

    // Keep lower-left corner position fixed
    // Lower-left corner is at (x, y + oldRows - 1) before rotation
    // After rotation, we want it to still be at (lowerLeftX, lowerLeftY)
    // So: x = lowerLeftX
    //     y = lowerLeftY - (newRows - 1)
    x = lowerLeftX;
    y = lowerLeftY - (newRows - 1);
}