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