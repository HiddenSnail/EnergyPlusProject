#ifndef PATHMANAGER_H
#define PATHMANAGER_H
#include <QtCore>
#include <QApplication>

class PathManager {
private:
    QHash<QString, QString> _pathMap;
    PathManager() {
        QString appDir = QApplication::applicationDirPath();
        _pathMap.insert("AppDir", appDir);
        _pathMap.insert("WeatherDir", appDir + "/weather");
        _pathMap.insert("SourceDir", appDir + "/source");
        _pathMap.insert("SourceNoPeDir", appDir + "/source/no_people");
        _pathMap.insert("OutPutDir", appDir + "/output");
        _pathMap.insert("EplusDir", appDir + "/EnergyPlusV8-1-0");
        _pathMap.insert("ProfileDir", appDir + "/profiles");

        _pathMap.insert("BaseModelDir", appDir + "/model/base");
        _pathMap.insert("NrModelDir", appDir + "/model/proposed/nr");
        _pathMap.insert("RModelDir", appDir + "/model/proposed/r");
        _pathMap.insert("RpModelDir", appDir + "/model/proposed/rp");
        _pathMap.insert("ShadingDir", appDir + "/model/proposed/shading");

        _pathMap.insert("BaseOpFile", appDir + "/model/base/base_operation.json");

        _pathMap.insert("NrConfigFile", appDir + "/model/proposed/nr/nr_config.json");
        _pathMap.insert("NrOpFile", appDir + "/model/proposed/nr/nr_operation.json");

        _pathMap.insert("RConfigFile", appDir + "/model/proposed/r/r_config.json");
        _pathMap.insert("ROpFile", appDir + "/model/proposed/r/r_operation.json");

        _pathMap.insert("RpConfigFile", appDir +  "/model/proposed/rp/rp_config.json");
        _pathMap.insert("RpOpFile", appDir + "/model/proposed/rp/rp_operation.json");

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
