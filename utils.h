#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <fstream>
#include <vector>
#include <io.h>
#include <regex>
#include <direct.h>
#include <iostream>

class Utils {
public:
    static std::vector<std::string> split(std::string str, std::string delim);
    static std::string getFileName(std::string filePath);
    static std::string getFileDir(std::string filePath);
    static bool checkDir(std::string dirPath);
};


#endif // UTILS_H
