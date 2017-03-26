#include "utils.h"

bool Utils::checkJsonFile(std::string filePath)
{
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

std::vector<std::string> Utils::split(std::string str, std::string delim)
{
    std::vector<std::string> resultVec;
    if (str.empty()) return resultVec;
    str += delim;

    size_t pos = str.find(delim);
    size_t size = str.size();

    while (pos != std::string::npos) {
        std::string target = str.substr(0, pos);
        if (!target.empty()) resultVec.push_back(target);
        str = str.substr(pos+1, size);
        pos = str.find(delim);
    }

    return resultVec;
}

std::string Utils::getFileName(std::string filePath)
{
   std::vector<std::string> vec = split(filePath, "\\");
   if (vec.size() > 0) return vec.back();
   else return "";
}
