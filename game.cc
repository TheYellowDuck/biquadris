// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
// Required Notice: Copyright (c) 2026 George Zhang and the original Biquadris team (CS 246, University of Waterloo) — https://github.com/TheYellowDuck

// game.cc
#include "game.h"
#include "effect.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <fstream>

using namespace std;

Game::Game(bool textOnly, int seed, const string& scriptFile1,
           const string& scriptFile2, int startLevel)
    : hiScore(0),
#ifdef BIQUADRIS_GRAPHICS
      textOnly(textOnly),
#endif
      shouldQuit(false) {

    loadHighScore();

    player1 = make_unique<Board>(1, scriptFile1, startLevel, seed);
    player2 = make_unique<Board>(2, scriptFile2, startLevel, seed + 1);
    textDisplay = make_unique<TextDisplay>();

#ifdef BIQUADRIS_GRAPHICS
    if (!textOnly) {
        graphicsDisplay = make_unique<Xwindow>(700, 640);
    }
#endif

    currentPlayer = player1.get();
    otherPlayer = player2.get();
}

Game::~Game() {}

void Game::switchPlayer() {
    swap(currentPlayer, otherPlayer);
}

bool Game::isPrefix(const string& prefix, const string& full) {
    if (prefix.length() > full.length()) return false;
    return equal(prefix.begin(), prefix.end(), full.begin());
}

string Game::getCommand() {
#ifdef BIQUADRIS_GRAPHICS
    if (!textOnly && graphicsDisplay) {
        return graphicsDisplay->getInput();
    }
#endif
    string line;
    if (!getline(cin, line)) return "eof";
    return line;
}

void Game::processCommand(const string& cmd, int multiplier) {
    if (isPrefix(cmd, "left")) {
        for (int i = 0; i < multiplier; ++i) {
            if (!currentPlayer->moveLeft()) {
                switchPlayer();
                return;
            }
        }
    } else if (isPrefix(cmd, "right")) {
        for (int i = 0; i < multiplier; ++i) {
            if (!currentPlayer->moveRight()) {
                switchPlayer();
                return;
            }
        }
    } else if (isPrefix(cmd, "down")) {
        for (int i = 0; i < multiplier; ++i) {
            if (!currentPlayer->moveDown()) {
                processCommand("drop", 1);
                return;
            }
        }
    } else if (isPrefix(cmd, "clockwise")) {
        for (int i = 0; i < multiplier; ++i) {
            if (!currentPlayer->rotateCW()) {
                switchPlayer();
                return;
            }
        }
    } else if (isPrefix(cmd, "counterclockwise")) {
        for (int i = 0; i < multiplier; ++i) {
            if (!currentPlayer->rotateCCW()) {
                switchPlayer();
                return;
            }
        }
    } else if (isPrefix(cmd, "drop")) {
        for (int i = 0; i < multiplier; ++i) {
            int linesCleared = currentPlayer->drop();

            if (linesCleared >= 2) {
                textDisplay->display(*player1, *player2);
#ifdef BIQUADRIS_GRAPHICS
                if (!textOnly && graphicsDisplay) {
                    graphicsDisplay->render(*player1, *player2);
                    graphicsDisplay->renderSpecialActionPrompt(currentPlayer->getPlayerNum());
                }
#endif
                cout << "Special action! Choose: blind, heavy, or force <blocktype>" << endl;
                string action;
#ifdef BIQUADRIS_GRAPHICS
                if (!textOnly && graphicsDisplay) {
                    action = graphicsDisplay->getSpecialActionInput();
                } else
#endif
                {
                    cin >> action;
                }

                if (isPrefix(action, "blind")) {
                    otherPlayer->applyEffect(make_unique<BlindEffect>());
                } else if (isPrefix(action, "heavy")) {
                    otherPlayer->applyEffect(make_unique<HeavyEffect>());
                } else if (isPrefix(action, "force")) {
                    char blockType;
#ifdef BIQUADRIS_GRAPHICS
                    if (!textOnly && graphicsDisplay) {
                        graphicsDisplay->renderBlockSelector();
                        blockType = graphicsDisplay->getBlockTypeInput();
                    } else
#endif
                    {
                        cin >> blockType;
                    }
                    otherPlayer->applyEffect(make_unique<ForceEffect>(blockType));
                }

#ifdef BIQUADRIS_GRAPHICS
                if (textOnly || !graphicsDisplay)
#endif
                    cin.ignore();
            }

            if (currentPlayer->hasLost()) {
#ifdef BIQUADRIS_GRAPHICS
                if (!textOnly && graphicsDisplay) {
                    graphicsDisplay->renderGameOver(
                        currentPlayer->getPlayerNum(),
                        otherPlayer->getPlayerNum(),
                        player1->getScore(),
                        player2->getScore(),
                        hiScore
                    );
                    while (true) {
                        string response = graphicsDisplay->getInput();
                        if (response == "restart") { restart(); return; }
                        if (response == "eof") {
                            cout << "Thanks for playing Biquadris!" << endl;
                            shouldQuit = true;
                            return;
                        }
                    }
                } else
#endif
                {
                    cout << "\n========================================" << endl;
                    cout << "          GAME OVER!" << endl;
                    cout << "========================================" << endl;
                    cout << "Player " << currentPlayer->getPlayerNum() << " loses!" << endl;
                    cout << "Player " << otherPlayer->getPlayerNum()   << " wins!" << endl;
                    cout << "\nFinal Scores:" << endl;
                    cout << "  Player 1: " << player1->getScore() << endl;
                    cout << "  Player 2: " << player2->getScore() << endl;
                    cout << "  High Score: " << hiScore << endl;
                    cout << "\nWould you like to restart? (yes/no): ";
                    string response;
                    cin >> response;
                    cin.ignore();
                    if (isPrefix(response, "yes") || isPrefix(response, "y")) {
                        restart(); return;
                    } else {
                        cout << "Thanks for playing Biquadris!" << endl;
                        shouldQuit = true;
                        return;
                    }
                }
            }
        }
        switchPlayer();
    } else if (isPrefix(cmd, "levelup")) {
        for (int i = 0; i < multiplier; ++i) currentPlayer->levelUp();
    } else if (isPrefix(cmd, "leveldown")) {
        for (int i = 0; i < multiplier; ++i) currentPlayer->levelDown();
    } else if (isPrefix(cmd, "norandom")) {
        string file;
        cin >> file;
        currentPlayer->setNonRandom(file);
        cin.ignore();
    } else if (isPrefix(cmd, "random")) {
        currentPlayer->setRandom();
    } else if (isPrefix(cmd, "sequence")) {
        string file;
        cin >> file;
        cin.ignore();
        ifstream commandFile(file);
        if (commandFile.is_open()) {
            string line;
            while (getline(commandFile, line)) {
                if (line.empty() || line[0] == '#') continue;
                int mult = 1;
                size_t idx = 0;
                while (idx < line.length() && isdigit(line[idx])) ++idx;
                if (idx > 0) {
                    mult = stoi(line.substr(0, idx));
                    line = line.substr(idx);
                }
                if (mult <= 0) mult = 1;
                processCommand(line, mult);
                if (shouldQuit) break;
            }
            commandFile.close();
        } else {
            cerr << "Error: Could not open sequence file " << file << endl;
        }
    } else if (isPrefix(cmd, "restart")) {
        restart();
    } else if (cmd.length() == 1 && (cmd[0] == 'I' || cmd[0] == 'J' ||
               cmd[0] == 'L' || cmd[0] == 'O' || cmd[0] == 'S' ||
               cmd[0] == 'T' || cmd[0] == 'Z')) {
        currentPlayer->replaceBlock(cmd[0]);
    }
}

void Game::run() {
    cout << "Welcome to Biquadris!" << endl;

    while (true) {
        textDisplay->display(*player1, *player2);
#ifdef BIQUADRIS_GRAPHICS
        if (!textOnly && graphicsDisplay) {
            graphicsDisplay->render(*player1, *player2);
        }
#endif
        cout << "Player " << currentPlayer->getPlayerNum() << "'s turn" << endl;

        string input = getCommand();
        if (input == "eof") break;

        int multiplier = 1;
        size_t i = 0;
        while (i < input.length() && isdigit(input[i])) ++i;
        if (i > 0) {
            multiplier = stoi(input.substr(0, i));
            input = input.substr(i);
        }
        if (multiplier <= 0) multiplier = 1;

        processCommand(input, multiplier);
        if (shouldQuit) break;

        if (player1->getScore() > hiScore) { hiScore = player1->getScore(); saveHighScore(); }
        if (player2->getScore() > hiScore) { hiScore = player2->getScore(); saveHighScore(); }
    }

    saveHighScore();
}

void Game::restart() {
    player1->reset();
    player2->reset();
    currentPlayer = player1.get();
    otherPlayer = player2.get();
}

void Game::loadHighScore() {
    ifstream file(".biquadris_highscore");
    if (file.is_open()) { file >> hiScore; file.close(); }
}

void Game::saveHighScore() {
    ofstream file(".biquadris_highscore");
    if (file.is_open()) { file << hiScore; file.close(); }
}
