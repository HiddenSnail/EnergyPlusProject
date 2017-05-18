#include "mcinfo.h"
#include <iomanip>

/**
 * @brief MachineInfo::convertDecToHexChar >> 十进制数转十六进制字符
 * @param dec: 无符号十进制数(0-15)
 * @return 若输入的数字大于或等于16则返回'\0'
 */
char MachineInfo::convertDecToHexChar(unsigned int dec)
{
    char letter = '\0';
    if (dec < 10)
    {
        letter = dec + '0';
    }
    else if (dec >= 10 && dec <= 15)
    {
        letter = dec - 10 + 'A';
    }
    else
    {
        ;
    }
    return letter;
}


/**
 * @brief MachineInfo::getMacAddress >> 获取机器的mac地址
 * @return
 */
int MachineInfo::getMacAddressList(std::list<std::string> &macAddressList)
{
    PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
    if (!pIpAdapterInfo)
    {
        return MEM_ERROR;
    }
    PIP_ADAPTER_INFO pIpAdapter = nullptr;
    unsigned long ulOutBufLen = sizeof(IP_ADAPTER_INFO);
    int errorKey = GetAdaptersInfo(pIpAdapterInfo, &ulOutBufLen);
    //pIpAdapterInfo分配的空间不足,重新进行分配
    if (ERROR_BUFFER_OVERFLOW == errorKey)
    {
        delete pIpAdapterInfo;
        pIpAdapterInfo = nullptr;
        pIpAdapterInfo = (PIP_ADAPTER_INFO) new BYTE[ulOutBufLen]; //注意是中括号
        if (!pIpAdapterInfo)
        {
            return MEM_ERROR;
        }
        errorKey = GetAdaptersInfo(pIpAdapterInfo, &ulOutBufLen);
    }

    if (NO_ERROR == errorKey)
    {
        pIpAdapter = pIpAdapterInfo;
        while (pIpAdapter)
        {
            std::string macAddress("");
            for (DWORD i = 0; i < pIpAdapter->AddressLength; i++)
            {
                int first = pIpAdapter->Address[i]/16;
                int second = pIpAdapter->Address[i]%16;
                macAddress.push_back(convertDecToHexChar(first));
                macAddress.push_back(convertDecToHexChar(second));
            }
            macAddressList.push_back(macAddress);
            pIpAdapter = pIpAdapter->Next;
        }
        delete pIpAdapterInfo;
        return SUCCESS;
    }
    else
    {
        delete pIpAdapterInfo;
        return OTHER_ERROR;
    }
}
