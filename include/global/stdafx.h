#ifndef STDAFX_H
#define STDAFX_H

//Qt lib
#include <QtCore>
#include <QRegExp>
#include <QJsonDocument>

//std lib
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

//custom lib
#include "qerrorobject.h"
#include "utils/pathmanager/pathmanager.h"

static QTextStream qout(stdout, QIODevice::WriteOnly);
static QTextStream qin(stdin, QIODevice::ReadOnly);
#endif // STDAFX_H
