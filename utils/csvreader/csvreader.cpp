#include "csvreader.h"

CsvReader::CsvReader()
{
    _pCsvFile = new QFile();
    _dataGrid.clear();
}

CsvReader::~CsvReader()
{
    delete _pCsvFile;
}

CsvReader::CsvReader(QString filePath)
{
    _pCsvFile = new QFile();
    _dataGrid.clear();
    setFilePath(filePath);
}

CsvReader::CsvReader(const CsvReader& csvReader)
{
    this->_pCsvFile = new QFile(csvReader._pCsvFile->fileName());
    this->_dataGrid.clear();
    //使用了QVector重载的赋值运算符(深拷贝)
    this->_dataGrid = csvReader._dataGrid;

}

/**
 * @brief CsvReader::operator = >> 重载赋值运算符
 * @param csvReader
 * @return
 */
CsvReader& CsvReader::operator = (const CsvReader& csvReader)
{
    if (this == &csvReader)
    {
        return *this;
    }
    else
    {
        this->_dataGrid.clear();
        //使用了QVector重载的赋值运算符(深拷贝)
        this->_dataGrid = csvReader._dataGrid;
        delete this->_pCsvFile;
        this->_pCsvFile = new QFile(csvReader._pCsvFile->fileName());
        return *this;
    }
}


/**
 * @brief CsvReader::setFilePath >> 设置csv文件路径
 * @param filePath
 */
bool CsvReader::setFilePath(QString filePath)
{
    QFileInfo fileInfo(filePath);
    if (fileInfo.suffix() == QString("csv"))
    {
        _pCsvFile->setFileName(filePath);
        return true;
    }
    else
    {
        return false;
    }
}


/**
 * @brief CsvReader::isInitFilePath >> 判断是否已经初始化文件路径
 * @return
 */
bool CsvReader::isInitFile()
{
    return !this->_pCsvFile->fileName().isEmpty();
}


/**
 * @brief CsvReader::analyze >> 解析csv文件到_dataGrid
 * @return
 */
bool CsvReader::analyze(char separator)
{
    if (_pCsvFile->open(QFile::ReadOnly))
    {
        QTextStream in(_pCsvFile);
        while (!in.atEnd())
        {
            QString dataStr = in.readLine();
            //允许读入空白符
            QStringList dataList = dataStr.split(separator);
            _dataGrid.push_back(dataList);
        }
        _pCsvFile->close();
        return true;
    }
    return false;
}


/**
 * @brief CsvReader::getColumnByTitle >> 根据首行标题获取csv文件的目标列
 * @param columnTitle
 * @return
 */
QStringList CsvReader::getColumnByTitle(QString columnTitle)
{
    QStringList columnData;
    columnData.clear();

    //判断二维数组是否有内容
    if (!_dataGrid.isEmpty())
    {
        //读取第一行,二维数组的第一行全部都是列名称
        QStringList titleRow = _dataGrid.first();
        //遍历该行,寻找目标列
        int col = -1;
        for (int i = 0; i < titleRow.size(); i++)
        {
            //如果找到该列,则记下该列的列数字
            if (titleRow[i] == columnTitle)
            {
                col = i;
                break;
            }
        }

        //取得该列所有数据(包括标题)
        if (col != -1)
        {
            for (int row = 0; row < _dataGrid.size(); row++)
            {
                columnData.push_back(_dataGrid[row][col]);
            }
        }
    }
    return columnData;
}


/**
 * @brief CsvReader::save >> 将csvReader中的内容保存到文件
 * @return
 */
bool CsvReader::save()
{
    if(_pCsvFile->open(QFile::WriteOnly))
    {
        QTextStream out(_pCsvFile);
        for (int row = 0; row < _dataGrid.size(); row++)
        {
            for (int column = 0; column < _dataGrid[row].size(); column++)
            {
                if (column == _dataGrid[row].size() - 1)
                {
                    out << _dataGrid[row][column];
                }
                else
                {
                    out << _dataGrid[row][column] << ",";
                }
            }
            out << endl;
        }
        _pCsvFile->close();
        return true;
    }
    else
    {
        qDebug() << QString("Can't save file(%1), it may be used by other program!").arg(_pCsvFile->fileName());
        return false;
    }
}


/**
 * @brief CsvReader::pushBackColumnData >> 向_dataGrid后插入一列数据
 * @param title: 列名称
 * @param columnData: 列数据
 */
void CsvReader::pushBackColumnData(QString title, QStringList columnData)
{
    if (_dataGrid.isEmpty())
    {
        QStringList titleRow;
        titleRow << title;
        _dataGrid.push_back(titleRow);
        for (int row = 0; row < columnData.size(); row++)
        {
            QStringList dataRow;
            dataRow << columnData[row];
            _dataGrid.push_back(dataRow);
        }
    }
    else
    {
        _dataGrid[0] << title;
        int curRowNum = _dataGrid.size();
        int curColNum = _dataGrid.first().size();
        int row = 1;
        if (curRowNum >= columnData.size())
        {
            for (row; row < _dataGrid.size(); row++)
            {
                if (row - 1 < columnData.size())
                {
                    _dataGrid[row] << columnData[row - 1];
                }
                else
                {
                    _dataGrid[row] << "";
                }
            }
        }
        else
        {
            for (row; row < columnData.size(); row++)
            {
                if (row < curRowNum)
                {
                    _dataGrid[row] << columnData[row - 1];
                }
                else
                {
                    QStringList newRow;
                    for (int i = 0; i < curColNum; i++)
                    {
                        newRow << "";
                    }
                    newRow[curColNum - 1] = columnData[row - 1];
                    _dataGrid.push_back(newRow);
                }
            }
        }
    }
}
