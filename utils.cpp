#include "utils.h"

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
   return vec.back();
}

std::string Utils::getFileDir(std::string filePath)
{
    std::vector<std::string> vec = split(filePath, "\\");
    std::string dir = vec[0];
    for (int i = 1; i < vec.size()-1; i++) {
        dir += "\\" + vec[i];
    }
    return dir;
}

bool Utils::checkDir(std::string dirPath)
{
    if (access(dirPath.c_str(), 00) != -1) return true;
    else {
        std::regex reg("([^\\\\\\s]+\\\\)+[^\\\\\\s]*");
        if (std::regex_match(dirPath, reg)) {
            std::vector<std::string> dirVec = split(dirPath, "\\");
            std::string curDir = "";
            for (int i = 0; i < dirVec.size(); i++) {
                curDir += dirVec[i] + "\\";
                if (access(curDir.c_str(), 00) == -1) mkdir(curDir.c_str());
                else continue;
            }
        } else {
            std::cerr << "The Dirpath format error!" << std::endl;
            return false;
        }
        return true;
    }
}
