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
    std::vector<std::string> _originContent;
    std::vector<std::string> _content;
    std::string _sourfileName;

    bool isTheKeyLine(std::string key, std::string line);
    bool isTheEndLine(std::string line);

    int getReplaceLocation(std::vector<std::string> text, std::string locationKey, std::string confirmKey);
    int getInsertLocation(std::vector<std::string> text, std::string locationKey, std::string confirmKey);

    bool replacePartStruct(Json::Value root);
    bool replaceAllStruct(Json::Value root);
    bool insertStruct(Json::Value root);
public:
    HandleMachine(std::string sourceFilePath) {
        std::ifstream input(sourceFilePath);
        if (!input.is_open()) {
            std::cerr << sourceFilePath << " source file open fail!" << std::endl;
        } else {
            std::string line;
            while (std::getline(input, line)) _originContent.push_back(line);
            _content.assign(_originContent.begin(), _originContent.end());
        }
    }
    void initCityData(std::string cityName, std::string cityFilePath);
    void configure(std::string cfgFilePath);
    bool operate(std::string opFilePath, std::string opKey, std::vector<std::string> dataVec);
    bool separate();
    void output();
};

#endif // HANDLEMACHINE_H
