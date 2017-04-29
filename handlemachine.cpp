#include "handlemachine.h"

QMutex HandleMachine::_startLock;
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
        qDebug() << "The root is empty";
        return false;
    }
    QJsonValue structPartReplace = root["structPartReplace"];
    if (structPartReplace.isNull()) return false;
    if (0 == structPartReplace.toInt(-1)) {
        qInfo() << "No need to replace!";
    } else {
        if(!structPartReplace.isArray()) { qFatal("Target is not array! Configure file maybe broken!"); }

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
        if(!structAllReplace.isArray()) { qFatal("Target is not array! Configure file maybe broken!"); }

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
        if(!structInsert.isArray()) { qFatal("Target is not array! Configure file maybe broken!"); }
        QJsonArray dataArray = structInsert.toArray();
        for (int i = 0; i < dataArray.size(); i++) {
            QJsonObject obj = dataArray[i].toObject();
            QString locationKey = obj["locationKey"].toString();
            QString confirmKey = obj["confirmKey"].toString();
            QJsonArray structData = obj["structData"].toArray();
            int beginLoc = getInsertLocation(locationKey, confirmKey);
            if (beginLoc > -1) {
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
    QString pathPrefix = PathManager::instance()->getPath("BaseModelDir");
    QString cityFilePath = QString(pathPrefix + "/%1.json").arg(cityName.toLower());
    return configure(cityFilePath);
}

/**
 * @brief HandleMachine::configure >> 对源.idf文本进行初始化配置
 * @param cfgFilePath
 */
bool HandleMachine::configure(QString cfgFilePath)
{
    qInfo() << QString("Start configure! [%1]").arg(_fileName);
    QFile cfgFile(cfgFilePath);
    if (!cfgFile.open(QFile::ReadOnly)) { qFatal("Configure file open fail!"); }

    QTextStream inStream(&cfgFile);
    QJsonDocument doc = QJsonDocument::fromJson(inStream.readAll().toLatin1());
    cfgFile.close();

    if (!doc.isObject() || doc.isNull()) {
        qFatal("The configure file maybe broken!");
    } else {
         QJsonObject root = doc.object();
         if (replacePartStruct(root) && replaceAllStruct(root) && insertStruct(root)) {
             qInfo() << QString("Configure success! [%1]").arg(_fileName);
             return true;
         } else {
             qFatal("Configure fail![%s]", _fileName.toStdString().c_str());
         }
    }
}

/**
 * @brief HandleMachine::save >> 保存配置操作后的.idf文件
 */
bool HandleMachine::save()
{
    QFile file(_filePath);
    if (file.open(QFile::WriteOnly)) {
        QTextStream out(&file);
        for (int i = 0; i < _content.size(); i++)
            out << _content[i] << endl;
        file.close();
        qInfo() << QString("Save success! [%1]").arg(_fileName);
        return true;
    } else {
        qFatal("Save fail! [%s]", _fileName.toStdString().c_str());
    }
}

/**
 * @brief HandleMachine::separate >> 分离函数
 * @param fileNameList: 分离厚度的文件名列表
 * @return
 */
void HandleMachine::separate(QStringList fileNameList)
{
    qInfo() << QString("Start seperate! [%1]").arg(_fileName);

    QDir appDir(QApplication::applicationDirPath());
    QString outPutPath = PathManager::instance()->getPath("OutPutDir");
    QDir outputDir(outPutPath);
    if (!outputDir.exists()) {
        appDir.mkdir("output");
    }

    QDir targetDir(outPutPath + "/" + _baseName);
    if (!targetDir.exists()) {
        outputDir.mkdir(_baseName);
    }

    QString filePath = QString(targetDir.path() + "/%1." + _fileSuffix);
    QFile sampleFile(filePath.arg(fileNameList.constFirst()));
    if (sampleFile.open(QFile::WriteOnly)) {
        QTextStream stream(&sampleFile);
        for (int i = 0; i < _content.size(); i++) {
            stream << _content[i] << endl;
        }
        sampleFile.close();
    } else {
        qFatal("Can't create %s model!", fileNameList.constFirst().toStdString().c_str());
    }

    bool sepState = true;
    for (int i = 1; i < fileNameList.size(); i++) {
        QFile file(filePath.arg(fileNameList[i]));
        if (file.exists()) { file.remove(); }
        sepState = sepState && sampleFile.copy(filePath.arg(fileNameList[i]));
    }

    if (sepState) {
        emit finishSep();
    } else {
        qFatal("Separate fail! [%s]", _fileName.toStdString().c_str());
    }
}

/**
 * @brief HandleMachine::startMachine >> 开启测评
 * @param weatherFileName
 */
void HandleMachine::startMachine(QString weatherFileName)
{
    qInfo() << QString("Start execute... [%1]").arg(_fileName);
    _startLock.lock();
    //修改Eplus的配置文件(.ini)
    QFile epIniFile(PathManager::instance()->getPath("EpIniFile"));
    QFile newEpIniFile("new.ini");
    if (epIniFile.open(QFile::ReadOnly) && newEpIniFile.open(QFile::WriteOnly)) {
        QTextStream stream1(&epIniFile);
        QTextStream stream2(&newEpIniFile);
        int flag = 0;  //0:未找到目标行, 1:已找到目标行, 2:已修改目标行
        while (!stream1.atEnd()) {
            QString line = stream1.readLine();
            switch (flag) {
            case 0: {
                QRegExp reg("\\[program\\]");
                if (reg.exactMatch(line)) {
                    flag = 1;
                }
                break;
            }
            case 1: {
                QDir epDir(PathManager::instance()->getPath("EplusDir"));
                line = QString("dir=%1\\").arg(epDir.path().replace("/", "\\"));
                flag = 2;
            }
            default:
                break;
            }
            stream2 << line << endl;
        }
        epIniFile.close();
        newEpIniFile.close();

        if (epIniFile.remove()) {
            newEpIniFile.rename(PathManager::instance()->getPath("EpIniFile"));
            qInfo() << "Energy+.ini configure success.";
        }
    } else {
        qFatal("Can't find Energy+.ini File!");
    }

    //修改Eplus的批处理文件(.bat)
    QFile epRunFile(PathManager::instance()->getPath("EpRunFile"));
    QFile newEpRunFile("new.bat");

    if (epRunFile.open(QFile::ReadOnly) && newEpRunFile.open(QFile::WriteOnly)) {
        QTextStream stream1(&epRunFile);
        QTextStream stream2(&newEpRunFile);
        int flag = 0;  //0:未找到目标行, 1:已修改input_path行, 2:已修改ouput_path行, 3:已修改weather_path行
        while (!stream1.atEnd()) {
            QString line = stream1.readLine();
            switch (flag) {
            case 0: {
                //input_path所在行定位
                QRegExp reg("(^\\s*set input_path=).*");
                if (reg.exactMatch(line)) {
                    line.replace(reg, QString("\\1%1\\").arg(_fileDir).replace("/", "\\"));
                    flag = 1;
                }
                break;
            }
            case 1: {
                //output_path所在行定位
                QRegExp reg("(^\\s*set output_path=).*");
                if (reg.exactMatch(line)) {
                    line.replace(reg, QString("\\1%1\\").arg(_fileDir).replace("/", "\\"));
                    flag = 2;
                }
                break;
            }
            case 2: {
                //weather_path所在行定位
                QRegExp reg("(^\\s*set weather_path=).*");
                if (reg.exactMatch(line)) {
                    QString weatherDir = PathManager::instance()->getPath("WeatherDir");
                    line.replace(reg, QString("\\1%1\\").arg(weatherDir).replace("/", "\\"));
                    flag = 3;
                }
                break;
            }
            default:
                break;
            }
            stream2 << line << endl;
        }
        epRunFile.close();
        newEpIniFile.close();
        if (epRunFile.remove()) {
            newEpRunFile.rename(PathManager::instance()->getPath("EpRunFile"));
            qInfo() << "RunEPlus.bat configure success.";
        }
    } else {
        qFatal("Can't find or replace RunEPlus.bat File!");
    }

    //执行RunEPlus.bat
    if (save()) {
        QProcess *p_proc = new QProcess();
        if (p_proc != NULL) {
            //设置脚本执行环境,而并非当前程序运行环境
            p_proc->setWorkingDirectory(PathManager::instance()->getPath("EplusDir"));
            //获取脚本文件信息
            QFileInfo shell =  QFileInfo(PathManager::instance()->getPath("EpRunFile"));
            //设置脚本参数
            QStringList args;

            args << _baseName << weatherFileName;
            //启动脚本
            p_proc->start(shell.absoluteFilePath(), args);

            if (p_proc->waitForStarted()) {
                qInfo() << "EnergyPlus process start!";
            } else {
                qFatal("EnergyPlus process start error!");
            }

            if(p_proc->waitForFinished()) {
                if (0 == p_proc->exitCode()) {
                    qInfo() << QString("Execute normal [%1]!").arg(_fileName);
                } else {
                    qFatal("Execute error! [%s]", _fileName.toStdString().c_str());
                }
            } else {
                qFatal("Time out! [%s]", _fileName.toStdString().c_str());
            }
            delete p_proc;
            p_proc = NULL;
            //发射完成信号
            emit finishExec();
        }
    }
    _startLock.unlock();
}

