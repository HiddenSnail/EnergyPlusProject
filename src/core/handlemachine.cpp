#include "./core/handlemachine.h"

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
 * @brief HandleMachine::configure >> 使用配置文件对源.idf文本进行配置
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
            eCode = _P_ERR_OBJ_->addError("FILE_BROKEN", QString("Configure file %1 broken.").arg(cfgFileInfo.filePath()));
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
        eCode = _P_ERR_OBJ_->addError("FILE_OPEN_FAIL", QString("Configure file %1 open fail.").arg(cfgFileInfo.filePath()));
        if (pErrCode != nullptr) *pErrCode = eCode;
        return false;
    }
}

/**
 * @brief HandleMachine::operate >> 对.idf文本进行动作配置，即配置数据来自外部输入
 * @param opPackage: opKey & opFunc
 * @param pErrCode
 * @return
 */
bool HandleMachine::operate(OperatePackage opPackage, ErrorCode *pErrCode)
{
    ErrorCode eCode = 0;
    QString opFilePath(PathManager::instance()->getPath("OpFile"));
    QFile opFile(opFilePath);
    if (!opFile.open(QFile::ReadOnly))
    {
        eCode = _P_ERR_OBJ_->addError("FILE_OPEN_FAIL", QString("Operation file %1 open fail.").arg(opFile.fileName()));
        if (pErrCode != nullptr) *pErrCode = eCode;
        return false;
    }

    QTextStream inStream(&opFile);
    QJsonDocument doc = QJsonDocument::fromJson(inStream.readAll().toLatin1());
    opFile.close();

    if (!doc.isObject() || doc.isNull())
    {
        eCode = _P_ERR_OBJ_->addError("FILE_BROKEN", QString("Operation file %1 broken.").arg(opFile.fileName()));
        if (pErrCode != nullptr) *pErrCode = eCode;
        return false;
    }
    else
    {
        QString opKey = opPackage.first;
        QJsonObject opObject = doc.object()[opKey].toObject();
        if (!opObject.isEmpty())
        {
            QString locationKey = opObject["locationKey"].toString();
            QString confirmKey = opObject["confirmKey"].toString();
            Q_ASSERT_X(opObject["offsets"].isArray(), "json try to array", "this json value isn't array");
            QJsonArray offsets = opObject["offsets"].toArray();

            int beginLoc = getReplaceLocation(locationKey, confirmKey);
            if (beginLoc > -1)
            {
                QRegularExpression reg("(^\\s*)(.*)(,|;)(.*)");
                QString fmt, prefix("\\1"), suffix("\\3\\4");
                QStringList oldDataList; //被操参数的旧值序列
                QVector<int> posVec; //操作参数的具体位置序列
                for (int index = 0; index < offsets.size(); index++)
                {
                    int targetPos = beginLoc + offsets[index].toInt();
                    posVec.push_back(targetPos);
                    QRegularExpressionMatch match;
                    if (_content[targetPos].contains(reg, &match))
                    {
                        oldDataList << match.captured(2);
                    }
                    else
                    {
                        eCode = _P_ERR_OBJ_->addError("FILE_BROKEN", QString("File %1 broken.").arg(_fileName));
                        if (pErrCode != nullptr) *pErrCode = eCode;
                        return false;
                    }
                }
                //使用opFunc回调函数获取被操作参数新值序列
                auto opFunc = opPackage.second;
                QStringList newDataList = opFunc(oldDataList);
                for (int index = 0; index < posVec.size(); index++)
                {
                    QString newData = newDataList[index];
                    fmt = prefix + newData + suffix;
                    _content[posVec[index]].replace(reg, fmt);
                }
                eCode = _P_ERR_OBJ_->addError("SUCCESS", QString("Operation #%1# to %2 success.").arg(opKey, _fileName));
                if (pErrCode != nullptr) *pErrCode = eCode;
                return true;
            }
            else
            {
                eCode = _P_ERR_OBJ_->addError("FILE_BROKEN", QString("Can't locate operation #%1# begin pos.").arg(opKey));
                if (pErrCode != nullptr) *pErrCode = eCode;
                return false;
            }
        }
        else
        {
            eCode = _P_ERR_OBJ_->addError("PARA_ERR", QString("Operation #%1# to %2 isn't exist.").arg(opKey, _fileName));
            if (pErrCode != nullptr) *pErrCode = eCode;
            return false;
        }
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


//--------------------------------Class OpHandleFuncCreator-----------------------------------

OperateFactory* OperateFactory::instance()
{
    static OperateFactory operateFactory;
    return &operateFactory;
}


/**
 * @brief OperateFactory::calElecEqtWatts >> 计算房间除照明外其它设备的单位面积功率
 * @param roomSize: 房间面积
 * @param tvKW: 电视功率(kW)
 * @param fridgeKW: 冰箱功率(kW)
 * @param otherDevKW: 其它设备功率(kW)
 * @return
 */
OperatePackage OperateFactory::opElectricEquipment(const double roomSize, const double tvKW, const double fridgeKW, const double otherDevKW,
                                                    const double tvUseNum, const double fridgeUseNum, const double otherDevUseNum)
{   
    QString opKey("opElectricEquipment");
    return OperatePackage(opKey, [=](QStringList oldDataList)
    {
        double averageWatts = (tvKW*tvUseNum + fridgeKW*fridgeUseNum + otherDevKW*otherDevUseNum)*1000.0/roomSize;
        QStringList dataList;
        dataList << QString::number(averageWatts, 'f', 2);
        return dataList;
    });
}

/**
 * @brief OperateFactory::opElectricEquipment >> (重载)计算房间除照明外其它设备的单位面积功率
 * @param resultAverageW: 返回的房间除照明外其它设备的单位面积功率(w)
 * @return
 */
OperatePackage OperateFactory::opElectricEquipment(const double retAverageW)
{
    QString opKey("opElectricEquipment");
    return OperatePackage(opKey, [=](QStringList oldDataList)
    {
        QStringList dataList;
        dataList << QString::number(retAverageW, 'f', 2);
        return dataList;
    });
}

/**
 * @brief OperateFactory::calLightsWatts >> 计算房间照明的单位面积功率
 * @param roomSize: 房间面积
 * @param lightWatts: 照明总功率
 * @param lightUseNum: 照明使用系数
 * @return
 */
OperatePackage OperateFactory::opLights(const double roomSize, const double lightKW, const double lightUseNum)
{
    QString opKey("opLights");
    return OperatePackage(opKey, [=](QStringList oldDataList)
    {
        double averageWatts = lightKW*lightUseNum*1000.0/roomSize;
        QStringList dataList;
        dataList << QString::number(averageWatts, 'f', 2);
        return dataList;
    });
}

/**
 * @brief OperateFactory::opLights >> (重载)计算房间照明的单位面积功率
 * @param retAverageW: 返回的房间照明的单位面积功率(W)
 * @return
 */
OperatePackage OperateFactory::opLights(const double retAverageW)
{
    QString opKey("opLights");
    return OperatePackage(opKey, [=](QStringList oldDataList)
    {
        QStringList dataList;
        dataList << QString::number(retAverageW, 'f', 2);
        return dataList;
    });
}

/**
 * @brief OperateFactory::opSchComByTempOffset >> 通过温度偏差设置设计日的时间范围及温度大小
 * @param startTime: 起始时间
 * @param keepHours: 持续时间
 * @param tempOffset: 温度偏差
 * @param isCool: 是否是使用冷气（即夏季设计日）
 * @return
 */
OperatePackage OperateFactory::opSchComByTempOffset(const QTime startTime, const unsigned int keepHours,
                                                   const unsigned int tempOffset, const bool isCool)
{
    QString opKey = isCool ? QString("opSchComCoolByOffset") : QString("opSchComHeatByOffset");
    return OperatePackage(opKey, [=](QStringList oldDataList)
    {
        QStringList dataList(oldDataList);
        int realTempOffset = isCool ? (int)tempOffset : -(int)tempOffset;

        //获取headTime和tailTime
        QRegularExpression reg("([2]{1}[0-3]{1}:[0-6]{1}[0-9]{1}|[1]{0,1}[0-9]{1}:[0-6]{1}[0-9]{1})");
        QRegularExpressionMatch result1, result2;
        QString headTimeStr, tailTimeStr;
        if (dataList[0].contains(reg, &result1) && dataList[2].contains(reg, &result2))
        {
            headTimeStr = result1.captured(1);
            tailTimeStr = result2.captured(1);
        }
        else
        {
            headTimeStr = "6:00";
            tailTimeStr = "22:00";
        }
        QTime headTime = QTime::fromString(headTimeStr, "H:mm");
        QTime tailTime = QTime::fromString(tailTimeStr, "H:mm");

        if (keepHours > 0 && keepHours < 24)
        {
            if (QTime(0, 0) == startTime)
            {
                QTime endTime(keepHours, 0);
                if (endTime < tailTime)
                {
                    dataList[0] = QString("Until: %1").arg(endTime.toString("H:mm"));
                    dataList[1] = QString::number(dataList[1].toInt() + realTempOffset);
                }
                else
                {
                    dataList[1] = QString::number(dataList[1].toInt() + realTempOffset);
                    dataList[2] = QString("Until: %1").arg(endTime.toString("H:mm"));
                    dataList[3] = QString::number(dataList[3].toInt() + realTempOffset);
                }
            }
            else
            {
                int totalHours = startTime.hour() + keepHours;
                QTime endTime((startTime.hour()+keepHours)%24, startTime.minute());
                if (totalHours > 24 || (totalHours == 24 && startTime.minute() > 0))
                {
                    dataList[0] = QString("Until: %1").arg(endTime.toString("H:mm"));
                    dataList[1] = QString::number(dataList[1].toInt() + realTempOffset);
                    dataList[2] = QString("Until: %1").arg(startTime.toString("H:mm"));
                    dataList[5] = QString::number(dataList[5].toInt() + realTempOffset);
                }
                else if (totalHours < 24)
                {
                    dataList[0] = QString("Until: %1").arg(startTime.toString("H:mm"));
                    dataList[2] = QString("Until: %1").arg(endTime.toString("H:mm"));
                    dataList[3] = QString::number(dataList[3].toInt() + realTempOffset);
                }
                else
                {
                    if (startTime > headTime)
                    {
                        dataList[2] = QString("Until: %1").arg(startTime.toString("H:mm"));
                        dataList[5] = QString::number(dataList[5].toInt() + realTempOffset);
                    }
                    else
                    {
                        dataList[0] = QString("Until: %1").arg(startTime.toString("H:mm"));
                        dataList[3] = QString::number(dataList[3].toInt() + realTempOffset);
                        dataList[5] = QString::number(dataList[5].toInt() + realTempOffset);
                    }
                }
            }
        }
        else if (24 == keepHours)
        {
            dataList[1] = QString::number(dataList[1].toInt() + realTempOffset);
            dataList[3] = QString::number(dataList[3].toInt() + realTempOffset);
            dataList[5] = QString::number(dataList[5].toInt() + realTempOffset);
        }
        else
        {
            ;
        }
        return dataList;
    });
}

/**
 * @brief OperateFactory::opSchComByTempOffset >> (重载)通过温度偏差设置设计日的时间范围及温度大小
 * @param tempOffset: 温度偏差
 * @return
 */
OperatePackage OperateFactory::opSchComByTempOffset(const unsigned int tempOffset, const bool isCool)
{
    QString opKey = isCool ? QString("opSchComCoolByOffset") : QString("opSchComHeatByOffset");
    return OperatePackage(opKey, [=](QStringList oldDataList)
    {
        QStringList dataList(oldDataList);
        for (int i = 1; i < dataList.size(); i+=2)
        {
            int temp = dataList[i].toInt();
            dataList[i] = (isCool) ? QString::number(temp + tempOffset) : QString::number(temp - tempOffset);
        }
        return dataList;
    });
}

/**
 * @brief OperateFactory::opSchComByTemp >> 通过指定温度设置设计日的时间范围及温度大小
 * @param temperature
 * @param isCool
 * @return
 */
OperatePackage OperateFactory::opSchComByTemp(const unsigned int temperature, const bool isCool)
{
    QString opKey = isCool ? QString("opSchComCool") : QString("opSchComHeat");
    return OperatePackage(opKey, [=](QStringList oldDataList)
    {
        QStringList dataList;
        for (int i = 0; i < oldDataList.size(); i++)
        {
            dataList << QString::number(temperature);
        }
        return dataList;
    });
}

/**
 * @brief OperateFactory::opTimeSpan >> 设置时间跨度
 * @param quarter: 季度(模5) {0: 全年，1：第一季度, 2：第二季, 3：第三季度, 4：第四季度}
 * @return
 */
OperatePackage OperateFactory::opTimeSpan(const unsigned int quarter)
{
    QString opKey("opTimeSpan");
    return OperatePackage(opKey, [=](QStringList oldDataList)
    {
        QStringList dataList;
        switch (quarter % 5) {
        case 0:
        {
            //year
            dataList << QString::number(1) << QString::number(1) << QString::number(12) << QString::number(31);
            break;
        }
        case 1:
        {
            dataList << QString::number(1) << QString::number(1) << QString::number(3) << QString::number(31);
            break;
        }
        case 2:
        {
            dataList << QString::number(4) << QString::number(1) << QString::number(6) << QString::number(30);
            break;
        }
        case 3:
        {
            dataList << QString::number(7) << QString::number(1) << QString::number(9) << QString::number(30);
            break;
        }
        case 4:
        {
            //year
            dataList << QString::number(10) << QString::number(1) << QString::number(12) << QString::number(31);
            break;
        }
        default:
        {
            dataList << QString::number(1) << QString::number(1) << QString::number(12) << QString::number(31);
            break;
        }}
        return dataList;
    });
}
