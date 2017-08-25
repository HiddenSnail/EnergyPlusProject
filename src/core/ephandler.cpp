#include "./core/ephandler.h"

ErrorCode EPHandler::prepareEnv()
{
    //修改Eplus的配置文件(.ini)
    QFile epIniFile(PathManager::instance()->getPath("EpIniFile"));
    if (!epIniFile.exists())
    {
        return _P_ERR_OBJ_->addError("FILE_MISS", "Can't find Energy+.ini.");
    }

    //检查有没有对Energy+.ini的备份文件，若没有则在当前目录备份Energy+.ini.backup
    //其目的是避免该关键文件丢失
    QFileInfo epIniFileBackupInfo(PathManager::instance()->getPath("EpIniFileBackup"));
    if (!epIniFileBackupInfo.exists())
    {
        epIniFile.copy(PathManager::instance()->getPath("EpIniFileBackup"));
    }

    QFile epIniFileBackup(PathManager::instance()->getPath("EpIniFileBackup"));
    QFile newEpIniFile("new.ini");

    if (epIniFileBackup.open(QFile::ReadOnly) && newEpIniFile.open(QFile::WriteOnly))
    {
        QTextStream stream1(&epIniFileBackup);
        QTextStream stream2(&newEpIniFile);
        int flag = 0;  //0:未找到目标行, 1:已找到目标行, 2:已修改目标行
        while (!stream1.atEnd())
        {
            QString line = stream1.readLine();
            switch (flag) {
            case 0:
            {
                QRegExp reg("\\[program\\]");
                if (reg.exactMatch(line))
                {
                    flag = 1;
                }
                break;
            }
            case 1:
            {
                QDir epDir(PathManager::instance()->getPath("EplusDir"));
                line = QString("dir=%1\\").arg(epDir.path().replace("/", "\\"));
                flag = 2;
                break;
            }
            default:
                break;
            }
            stream2 << line << endl;
        }
        epIniFileBackup.close();
        newEpIniFile.close();

        //若Energy+.ini删除成功或者不存在
        if (epIniFile.remove() || !epIniFile.exists())
        {
            newEpIniFile.rename(PathManager::instance()->getPath("EpIniFile"));
            return _P_ERR_OBJ_->addError("SUCCESS", "Energy+.ini configure success.");
        }
        else
        {
            return _P_ERR_OBJ_->addError("PER_DENIED", "Can't remove original Energy+.ini.");
        }
    }
    else
    {
        return _P_ERR_OBJ_->addError("FILE_OPEN_FAIL", "Energy+.ini.backup can't open!");
    }

}

ErrorCode EPHandler::getEpRunBat()
{
    QFile epRunFile(PathManager::instance()->getPath("EpRunFile"));
    if (!epRunFile.exists())
    {
        return _P_ERR_OBJ_->addError("FILE_MISS", "Can't find RunEPlus.bat.");
    }

    if (epRunFile.open(QFile::ReadOnly))
    {
        QTextStream in(&epRunFile);
        while (!in.atEnd())
        {
            _epRunBat << in.readLine();
        }
        epRunFile.close();
        return _P_ERR_OBJ_->addError("SUCCESS", "RunEPlus.bat read success.");
    }
    else
    {
        return _P_ERR_OBJ_->addError("FILE_OPEN_FAIL", "RunEPlus.bat can't open.");
    }
}

ErrorCode EPHandler::verifyEpRunBat()
{
    //0:未找到_proPathPos
    //1:未找到_inputPos
    //2:未找到_outputPos
    //3: 未找到_weatherPos
    int flag = 0;
    for (int index = 0; index < _epRunBat.size(); index++)
    {
        switch (flag) {
        case 0:
        {
            //program_path所在行定位
            if (_regPath.exactMatch(_epRunBat[index]))
            {
                _proPathPos = index;
                flag = 1;
            }
            break;
        }
        case 1:
        {
            //input_path所在行定位
            if (_regIn.exactMatch(_epRunBat[index]))
            {
                _inputPos = index;
                flag = 2;
            }
            break;
        }
        case 2:
        {
            //output_path所在行定位
            if (_regOut.exactMatch(_epRunBat[index]))
            {
                _outputPos = index;
                flag = 3;
            }
            break;
        }
        case 3:
        {
            //weather_path所在行定位
            if (_regWea.exactMatch(_epRunBat[index]))
            {
                _weatherPos = index;
                flag = 4;
            }
        }
        default:
            break;
        }
        if (flag >= 4) break;
    }

    if (flag < 4)
    {
        return _P_ERR_OBJ_->addError("FILE_BROKEN", "RunEPlus.bat is broken.");
    }
    else
    {
        return _P_ERR_OBJ_->addError("SUCCESS", "RunEPlus.bat is good.");
    }
}

EPHandler::EPHandler()
{
    try
    {
        ErrorCode eCode = -1;
        eCode = prepareEnv();
        if (eCode != 0) throw eCode;
        eCode = getEpRunBat();
        if (eCode != 0) throw eCode;
        eCode = verifyEpRunBat();
        if (eCode != 0) throw eCode;
    } catch(ErrorCode code)
    {
        std::cout << "Instance EPHandler fail." << std::endl;
        exit(code);
    }
}

EPHandler* EPHandler::instance()
{
    static EPHandler epHandler;
    return &epHandler;
}

ErrorCode EPHandler::callEplus(QString filePath, QString weatherFileName)
{
    ErrorCode eCode = 0;
    QFileInfo info(filePath);
    if (!info.exists())
    {
        eCode = _P_ERR_OBJ_->addError("FILE_MISS", "In.idf can't find.");
        return eCode;
    }

    QStringList realRunBat = _epRunBat;
    realRunBat[_proPathPos].replace(_regPath, QString("\\1%1\\").arg(PathManager::instance()->getPath("EplusDir")).replace("/", "\\"));
    realRunBat[_inputPos].replace(_regIn, QString("\\1%1\\").arg(info.dir().path()).replace("/", "\\"));
    realRunBat[_outputPos].replace(_regOut, QString("\\1%1\\").arg(info.dir().path()).replace("/", "\\"));
    realRunBat[_weatherPos].replace(_regWea, QString("\\1%1\\").arg(PathManager::instance()->getPath("WeatherDir")).replace("/", "\\"));

    QString workDir = info.dir().absolutePath();
    QString realRunBatPath = workDir + "/Run" + info.baseName() + ".bat";

    //在该函数为线程安全函数的前提下,batFile的名称必须是唯一的
    QFile batFile(realRunBatPath);
    if (batFile.open(QFile::WriteOnly))
    {
        QTextStream out(&batFile);
        for (auto line: realRunBat)
        {
            out << line << endl;
        }
        batFile.close();
    }
    else
    {
        eCode = _P_ERR_OBJ_->addError("FILE_OPEN_FAIL", "Real RunEPlus.bat can't create.");
        return eCode;
    }

    QProcess *p_proc = new QProcess();
    if (p_proc != NULL)
    {
        //设置脚本执行环境,而并非当前程序运行环境
        p_proc->setWorkingDirectory(workDir);

        //设置脚本参数
        QStringList args;
        args << info.baseName() << weatherFileName;

        //启动脚本
        p_proc->start(realRunBatPath, args);
        std::cout << info.fileName().toStdString() << ": EnergyPlus.exe start." << std::endl;

        if (p_proc->waitForStarted())
        {
            if(p_proc->waitForFinished(30*60000))
            {
                if (0 == p_proc->exitCode())
                {
                    eCode = _P_ERR_OBJ_->addError("SUCCESS", "EnergyPlus.exe execute finish.");
                }
                else
                {
                    eCode = _P_ERR_OBJ_->addError("EXC_ERR", "EnergyPlus.exe execute error.");
                }
            }
            else
            {
                eCode = _P_ERR_OBJ_->addError("EXC_TIMEOUT", "EnergyPlus.exe execute timeout.");
            }
        }
        else
        {
            eCode = _P_ERR_OBJ_->addError("START_FAIL", "EnergyPlus.exe start fail.");
        }
        delete p_proc;
        p_proc = NULL;
    }
    else
    {
        eCode = _P_ERR_OBJ_->addError("MEM_ERR", "Can't allocate memory for EnergyPlus.exe.");
    }

    batFile.remove();
    return eCode;
}
