#pragma once
#include "./global/stdafx.h"
#include "sheet.h"
#include "roomstate.h"
//RawDataSheet表单处理类
class RawDataSheetProcessor {
private:
    RawDataSheet _dataSheet;
    RoomState _roomState;
public:
    RawDataSheetProcessor() {}
    RawDataSheetProcessor(const RawDataSheet &dataSheet, const RoomState &roomState);
    void setData(const RawDataSheet &dataSheet);
    void setRoomState(const RoomState &roomState);
    void fixFanWatts();
    bool isReady();
    HCFSheet produceHCFSheet(const RoomState::RoomType roomType);
};

//HCFSheet表单处理类
class HCFSheetProcessor {
private:
    static QMap<int, double> _coolLoadReducePercentMap; //空调制冷下限温度与冷负荷百分比缩减关系
    static QMap<int, double> _heatLoadReducePercentMap; //空调供热上限温度与热负荷百分比缩减关系
    static Column<double> _deviceRatioCol; //24小时的设备使用系数
    static Column<double> _lightRatioCol; //24小时的照明使用系数
    static QVector<QPair<double, double>> _screwWattsAndCOPVec; //螺杆机的制冷量(kW)和COP数组
    static bool _isPreCoreData;
    static void initLoadReduceMap();
    static void initRatioVec();
    static void initScrewAndCOPVec();
    /**
     * @todo: 针对传参方式重构函数：不采用传值方式，使用const限定不变参数
     **/
    static EnergySheet mainProcess(HCFSheet &hcfSheet, RoomState &roomState,
                                   const RoomState::RoomType type, const bool isInheritCoreData);
    HCFSheet _hcfSheet;
    RoomState _roomState;
public:
    static void load();
    HCFSheetProcessor() {}
    HCFSheetProcessor(const HCFSheet &hcfSheet, const RoomState &roomState);
    //设置成员变量
    void setData(const HCFSheet &hcfSheet, const RoomState &RoomState);
    //是否可以开始计算
    bool isReady();
    //新风模式配置
    void newWindConfig(const int roomSize, const int hours);
    //两管制配置
    void pip2Config();
    //空调面板温度负荷修正配置
    void ACConfig(const int coolTemp, const int heatTemp);
    //计算结果生成EnergySheet表单
    EnergySheet produceEnergySheet(const RoomState::RoomType roomType);
};
