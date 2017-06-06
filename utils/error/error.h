#pragma once
#include <QtCore>
#include <string>
#include <deque>
#include <map>
typedef int ErrorCode;

class Error {
public:
    enum Level {SUCCESS = 0 , EXCEPTION, FATAL};
    static std::map<Level, std::string> _levelStr;
    ErrorCode _eCode;
    std::string _eMessage;
    Level _eLevel;
    std::string _eDetail;
    Error() {}
    ~Error() {}
    Error(int eCode, std::string eMessage, Level eLevel);
    Error(const Error &error);
    Error& operator = (const Error &error);
    Error& setDetail(std::string detail);
    std::string content() const;
};

class ErrorHolder {
protected:
    std::deque<Error> _errorQueue;
public:
    ErrorHolder() {}
    void addError(const Error &error);
    bool getError(Error &error);
    bool isEmpty();
    void clear();
};
