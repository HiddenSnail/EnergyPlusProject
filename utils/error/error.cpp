#include "error.h"

//----------------------------------Class Error-------------------------------------

std::map<Error::Level, std::string> Error::_levelStr =
{
    {Error::SUCCESS, "Success"},
    {Error::EXCEPTION, "Exception"},
    {Error::FATAL, "Fatal"}
};

Error::Error(int eCode, std::string eMessage, Level eLevel)
{
    this->_eCode = eCode;
    this->_eMessage = eMessage;
    this->_eLevel = eLevel;
}

Error::Error(const Error &error)
{
    _eCode = error._eCode;
    _eLevel = error._eLevel;
    _eMessage = error._eMessage;
    _eDetail = error._eDetail;
}

Error& Error::operator=(const Error &error)
{
    if (this != &error)
    {
        this->_eCode = error._eCode;
        this->_eLevel = error._eLevel;
        this->_eMessage = error._eMessage;
        this->_eDetail = error._eDetail;
    }
    return *this;
}

Error& Error::setDetail(std::string detail)
{
    this->_eDetail = detail;
    return *this;
}

std::string Error::content() const
{
    std::string info = "[" + _levelStr[_eLevel] + " " + std::to_string(_eCode) + "] " + _eMessage;
    if (!_eDetail.empty())
    {
        info += ": " + _eDetail;
    }
    return info;
}

//----------------------------------Class ErrorHolder---------------------------------//

void ErrorHolder::addError(const Error &error)
{
    _errorQueue.push_back(error);
}

bool ErrorHolder::getError(Error &error)
{
    if (!isEmpty())
    {
        error = _errorQueue.front();
        _errorQueue.pop_front();
        return true;
    }
    else
    {
        return false;
    }
}

bool ErrorHolder::isEmpty()
{
    return _errorQueue.empty();
}

void ErrorHolder::clear()
{
    _errorQueue.clear();
}
