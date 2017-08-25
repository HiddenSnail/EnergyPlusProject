#pragma once
#include "./global/stdafx.h"
#include "column.h"
#include "./utils/csvreader/csvreader.h"


//初始数据表单，从csv文件中直接读取
struct RawDataSheet {
    static const int IgnoreLines = 49;
    enum TimeSpan {year = 0, quarter1, quarter2, quarter3, quarter4};
    enum ID {eHeatCol = 0, wHeatCol, sHeatCol, nHeatCol, dsHeatCol,
                  eCoolCol, wCoolCol, sCoolCol, nCoolCol, dsCoolCol,
                  eFanCol, wFanCol, sFanCol, nFanCol};
private:
    QMap<ID, Column<double>> _memMap = {
        {eHeatCol, Column<double>()},
        {wHeatCol, Column<double>()},
        {sHeatCol, Column<double>()},
        {nHeatCol, Column<double>()},
        {dsHeatCol, Column<double>()},
        {eCoolCol, Column<double>()},
        {wCoolCol, Column<double>()},
        {sCoolCol, Column<double>()},
        {nCoolCol, Column<double>()},
        {dsCoolCol, Column<double>()},
        {eFanCol, Column<double>()},
        {wFanCol, Column<double>()},
        {sFanCol, Column<double>()},
        {nFanCol, Column<double>()}
    };
    Column<double> convertToCol(const QStringList &strList);
    QStringList getSubSequence(const QStringList &list, const int begin, const int end);
public:
    RawDataSheet() {}
    RawDataSheet(const RawDataSheet &sheet);
    RawDataSheet(const QString &csvFilePath, const TimeSpan timeSpan = year);
    void setData(QString csvFilePath, TimeSpan timeSpan = year);
    bool isComplete() const;
    RawDataSheet& operator = (const RawDataSheet &rhs);
    Column<double>& operator [] (const ID id);
};


//冷热负荷与风机能耗表单
struct HCFSheet {
    friend HCFSheet operator + (const HCFSheet &lhs, const HCFSheet &rhs);
public:
    Column<double> _heatLoad;
    Column<double> _coolLoad;
    Column<double> _fansWatts;
    HCFSheet() {}
    HCFSheet(const HCFSheet &sheet);
    bool isComplete();
    HCFSheet& operator = (const HCFSheet &rhs);
    HCFSheet& operator += (const HCFSheet &rhs);
};

//能耗表单
struct EnergySheet {
    friend EnergySheet operator + (const EnergySheet &lhs, const EnergySheet &rhs);
public:
    enum ID {Light = 0, Device, CooMachine, BoilerFuelUse, CooTower,
            FreWaterPump, CooWaterPump, HotWaterPump, Fan};
    static QMap<QString, ID> _idMap;
    QMap<ID, Column<double>> _memMap =
    {
        {Light, Column<double>()}, //照明能耗(J)
        {Device, Column<double>()}, //设备能耗(J)
        {CooMachine, Column<double>()},//冷机能耗(J)
        {BoilerFuelUse, Column<double>()},//锅炉能耗(J)
        {CooTower, Column<double>()},//冷却塔能耗(J)
        {FreWaterPump, Column<double>()},//冷冻水泵能耗(J)
        {CooWaterPump, Column<double>()},//冷却水泵能耗(J)
        {HotWaterPump, Column<double>()},//热水泵能耗(J)
        {Fan, Column<double>()} //风机能耗(J)
    };
    EnergySheet();
    EnergySheet(const EnergySheet &sheet);
    bool isComplete() const;
    double sum() const;
    double sum(int begin, int length) const;
    EnergySheet& operator = (const EnergySheet &rhs);
    EnergySheet& operator += (const EnergySheet &rhs);
    Column<double>& operator [] (const ID id);
    Column<double>& operator [] (const QString id);
    void saveToFile(const QString filePath);
};
