#pragma once
#include <QtCore>
#include <string>
#include <deque>
#include <map>
typedef int ErrorCode;

//Error类
class Error {
public:
    enum Level {SUCCESS = 0 , EXCEPTION, FATAL};
    static std::map<Level, std::string> _levelStr;
    ErrorCode _eCode; //错误码
    std::string _eMessage; //错误信息
    Level _eLevel; //错误等级
    std::string _eDetail; //错误详情
    Error() {}
    ~Error() {}
    Error(int eCode, std::string eMessage, Level eLevel);
    Error(const Error &error);
    Error& operator = (const Error &error);
    Error& setDetail(std::string detail);
    std::string content() const;
};

//Error容器类
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
