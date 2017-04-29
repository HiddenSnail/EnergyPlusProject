#ifndef ENERGYFORM_H
#define ENERGYFORM_H
#include <QtCore>

class EnergyForm {
public:
    QStringList _coolLoad; //冷负荷
    QStringList _heatLoad; //热负荷
    QStringList _fanWatts; //风机功率(W)

    QStringList _lightEnergy; //照明能耗(J)
    QStringList _deviceEnergy; //设备能耗(J)
    QStringList _cooMachineEnergy; //冷机能耗(J)
    QStringList _boilerFuelUse; //锅炉能耗(J)
    QStringList _cooTowerEnergy; //冷却塔能耗(J)
    QStringList _freWaterPumpEnergy; //冷冻水泵能耗(J)
    QStringList _cooWaterPumpEnergy; //冷却水泵能耗(J)
    QStringList _hotWaterPumpEnergy; //热水泵能耗(J)
    QStringList _fanEnergy; //风机能耗(J)
};

#endif // ENERGYFORM_H
