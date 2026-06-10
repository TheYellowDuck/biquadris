// level.cc
#include "level.h"
#include <cstdlib>
#include <iostream>

using namespace std;

Level::Level(int levelNum, const string& scriptFile) 
    : levelNum(levelNum), scriptFile(scriptFile), useRandom(levelNum != 0) {
    
    if (levelNum == 0 || levelNum >= 3) {
        scriptStream = make_unique<ifstream>(scriptFile);
        if (!scriptStream->is_open()) {
            cerr << "Warning: Could not open " << scriptFile << endl;
        }
    }
}

Level::~Level() {}

char Level::getNextBlock() {
    if (levelNum == 0 || (levelNum >= 3 && !useRandom)) {
        return getScriptBlock();
    } else {
        return getRandomBlock();
    }
}

char Level::getScriptBlock() {
    char block;
    if (scriptStream && *scriptStream >> block) {
        return block;
    } else {
        scriptStream->clear();
        scriptStream->seekg(0);
        if (scriptStream && *scriptStream >> block) {
            return block;
        }
    }
    return 'I';
}

char Level::getRandomBlock() {
    const char blocks[] = {'I', 'J', 'L', 'O', 'S', 'T', 'Z'};
    
    if (levelNum == 1) {
        int r = rand() % 12;
        if (r < 1) return 'S';
        if (r < 2) return 'Z';
        if (r < 4) return 'I';
        if (r < 6) return 'J';
        if (r < 8) return 'L';
        if (r < 10) return 'O';
        return 'T';
    } else if (levelNum == 2) {
        return blocks[rand() % 7];
    } else if (levelNum >= 3) {
        int r = rand() % 9;
        if (r < 2) return 'S';
        if (r < 4) return 'Z';
        if (r < 5) return 'I';
        if (r < 6) return 'J';
        if (r < 7) return 'L';
        if (r < 8) return 'O';
        return 'T';
    }
    
    return 'I';
}

void Level::setNonRandom(const string& file) {
    scriptFile = file;
    scriptStream = make_unique<ifstream>(file);
    useRandom = false;
}

void Level::setRandom() {
    useRandom = true;
}