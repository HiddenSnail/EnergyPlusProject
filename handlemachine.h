#ifndef HANDLEMACHINE_H
#define HANDLEMACHINE_H
#define DEFAULT_KEY "__default_key__"
#define DEFAUL_VALUE "__default_value__"
#include <QRegExp>
#include <QtCore>
#include <QJsonDocument>
#include <QtDebug>
#include "utils.h"
#include "path.h"
#include "global.h"

class HandleMachine {
private:
    QVector<QString> _originContent;
    QVector<QString> _content;
    QString _sourfilePath;
    QString _sourfileName;

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
        sourceFile.open(QFile::ReadOnly);
        if (sourceFile.isOpen ()) {
            _sourfilePath = sourceFilePath;
            _sourfileName = sourceFilePath.split("/").last();
            QTextStream in(&sourceFile);
            while (!in.atEnd()) {
                QString line = in.readLine();
                _originContent.push_back(line);
            }
            _content = _originContent;
            sourceFile.close();
        } else {
            qFatal("Source File open fail");
        }
    }

    bool initCityData(QString cityName);
    bool configure(QString cfgFilePath);
    bool operate(QString opFilePath, QString opKey, QVector<QString> dataVec);
    void save();
    bool separate();
//    void startMachine(QString cityName);
    void printFile();
};

#endif // HANDLEMACHINE_H
