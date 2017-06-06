#ifndef HANDLEMACHINE_H
#define HANDLEMACHINE_H
#define DEFAULT_KEY "__default_key__"
#define DEFAUL_VALUE "__default_value__"
#include "./global/stdafx.h"
#include "./utils/no_class_funcs.h"

//注：template函数要声明定义在同一个文件下
class HandleMachine: public QObject {
    Q_OBJECT
private:
    QStringList _content;
    QString _filePath;
    QString _fileName;
    QString _baseName;

    bool isTheKeyLine(QString key, QString line);
    bool isTheEndLine(QString line);

    int getReplaceLocation(QString locationKey, QString confirmKey);
    int getInsertLocation(QString locationKey, QString confirmKey);

    bool replacePartStruct(QJsonObject root);
    bool replaceAllStruct(QJsonObject root);
    bool insertStruct(QJsonObject root);

public:
    explicit HandleMachine(QString sourceFilePath);
    ~HandleMachine() { }
    bool initCityData(QString cityName, ErrorCode *pErrCode = nullptr);
    bool configure(QString cfgFilePath, ErrorCode *pErrCode = nullptr);
    template<class T>
    bool operate(QString opFilePath , QString opKey, T* handleObj, QStringList (T::*handleFunc)(QStringList oldData), ErrorCode *pErrCode = nullptr);
    bool save(ErrorCode *pErrCode = nullptr);
    bool separate(QStringList fileNameList, ErrorCode *pErrCode = nullptr);
};

/**
 * @brief HandleMachine::operate >> 对.idf文本进行动作配置，即配置数据来自外部输入
 * @param opFilePath
 * @param opKey
 * @param dataList
 * @return
 */
template<class T>
bool HandleMachine::operate(QString opFilePath ,QString opKey, T* handleObj, QStringList (T::*handleFunc)(QStringList oldData), ErrorCode *pErrCode)
{
    ErrorCode eCode = 0;
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

                if (handleFunc != nullptr)
                {
                    //使用handleFunc回调函数获取被操作参数新值序列
                    QStringList newDataList = (handleObj->*handleFunc)(oldDataList);
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
                    eCode = _P_ERR_OBJ_->addError("PARA_ERR", QString("Operation #%1# to %2 handle function is null.").arg(opKey, _fileName));
                    if (pErrCode != nullptr) *pErrCode = eCode;
                    return false;
                }
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

#endif // HANDLEMACHINE_H
