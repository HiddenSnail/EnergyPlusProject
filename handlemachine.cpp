#include "handlemachine.h"

/**
 * @brief HandleMachine::isTheKeyLine >> 判断是否为key所在的行
 * @param key
 * @param line
 * @return
 */
bool HandleMachine::isTheKeyLine(QString key, QString line)
{
    //关键词__default_key__将会被默认为true, 它不能用来作为idf文件的变量
    if (DEFAULT_KEY == key) return true;
    QString pattern = QString("^(\\s)*%1,.*").arg(key);
    QRegExp reg(pattern);
    return reg.exactMatch(line);
}

/**
 * @brief HandleMachine::isTheEndLine >> 判断是否为结尾行
 * @param line
 * @return
 */
bool HandleMachine::isTheEndLine(QString line)
{
    //正则表达式待修正
    QString pattern = QString(".*;.*");
    QRegExp reg(pattern);
    return reg.exactMatch(line);
}

/**
 * @brief getReplaceLocation >> 获取替换本文的开始位置
 * @param locationKey
 * @param confirmKey
 * @return
 */
int HandleMachine::getReplaceLocation(QString locationKey, QString confirmKey)
{
    if (locationKey.isEmpty() || confirmKey.isEmpty()) {
        qDebug() << "The two keys maybe have one empty!";
        return -1;
    }
    bool isFindKey1 = false, isFindKey2 = false;
    int  targetLoc = -1;

    for (int lineIndex = 0; lineIndex < _content.size(); lineIndex++) {
        if (!isFindKey1 && !isFindKey2) {
            if (isTheKeyLine(locationKey, _content[lineIndex])) {
                isFindKey1 = true;
                targetLoc = lineIndex;
            }
        }
        else if (isFindKey1 && !isFindKey2) {
            if (isTheKeyLine(confirmKey, _content[lineIndex])) {
                isFindKey2 = true;
                break;
            } else {
                if (isTheEndLine((_content[lineIndex]))) {
                    isFindKey1 = false;
                    isFindKey2 = false;
                    targetLoc = -1;
                }
            }
        }
        else {
            qDebug() << "Flag states:" << isFindKey1 << "&&" << isFindKey2;
            qDebug() << _content.size();
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
int HandleMachine::getInsertLocation(QString locationKey, QString confirmKey)
{
    if (locationKey.isEmpty() || confirmKey.isEmpty()) {
        qDebug() << "The two keys maybe have one empty!";
        return -1;
    }
    bool isFindKey1 = false, isFindKey2 = false;
    int targetLoc = -1, blankPos = 0;
    int lineIndex;
    for (lineIndex = 0; lineIndex < _content.size(); lineIndex++) {
        //找到里targetLoc最近的空白行
        if (targetLoc == -1 && _content[lineIndex].isEmpty()) blankPos = lineIndex;
        if (!isFindKey1 && !isFindKey2) {
            if (isTheKeyLine(locationKey, _content[lineIndex])) {
                isFindKey1 = true;
                if (targetLoc == -1) targetLoc = blankPos;
            }
        }
        else if (isFindKey1 && !isFindKey2) {
            //找到了与带插入文本相同的文本，故不必插入，返回-1
            if (isTheKeyLine(confirmKey, _content[lineIndex])) {
                isFindKey2 = true;
                return -1;
            } else {
                if (isTheEndLine(_content[lineIndex])) {
                    isFindKey1 = false;
                    isFindKey2 = false;
                }
            }
        }
        else {
            qDebug() << "Flag states:" << isFindKey1 << "&&" << isFindKey2;
            qDebug() << targetLoc;
            qDebug() << _content.size();
            return -1;
        }
    }

    if (targetLoc > -1) return targetLoc;
    else return lineIndex;
}

/**
 * @brief HandleMachine::replacePartStruct >> 替换源.idf文本中一些结构的部分内容(对json文件的结构有要求)
 * @param root
 * @return
 */
bool HandleMachine::replacePartStruct(QJsonObject root)
{
    if (root.isEmpty()) {
        qInfo() << "The root is empty";
        return false;
    }
    QJsonValue structPartReplace = root["structPartReplace"];
    if (structPartReplace.isNull()) return false;

    if (0 == structPartReplace.toInt(-1)) {
        qInfo() << "No need to replace!";
    } else {
        //断言structPartReplace是一个Json数组
        Q_ASSERT(structPartReplace.isArray());
        QJsonArray dataArray = structPartReplace.toArray();
        for (int i = 0; i < dataArray.size(); i++) {
            QString locationKey = dataArray[i].toObject()["locationKey"].toString();
            QString confirmKey = dataArray[i].toObject()["confirmKey"].toString();
            QJsonArray rpData = dataArray[i].toObject()["rpData"].toArray();
            int beginLoc = getReplaceLocation(locationKey, confirmKey);

            if (beginLoc > -1) {
                for (int cur = 0; cur < rpData.size(); cur++) {
                    QRegExp reg("(^\\s*)(.*)(,|;)(.*)");
                    QString fmt, prefix("\\1"), suffix("\\3\\4");
                    int targetPos = beginLoc + rpData[cur].toObject()["offset"].toInt();
                    QString data = rpData[cur].toObject()["data"].toString();
                    fmt = prefix + data + suffix;
                    _content[targetPos].replace(reg, fmt);
                }
            }
        }
    }
    return true;
}

/**
 * @brief HandleMachine::replaceAllStruct >> 替换源.idf文本中一些结构的全部内容(对json文件的结构有要求)
 * @param root
 * @return
 */
bool HandleMachine::replaceAllStruct(QJsonObject root)
{
    if (root.isEmpty()) {
        qInfo() << "The root is empty";
        return false;
    }
    QJsonValue structAllReplace = root["structAllReplace"];
    if (structAllReplace.isNull()) return false;
    if (0 == structAllReplace.toInt(-1)) {
        qInfo() << "No need to all replace";
    } else {
        Q_ASSERT_X(structAllReplace.isArray(), "json try to array", "this json value isn't array");
        QJsonArray dataArray = structAllReplace.toArray();
        for (int i = 0; i < dataArray.size(); i++) {
            QJsonObject obj = dataArray[i].toObject();
            QString locationKey = obj["locationKey"].toString();
            QString confirmKey = obj["confirmKey"].toString();
            QJsonArray structData = obj["structData"].toArray();

            int beginLoc = getReplaceLocation(locationKey, confirmKey);
            if (beginLoc > -1) {
                for (int j = 0; j < structData.size(); j++) {
                    _content[beginLoc++] = structData[j].toString();
                }
            }
        }
    }
    return true;
}

/**
 * @brief HandleMachine::insertStruct >> 向源.idf文本中插入新的结构(对json文件的结构有要求)
 * @param root
 * @return
 */
bool HandleMachine::insertStruct(QJsonObject root)
{
    if (root.isEmpty()) {
        qInfo() << "The root is empty!";
        return false;
    }
    QJsonValue structInsert = root["structInsert"];
    if (structInsert.isNull()) return false;
    if (0 == structInsert.toInt(-1)) {
        qInfo() << "No need to insert";
    } else {
        Q_ASSERT_X(structInsert.isArray(), "json try to array", "this json value isn't array");
        QJsonArray dataArray = structInsert.toArray();
        for (int i = 0; i < dataArray.size(); i++) {
            QJsonObject obj = dataArray[i].toObject();
            QString locationKey = obj["locationKey"].toString();
            QString confirmKey = obj["confirmKey"].toString();
            QJsonArray structData = obj["structData"].toArray();
            int beginLoc = getInsertLocation(locationKey, confirmKey);
            if (beginLoc > -1) {
                QVector<QString> structDataVec;
                _content.insert(beginLoc++, "");
                for (int j = 0; j < structData.size(); j++) {
                    _content.insert(beginLoc++, structData[j].toString());
                }
            }
        }
    }
    return true;
}

/**
 * @brief HandleMachine::initCityData >> 根据city变量对源.idf文本进行初始化设置
 * @param cityName
 * @param cityFilePath
 */
bool HandleMachine::initCityData(QString cityName)
{
    QString pathPrefix(BASE_MODEL_DIR);
    QString cityFilePath = QString(pathPrefix + "/%1.json").arg(cityName.toLower());
    return configure(cityFilePath);
    return true;
}

/**
 * @brief HandleMachine::configure >> 对源.idf文本进行初始化配置
 * @param cfgFilePath
 */
bool HandleMachine::configure(QString cfgFilePath)
{
    QFile cfgFile(cfgFilePath);
    if (!cfgFile.open(QFile::ReadOnly)) {
        qInfo() << "Configure file open fail!";
        return false;
    }

    QTextStream inStream(&cfgFile);
    QJsonDocument doc = QJsonDocument::fromJson(inStream.readAll().toLatin1());
    cfgFile.close();

    if (!doc.isObject() || doc.isNull()) {
        qInfo() << "The configure file maybe broken!";
        return false;
    } else {
         QJsonObject root = doc.object();
         if (replacePartStruct(root) && replaceAllStruct(root) && insertStruct(root)) {
             qInfo() << "Configure success!";
             return true;
         } else {
             qInfo() << "Configure fail!";
             return false;
         }
    }
}


/**
 * @brief HandleMachine::operate >> 对.idf文本进行动作配置，即配置数据来自外部输入
 * @param opFilePath
 * @param opKey
 * @param dataVec
 * @return
 */
bool HandleMachine::operate(QString opFilePath ,QString opKey, QVector<QString> dataVec)
{
    QFile opFile(opFilePath);
    if (!opFile.open(QFile::ReadOnly)) {
        qInfo() << "Operation file open fail!";
        return false;
    }
    QTextStream inStream(&opFile);
    QJsonDocument doc = QJsonDocument::fromJson(inStream.readAll().toLatin1());
    opFile.close();

    if (!doc.isObject() || doc.isNull()) {
        qInfo() << "The Operation file maybe broken!";
        return false;
    } else {
        QJsonObject opObject = doc.object()[opKey].toObject();
        if (!opObject.isEmpty()) {
            QString locationKey = opObject["locationKey"].toString();
            QString confirmKey = opObject["confirmKey"].toString();
            Q_ASSERT_X(opObject["offsets"].isArray(), "json try to array", "this json value isn't array");
            QJsonArray offsets = opObject["offsets"].toArray();
            Q_ASSERT_X(offsets.size() == dataVec.size(), "operation args", "the operation args number is wrong");
            int beginLoc = getReplaceLocation(locationKey, confirmKey);
            if (beginLoc > -1) {
                for (int index = 0; index < offsets.size(); index++) {
                    QRegExp reg("(^\\s*)(.*)(,|;)(.*)");
                    QString fmt, prefix("\\1"), suffix("\\3\\4");
                    int targetPos = beginLoc + offsets[index].toInt();

                    if (DEFAUL_VALUE != dataVec[index]) {
                        fmt = prefix + dataVec[index] + suffix;
                         _content[targetPos].replace(reg, fmt);
                    }
                    else continue;
                }
                qInfo() << "Operation success!";
                return true;
            }
        }
        qInfo() << "Operation fail!";
        return false;
    }
}

/**
 * @brief HandleMachine::save >> 保存配置操作后的.idf文件
 */
void HandleMachine::save()
{
    QFile file(_sourfilePath);
    if (file.open(QFile::WriteOnly)) {
        QTextStream out(&file);
        for (int i = 0; i < _content.size(); i++)
            out << _content[i] << endl;
        file.close();
        qInfo() << "Save success!";
    } else {
        qInfo() << "Save fail!";
    }
}

bool HandleMachine::separate()
{
    QString outputDir;
    int dotPos = _sourfileName.indexOf('.');
    QString realName = _sourfileName.left(dotPos);

    outputDir = ".\\output\\" + realName;

    if (Utils::checkDir(outputDir.toStdString())) {
        //基本
        QString baseDir = outputDir + "\\base";
        mkdir(baseDir.toStdString().c_str());
        //未租
        QString nrDir = outputDir + "\\nr";
        mkdir(nrDir.toStdString().c_str());
        //已租有人
        QString rpDir = outputDir + "\\rp";
        mkdir(rpDir.toStdString().c_str());
        //已租无人
        QString rDir = outputDir + "\\r";
        mkdir(rDir.toStdString().c_str());

        QFile baseFile(baseDir + "\\base.idf");
        QFile nrFile(nrDir + "\\nr.idf");
        QFile rpFile(rpDir + "\\np.idf");
        QFile rFile(rDir + "\\r.idf");
        if (baseFile.open(QFile::WriteOnly) && nrFile.open(QFile::WriteOnly)
                && rpFile.open(QFile::WriteOnly) && rFile.open(QFile::WriteOnly)) {
            QTextStream baseStream(&baseFile);
            QTextStream nrStream(&nrFile);
            QTextStream rpStream(&rpFile);
            QTextStream rStream(&rFile);

            for (int i = 0; i < _content.size(); i++) {
                baseStream << _content[i] << endl;
                nrStream << _content[i] << endl;
                rpStream << _content[i] << endl;
                rStream << _content[i] << endl;
            }
            baseFile.close();
            nrFile.close();
            rpFile.close();
            rFile.close();
            qInfo() << "Separeting success!";
            return true;
        }
    }
    qDebug() << "Separeting fail!";
    return false;
}

//void HandleMachine::startMachine(std::string cityName)
//{
//     std::string targetDir = Utils::getFileDir(_sourfilePath);
//    //将.ini文件拷贝至当前idf文件所在文件夹
//    std::ifstream sourceIddFile(EP_IDD_FILE);
//    std::ofstream destIddFile(targetDir + "\\" + Utils::getFileName(EP_IDD_FILE));
//    if (sourceIddFile.is_open() && destIddFile.is_open()) {
//        std::string line("");
//        while (std::getline(sourceIddFile, line))
//            destIddFile << line << std::endl;
//        sourceIddFile.close();
//        destIddFile.close();
//    } else {
//        std::cerr << "Idd file open fail!" << std::endl;
//        return;
//    }

//    //将要执行idf文件拷贝出一份新的，名为in.idf的文件
//    std::ofstream destCoreFile(targetDir + "\\" + "in.idf");
//    std::ifstream sourceCoreFile(_sourfilePath);
//    if (destCoreFile.is_open() && sourceCoreFile.is_open()) {
//        std::string line("");
//        while (std::getline(sourceCoreFile, line))
//            destCoreFile << line << std::endl;
//        sourceCoreFile.close();
//        destCoreFile.close();
//    } else {
//        std::cerr << "Core file open fail!" << std::endl;
//        return;
//    }

//    //将当前城市的天气文件拷贝至当前idf文件所在文件夹
//    std::ofstream destWeatherFile(targetDir + "\\" + "in.epw");
//    std::string weatherFilePath("");
//    weatherFilePath = weatherFilePath + WEATHER_DIR + "\\" + cityName + "_IWEC.epw";
//    std::ifstream sourceWeatherFile(weatherFilePath);
//    if (destWeatherFile.is_open() && sourceWeatherFile.is_open()) {
//        std::string line("");
//        while (std::getline(sourceWeatherFile, line))
//            destWeatherFile << line << std::endl;
//        sourceWeatherFile.close();
//        destWeatherFile.close();
//    } else {
//        std::cerr << "Weather file open fail!" << std::endl;
//        return;
//    }

//    char buffer[1000];
//    _getcwd(buffer,1000);
//    std::string workDir(buffer); //当前工作路径
//    _chdir(targetDir.c_str()); //将工作路径定位到targetDir

//    //调用ExpandObjects.exe
//    std::string epObjPath("");
//    epObjPath = epObjPath + workDir + "\\" + EP_OBJ_EXE;
//    std::cout << epObjPath << std::endl;
//    system(epObjPath.c_str());

//    //调用EnergyPlus.exe
//    std::string epPath("");
//    epPath = epPath + workDir + "\\" + EP_EXE;
//    std::cout << epPath << std::endl;

//    system(epPath.c_str());
//    _chdir(workDir.c_str()); //恢复原工作路径
//}

void HandleMachine::printFile()
{
    QFile fs("out.txt");
    if (fs.open(QIODevice::ReadWrite)) {
        QTextStream out(&fs);
        for (int i = 0; i < _content.size(); i++) {
            out << _content[i] << endl;
        }
        out.flush();
        fs.close();
    }
}

