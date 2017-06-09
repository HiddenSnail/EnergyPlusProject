#ifndef PATHMANAGER_H
#define PATHMANAGER_H
#include <QtCore>
#include <QApplication>

class PathManager {
private:
    QHash<QString, QString> _pathMap;
    PathManager()
    {
        QString appDir = QApplication::applicationDirPath();
        _pathMap.insert("AppDir", appDir);
        _pathMap.insert("WeatherDir", appDir + "/weather");
        _pathMap.insert("SourceDir", appDir + "/source");
        _pathMap.insert("SourceNoPeDir", appDir + "/source/no_people");
        _pathMap.insert("OutPutDir", appDir + "/output");
        _pathMap.insert("EplusDir", appDir + "/EnergyPlusV8-1-0");
        _pathMap.insert("ProfileDir", appDir + "/profiles");
        _pathMap.insert("ConfigDir", appDir + "/setting/config");

        _pathMap.insert("OpFile", appDir + "/setting/op/operation.json");

        _pathMap.insert("EpIniFile", appDir + "/EnergyPlusV8-1-0/Energy+.ini");
        _pathMap.insert("EpRunFile", appDir + "/EnergyPlusV8-1-0/RunEPlus.bat");
        _pathMap.insert("EpIniFileBackup", appDir + "/EnergyPlusV8-1-0/Energy+.ini.backup");
        _pathMap.insert("EpRunFileBackup", appDir + "/EnergyPlusV8-1-0/RunEPlus.bat.backup");
    }
public:
    static PathManager* instance();
    QString getPath(QString pathKey);
};

#endif // PATHMANAGER_H
