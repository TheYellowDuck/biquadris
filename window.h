// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
// Required Notice: Copyright (c) 2026 George Zhang and the original Biquadris team (CS 246, University of Waterloo) — https://github.com/TheYellowDuck

// window.h  —  SDL2 graphical display
#ifndef WINDOW_H
#define WINDOW_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include "board.h"

class Xwindow {
    SDL_Window*   sdlWindow   = nullptr;
    SDL_Renderer* sdlRenderer = nullptr;
    TTF_Font*     font        = nullptr;
    TTF_Font*     fontLarge   = nullptr;
    const Board*  lastP1      = nullptr;  // kept for overlay re-rendering
    const Board*  lastP2      = nullptr;
    int width, height;
    std::string inputBuffer;

    void setColor(int color);
    void renderText(int x, int y, const std::string& text, SDL_Color color, TTF_Font* f);
    int  blockColor(char type) const;
    void drawBoard(const Board& board, int offsetX, int offsetY, SDL_Color accent);
    void drawBaseFrame();

public:
    Xwindow(int width = 700, int height = 640);
    ~Xwindow();

    enum { White=0, Black, Red, Green, Blue, Cyan, Yellow, Magenta, Orange, Grey };

    void fillRectangle(int x, int y, int w, int h, int color = Black);
    void drawString(int x, int y, const std::string& msg, int color = Black);
    void render(const Board& player1, const Board& player2);
    void renderGameOver(int losingPlayer, int winningPlayer,
                        int score1, int score2, int hiScore);
    void renderSpecialActionPrompt(int playerNum);
    void renderBlockSelector();
    void clear();

    bool        checkForInput();
    std::string getInput();
    std::string getSpecialActionInput();
    char        getBlockTypeInput();
};

#endif
