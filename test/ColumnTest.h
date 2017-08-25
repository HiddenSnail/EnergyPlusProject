#include <QtTest/QtTest>
#include "core/column.h"

class ColumnTest: public QObject
{
    Q_OBJECT
private slots:
    void testEq();
    void testAdd();
};
