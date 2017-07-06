#include "./core/sheet.h"
#include <map>

//---Struct RawDataSheet---//

RawDataSheet::RawDataSheet(QString csvFilePath, TimeSpan timeSpan)
{
    setData(csvFilePath, timeSpan);
}

RawDataSheet::RawDataSheet(const RawDataSheet &sheet)
{
    *this = sheet;
}

void RawDataSheet::setData(QString csvFilePath, TimeSpan timeSpan)
{
    CsvReader reader(csvFilePath);
    if (reader.analyze(','))
    {
        QString beginRegFormat("^(\\s)*%1/%2(\\s)*01:00:00(\\s)*");
        QString endRegFormat("^(\\s)*%1/%2(\\s)*24:00:00(\\s)*");
        QRegExp beginReg, endReg;
        switch (timeSpan) {
        case year:
        {
            beginReg.setPattern(beginRegFormat.arg("01").arg("01"));
            endReg.setPattern(endRegFormat.arg("12").arg("31"));
            break;
        }
        case quarter1:
        {
            beginReg.setPattern(beginRegFormat.arg("01").arg("01"));
            endReg.setPattern(endRegFormat.arg("03").arg("31"));
            break;
        }
        case quarter2:
        {
            beginReg.setPattern(beginRegFormat.arg("04").arg("01"));
            endReg.setPattern(endRegFormat.arg("06").arg("30"));
            break;
        }
        case quarter3:
        {
            beginReg.setPattern(beginRegFormat.arg("07").arg("01"));
            endReg.setPattern(endRegFormat.arg("09").arg("30"));
            break;
        }
        case quarter4:
        {
            beginReg.setPattern(beginRegFormat.arg("10").arg("01"));
            endReg.setPattern(endRegFormat.arg("12").arg("31"));
            break;
        }
        default:
            break;
        }

        QStringList dateList = reader.getColumnByTitle("Date/Time");
        int beginPos = -1, endPos = -1;
        for(int index = RawDataSheet::IgnoreLines; index < dateList.size(); index++)
        {
            if (beginPos > -1)
            {
                if (endReg.exactMatch(dateList[index]))
                {
                    endPos = index;
                    break;
                }
            }
            else
            {
                if (beginReg.exactMatch(dateList[index]))
                {
                    beginPos = index;
                }
            }
        }

        std::map<ID, QStringList> dataMap =
        {
            {eHeatCol, reader.getColumnByTitle("BLOCK2:EAST HEATING COIL:Heating Coil Heating Rate [W](Hourly)")},
            {wHeatCol, reader.getColumnByTitle("BLOCK2:WEST HEATING COIL:Heating Coil Heating Rate [W](Hourly)")},
            {sHeatCol, reader.getColumnByTitle("BLOCK2:SOUTH HEATING COIL:Heating Coil Heating Rate [W](Hourly)")},
            {nHeatCol, reader.getColumnByTitle("BLOCK2:NORTH HEATING COIL:Heating Coil Heating Rate [W](Hourly)")},
            {dsHeatCol, reader.getColumnByTitle("DOAS SYSTEM HEATING COIL:Heating Coil Heating Rate [W](Hourly)")},
            {eCoolCol, reader.getColumnByTitle("BLOCK2:EAST COOLING COIL:Cooling Coil Total Cooling Rate [W](Hourly)")},
            {wCoolCol, reader.getColumnByTitle("BLOCK2:WEST COOLING COIL:Cooling Coil Total Cooling Rate [W](Hourly)")},
            {sCoolCol, reader.getColumnByTitle("BLOCK2:SOUTH COOLING COIL:Cooling Coil Total Cooling Rate [W](Hourly)")},
            {nCoolCol, reader.getColumnByTitle("BLOCK2:NORTH COOLING COIL:Cooling Coil Total Cooling Rate [W](Hourly)")},
            {dsCoolCol, reader.getColumnByTitle("DOAS SYSTEM COOLING COIL:Cooling Coil Total Cooling Rate [W](Hourly)")},
            {eFanCol, reader.getColumnByTitle("BLOCK2:EAST SUPPLY FAN:Fan Electric Power [W](Hourly)")},
            {wFanCol, reader.getColumnByTitle("BLOCK2:WEST SUPPLY FAN:Fan Electric Power [W](Hourly)")},
            {sFanCol, reader.getColumnByTitle("BLOCK2:SOUTH SUPPLY FAN:Fan Electric Power [W](Hourly)")},
            {nFanCol, reader.getColumnByTitle("BLOCK2:NORTH FAN COIL:Fan Coil Fan Electric Power [W](Hourly)")}
        };

        _memMap.clear();
        for (auto &pair: dataMap)
        {
            if (pair.first == dsHeatCol && pair.second.size() == 0)
            {
                _memMap[pair.first] = Column<double>(endPos - beginPos + 1, 0);
                continue;
            } else if(pair.first == dsCoolCol && pair.second.size() == 0)
            {
                _memMap[pair.first] = Column<double>(endPos - beginPos + 1, 0);
                continue;
            }
            else
            {
                QStringList sub = getSubSequence(pair.second, beginPos, endPos);
                _memMap[pair.first] = convertToCol(sub);
            }
        }
    }
    else
    {
        _P_ERR_OBJ_->addError("FILE_MISS", QString("Can't find csv file %1, RawDataSheet set data fail.").arg(csvFilePath));
    }
}

bool RawDataSheet::isComplete() const
{
    for (auto &pair: _memMap)
    {
        if (pair.second.isEmpty())
        {
            return false;
        }
    }
    return true;
}


RawDataSheet& RawDataSheet::operator = (const RawDataSheet &rhs)
{
    if (this != &rhs)
    {
        this->_memMap = rhs._memMap;
    }
    return *this;
}

QStringList RawDataSheet::getSubSequence(const QStringList &list, const int begin, const int end)
{
    QStringList subList;
    if (begin <= end && end < list.size() && begin >= 0)
    {
        for(int index = begin; index <= end; index++)
        {
            subList << list[index];
        }
    }
    return subList;
}

Column<double> RawDataSheet::convertToCol(const QStringList &strList)
{
    QVector<double> colData;
    for(auto data: strList)
    {
        colData << data.toDouble();
    }
    return Column<double>(colData);
}

Column<double>& RawDataSheet::operator [] (const ID id)
{
    return _memMap[id];
}


//---Strcut HCFSheet---//
HCFSheet::HCFSheet(const HCFSheet &sheet)
{
    *this = sheet;
}

bool HCFSheet::isComplete()
{
    return (_heatLoad.isEmpty() || _coolLoad.isEmpty() || _fansWatts.isEmpty()) ? false : true;
}

HCFSheet& HCFSheet::operator = (const HCFSheet &rhs)
{
    if (this != &rhs)
    {
        this->_heatLoad = rhs._heatLoad;
        this->_coolLoad = rhs._coolLoad;
        this->_fansWatts = rhs._fansWatts;
    }
    return *this;
}

HCFSheet& HCFSheet::operator += (const HCFSheet &rhs)
{
    this->_heatLoad += rhs._heatLoad;
    this->_coolLoad += rhs._coolLoad;
    this->_fansWatts += rhs._fansWatts;
    return *this;
}

HCFSheet operator + (const HCFSheet &lhs, const HCFSheet &rhs)
{
    HCFSheet result(lhs);
    result += rhs;
    return result;
}


//---Struct EnergySheet---//
EnergySheet::EnergySheet()
{
    for (auto &pair: _memMap)
    {
        pair.second.clear();
    }
}

EnergySheet::EnergySheet(const EnergySheet &sheet)
{
    *this = sheet;
}

bool EnergySheet::isComplete() const
{
    for (auto &pair: _memMap)
    {
        if (pair.second.isEmpty())
        {
            return false;
        }
    }
    return true;
}

double EnergySheet::sum() const
{
    double sum = 0;
    for (auto &pair: _memMap)
    {
        sum += pair.second.sum();
    }
    return sum;
}

EnergySheet& EnergySheet::operator = (const EnergySheet &rhs)
{
    if (this != &rhs)
    {
        this->_memMap.clear();
        this->_memMap = rhs._memMap;
    }
    return *this;
}

Column<double>& EnergySheet::operator [] (const EnergySheet::ID id)
{
    return _memMap[id];
}

EnergySheet& EnergySheet::operator += (const EnergySheet &rhs)
{
    auto val = rhs;
    for (auto &pair: _memMap)
    {
        pair.second += val[pair.first];
    }
    return *this;
}

EnergySheet operator + (const EnergySheet &lhs, const EnergySheet &rhs)
{
    EnergySheet result = lhs;
    result += rhs;
    return result;
}

void EnergySheet::saveToFile(QString filePath)
{
    QFile file(filePath);
    if (file.open(QFile::WriteOnly))
    {
        QTextStream stream(&file);
        for (auto pair: _memMap)
        {
            for (auto data: pair.second.data())
            {
                stream << QString::number(data) << ",";
            }
            stream << endl;
        }
        file.close();
    }
}
