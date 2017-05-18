#ifndef USRACCOUNT_H
#define USRACCOUNT_H
#include "./utils/mcinfo/mcinfo.h"

class UsrAccount
{
private:
    UsrAccount() {}
public:
    static std::string createUsrId(const MacAddress &macAddress);
    static bool checkPassword(const std::string userId, const std::string password);
};

#endif // USRACCOUNT_H
