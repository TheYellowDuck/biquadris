// textdisplay.cc
#include "textdisplay.h"
#include <iostream>
#include <iomanip>

using namespace std;

TextDisplay::TextDisplay() {}
TextDisplay::~TextDisplay() {}

void TextDisplay::displayBlock(const Block* block) {
    if (!block) return;
    
    auto shape = block->getShape();
    for (size_t i = 0; i < shape.size(); ++i) {
        for (size_t j = 0; j < shape[i].size(); ++j) {
            if (shape[i][j]) {
                cout << block->getType();
            } else {
                cout << ' ';
            }
        }
        cout << endl;
    }
}

void TextDisplay::display(const Board& player1, const Board& player2) {
    cout << "\n====================================" << endl;
    cout << "  PLAYER  1              PLAYER  2  " << endl;
    cout << "====================================" << endl;
    cout << "Level: " << setw(6) << right << player1.getLevel()
         << "          Level: " << setw(6) << player2.getLevel() << endl;
    cout << "Score: " << setw(6) << right << player1.getScore()
         << "          Score: " << setw(6) << right << player2.getScore() << endl;
    cout << "+-----------+          +-----------+" << endl;

    // Display boards side by side (including 3 top rows for rotation)
    for (int y = 0; y < 18; ++y) {
        cout << "|";
        for (int x = 0; x < 11; ++x) {
            char cell = player1.getCell(x, y);
            
            // Check if current block occupies this position
            const Block* curBlock = player1.getCurrentBlock();
            if (curBlock) {
                auto shape = curBlock->getShape();
                int bx = curBlock->getX();
                int by = curBlock->getY();
                
                bool inBlock = false;
                for (size_t i = 0; i < shape.size() && !inBlock; ++i) {
                    for (size_t j = 0; j < shape[i].size() && !inBlock; ++j) {
                        if (shape[i][j] && by + (int)i == y && bx + (int)j == x) {
                            cell = curBlock->getType();
                            inBlock = true;
                        }
                    }
                }
            }
            
            cout << cell;
        }

        cout << "|          |";

        for (int x = 0; x < 11; ++x) {
            char cell = player2.getCell(x, y);
            
            const Block* curBlock = player2.getCurrentBlock();
            if (curBlock) {
                auto shape = curBlock->getShape();
                int bx = curBlock->getX();
                int by = curBlock->getY();
                
                bool inBlock = false;
                for (size_t i = 0; i < shape.size() && !inBlock; ++i) {
                    for (size_t j = 0; j < shape[i].size() && !inBlock; ++j) {
                        if (shape[i][j] && by + (int)i == y && bx + (int)j == x) {
                            cell = curBlock->getType();
                            inBlock = true;
                        }
                    }
                }
            }
            
            cout << cell;
        }

        cout << "|" << endl;
    }

    cout << "+-----------+          +-----------+" << endl;
    cout << "Next:                  Next:" << endl;
    
    // Display next blocks
    const Block* next1 = player1.getNextBlock();
    const Block* next2 = player2.getNextBlock();
    
    if (next1 && next2) {
        auto shape1 = next1->getShape();
        auto shape2 = next2->getShape();
        
        size_t maxRows = max(shape1.size(), shape2.size());
        
        for (size_t i = 0; i < maxRows; ++i) {
            // Track how many characters we printed for player 1
            int charsPrinted = 0;

            if (i < shape1.size()) {
                for (size_t j = 0; j < shape1[i].size(); ++j) {
                    cout << (shape1[i][j] ? next1->getType() : ' ');
                    charsPrinted++;
                }
            }

            // Pad to ensure player 2's block always starts at column 23
            const int targetColumn = 23;
            for (int p = charsPrinted; p < targetColumn; ++p) {
                cout << ' ';
            }

            if (i < shape2.size()) {
                for (size_t j = 0; j < shape2[i].size(); ++j) {
                    cout << (shape2[i][j] ? next2->getType() : ' ');
                }
            }

            cout << endl;
        }
    }
    
    cout << endl;
}