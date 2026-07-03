// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
// Required Notice: Copyright (c) 2026 George Zhang and the original Biquadris team (CS 246, University of Waterloo) — https://github.com/TheYellowDuck

// level.h
#ifndef LEVEL_H
#define LEVEL_H

#include <string>
#include <fstream>
#include <memory>

class Level {
protected:
    int levelNum;
    std::string scriptFile;
    std::unique_ptr<std::ifstream> scriptStream;
    bool useRandom;

public:
    Level(int levelNum, const std::string& scriptFile);
    virtual ~Level();
    
    virtual char getNextBlock();
    void setNonRandom(const std::string& file);
    void setRandom();
    
private:
    char getRandomBlock();
    char getScriptBlock();
};

#endif
