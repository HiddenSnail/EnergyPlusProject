#ifndef CSVREADER_H
#define CSVREADER_H
#include <QtCore>

class CsvReader {
private:
    QVector<QStringList> _dataGrid; //csv文件的二维数组
    QFile *_pCsvFile; //csv文件指针
public:
    //默认构造函数
    CsvReader();

    //析构函数
    ~CsvReader();

    //带一个参数的构造函数
    explicit CsvReader(QString filePath);

    //重载=运算符
    CsvReader& operator = (const CsvReader& csvReader);

    //拷贝构造函数
    CsvReader(const CsvReader& csvReader);

    //设置csv文件路径
    bool setFilePath(QString filePath);

    //是否已经初始化文件路径
    bool isInitFile();

    //解析csv文件到_dataGrid
    bool analyze(char separator = ',');

    //根据首行标题获取csv文件的目标列
    QStringList getColumnByTitle(QString columnTitle);

    //将csvReader中的内容保存到文件
    bool save();

    //向_dataGrid后插入一列数据
    void pushBackColumnData(QString title, QStringList columnData);

};

#endif // CSVREADER_H
