#ifndef STDAFX_H
#define STDAFX_H

#include <QtCore>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
typedef std::vector< std::string > MACAddresses;
typedef std::string MACAddress;

static QTextStream qout(stdout, QIODevice::WriteOnly);
static QTextStream qin(stdin, QIODevice::ReadOnly);
#endif // STDAFX_H
