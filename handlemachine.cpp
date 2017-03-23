#include "handlemachine.h"

/**
 * @brief HandleMachine::isTheKeyLine >> 判断是否为key所在的行
 * @param key
 * @param line
 * @return
 */
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

/**
 * @brief HandleMachine::isTheEndLine >> 判断是否为结尾行
 * @param line
 * @return
 */
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

/**
 * @brief getReplaceLocation >> 获取替换本文的开始位置
 * @param text
 * @param locationKey
 * @param confirmKey
 * @return
 */
int HandleMachine::getReplaceLocation(std::vector<std::string> text, std::string locationKey, std::string confirmKey)
{
    bool isFindKey1 = false, isFindKey2 = false;
    int spacing = 0, targetLoc = -1;

    for (int lineIndex = 0; lineIndex < text.size(); lineIndex++) {
        if (!isFindKey1 && !isFindKey2) {
            if (isTheKeyLine(locationKey, text[lineIndex])) {
                isFindKey1 = true;
            }
        }
        else if (isFindKey1 && !isFindKey2) {
            if (isTheKeyLine(confirmKey, text[lineIndex])) {
                isFindKey2 = true;
                targetLoc = lineIndex - spacing - 1;
                break;
            } else {
                if (isTheEndLine((text[lineIndex]))) {
                    isFindKey1 = false;
                    isFindKey2 = false;
                    spacing = 0;
                } else {
                    spacing += 1;
                }
            }
        }
        else {
            std::cerr << "Something wrong happen! [GetReLoc] operation fail!" << std::endl;
            return -1;
        }
    }
    return targetLoc;
}

/**
 * @brief getInsertLocation >> 获取插入文本的开始开始位置
 * @param text
 * @param locationKey
 * @param confirmKey
 * @return
 */
int HandleMachine::getInsertLocation(std::vector<std::string> text, std::string locationKey, std::string confirmKey)
{
    bool isFindKey1 = false, isFindKey2 = false;
    int targetLoc = -1;
    int lineIndex;
    for (lineIndex = 0; lineIndex < text.size(); lineIndex++) {
        if (!isFindKey1 && !isFindKey2) {
            if (isTheKeyLine(locationKey, text[lineIndex])) {
                isFindKey1 = true;
                if (targetLoc == -1) targetLoc = lineIndex;
            }
        }
        else if (isFindKey1 && !isFindKey2) {
            if (isTheKeyLine(confirmKey, text[lineIndex])) {
                isFindKey2 = true;
                return -1;
            } else {
                if (isTheEndLine(text[lineIndex])) {
                    isFindKey1 = false;
                    isFindKey2 = false;
                }
            }
        }
        else {
            std::cerr << "Something wrong happen! [GetInLoc] operation fail!" << std::endl;
            return -1;
        }
    }

    if (!isFindKey2) return targetLoc;
    else return -1;
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
        return;
    }

    //内容替换部分
    Json::Value contentChange = root["contentChange"];
    if (!contentChange.isNull()) {
        for (int i = 0; i<contentChange.size(); i++) {
            std::string locationKey = contentChange[i]["locationKey"].asString();
            std::string confirmKey = contentChange[i]["confirmKey"].asString();
            Json::Value structData = contentChange[i]["structData"];

            int beginLoc = getReplaceLocation(content, locationKey, confirmKey);
            if (beginLoc > -1) {
                for (int j = 0; j < structData.size(); j++) {
                    content[beginLoc++] = structData[j].asString();
                }
            }
        }
    }

    //内容插入部分
    Json::Value contentInsert = root["contentInsert"];
    if (!contentInsert.isNull()) {
        for (int i = 0; i < contentInsert.size(); i++) {
            std::string locationKey = contentInsert[i]["locationKey"].asString();
            std::string confirmKey = contentInsert[i]["confirmKey"].asString();
            Json::Value structData = contentInsert[i]["structData"];

            int beginLoc = getInsertLocation(content, locationKey, confirmKey);
            std::cout << beginLoc << std::endl;
            if (beginLoc > -1) {
                std::vector<std::string> structDataVec;
                for (int j = 0; j < structData.size(); j++) {
                    structDataVec.push_back(structData[j].asString());
                }
                structDataVec.push_back("");
                content.insert(content.begin()+beginLoc, structDataVec.begin(), structDataVec.end());
            }
        }
    }

    std::ofstream outfile("E:\\WorkSpace\\output\\base.idf", std::ios::out | std::ios::trunc);
    if (!outfile.is_open()) {
        std::cerr << "can't output base source file!" << std::endl;
        return;
    }

    std::cout << "Output" << std::endl;
    for (int i = 0; i < content.size(); i++) {
        outfile << content[i] << std::endl;
    }
    iSourceFile.close();
    iCityFile.close();
    outfile.close();
}
