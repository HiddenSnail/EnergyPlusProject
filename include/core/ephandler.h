#ifndef EPHANDLER_H
#define EPHANDLER_H
#include "global/stdafx.h"

class EPHandler {
private:
    const QRegExp _regPath = QRegExp("(^\\s*set program_path=).*");
    const QRegExp _regIn = QRegExp("(^\\s*set input_path=).*");
    const QRegExp _regOut = QRegExp("(^\\s*set output_path=).*");
    const QRegExp _regWea = QRegExp("(^\\s*set weather_path=).*");
    QStringList _epRunBat;
    unsigned int _proPathPos;
    unsigned int _inputPos;
    unsigned int _outputPos;
    unsigned int _weatherPos;

    EPHandler();
    //配置Ep的配置文件
    ErrorCode prepareEnv();
    //获取eplus运行的批处理文件
    ErrorCode getEpRunBat();
    //验证eplus运行的批处理文件
    ErrorCode verifyEpRunBat();
public:
    static EPHandler* instance();
    ErrorCode callEplus(QString filePath, QString weatherFileName);
};

#endif // EPCALL_H
