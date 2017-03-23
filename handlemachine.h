#ifndef HANDLEMACHINE_H
#define HANDLEMACHINE_H
#define DEFAULT_KEY "__default_key__"
#include <iostream>
#include <string>
#include <regex>
#include <fstream>
#include "json/json.h"

class HandleMachine {
private:
    bool isTheKeyLine(std::string key, std::string line);
    bool isTheEndLine(std::string line);
    int getReplaceLocation(std::vector<std::string> text, std::string locationKey, std::string confirmKey);
    int getInsertLocation(std::vector<std::string> text, std::string locationKey, std::string confirmKey);
public:
    HandleMachine() {}
    void initData(std::string cityName, std::string sourceFilePath, std::string cityFilePath);
};

#endif // HANDLEMACHINE_H
