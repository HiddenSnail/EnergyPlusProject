#include "qerrorobject.h"
//---Class QErrorObject---//

QErrorObject *_P_ERR_OBJ_ = QErrorObject::instance();

std::map<std::string, Error> QErrorObject::_errorMap
{
    {"SUCCESS", Error(0, "Success", Error::SUCCESS)},
    {"OPERANDS_ERR", Error(10 ,"Operands type conflict", Error::EXCEPTION)},
    {"OVERFLOW", Error(11, "Container overflow", Error::EXCEPTION)},
    {"UNEXP_ERR", Error(-1, "Unexcpected error happen", Error::FATAL)},
    {"FILE_OPEN_FAIL", Error(101, "File open fail", Error::FATAL)},
    {"FILE_MISS", Error(102, "File missing", Error::FATAL)},
    {"FILE_BROKEN", Error(103, "File broken", Error::FATAL)},
    {"FILE_IO_ERR", Error(104, "File I/O operate fail", Error::FATAL)},
    {"PER_DENIED", Error(120, "Permission denied", Error::FATAL)},
    {"MEM_ERR", Error(110, "Memery alloc error", Error::FATAL)},
    {"EXC_ERR", Error(130, "Program can't excuted", Error::FATAL)},
    {"EXC_TIMEOUT", Error(131, "Program excuted timeout", Error::EXCEPTION)},
    {"START_FAIL", Error(132, "Program start fail", Error::FATAL)},
    {"PARA_ERR", Error(140, "Parameter error", Error::EXCEPTION)},
    {"SYS_FUNC_ERR", Error(141, "System function error", Error::FATAL)}
};


QErrorObject* QErrorObject::instance()
{
    static QErrorObject qErrorObject;
    return &qErrorObject;
}

ErrorCode QErrorObject::addError(const std::string errorKey, const std::string detail)
{
    Error error = QErrorObject::_errorMap[errorKey].setDetail(detail);
    ErrorHolder::addError(error);
    switch (error._eLevel) {
    case Error::SUCCESS:
    {
        emit successSig();
        break;
    }
    case Error::EXCEPTION:
    {
        emit exceptionSig();
        break;
    }
    case Error::FATAL:
    {
        emit fatalSig();
        break;
    }
    default:
        break;
    }
    return error._eCode;
}

ErrorCode QErrorObject::addError(const char *errorKey, const char *detail)
{
    return addError(std::string(errorKey), std::string(detail));
}

ErrorCode QErrorObject::addError(const QString errorKey, const QString detail)
{
    return addError(errorKey.toStdString(), detail.toStdString());
}

//---Class QErrorHandler---//
QErrorHandler *_P_ERR_HDL_ = QErrorHandler::instance();

QErrorHandler* QErrorHandler::instance()
{
    static QErrorHandler errorHandler;
    return &errorHandler;
}


void QErrorHandler::handle(const Error &error)
{
    std::cout << error.content() << std::endl;
}

void QErrorHandler::doSuccess()
{
    assert(_P_ERR_OBJ_->isEmpty() != true);
    _mutex.lock();
    Error e;
    _P_ERR_OBJ_->getError(e);
    handle(e);
    QFile errorFile("error.log");
    if (errorFile.open(QFile::WriteOnly))
    {
        QTextStream outStream(&errorFile);
        outStream.setCodec("UTF-8");
        outStream << QString::fromStdString(e.content()) << "\n";
        errorFile.close();
    }
    _mutex.unlock();
}

void QErrorHandler::doException()
{
    assert(_P_ERR_OBJ_->isEmpty() != true);
    _mutex.lock();
    Error e;
    _P_ERR_OBJ_->getError(e);
    handle(e);
    QFile errorFile("error.log");
    if (errorFile.open(QFile::WriteOnly))
    {
        QTextStream outStream(&errorFile);
        outStream.setCodec("UTF-8");
        outStream << QString::fromStdString(e.content()) << "\n";
        errorFile.close();
    }
    _mutex.unlock();
}

void QErrorHandler::doFatal()
{
    assert(_P_ERR_OBJ_->isEmpty() != true);
    _mutex.lock();
    Error e;
    _P_ERR_OBJ_->getError(e);
    handle(e);
    QFile errorFile("error.log");
    if (errorFile.open(QFile::WriteOnly))
    {
        QTextStream outStream(&errorFile);
        outStream.setCodec("UTF-8");
        outStream << QString::fromStdString(e.content()) << "\n";
        errorFile.close();
    }
    _mutex.unlock();
    exit(e._eCode);
}

//---Install function---//
void installErrorHandler()
{
    QObject::connect(_P_ERR_OBJ_, &QErrorObject::successSig, _P_ERR_HDL_, &QErrorHandler::doSuccess);
    QObject::connect(_P_ERR_OBJ_, &QErrorObject::exceptionSig, _P_ERR_HDL_, &QErrorHandler::doException);
    QObject::connect(_P_ERR_OBJ_, &QErrorObject::fatalSig, _P_ERR_HDL_, &QErrorHandler::doFatal);
}
