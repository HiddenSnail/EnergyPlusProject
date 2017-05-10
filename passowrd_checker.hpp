#ifndef PASSOWRD_CHECKER_HPP
#define PASSOWRD_CHECKER_HPP
#include "stdafx.h"

bool checkPassword(const std::string userId, const std::string password)
{
    std::string key(userId);
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

#endif // PASSOWRD_CHECKER_H
