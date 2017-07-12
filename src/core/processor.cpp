#include "./core/processor.h"


//----------------------------------Class RawDataSheetProcessor---------------------------------


RawDataSheetProcessor::RawDataSheetProcessor(const RawDataSheet &dataSheet ,const RoomState &roomState)
    :_dataSheet(dataSheet), _roomState(roomState)
{

}

void RawDataSheetProcessor::setData(const RawDataSheet &dataSheet)
{
    _dataSheet = dataSheet;
}

void RawDataSheetProcessor::setRoomState(const RoomState &roomState)
{
    _roomState = roomState;
}

bool RawDataSheetProcessor::isReady()
{
    return (_dataSheet.isComplete() && _roomState.isCanAnalyze()) ? true : false;
}

void RawDataSheetProcessor::fixFanWatts()
{
    if (isReady())
    {
        std::function<void(Column<double>&)> fix = [](Column<double> &fanCol)
        {
            const double maxWatts = *std::max_element(fanCol.data().begin(), fanCol.data().end());
            QVector<double> realDataVec = {maxWatts*0.1, maxWatts*0.45, maxWatts};
            for (auto &data: fanCol.data())
            {
                for (auto realData: realDataVec)
                {
                    if (data < realData)
                    {
                        data = realData;
                        break;
                    }
                }
                data = data > realDataVec.back() ? realDataVec.back() : data;
            }
        };

        fix(_dataSheet[RawDataSheet::eFanCol]);
        fix(_dataSheet[RawDataSheet::wFanCol]);
        fix(_dataSheet[RawDataSheet::sFanCol]);
        fix(_dataSheet[RawDataSheet::nFanCol]);
    }
}

HCFSheet RawDataSheetProcessor::produceHCFSheet(RoomState::RoomType roomType)
{
    HCFSheet sheet;
    if (isReady())
    {
        double eRate = (double)_roomState.getEastRoomsNumber()/_roomState.getTotalRoomsNumber();
        double wRate = (double)_roomState.getWestRoomsNumber()/_roomState.getTotalRoomsNumber();
        double sRate = (double)_roomState.getSouthRoomsNumber()/_roomState.getTotalRoomsNumber();
        double nRate = (double)_roomState.getNorthRoomsNumber()/_roomState.getTotalRoomsNumber();
        double dsRate = 0.25;
        Column<int> roomsCol = _roomState.getRoomsVec(roomType);
        sheet._heatLoad = _dataSheet[RawDataSheet::eHeatCol]*roomsCol*eRate +
                _dataSheet[RawDataSheet::wHeatCol]*roomsCol*wRate +
                _dataSheet[RawDataSheet::sHeatCol]*roomsCol*sRate +
                _dataSheet[RawDataSheet::nHeatCol]*roomsCol*nRate +
                _dataSheet[RawDataSheet::dsHeatCol]*roomsCol*dsRate;
        sheet._coolLoad = _dataSheet[RawDataSheet::eCoolCol]*roomsCol*eRate +
                _dataSheet[RawDataSheet::wCoolCol]*roomsCol*wRate +
                _dataSheet[RawDataSheet::sCoolCol]*roomsCol*sRate +
                _dataSheet[RawDataSheet::nCoolCol]*roomsCol*nRate +
                _dataSheet[RawDataSheet::dsCoolCol]*roomsCol*dsRate;
        sheet._fansWatts = _dataSheet[RawDataSheet::eFanCol]*roomsCol*eRate +
                _dataSheet[RawDataSheet::wFanCol]*roomsCol*wRate +
                _dataSheet[RawDataSheet::sFanCol]*roomsCol*sRate +
                _dataSheet[RawDataSheet::nFanCol]*roomsCol*nRate;
    }
    return sheet;
}

//-------------------------------------Class HCFSheetProcessor-----------------------------------------

QMap<int, double> HCFSheetProcessor::_coolLoadReducePercentMap; //空调制冷下限温度与冷负荷百分比缩减关系
QMap<int, double> HCFSheetProcessor::_heatLoadReducePercentMap; //空调供热上限温度与热负荷百分比缩减关系
Column<double> HCFSheetProcessor::_deviceRatioCol; //24小时的设备使用系数
Column<double> HCFSheetProcessor::_lightRatioCol; //24小时的照明使用系数
QVector<QPair<double, double>> HCFSheetProcessor::_screwWattsAndCOPVec; //螺杆机的制冷量(kW)和COP数组
bool HCFSheetProcessor::_isPreCoreData = false;

void HCFSheetProcessor::initLoadReduceMap()
{
    //获取冷热负荷缩减百分比关系
    QFile loadReduceProfile(PathManager::instance()->getPath("ProfileDir") + "/load/loadReducePercent_profile.json");
    if (!loadReduceProfile.open(QFile::ReadOnly))
    {
        _P_ERR_OBJ_->addError("FILE_OPEN_FAIL" ,"Can't read the loadReducePercent profile.");
    }
    //lrp == loadReduceProfile的缩写
    QTextStream lrpStream(&loadReduceProfile);
    lrpStream.setCodec("UTF-8");
    QJsonDocument lrpDoc = QJsonDocument::fromJson(lrpStream.readAll().toUtf8());
    loadReduceProfile.close();

    if (!lrpDoc.isObject() || lrpDoc.isEmpty())
    {
        _P_ERR_OBJ_->addError("FILE_BROKEN", "The loadReducePercent profile file broken.");
    }
    QJsonObject lrpRoot = lrpDoc.object();
    int coolTempTop = lrpRoot["coolTempTop"].toInt();
    int coolTempBottom = lrpRoot["coolTempBottom"].toInt();
    QJsonArray coolLoadReducePercentArray = lrpRoot["coolReducePercent"].toArray();
    for (int temp = coolTempBottom; temp <= coolTempTop; temp++)
    {
        double percent = coolLoadReducePercentArray[temp - coolTempBottom].toDouble();
        HCFSheetProcessor::_coolLoadReducePercentMap.insert(temp, percent);
    }
    int heatTempTop = lrpRoot["heatTempTop"].toInt();
    int heatTempBottom = lrpRoot["heatTempBottom"].toInt();
    QJsonArray heatLoadReducePercentArray = lrpRoot["heatReducePercent"].toArray();
    for (int temp = heatTempBottom; temp <= heatTempTop; temp++)
    {
        double percent = heatLoadReducePercentArray[temp - heatTempBottom].toDouble();
        HCFSheetProcessor::_heatLoadReducePercentMap.insert(temp, percent);
    }
}

void HCFSheetProcessor::initRatioVec()
{
    std::function<void(QString filePath, Column<double>&)> getData = [](QString filePath, Column<double> &col)
    {
        QFile ratioProfile(filePath);
        if (!ratioProfile.open(QFile::ReadOnly))
        {
            _P_ERR_OBJ_->addError("FILE_OPEN_FAIL" ,"Can't read the ratio profile.");
        }
        QTextStream stream(&ratioProfile);
        stream.setCodec("UTF-8");
        QJsonDocument doc = QJsonDocument::fromJson(stream.readAll().toUtf8());
        ratioProfile.close();
        if (!doc.isObject() || doc.isNull())
        {
            _P_ERR_OBJ_->addError("FILE_BROKEN", "The ratio profile file broken.");
        }
        QJsonObject root = doc.object();
        for (int i = 0; i < root.size(); i++)
        {
            col.push_back(root[QString::number(i+1)].toDouble());
        }
    };

    //24小时的设备使用系数
    QString deviceRatioProfilePath(PathManager::instance()->getPath("ProfileDir") + "/ratio/deviceRatio_profile.json");
    getData(deviceRatioProfilePath, _deviceRatioCol);

    //24小时的照明使用系数
    QString lightRatioProfilePath(PathManager::instance()->getPath("ProfileDir") + "/ratio/lightRatio_profile.json");
    getData(lightRatioProfilePath, _lightRatioCol);
}

void HCFSheetProcessor::initScrewAndCOPVec()
{
    QFile screwProfile(PathManager::instance()->getPath("ProfileDir") + "/screw_profile.json");
    if (!screwProfile.open(QFile::ReadOnly))
    {
        _P_ERR_OBJ_->addError("FILE_OPEN_FAIL" ,"Can't read the screw profile.");
    }

    QTextStream stream(&screwProfile);
    stream.setCodec("UTF-8");
    QJsonDocument doc = QJsonDocument::fromJson(stream.readAll().toUtf8());
    screwProfile.close();
    if (!doc.isObject() || doc.isNull())
    {
        _P_ERR_OBJ_->addError("FILE_BROKEN", "The screw profile file broken.");
    }
    QJsonArray dataArray = doc.object()["data"].toArray();

    for (int i = 0; i < dataArray.size(); i++)
    {
        QJsonArray object = dataArray[i].toArray();
        QPair<double, double> pair(object[0].toDouble(), object[1].toDouble());
        _screwWattsAndCOPVec.push_back(pair);
    }

    //对screwWattsAndCOP进行排序，按制冷量从小到大排序
    qSort(_screwWattsAndCOPVec.begin(), _screwWattsAndCOPVec.end(),[](QPair<double, double>&a, QPair<double, double>&b)
    {
        return a.first < b.first;
    });
}

EnergySheet HCFSheetProcessor::mainProcess(HCFSheet hcfSheet, RoomState roomState,
                                           const RoomState::RoomType type, const bool isInheritCoreData)
{
    EnergySheet result;
    double deviceWatts = roomState.getDeviceKW() * 1000;
    double lightWatts = roomState.getLightKW() * 1000;
    Column<int> roomCol = roomState.getRoomsVec(type);

    result[EnergySheet::Device] = Column<double>(hcfSheet._coolLoad.size(), 1);
    result[EnergySheet::Device] *= roomCol * deviceWatts * 3600 * _deviceRatioCol;
    result[EnergySheet::Light] = Column<double>(hcfSheet._coolLoad.size(), 1);
    result[EnergySheet::Light] *= roomCol * lightWatts * 3600 * _lightRatioCol;

    static double maxHeatLoad = 0;
    static double maxCoolLoad = 0;
    if (isInheritCoreData != true)
    {
        maxHeatLoad = *std::max_element(hcfSheet._heatLoad.data().begin(), hcfSheet._heatLoad.data().end());
        maxCoolLoad = *std::max_element(hcfSheet._coolLoad.data().begin(), hcfSheet._coolLoad.data().end());
    }

    int screwMachineNum;
    if (maxCoolLoad*1.15/3 > 150000)
    {
        screwMachineNum = 3;
    }
    else if (maxCoolLoad*1.15/2 > 150000)
    {
        screwMachineNum = 2;
    }
    else
    {
        screwMachineNum = 1;
    }

    //计算螺杆机(冷机)容量(W)以及COP并修正
    double screwCapacity = maxCoolLoad/screwMachineNum*1.15;
    double cop = -1;
    for (auto pair: _screwWattsAndCOPVec)
    {
        if (pair.first >= screwCapacity/1000)
        {
            screwCapacity = pair.first*1000;
            cop = pair.second;
            break;
        }
    }
    cop = (cop != -1) ? cop : _screwWattsAndCOPVec.back().second;

    //锅炉容量(W)
    double boilerCapacity = maxHeatLoad*1.05;
    //(螺杆机)冷冻水泵额定流量(m³/s)
    double freWaterPumpFlow = maxCoolLoad/(5*1000*4200*screwMachineNum);
    //(螺杆机)冷冻泵额定功率(W)
    double freWaterPumpWatts = freWaterPumpFlow*(100000+200*200)/0.6;
    //(螺杆机)冷却水泵额定流量(m³/s)
    double cooWaterPumpFlow = freWaterPumpFlow*1.5;
    //(螺杆机)冷却水泵额定功率(W)
    double cooWaterPumpWatts = cooWaterPumpFlow*(100000+200*200)/0.6;
    //热水泵额定流量(m³/s)
    double hotWaterPumpFlow = maxHeatLoad/(10*1000*4200);
    //热水泵额定功率(W)
    double hotWaterPumpWatts = hotWaterPumpFlow*(100000+200*200)/0.6;


    //8760小时,每个时刻运行的冷机台数
    Column<double> runningCooMachineNumCol;
    runningCooMachineNumCol = hcfSheet._coolLoad / (0.95 * screwCapacity);
    for (auto &data: runningCooMachineNumCol.data())
    {
        //上取整
        data = ceil(data);
    }

    //8760小时,每个时刻运行的冷机的负荷(W)
    Column<double> runningCooMachineLoadCol = hcfSheet._coolLoad / runningCooMachineNumCol;

    //8760小时,每个时刻的部分负荷率PLR
    Column<double> partLoadPercentPLRCol = runningCooMachineLoadCol / screwCapacity;

    //8760小时,每个时刻的修正曲线值
    Column<double> correctedValueCol = partLoadPercentPLRCol * 1.5534
            - partLoadPercentPLRCol * partLoadPercentPLRCol * 1.1552 +  0.5958;

    //8760小时,每个时刻的COP值
    Column<double> currentCopCol = correctedValueCol * cop;

    //8760小时,每个时刻运行的冷机的功率(W)
    Column<double> runningCooMachineWattsCol = runningCooMachineLoadCol / currentCopCol;

    //8760小时,每个时刻运行的冷机的能耗(J)
    result[EnergySheet::CooMachine] = runningCooMachineWattsCol * runningCooMachineNumCol * 3600;

    //8760小时,每个时刻的锅炉PLR
    Column<double> boilerPLRCol = hcfSheet._heatLoad / boilerCapacity;

    //8760小时,每个时刻的锅炉实际效率
    Column<double> realEffCol = (boilerPLRCol * 0.0633 - boilerPLRCol *  boilerPLRCol * 0.0333 + 0.97) * 0.8;

    //8760小时,每个时刻的锅炉能耗(J)
    result[EnergySheet::BoilerFuelUse] = hcfSheet._heatLoad / realEffCol;

    //8760小时,每个时刻每台冷却塔的总散热量(W)
    Column<double> perCooTowerLoseHeatCol = (Column<double>(currentCopCol.size(), 1) / currentCopCol + 1) * runningCooMachineLoadCol;

    //获取冷却塔的最大散热量(W)
    double maxCooTowerLoseHeat = *std::max_element(perCooTowerLoseHeatCol.data().begin(),
                                                   perCooTowerLoseHeatCol.data().end());

    //计算冷却塔风机额定功率(W)
    double cooTowerFanWatts = 0.0088 * maxCooTowerLoseHeat - 1.1766;

    //8760小时,每个时刻每台冷却塔风机的功率(W)
    Column<double> cooTowerFanWattsCol = (partLoadPercentPLRCol * 0.30850535 - partLoadPercentPLRCol * 0.54137364 * partLoadPercentPLRCol +
                                          partLoadPercentPLRCol * 0.87198823 * partLoadPercentPLRCol * partLoadPercentPLRCol + 0.35071223) * cooTowerFanWatts;

    //8760小时,每个时刻冷却塔的能耗(J)
    result[EnergySheet::CooTower] = cooTowerFanWattsCol * 3600 * runningCooMachineNumCol;

    //8760小时,每个时刻的冷冻水泵当前功率(W)
    Column<double> freWaterPumpWattsCol = partLoadPercentPLRCol * freWaterPumpWatts;

    //8760小时,每个时刻的冷冻水泵能耗(J)
    result[EnergySheet::FreWaterPump] = freWaterPumpWattsCol * 3600 * runningCooMachineNumCol;

    //8760小时,每个时刻的冷却水泵当前功率(W)
    Column<double> cooWaterPumpWattsCol = partLoadPercentPLRCol * cooWaterPumpWatts;

    //8760小时,每个时刻的冷却水泵能耗(J)
    result[EnergySheet::CooWaterPump] = cooWaterPumpWattsCol * 3600 * runningCooMachineNumCol;

    //8760小时,每个时刻的热水泵当前功率(W)
    Column<double> hotWaterPumpWattsCol = boilerPLRCol * hotWaterPumpWatts;

    //8760小时,每个时刻的热水泵能耗(J)
    result[EnergySheet::HotWaterPump] = hotWaterPumpWattsCol * 3600;

    //8760小时,每个时刻的风机能耗(J)
    result[EnergySheet::Fan] = hcfSheet._fansWatts * 3600;

    return result;
}


void HCFSheetProcessor::load()
{
    HCFSheetProcessor::initLoadReduceMap();
    HCFSheetProcessor::initRatioVec();
    HCFSheetProcessor::initScrewAndCOPVec();
    HCFSheetProcessor::_isPreCoreData = true;
}

HCFSheetProcessor::HCFSheetProcessor(const HCFSheet &hcfSheet, const RoomState &roomState)
{
    setData(hcfSheet, roomState);
}

void HCFSheetProcessor::setData(const HCFSheet &hcfSheet, const RoomState &roomState)
{
    _hcfSheet = hcfSheet;
    _roomState = roomState;
}

bool HCFSheetProcessor::isReady()
{
    return (_hcfSheet.isComplete() && HCFSheetProcessor::_isPreCoreData && _roomState.isCanAnalyze()) ? true : false;
}

void HCFSheetProcessor::newWindConfig(const int roomSize, const int hours)
{
    if (isReady() && roomSize > 0 && hours > 0)
    {
        //换气次数
        int changeTimes = 1;
        //流量
        double airFlow = roomSize*changeTimes*3/3600.0;
        //压头(Pa)
        double pumpHead = 75.0;
        //效率
        double efficiency = 0.7;
        //24小时，每小时的风机功率
        double fanWatts = airFlow*pumpHead/efficiency;
        //时长
        int usingTime = hours;

        Column<double> fanWattsCol;
        for (int i = 0; i < 24; i++)
        {
            if (i < usingTime)
            {
                fanWattsCol.push_back(fanWatts);
            }
            else
            {
                fanWattsCol.push_back(0);
            }
        }
        _hcfSheet._fansWatts += fanWattsCol;
    }
}

void HCFSheetProcessor::pip2Config()
{
    if (isReady())
    {
        _hcfSheet._heatLoad -= _hcfSheet._coolLoad;
        _hcfSheet._coolLoad = _hcfSheet._heatLoad * -1;
        _hcfSheet._heatLoad.replace([](double data)
        {
            return (data < 0) ? true : false;
        }, 0);
        _hcfSheet._coolLoad.replace([](double data)
        {
            return (data < 0) ? true : false;
        }, 0);
    }
}

void HCFSheetProcessor::ACConfig(const int coolTemp, const int heatTemp)
{
    if (isReady())
    {
        double coolLoadPercent = 0, heatLoadPercent = 0;
        coolLoadPercent = 1 - HCFSheetProcessor::_coolLoadReducePercentMap[coolTemp];
        heatLoadPercent = 1 - HCFSheetProcessor::_heatLoadReducePercentMap[heatTemp];
        _hcfSheet._coolLoad *= coolLoadPercent;
        _hcfSheet._heatLoad *= heatLoadPercent;
    }
}

EnergySheet HCFSheetProcessor::produceEnergySheet(const RoomState::RoomType roomType)
{
    if (roomType == RoomState::all)
    {
        return HCFSheetProcessor::mainProcess(_hcfSheet, _roomState, RoomState::all, false);
    }
    else
    {
        return HCFSheetProcessor::mainProcess(_hcfSheet, _roomState, RoomState::rentedPe, true);
    }
}
