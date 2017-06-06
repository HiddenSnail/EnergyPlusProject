#ifndef QERROROBJECT_H
#define QERROROBJECT_H

#include "utils/error/error.h"
#include <map>
#include <iostream>
#include <assert.h>

class QErrorObject: public QObject, public ErrorHolder {
    Q_OBJECT
private:
    QErrorObject() {}
public:
    static QErrorObject* instance();
    static std::map<std::string, Error> _errorMap;
    ErrorCode addError(const std::string errorKey, const std::string detail = "");
    ErrorCode addError(const char *errorKey, const char *detail = "");
    ErrorCode addError(const QString errorKey, const QString detail = "");
signals:
    void successSig();
    void fatalSig();
    void exceptionSig();
};
extern QErrorObject *_P_ERR_OBJ_;


class QErrorHandler: public QObject {
    Q_OBJECT
private:
    QMutex _mutex;
    QErrorHandler() {}
public:
    static QErrorHandler* instance();
    void handle(const Error &error);
    void doSuccess();
    void doException();
    void doFatal();
};
extern QErrorHandler *_P_ERR_HDL_;

void installErrorHandler();

#endif // QERROROBJECT_H
