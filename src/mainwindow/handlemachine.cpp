#include "./mainwindow/handlemachine.h"


HandleMachine::HandleMachine(QString sourceFilePath)
{
    try
    {
        QFile sourceFile(sourceFilePath);
        if (sourceFile.open(QFile::ReadOnly)) {
            QFileInfo info(sourceFilePath);
            _filePath = info.filePath();
            _fileName = info.fileName();
            _baseName = info.baseName();
            QTextStream in(&sourceFile);
            while (!in.atEnd())
            {
                QString line = in.readLine();
                _content.push_back(line);
            }
            sourceFile.close();
        }
        else
        {
            throw _P_ERR_OBJ_->addError("FILE_OPEN_FAIL");
        }
    } catch (ErrorCode code)
    {
        std::cout << "HandleMachine instance fail." << std::endl;
        exit(code);
    }
}


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
    if (locationKey.isEmpty() || confirmKey.isEmpty())
    {
        qDebug() << "The two keys maybe have one empty!";
        return -1;
    }
    bool isFindKey1 = false, isFindKey2 = false;
    int  targetLoc = -1;

    for (int lineIndex = 0; lineIndex < _content.size(); lineIndex++)
    {
        if (!isFindKey1 && !isFindKey2)
        {
            if (isTheKeyLine(locationKey, _content[lineIndex]))
            {
                isFindKey1 = true;
                targetLoc = lineIndex;
            }
        }
        else if (isFindKey1 && !isFindKey2)
        {
            if (isTheKeyLine(confirmKey, _content[lineIndex]))
            {
                isFindKey2 = true;
                break;
            }
            else
            {
                if (isTheEndLine((_content[lineIndex])))
                {
                    isFindKey1 = false;
                    isFindKey2 = false;
                    targetLoc = -1;
                }
            }
        }
        else
        {
            qDebug() << "Flag states:" << isFindKey1 << "&&" << isFindKey2;
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
    if (locationKey.isEmpty() || confirmKey.isEmpty())
    {
        return -1;
    }
    bool isFindKey1 = false, isFindKey2 = false;
    int targetLoc = -1, blankPos = 0;
    int lineIndex;
    for (lineIndex = 0; lineIndex < _content.size(); lineIndex++)
    {
        //找到里targetLoc最近的空白行
        if (targetLoc == -1 && _content[lineIndex].isEmpty()) blankPos = lineIndex;
        if (!isFindKey1 && !isFindKey2)
        {
            if (isTheKeyLine(locationKey, _content[lineIndex]))
            {
                isFindKey1 = true;
                if (targetLoc == -1) targetLoc = blankPos;
            }
        }
        else if (isFindKey1 && !isFindKey2)
        {
            //找到了与带插入文本相同的文本，故不必插入，返回-1
            if (isTheKeyLine(confirmKey, _content[lineIndex]))
            {
                isFindKey2 = true;
                return -1;
            }
            else
            {
                if (isTheEndLine(_content[lineIndex]))
                {
                    isFindKey1 = false;
                    isFindKey2 = false;
                }
            }
        }
        else
        {
            qDebug() << "Flag states:" << isFindKey1 << "&&" << isFindKey2;
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
    if (root.isEmpty())
    {
        qDebug() << "The root is empty";
        return false;
    }
    QJsonValue structPartReplace = root["structPartReplace"];
    if (structPartReplace.isNull()) return false;
    if (0 == structPartReplace.toInt(-1)) {}
    else
    {
        Q_ASSERT_X(structPartReplace.isArray(), "jsondata", "json data format unaccept.");
        QJsonArray dataArray = structPartReplace.toArray();
        for (int i = 0; i < dataArray.size(); i++)
        {
            QString locationKey = dataArray[i].toObject()["locationKey"].toString();
            QString confirmKey = dataArray[i].toObject()["confirmKey"].toString();
            QJsonArray rpData = dataArray[i].toObject()["rpData"].toArray();
            int beginLoc = getReplaceLocation(locationKey, confirmKey);

            if (beginLoc > -1)
            {
                for (int cur = 0; cur < rpData.size(); cur++)
                {
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
    if (root.isEmpty())
    {
        qInfo() << "The root is empty";
        return false;
    }
    QJsonValue structAllReplace = root["structAllReplace"];
    if (structAllReplace.isNull()) return false;
    if (0 == structAllReplace.toInt(-1)) {}
    else
    {
        Q_ASSERT_X(structAllReplace.isArray(), "jsondata", "json data format unaccept.");
        QJsonArray dataArray = structAllReplace.toArray();
        for (int i = 0; i < dataArray.size(); i++)
        {
            QJsonObject obj = dataArray[i].toObject();
            QString locationKey = obj["locationKey"].toString();
            QString confirmKey = obj["confirmKey"].toString();
            QJsonArray structData = obj["structData"].toArray();

            int beginLoc = getReplaceLocation(locationKey, confirmKey);
            if (beginLoc > -1)
            {
                foreach(auto &data, structData)
                {
                    _content[beginLoc++] = data.toString();
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
    if (root.isEmpty())
    {
        qInfo() << "The root is empty!";
        return false;
    }
    QJsonValue structInsert = root["structInsert"];
    if (structInsert.isNull()) return false;
    if (0 == structInsert.toInt(-1)) {}
    else
    {
        Q_ASSERT_X(structInsert.isArray(), "jsondata", "json data format unaccept.");
        QJsonArray dataArray = structInsert.toArray();
        for (int i = 0; i < dataArray.size(); i++)
        {
            QJsonObject obj = dataArray[i].toObject();
            QString locationKey = obj["locationKey"].toString();
            QString confirmKey = obj["confirmKey"].toString();
            QJsonArray structData = obj["structData"].toArray();
            int beginLoc = getInsertLocation(locationKey, confirmKey);
            if (beginLoc > -1)
            {
                _content.insert(beginLoc++, "");
                foreach(auto &data, structData)
                {
                    _content.insert(beginLoc++, data.toString());
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
bool HandleMachine::initCityData(QString cityName, ErrorCode *pErrCode)
{
    QString pathPrefix = PathManager::instance()->getPath("BaseModelDir");
    QString cityFilePath = QString(pathPrefix + "/%1.json").arg(cityName.toLower());
    return configure(cityFilePath, pErrCode);
}

/**
 * @brief HandleMachine::configure >> 对源.idf文本进行初始化配置
 * @param cfgFilePath
 */
bool HandleMachine::configure(QString cfgFilePath, ErrorCode *pErrCode)
{
    ErrorCode eCode = 0;
    QFileInfo cfgFileInfo(cfgFilePath);
    QFile cfgFile(cfgFilePath);
    if (cfgFile.open(QFile::ReadOnly))
    {
        QTextStream inStream(&cfgFile);
        QJsonDocument doc = QJsonDocument::fromJson(inStream.readAll().toLatin1());
        cfgFile.close();

        if (!doc.isObject() || doc.isNull())
        {
            eCode = _P_ERR_OBJ_->addError("FILE_BROKEN", QString("Configure file %1 broken.").arg(cfgFileInfo.fileName()));
            if (pErrCode != nullptr) *pErrCode = eCode;
            return false;
        }
        else
        {
             QJsonObject root = doc.object();
             if (replacePartStruct(root) && replaceAllStruct(root) && insertStruct(root))
             {
                 eCode = _P_ERR_OBJ_->addError("SUCCESS", "Configure success.");
                 if (pErrCode != nullptr) *pErrCode = eCode;
                 return true;
             }
             else
             {
                 eCode = _P_ERR_OBJ_->addError("SYS_FUNC_ERR", "System function call by HandleMachine::configure error.");
                 if (pErrCode != nullptr) *pErrCode = eCode;
                 return false;
             }
        }
    }
    else
    {
        eCode = _P_ERR_OBJ_->addError("FILE_OPEN_FAIL", QString("Configure file %1 open fail.").arg(cfgFileInfo.fileName()));
        if (pErrCode != nullptr) *pErrCode = eCode;
        return false;
    }
}

/**
 * @brief HandleMachine::save >> 保存配置操作后的.idf文件
 */
bool HandleMachine::save(ErrorCode *pErrCode)
{
    ErrorCode eCode = 0;
    QFile file(_filePath);
    if (file.open(QFile::WriteOnly))
    {
        QTextStream out(&file);
        foreach (auto &data, _content)
        {
            out << data << endl;
        }
        file.close();
        eCode = _P_ERR_OBJ_->addError("SUCCESS", QString("HandleMachine save %1 success.").arg(_fileName));
        if (pErrCode != nullptr) *pErrCode = eCode;
        return true;
    }
    else
    {
       eCode = _P_ERR_OBJ_->addError("FILE_IO_ERR", QString("HandleMachine save %1 fail.").arg(_fileName));
       if (pErrCode != nullptr) *pErrCode = eCode;
       return false;
    }
}

/**
 * @brief HandleMachine::separate >> 分离函数
 * @param fileNameList: 分离后的文件名列表
 * @return
 */
bool HandleMachine::separate(QStringList fileNameList, ErrorCode *pErrCode)
{
    ErrorCode eCode = 0;
    if (fileNameList.isEmpty())
    {
        eCode = _P_ERR_OBJ_->addError("PARA_ERR", "Function HandleMachine::separate's parameters unaccept.");
        if (pErrCode != nullptr) *pErrCode = eCode;
        return false;
    }
    QDir appDir(QApplication::applicationDirPath());
    QString outPutPath =  PathManager::instance()->getPath("OutPutDir");
    QDir outputDir(outPutPath);
    if (!outputDir.exists())
    {
        appDir.mkdir("output");
    }

    QDir targetDir(outPutPath + "/" + _baseName);
    foreach (QString name, fileNameList)
    {
        outputDir.mkpath(_baseName + "/" + name);
    }

    QString filePath = QString(targetDir.path() + "/%1/%1.idf");
    QFile firstFile(filePath.arg(fileNameList.constFirst()));

    if (firstFile.open(QFile::WriteOnly))
    {
        QTextStream stream(&firstFile);
        for (int i = 0; i < _content.size(); i++)
        {
            stream << _content[i] << endl;
        }
        firstFile.close();
    }
    else
    {
        eCode = _P_ERR_OBJ_->addError("FILE_OPEN_FAIL", QString("Can't create %1.").arg(fileNameList.constFirst()));
        if (pErrCode != nullptr) *pErrCode = eCode;
        return false;
    }

    bool sepState = true;
    for (int i = 1; i < fileNameList.size(); i++)
    {
        QFile file(filePath.arg(fileNameList[i]));
        if (file.exists())
        {
            file.remove();
        }
        sepState = sepState && firstFile.copy(filePath.arg(fileNameList[i]));
    }

    if (sepState)
    {
        eCode = _P_ERR_OBJ_->addError("SUCCESS", "Seperate idf files success.");
        if (pErrCode != nullptr) *pErrCode = eCode;
        return true;
    }
    else
    {
        eCode = _P_ERR_OBJ_->addError("EILE_IO_ERR", "Seperate idf files fail.");
        if (pErrCode != nullptr) *pErrCode = eCode;
        return false;
    }
}

