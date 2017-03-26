#include "handlemachine.h"

/**
 * @brief HandleMachine::isTheKeyLine >> 判断是否为key所在的行
 * @param key
 * @param line
 * @return
 */
bool HandleMachine::isTheKeyLine(std::string key, std::string line)
{
    //关键词__default_key__将会被默认为true, 它不能用来作为idf文件的变量
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
    int  targetLoc = -1;

    for (int lineIndex = 0; lineIndex < text.size(); lineIndex++) {
        if (!isFindKey1 && !isFindKey2) {
            if (isTheKeyLine(locationKey, text[lineIndex])) {
                isFindKey1 = true;
                targetLoc = lineIndex;
            }
        }
        else if (isFindKey1 && !isFindKey2) {
            if (isTheKeyLine(confirmKey, text[lineIndex])) {
                isFindKey2 = true;
                break;
            } else {
                if (isTheEndLine((text[lineIndex]))) {
                    isFindKey1 = false;
                    isFindKey2 = false;
                    targetLoc = -1;
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

    if (!isFindKey1 && !isFindKey2) {
        if (targetLoc > -1) return targetLoc;
        else return lineIndex;
    } else return -1;
}

/**
 * @brief HandleMachine::replacePartStruct >> 替换源.idf文本中一些结构的部分内容(对json文件的结构有要求)
 * @param root
 * @return
 */
bool HandleMachine::replacePartStruct(Json::Value root)
{
    Json::Value structPartReplace = root["structPartReplace"];
    if (!structPartReplace.isNull()) {
        if (structPartReplace.isInt() && structPartReplace.asInt() == 0) {
            return true;
        } else {
            for (int i = 0; i < structPartReplace.size(); i++) {
                std::string locationKey = structPartReplace[i]["locationKey"].asString();
                std::string confirmKey = structPartReplace[i]["confirmKey"].asString();
                Json::Value rpData = structPartReplace[i]["rpData"];

                int beginLoc = getReplaceLocation(content, locationKey, confirmKey);
                if (beginLoc > -1) {
                    for (int cur = 0; cur < rpData.size(); cur++) {
                        std::regex reg("(^\\s*)([^]*?)(,|;)(.*)");
                        std::string fmt;
                        std::string prefix = "$01";
                        std::string suffix = "$03$04";

                        int targetPos = beginLoc+rpData[cur]["offset"].asInt();
                        std::string data = rpData[cur]["data"].asString();
                        std::string textLine = content[targetPos];
                        fmt = prefix + data + suffix;

                        std::string newLine = std::regex_replace(textLine, reg, fmt);
                        content[targetPos] = newLine;
                    }
                }
            }
        }
    } else return false;
}

/**
 * @brief HandleMachine::replaceAllStruct >> 替换源.idf文本中一些结构的全部内容(对json文件的结构有要求)
 * @param root
 * @return
 */
bool HandleMachine::replaceAllStruct(Json::Value root)
{
    Json::Value structAllReplace = root["structAllReplace"];
    if (!structAllReplace.isNull()) {
        if (structAllReplace.isInt() && structAllReplace.asInt() == 0) {
            return true;
        } else {
            for (int i = 0; i < structAllReplace.size(); i++) {
                std::string locationKey = structAllReplace[i]["locationKey"].asString();
                std::string confirmKey = structAllReplace[i]["confirmKey"].asString();
                Json::Value structData = structAllReplace[i]["structData"];

                int beginLoc = getReplaceLocation(content, locationKey, confirmKey);
                if (beginLoc > -1) {
                    for (int j = 0; j < structData.size(); j++) {
                        content[beginLoc++] = structData[j].asString();
                    }
                }
            }
        }
    } else return false;
}

/**
 * @brief HandleMachine::insertStruct >> 向源.idf文本中插入新的结构(对json文件的结构有要求)
 * @param root
 * @return
 */
bool HandleMachine::insertStruct(Json::Value root)
{
    Json::Value structInsert = root["structInsert"];
    if (!structInsert.isNull()) {
        if (structInsert.isInt() && structInsert.asInt() == 0) {
            return true;
        } else {
            for (int i = 0; i < structInsert.size(); i++) {
                std::string locationKey = structInsert[i]["locationKey"].asString();
                std::string confirmKey = structInsert[i]["confirmKey"].asString();
                Json::Value structData = structInsert[i]["structData"];

                int beginLoc = getInsertLocation(content, locationKey, confirmKey);
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
    } else return false;
}

/**
 * @brief HandleMachine::initCityData >> 根据city变量对源.idf文本进行初始化设置
 * @param cityName
 * @param cityFilePath
 */
void HandleMachine::initCityData(std::string cityName, std::string cityFilePath)
{
    std::ifstream iCityFile(cityFilePath);
    if (!iCityFile.is_open()) {
        std::cerr << "City source file open fail!" << std::endl;
        return;
    }

    //读取json文件，并进行内容修改操作
    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(iCityFile, root, false)) {
        std::cerr << "The city source file may have error!" << std::endl;
        return;
    }

    std::string city = root["city"].asString();
    if (cityName.compare(city) != 0) {
        std::cerr << "non correct city source file" << std::endl;
        return;
    }

    if (replacePartStruct(root) && replaceAllStruct(root) && insertStruct(root)) {
        std::cout << "Init city data success!" << std::endl;
    } else {
        std::cout << "Fail" << std::endl;
    }
    iCityFile.close();
}

/**
 * @brief HandleMachine::configure >> 对源.idf文本进行初始化配置
 * @param cfgFilePath
 */
void HandleMachine::configure(std::string cfgFilePath)
{
    std::ifstream cfgFile(cfgFilePath);
    if (!cfgFile.is_open()) {
        std::cerr << "Configure file open fail!" << std::endl;
        return;
    }

    //读取json文件，并进行内容修改操作
    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(cfgFile, root, false)) {
        std::cerr << "The configure file may have error!" << std::endl;
        return;
    }

    if (replacePartStruct(root) && replaceAllStruct(root) && insertStruct(root)) {
        std::cout << "Configure success!" << std::endl;
    } else {
        std::cerr << "Configure fail!" << std::endl;
    }
    cfgFile.close();
}


/**
 * @brief HandleMachine::operate >> 对.idf文本进行动作配置，即配置数据来自外部输入
 * @param opFilePath
 * @param opKey
 * @param dataVec
 * @return
 */
bool HandleMachine::operate(std::string opFilePath ,std::string opKey, std::vector<std::string> dataVec)
{
    std::ifstream opFile(opFilePath);
    if (opFile.is_open()) {
        Json::Reader reader;
        Json::Value root;
        if (reader.parse(opFile, root, false)) {
            Json::Value opObject = root[opKey];
            if (!opObject.isNull()) {
                std::string locationKey = opObject["locationKey"].asString();
                std::string confirmKey = opObject["confirmKey"].asString();
                Json::Value offsets = opObject["offsets"];

                if (offsets.size() == dataVec.size()) {
                    int beginLoc = getReplaceLocation(content, locationKey, confirmKey);
                    if (beginLoc > -1) {
                        for (int index = 0; index < offsets.size(); index++) {
                            std::regex reg("(^\\s*)([^]*?)(,|;)(.*)");
                            std::string fmt;
                            std::string prefix = "$01";
                            std::string suffix = "$03$04";

                            int targetPos = beginLoc + offsets[index].asInt();
                            std::string textLine = content[targetPos];
                            fmt = prefix + dataVec[index] + suffix;

                            std::string newLine = std::regex_replace(textLine, reg, fmt);
                            content[targetPos] = newLine;
                        }
                        return true;
                    } else {
                        std::cerr << "Can't locate the position!" << std::endl;
                    }
                } else {
                    std::cerr << "The dataVec is unacceptale!" << std::endl;
                }
            } else {
                std::cerr << "Can't find the opKey: [" << opKey << "]" << std::endl;
            }
        } else {
            std::cerr << "The operation file may have error!" << std::endl;
        }
    } else {
        std::cerr << "The operation file open fail" << std::endl;
    }
    return false;
}

bool HandleMachine::separate()
{
    //待租
    std::ofstream nr("E:\\WorkSpace\\output\\25_nr.idf",std::ios::out | std::ios::trunc);
    //已租无人
    std::ofstream r("E:\\WorkSpace\\output\\25_r.idf",std::ios::out | std::ios::trunc);
    //已租有人
    std::ofstream rp("E:\\WorkSpace\\output\\25_rp.idf",std::ios::out | std::ios::trunc);

    if (!nr.is_open() && !r.is_open() && !rp.is_open()) return false;

    std::cout << "Separeting..." << std::endl;

    for (int i = 0; i < content.size(); i++) {
        nr << content[i] << std::endl;
        r << content[i] << std::endl;
        rp << content[i] << std::endl;
    }
    nr.close();
    r.close();
    rp.close();
    std::cout << "Separeting success!" << std::endl;
    return true;
}

void HandleMachine::output()
{
    std::ofstream outfile("E:\\WorkSpace\\output\\modify.idf", std::ios::out | std::ios::trunc);
    if (!outfile.is_open()) {
        std::cerr << "can't output base source file!" << std::endl;
        return;
    }

    std::cout << "Output" << std::endl;
    for (int i = 0; i < content.size(); i++) {
        outfile << content[i] << std::endl;
    }
    outfile.close();
}

