// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
// Required Notice: Copyright (c) 2026 George Zhang and the original Biquadris team (CS 246, University of Waterloo) — https://github.com/TheYellowDuck

// main.cc
#include <iostream>
#include <string>
#include <sstream>
#include "game.h"

using namespace std;

int main(int argc, char *argv[]) {
    bool textOnly = false;
    int seed = 0;
    string scriptFile1 = "biquadris_sequence1.txt";
    string scriptFile2 = "biquadris_sequence2.txt";
    int startLevel = 0;

    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-text") {
            textOnly = true;
        } else if (arg == "-seed" && i + 1 < argc) {
            seed = stoi(argv[++i]);
        } else if (arg == "-scriptfile1" && i + 1 < argc) {
            scriptFile1 = argv[++i];
        } else if (arg == "-scriptfile2" && i + 1 < argc) {
            scriptFile2 = argv[++i];
        } else if (arg == "-startlevel" && i + 1 < argc) {
            startLevel = stoi(argv[++i]);
        }
    }

    Game game(textOnly, seed, scriptFile1, scriptFile2, startLevel);
    game.run();

    return 0;
}
