#include "./core/roomstate.h"

RoomState::RoomState()
{
    _totalRooms = -4;
    _eastRooms = -1;
    _westRooms = -1;
    _southRooms = -1;
    _northRooms = -1;
    _rentalRate = -1.0;
    _keepOnRate = -1.0;
    _deviceKW = -1.0;
    _lightKW = -1.0;
    _inRoomRateVec.clear();
}

RoomState::RoomState(const unsigned int eastRooms, const unsigned int westRooms,
         const unsigned int southRooms, const unsigned int northRooms, const double rentalRate,
        const double keepOnRate, const D24hVector &inRoomRateVec, const double deviceKW, const double lightKW):
    _eastRooms(eastRooms), _westRooms(westRooms), _southRooms(southRooms),
    _northRooms(northRooms), _deviceKW(deviceKW), _lightKW(lightKW)

{
    _totalRooms = _eastRooms + _westRooms + _southRooms + _northRooms;
    _rentalRate = (rentalRate <=1 && rentalRate >= 0) ? rentalRate : -1.0;
    _keepOnRate = (keepOnRate <=1 && keepOnRate >= 0) ? keepOnRate : -1.0;
    if (inRoomRateVec.size() == 24)
    {
        _inRoomRateVec = inRoomRateVec;
    }
    else
    {
        _inRoomRateVec.clear();
    }
}

bool RoomState::isCanAnalyze()
{


    if (_totalRooms > 0 && _eastRooms >= 0 && _westRooms >= 0 && _southRooms >= 0 && _northRooms >= 0
            && _rentalRate >= 0 && _keepOnRate >= 0 && _deviceKW >= 0 && _lightKW >= 0
            && _inRoomRateVec.size() > 0)
    {
        return true;
    }
    else
    {
        if (_totalRooms < 0) qDebug() << "_totalRooms" << _totalRooms;
        if (_eastRooms < 0) qDebug() << "_eastRooms:" << _eastRooms;
        if (_westRooms < 0) qDebug() << "_westRooms:" << _westRooms;
        if (_southRooms < 0) qDebug() << "_southRooms:" << _southRooms;
        if (_northRooms < 0) qDebug() << "_northRooms:" << _northRooms;
        if (_rentalRate < 0) qDebug() << "_rental Rate:" << _rentalRate;
        if (_keepOnRate < 0) qDebug() << "_keepOn Rate:" << _keepOnRate;
        if (_rentalRate < 0) qDebug() << "_rental Rate:" << _rentalRate;
        if (_deviceKW < 0) qDebug() << "_deviceKW:" << _deviceKW;
        if (_lightKW < 0) qDebug() << "_lightKW:" << _lightKW;
        return false;
    }
}

RoomState::RoomState(const RoomState &roomState)
{
    *this = roomState;
}

RoomState& RoomState::operator = (const RoomState &roomState)
{
    if (this != &roomState)
    {
        this->_inRoomRateVec.clear();
        this->_noRentedRoomsVec.clear();
        this->_rentedNoPeRoomsVec.clear();
        this->_rentedPeRoomsVec.clear();
        this->_totalRoomsVec.clear();

        this->_eastRooms = roomState._eastRooms;
        this->_westRooms = roomState._westRooms;
        this->_southRooms = roomState._southRooms;
        this->_northRooms = roomState._northRooms;
        this->_totalRooms = roomState._totalRooms;
        this->_rentalRate = roomState._rentalRate;
        this->_keepOnRate = roomState._keepOnRate;
        this->_inRoomRateVec = roomState._inRoomRateVec;

        this->_deviceKW = roomState._deviceKW;
        this->_lightKW = roomState._lightKW;

        this->_noRentedRoomsVec = roomState._noRentedRoomsVec;
        this->_rentedNoPeRoomsVec = roomState._rentedNoPeRoomsVec;
        this->_rentedPeRoomsVec = roomState._rentedPeRoomsVec;
        this->_totalRoomsVec = roomState._totalRoomsVec;
    }
    return *this;
}

void RoomState::analyze()
{
    if (isCanAnalyze())
    {
        reset();
        //待租房间数
        int noRentedRooms = (int)ceil(_totalRooms*(1 - _rentalRate));
        //已租房间数
        int rentedRooms = _totalRooms - noRentedRooms;
        for(double rentalRate: _inRoomRateVec)
        {
            //每小时已租无人房间数
            int rentedNoPeRooms = ceil(rentedRooms * (1 - rentalRate)* (1 - _keepOnRate));
            //每小时已租有人房间数
            int rentedPeRooms = rentedRooms - rentedNoPeRooms;

            _noRentedRoomsVec.push_back(noRentedRooms);
            _rentedNoPeRoomsVec.push_back(rentedNoPeRooms);
            _rentedPeRoomsVec.push_back(rentedPeRooms);
            _totalRoomsVec.push_back(_totalRooms);
        }
    }
}

void RoomState::reset()
{
    _totalRoomsVec.clear();
    _noRentedRoomsVec.clear();
    _rentedNoPeRoomsVec.clear();
    _rentedPeRoomsVec.clear();
}

I24hVector RoomState::getTotalRoomsVec()
{
    if (_totalRoomsVec.isEmpty())
    {
        analyze();
    }
    return _totalRoomsVec;
}

I24hVector RoomState::getNoRentedRoomsVec()
{
    if (_noRentedRoomsVec.isEmpty())
    {
        analyze();
    }
    return _noRentedRoomsVec;
}

I24hVector RoomState::getRentedNoPeRoomsVec()
{
    if (_rentedNoPeRoomsVec.isEmpty())
    {
        analyze();
    }
    return _rentedNoPeRoomsVec;
}

I24hVector RoomState::getRentedPeRoomsVec()
{
    if (_rentedPeRoomsVec.isEmpty())
    {
        analyze();
    }
    return _rentedPeRoomsVec;
}

I24hVector RoomState::getRoomsVec(const RoomType roomType)
{
    return _memFuncTable[roomType](this);
}

int RoomState::getTotalRoomsNumber() const
{
    return _totalRooms;
}

int RoomState::getEastRoomsNumber() const
{
    return _eastRooms;
}

void RoomState::setEastRoomNumber(const unsigned int eastRooms)
{
    _eastRooms = eastRooms;
    _totalRooms = _eastRooms + _westRooms + _southRooms + _northRooms;
    reset();
}

int RoomState::getWestRoomsNumber() const
{
    return _westRooms;
}


void RoomState::setWestRoomsNumber(const unsigned int westRooms)
{
    _westRooms = westRooms;
    _totalRooms = _eastRooms + _westRooms + _southRooms + _northRooms;
    reset();
}

int RoomState::getSouthRoomsNumber() const
{
    return _southRooms;
}

void RoomState::setSouthRoomsNumber(const unsigned int southRooms)
{
    _southRooms = southRooms;
    _totalRooms = _eastRooms + _westRooms + _southRooms + _northRooms;
    reset();
}

int RoomState::getNorthRoomsNumber() const
{
    return _northRooms;
}

void RoomState::setNorthRoomsNumber(const unsigned int northRooms)
{
    _northRooms = northRooms;
    _totalRooms = _eastRooms + _westRooms + _southRooms + _northRooms;
    reset();
}

double RoomState::getRentalRate() const
{
    return _rentalRate;
}

void RoomState::setRentalRate(const double rentalRate)
{
    _rentalRate = (rentalRate >= 0 && rentalRate <= 1) ? rentalRate : -1.0;
    reset();
}

double RoomState::getKeepOnRate() const
{
    return _keepOnRate;
}

void RoomState::setKeepOnRate(const double keepOnRate)
{
    _keepOnRate = (keepOnRate >= 0 && keepOnRate <= 1) ? keepOnRate : -1.0;
    reset();
}

D24hVector RoomState::getInRoomRateVec() const
{
    return _inRoomRateVec;
}

bool RoomState::setInRoomRateVec(D24hVector inRoomRateVec)
{
    if (inRoomRateVec.size() == 24)
    {
        _inRoomRateVec = inRoomRateVec;
        reset();
        return true;
    }
    else
    {
        return false;
    }
}

double RoomState::getDeviceKW() const
{
    return _deviceKW;
}

void RoomState::setDeviceKW(const double deviceKW)
{
    _deviceKW = deviceKW;
}

double RoomState::getLightKW() const
{
    return _lightKW;
}

void RoomState::setLightKW(const double lightKW)
{
    _lightKW = lightKW;
}
