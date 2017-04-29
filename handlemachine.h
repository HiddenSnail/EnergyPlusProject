#ifndef HANDLEMACHINE_H
#define HANDLEMACHINE_H
#define DEFAULT_KEY "__default_key__"
#define DEFAUL_VALUE "__default_value__"
#include <QRegExp>
#include <QtCore>
#include <QJsonDocument>
#include "pathmanager.h"
#include "global.h"

//注：template函数要声明定义在同一个文件下
class HandleMachine: public QObject {
    Q_OBJECT
private:
    static QMutex _startLock;
    QStringList _originContent;
    QStringList _content;
    QString _fileDir;
    QString _filePath;
    QString _fileName;
    QString _baseName;
    QString _fileSuffix;

    bool isTheKeyLine(QString key, QString line);
    bool isTheEndLine(QString line);

    int getReplaceLocation(QString locationKey, QString confirmKey);
    int getInsertLocation(QString locationKey, QString confirmKey);

    bool replacePartStruct(QJsonObject root);
    bool replaceAllStruct(QJsonObject root);
    bool insertStruct(QJsonObject root);

public:
    HandleMachine(QString sourceFilePath) {
        QFile sourceFile(sourceFilePath);
        if (sourceFile.open(QFile::ReadOnly)) {
            QFileInfo info(sourceFilePath);
            _filePath = info.filePath();
            _fileName = info.fileName();
            _baseName = info.baseName();
            _fileSuffix = info.suffix();
            _fileDir = info.dir().path();
            QTextStream in(&sourceFile);
            while (!in.atEnd()) {
                QString line = in.readLine();
                _originContent.push_back(line);
            }
            _content = _originContent;
            sourceFile.close();
            qInfo() << QString("Instance success! [%1]").arg(_fileName);
        } else {
            qFatal("Source file open fail!");
        }
    }

    ~HandleMachine() { qInfo() << QString("Aready delete! [%1]").arg(_fileName); }

public:
    template<class T>
    void print(T t) { qDebug() << t; }

    bool initCityData(QString cityName);

    bool configure(QString cfgFilePath);

    template<class T>
    bool operate(QString opFilePath , QString opKey, T* handleObj, QStringList (T::*handleFunc)(QStringList oldData));

    bool save();
    void separate(QStringList fileNameList);
    void startMachine(QString weatherFileName);
signals:
    void finishExec();
    void finishSep();
};

/**
 * @brief HandleMachine::operate >> 对.idf文本进行动作配置，即配置数据来自外部输入
 * @param opFilePath
 * @param opKey
 * @param dataList
 * @return
 */
template<class T>
bool HandleMachine::operate(QString opFilePath ,QString opKey, T* handleObj, QStringList (T::*handleFunc)(QStringList oldData))
{
    qInfo() << QString("Start operation(%1)! [%2]").arg(opKey, _fileName);
    QFile opFile(opFilePath);
    if (!opFile.open(QFile::ReadOnly)) {
        qFatal("Operation file open fail! FilePath: %s", opFilePath.toStdString().c_str());
    }
    QTextStream inStream(&opFile);
    QJsonDocument doc = QJsonDocument::fromJson(inStream.readAll().toLatin1());
    opFile.close();

    if (!doc.isObject() || doc.isNull()) {
        qFatal("The Operation file maybe broken! FilePath: %s", opFilePath.toStdString().c_str());
    } else {
        QJsonObject opObject = doc.object()[opKey].toObject();
        if (!opObject.isEmpty()) {
            QString locationKey = opObject["locationKey"].toString();
            QString confirmKey = opObject["confirmKey"].toString();
            Q_ASSERT_X(opObject["offsets"].isArray(), "json try to array", "this json value isn't array");
            QJsonArray offsets = opObject["offsets"].toArray();

            int beginLoc = getReplaceLocation(locationKey, confirmKey);
            if (beginLoc > -1) {
                QRegularExpression reg("(^\\s*)(.*)(,|;)(.*)");
                QString fmt, prefix("\\1"), suffix("\\3\\4");
                QStringList oldDataList; //被操参数的旧值序列
                QVector<int> posVec; //操作参数的具体位置序列
                for (int index = 0; index < offsets.size(); index++) {
                    int targetPos = beginLoc + offsets[index].toInt();
                    posVec.push_back(targetPos);
                    QRegularExpressionMatch match;
                    if (_content[targetPos].contains(reg, &match)) {
                        oldDataList << match.captured(2);
                    } else {
                        qFatal("The idf file maybe broken! [%s]", _fileName.toStdString().c_str());
                    }
                }
                if (NULL == handleFunc) {  qFatal("No handle function! [%s]", _fileName.toStdString().c_str()); }
                //使用handleFunc回调函数获取被操作参数新值序列
                QStringList newDataList = (handleObj->*handleFunc)(oldDataList);
                for (int index = 0; index < posVec.size(); index++) {
                    QString newData = newDataList[index];
                    fmt = prefix + newData + suffix;
                    _content[posVec[index]].replace(reg, fmt);
                }
                qInfo() << QString("Operation(%1) success! [%2]").arg(opKey, _fileName);
                return true;
            }
        }
        qDebug()<< QString("Operation(%1) fail! [%2]").arg(opKey, _fileName);
        return false;
    }
}

#endif // HANDLEMACHINE_H
