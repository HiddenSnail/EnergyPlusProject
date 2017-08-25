#include "test/ColumnTest.h"

void ColumnTest::testEq()
{
    Column<int> col1 = {1, 2, 3};
    Column<int> col2 = {1, 2, 3};
    QVERIFY(col1 == col2);
    //类型转换
    Column<double> col3 = {1.0, 2, 3};
    QVERIFY(col1 == col3);
    //不同长度
    Column<int> col4 = {0}, col5 = {0, 0};
    QVERIFY(!(col4 == col5));
}

void ColumnTest::testAdd()
{
    Column<int> col1 = {1, 2, 3, 4, 5};
    Column<int> col2 = {1, 1, 1, 1, 1};
    Column<int> col3 = {2, 3, 4, 5, 6};
    QVERIFY((col1 + col2 == col3));

    Column<int> col4 = {-1, -2, -3};
    Column<int> col5 = {3, 2, 1};
    QVERIFY((col4 + 4 == col5));

    Column<double> col6 = {1.3, 1.1, 1.0};
    Column<double> col7 = {2.3, 3.1, 4};
    Column<int> col8 = {2, 3, 4, 5, 6};
    QVERIFY((col6 + col1 == col7)); //检验首元素
    QVERIFY((col1 + col6 == col8)); //检验循环加法
}


