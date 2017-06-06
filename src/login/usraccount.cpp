#include "./login/usraccount.h"

QString UsrAccount::createUsrId(const MacAddress &macAddress)
{
    std::string usrId = macAddress;
    for (auto &letter: usrId)
    {
       if (letter >= 65 && letter <= 70)
       {
           letter -= 17;
       }
    }
    return QString(usrId.c_str());
}

bool UsrAccount::checkPassword(const QString usrId, const QString password)
{
    std::string key = usrId.toStdString();
    key[2] += 17;
    key[3] += 49;
    key[6] += 17;
    key[7] += 49;
    key[9] += 17;
    key = key.substr(2,8);
    if (key.compare(password.toStdString()) == 0)
    {
        return true;
    } else
    {
        return false;
    }
}

int UsrAccount::rememberPassword(const QString password)
{
    if (password.size() == 8)
    {
        std::string psw = password.toStdString();
        char temp = psw[3];
        psw[3] = psw[6];
        psw[6] = temp;

        psw[0] += 19;
        psw[1] += 4;
        psw[2] += 21;
        psw[3] += 11;
        psw[4] -= 10;
        psw[5] -= 32;
        psw[6] -= 5;
        psw[7] -= 13;

        QFile pswFile("lzm.psw");
        if (pswFile.open(QFile::WriteOnly))
        {
            QTextStream outStream(&pswFile);
            outStream << QString(psw.c_str());
            pswFile.close();
            return 0;
        }
        else
        {
            return -2;
        }
    }
    else
    {
        return -1;
    }

}

int UsrAccount::getPassword(QString &password)
{
    QFile pswFile("lzm.psw");
    if (pswFile.open(QFile::ReadOnly))
    {
        QTextStream inStream(&pswFile);
        std::string psw = inStream.readLine().toStdString();
        pswFile.close();

        psw[0] -= 19;
        psw[1] -= 4;
        psw[2] -= 21;
        psw[3] -= 11;
        psw[4] += 10;
        psw[5] += 32;
        psw[6] += 5;
        psw[7] += 13;

        char temp = psw[3];
        psw[3] = psw[6];
        psw[6] = temp;

        password = QString(psw.c_str());
        return 0;
    }
    else
    {
        return -1;
    }
}

void UsrAccount::makeLiscense(const QString seed)
{
    QFile license("lzm.license");
    if (license.open(QFile::WriteOnly))
    {
        QTextStream outStream(&license);
        outStream.setCodec("UTF-8");
        QString data = seed;
        data.insert(3, 'h');
        data.insert(5, 's');
        data.insert(9, 'e');
        QString md5str = QCryptographicHash::hash(data.toUtf8(), QCryptographicHash::Md5);
        outStream << md5str;
        license.close();
    }
    else
    {
        qDebug() << "Can't make license!";
    }
}

bool UsrAccount::checkLicense(QString seed)
{
    QFile license("lzm.license");
    if (license.open(QFile::ReadOnly))
    {
        QTextStream inStream(&license);
        inStream.setCodec("UTF-8");
        QString md5Str = inStream.readLine();
        license.close();

        QString data = seed;
        data.insert(3, 'h');
        data.insert(5, 's');
        data.insert(9, 'e');
        QString md5Data = QCryptographicHash::hash(data.toUtf8(), QCryptographicHash::Md5);

        if (md5Str == md5Data)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}
