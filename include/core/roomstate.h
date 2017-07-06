#pragma once
#include <QtCore>

typedef QVector<double> D24hVector; //24小时的double数据向量类型,长度要为24
typedef QVector<int> I24hVector; //24小时的int数据向量类型,长度要为24

class RoomState {
private:
    int _totalRooms;
    int _eastRooms;
    int _westRooms;
    int _southRooms;
    int _northRooms;
    double _rentalRate;
    double _keepOnRate;
    D24hVector _inRoomRateVec;
    double _deviceKW; //设备功率(KW)
    double _lightKW; //照明功率(KW)

    I24hVector _noRentedRoomsVec; //24小时, 每小时未租房间数
    I24hVector _rentedNoPeRoomsVec; //24小时, 每小时已租无人房间数
    I24hVector _rentedPeRoomsVec; //24小时, 每小时已租有人房间数
    I24hVector _totalRoomsVec; //24小时, 每小时房间总数
    const QVector<std::function<I24hVector(RoomState*)> > _memFuncTable =
    {
            &RoomState::getTotalRoomsVec,
            &RoomState::getNoRentedRoomsVec,
            &RoomState::getRentedNoPeRoomsVec,
            &RoomState::getRentedPeRoomsVec
    };
    I24hVector getTotalRoomsVec();
    I24hVector getNoRentedRoomsVec();
    I24hVector getRentedNoPeRoomsVec();
    I24hVector getRentedPeRoomsVec();
    void reset();
    void analyze();
public:
    enum RoomType {all = 0, noRented, rentedNoPe, rentedPe};
    RoomState();
    RoomState(const unsigned int eastRooms, const unsigned int westRooms,
             const unsigned int southRooms, const unsigned int northRooms,
             const double rentalRate, const double keepOnRate, const D24hVector &inRoomRateVec,
              const double deviceKW, const double lightKW);
    bool isCanAnalyze();
    RoomState(const RoomState &roomState);
    RoomState& operator = (const RoomState &roomState);
    I24hVector getRoomsVec(const RoomType roomType);
    int getTotalRoomsNumber() const;
    int getEastRoomsNumber() const;
    void setEastRoomNumber(const unsigned int eastRooms);
    int getWestRoomsNumber() const;
    void setWestRoomsNumber(const unsigned int westRooms);
    int getSouthRoomsNumber() const;
    void setSouthRoomsNumber(const unsigned int southRooms);
    int getNorthRoomsNumber() const;
    void setNorthRoomsNumber(const unsigned int northRooms);
    double getRentalRate() const;
    void setRentalRate(const double rentalRate);
    double getKeepOnRate() const;
    void setKeepOnRate(const double keepOnRate);
    D24hVector getInRoomRateVec() const;
    bool setInRoomRateVec(D24hVector inRoomRateVec);

    double getDeviceKW() const;
    void setDeviceKW(const double deviceKW);
    double getLightKW() const;
    void setLightKW(const double lightKW);

};
