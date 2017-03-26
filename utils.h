#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <fstream>
#include <vector>
#include "json/json.h"

class Utils {
public:
    static bool checkJsonFile(std::string filePath);
    static std::vector<std::string> split(std::string str, std::string delim);
    static std::string getFileName(std::string filePath);
};


#endif // UTILS_H
