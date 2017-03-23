#include "handlemachine.h"

bool HandleMachine::isTheKeyLine(std::string key, std::string line)
{
    //关键词__default_key__将会被默认为true, 即不能用来作为idf文件的变量
    if (key.compare(DEFAULT_KEY) == 0) return true;
    std::regex reg("^(\\s)*" + key + ",.*");
    if (std::regex_match(line, reg)) {
        return true;
    } else {
        return false;
    }
}

bool HandleMachine::isTheEndLine(std::string line)
{
    //正则表达式待修正
    std::regex reg(".*;.*");
    if (std::regex_match(line, reg)) {
        return true;
    } else {
        return false;
    }
}

void HandleMachine::initData(std::string cityName, std::string sourceFilePath, std::string cityFilePath)
{
    std::ifstream iSourceFile(sourceFilePath);
    std::ifstream iCityFile(cityFilePath);
    if (!iSourceFile.is_open()) {
        std::cerr << ".idf source file open fail!" << std::endl;
        return;
    }
    if (!iCityFile.is_open()) {
        std::cerr << "city source file open fail!" << std::endl;
        return;
    }

    //将原始数据读入vector
    std::string line;
    std::vector<std::string> content;
    while (std::getline(iSourceFile, line)) {
        content.push_back(line);
    }

    //读取json文件，并进行内容修改操作
    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(iCityFile, root, false)) {
        std::cerr << "the city source file may have error!" << std::endl;
        return;
    }

    std::string city = root["city"].asString();
    if (cityName.compare(city) != 0) {
        std::cerr << "non correct city source file" << std::endl;
    }

    //内容替换部分
    Json::Value contentChange = root["contentChange"];
    if (contentChange.isNull()) {
        std::cerr << "the city source file may have error!" << std::endl;
        return;
    }

    for (int i = 0; i < contentChange.size(); i++) {
        std::string locationKey = contentChange[i]["locationKey"].asString();
        std::string confirmKey = contentChange[i]["confirmKey"].asString();
        Json::Value structData = contentChange[i]["structData"];

//        std::cout << "locationKey: " << locationKey << std::endl;
//        std::cout << "confirmKey: " << confirmKey << std::endl;
//        std::cout << std::endl;

        bool isRightType = false, isRightName = false;
        int spacing = 0;

        for (int j = 0; j < content.size(); j++) {
            if (isRightType && isRightName) {
                //开始进行替换
                for (int k = 0; k < structData.size(); k++) {
                    content[j++] = structData[k].asString();
                }
                break;

            } else if (isRightType == true && isRightName == false) {
                //开始验证confirmKey
                if (isTheKeyLine(confirmKey, content[j])) {
                    isRightName = true;
                    j = j-spacing-2;
                } else {
                    if (isTheEndLine(content[j])) {
                        isRightType = false;
                        isRightName = false;
                        spacing = 0;
                    } else {
                        spacing += 1;
                    }
                }
            } else {
                //开始验证locationKey
                if (isTheKeyLine(locationKey, content[j])) {
                    isRightType = true;
                }
            }
        }
    }

    //内容插入部分

    std::ofstream outfile("copy.idf");
    std::cout << "Output" << std::endl;
    for (int i = 0; i < content.size(); i++) {
        outfile << content[i] << std::endl;
    }
    iSourceFile.close();
    iCityFile.close();
    outfile.close();
}
