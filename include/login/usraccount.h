#ifndef USRACCOUNT_H
#define USRACCOUNT_H
#include <QFile>
#include <QtCore>
#include "./utils/mcinfo/mcinfo.h"

class UsrAccount
{
private:
    UsrAccount() {}
public:
    static QString createUsrId(const MacAddress &macAddress);
    static bool checkPassword(const QString userId, const QString password);
    static int rememberPassword(const QString password);
    static int getPassword(QString &password);
    static void makeLiscense(const QString seed);
    static bool checkLicense(const QString seed);
};

#endif // USRACCOUNT_H
