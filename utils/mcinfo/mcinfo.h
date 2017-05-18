#ifndef MCINFO_H
#define MCINFO_H
#include <string>
#include <iostream>
#include <list>
#include <winsock2.h>
#include <stdlib.h>
#include <iphlpapi.h>
#include <winerror.h>
#pragma comment(lib, "iphlpapi.lib")

/**
 * @brief The MachineInfo class >> 获取机器硬件信息专用类
 */
typedef std::string MacAddress;
typedef std::list<MacAddress> MacAddressList;

class MachineInfo {
public:
    enum Error {SUCCESS = 0, MEM_ERROR = 11, OTHER_ERROR = 20};
    static int getMacAddressList(MacAddressList &macAddressList);
private:
    MachineInfo(){}
    static char convertDecToHexChar(unsigned int dec);

};

#endif //MCINFO_H
