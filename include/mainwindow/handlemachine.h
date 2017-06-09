#ifndef HANDLEMACHINE_H
#define HANDLEMACHINE_H
#define DEFAULT_KEY "__default_key__"
#define DEFAUL_VALUE "__default_value__"
#include "./global/stdafx.h"
#include "./utils/no_class_funcs.h"

typedef std::function<QStringList(QStringList)> OperateFunc;
typedef std::pair<QString, OperateFunc> OperatePackage;


//注：template函数要声明定义在同一个文件下
class HandleMachine: public QObject {
    Q_OBJECT
private:
    QStringList _content;
    QString _filePath;
    QString _fileName;
    QString _baseName;
    QString _curOpkey;

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
    bool configure(QString cfgFilePath, ErrorCode *pErrCode = nullptr);
    bool operate(OperatePackage opPackage, ErrorCode *pErrCode = nullptr);
    bool save(ErrorCode *pErrCode = nullptr);
    bool separate(QStringList fileNameList, ErrorCode *pErrCode = nullptr);
};


class OperateFactory {
public:
    static OperatePackage opElectricEquipment(const double roomSize, const double tvKW, const double fridgeKW, const double otherDevKW,
                                           const double tvUseNum = 1.0, const double fridgeUseNum = 1.0, const double otherDevUseNum = 1.0);
    static OperatePackage opElectricEquipment(const double retAverageW);

    static OperatePackage opLights(const double roomSize, const double lightKW, const double lightUseNum = 1.0);
    static OperatePackage opLights(const double retAverageW);

    static OperatePackage opSchComByTempOffset(const QTime startTime, const unsigned int keepHours, const unsigned int tempOffset, const bool isCool = true);
    static OperatePackage opSchComByTempOffset(const unsigned int tempOffset, const bool isCool = true);

    static OperatePackage opSchComByTemp(const unsigned int temperature, const bool isCool = true);

    static OperatePackage opTimeSpan(const unsigned int quarter);
};

#endif // HANDLEMACHINE_H
