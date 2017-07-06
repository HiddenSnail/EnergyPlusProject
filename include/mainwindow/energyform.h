 #ifndef ENERGYFORM_H
#define ENERGYFORM_H
#include <QtCore>
#include <algorithm>

class EnergyForm {
public:
    QStringList _coolLoad; //冷负荷
    QStringList _heatLoad; //热负荷
    QStringList _fanWatts; //风机功率(W)
public:
    QStringList _lightEnergy; //照明能耗(J)
    QStringList _deviceEnergy; //设备能耗(J)
    QStringList _cooMachineEnergy; //冷机能耗(J)
    QStringList _boilerFuelUse; //锅炉能耗(J)
    QStringList _cooTowerEnergy; //冷却塔能耗(J)
    QStringList _freWaterPumpEnergy; //冷冻水泵能耗(J)
    QStringList _cooWaterPumpEnergy; //冷却水泵能耗(J)
    QStringList _hotWaterPumpEnergy; //热水泵能耗(J)
    QStringList _fanEnergy; //风机能耗(J)

public:
    bool isReadyToCalEnergy() {
        if (!_coolLoad.isEmpty() && !_heatLoad.isEmpty() &&!_fanWatts.isEmpty()) {
            return true;
        } else {
            return false;
        }
    }

    int getFormDataSize() {
        if (_coolLoad.size() == _heatLoad.size() && _heatLoad.size() == _fanWatts.size()) {
            return _coolLoad.size();
        } else{
            qDebug() << "Form data have error!";
            return -1;
        }
    }

    void outToFile(QString fileName, int roomSize, int roomNumber)
    {
        QFile file(fileName);
        if (file.open(QFile::WriteOnly))
        {
            QTextStream outStream(&file);
            double lightJ = getCoreData(_lightEnergy, roomSize, roomNumber);
            outStream << QString::fromLocal8Bit("照明能耗: ") << lightJ << "\n" << endl;

            double deviceJ = getCoreData(_deviceEnergy, roomSize, roomNumber);
            outStream << QString::fromLocal8Bit("设备能耗: ") << deviceJ << "\n" << endl;

            double cooMachineJ = getCoreData(_cooMachineEnergy, roomSize, roomNumber);
            outStream << QString::fromLocal8Bit("冷机能耗: ") << cooMachineJ << "\n" << endl;

            double boilerFuelUseJ = getCoreData(_boilerFuelUse, roomSize, roomNumber);
            outStream << QString::fromLocal8Bit("锅炉能耗: ") << boilerFuelUseJ << "\n" << endl;

            double cooTowerJ = getCoreData(_cooTowerEnergy, roomSize, roomNumber);
            outStream << QString::fromLocal8Bit("冷却塔能耗: ") << cooTowerJ  << "\n" << endl;

            double freWaterPumpJ = getCoreData(_freWaterPumpEnergy, roomSize, roomNumber);
            outStream << QString::fromLocal8Bit("冷冻水泵能耗: ") << freWaterPumpJ << "\n" << endl;

            double cooWaterPumpJ = getCoreData(_cooWaterPumpEnergy, roomSize, roomNumber);
            outStream << QString::fromLocal8Bit("冷却水泵能耗: ") << cooWaterPumpJ << "\n" << endl;

            double hotWaterPumpJ = getCoreData(_hotWaterPumpEnergy, roomSize, roomNumber);
            outStream << QString::fromLocal8Bit("热水泵能耗: ") << hotWaterPumpJ << "\n" << endl;

            double fanJ = getCoreData(_fanEnergy, roomSize, roomNumber);
            outStream << QString::fromLocal8Bit("风机能耗: ") << fanJ << "\n" << endl;

            double sum = lightJ + deviceJ + cooMachineJ + boilerFuelUseJ + cooTowerJ
                    + freWaterPumpJ + cooWaterPumpJ + hotWaterPumpJ + fanJ;
            outStream << QString::fromLocal8Bit("总能耗: ") << sum << "\n" << endl;
            file.close();
        } else {
            qDebug() << "Can't create EnergyForm file!";
        }
    }

    double getCoreData(QStringList dataList, int roomSize, int roomNumber)
    {
        double sum = 0;
        std::for_each(dataList.begin(), dataList.end(), [&sum](QString data){
            sum += data.toDouble();
        });
        double average = sum/(1000.0*3600*roomSize*roomNumber);
        return average;
    }
};

#endif // ENERGYFORM_H
