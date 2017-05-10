#ifndef USERID_CREATOR_HPP
#define USERID_CREATOR_HPP
#include "stdafx.h"
#include "mac_address.hpp"
#include "convert_pip.hpp"

std::string createUserId(const MACAddress& macAddress)
{
    std::string id("");
    id += macAddress.substr(0, 2) + macAddress.substr(3, 2)
            + macAddress.substr(6, 2) + macAddress.substr(9, 2)
            + macAddress.substr(12, 2) + macAddress.substr(15, 2);
    id = convert(id);
    return id;
}

#endif // USERIDCREATOR_HPP
