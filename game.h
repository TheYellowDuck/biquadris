// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
// Required Notice: Copyright (c) 2026 George Zhang and the original Biquadris team (CS 246, University of Waterloo) — https://github.com/TheYellowDuck

// game.h
#ifndef GAME_H
#define GAME_H

#include <string>
#include <memory>
#include "board.h"
#include "textdisplay.h"
#ifdef BIQUADRIS_GRAPHICS
#include "window.h"
#endif

class Game {
    std::unique_ptr<Board> player1;
    std::unique_ptr<Board> player2;
    std::unique_ptr<TextDisplay> textDisplay;
#ifdef BIQUADRIS_GRAPHICS
    std::unique_ptr<Xwindow> graphicsDisplay;
#endif
    Board* currentPlayer;
    Board* otherPlayer;
    int hiScore;
#ifdef BIQUADRIS_GRAPHICS
    bool textOnly;
#endif
    bool shouldQuit;  // Flag to indicate when to quit the game

    void switchPlayer();
    void processCommand(const std::string& cmd, int multiplier);
    std::string getCommand();
    bool isPrefix(const std::string& prefix, const std::string& full);
    void loadHighScore();
    void saveHighScore();

public:
    Game(bool textOnly, int seed, const std::string& scriptFile1, 
         const std::string& scriptFile2, int startLevel);
    ~Game();
    void run();
    void restart();
};

#endif
