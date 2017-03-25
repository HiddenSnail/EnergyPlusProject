#ifndef UTIL_H
#define UTIL_H
#include <string>
#include <fstream>
#include "json/json.h"
bool checkJsonFile(std::string filePath) {
    std::ifstream inFile(filePath);
    if (inFile.is_open()) {
        Json::Reader reader;
        Json::Value root;
        if (reader.parse(inFile, root)) {
            std::cout << "Good file" << std::endl;
            return true;
        } else {
            std::cerr << "Bad file" << std::endl;
            return false;
        }
    } else {
        std::cerr << "File open fail" << std::endl;
        return false;
    }
}

#endif // UTIL_H
