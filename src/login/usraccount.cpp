#include "./login/usraccount.h"

std::string UsrAccount::createUsrId(const MacAddress &macAddress)
{
    std::string usrId(macAddress);
    for (auto &letter: usrId)
    {
       if (letter >= 65 && letter <= 70)
       {
           letter -= 17;
       }
    }
    return usrId;
}

bool UsrAccount::checkPassword(const std::string usrId, const std::string password)
{
    std::string key(usrId);
    key[2] += 17;
    key[3] += 49;
    key[6] += 17;
    key[7] += 49;
    key[9] += 17;
    key = key.substr(2,8);
    if (key.compare(password) == 0)
    {
        return true;
    } else
    {
        return false;
    }
}
