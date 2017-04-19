#ifndef HANDLEMACHINE_H
#define HANDLEMACHINE_H
#define DEFAULT_KEY "__default_key__"
#define DEFAUL_VALUE "__default_value__"
#include <QRegExp>
#include <QtCore>
#include <QJsonDocument>
#include <QtDebug>
#include "pathmanager.h"
#include "utils.h"
#include "global.h"

class HandleMachine: public QObject {
    Q_OBJECT
private:
    static QMutex _lock;
    QVector<QString> _originContent;
    QVector<QString> _content;
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
        sourceFile.open(QFile::ReadOnly);
        if (sourceFile.isOpen ()) {
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
            qInfo() << "Instance success!";
        } else {
            qFatal("Source File open fail!");
        }
    }

    ~HandleMachine() { qInfo() << "Aready delete"; }

public slots:
    bool initCityData(QString cityName);
    bool configure(QString cfgFilePath);
    bool operate(QString opFilePath, QString opKey, QVector<QString> dataVec);
    bool save();
    bool separate();
    void startMachine(QString weatherFileName);

signals:
    void finishExec();
};

#endif // HANDLEMACHINE_H
