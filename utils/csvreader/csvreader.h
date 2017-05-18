#ifndef CSVREADER_H
#define CSVREADER_H
#include <QtCore>

class CsvReader {
private:
    QVector<QStringList> _dataGrid; //csv文件的二位数组
    QFile _csvFile; //csv文件
public:
    CsvReader() {}

    //拷贝构造函数
    CsvReader(const CsvReader& csvReader)
    {
        for (int i = 0; i < csvReader._dataGrid.size(); i++) {
            _dataGrid.push_back(csvReader._dataGrid[i]);
        }
        _csvFile.setFileName(csvReader._csvFile.fileName());
    }

    //重载=运算符
    CsvReader& operator = (const CsvReader& csvReader)
    {
        if (this == &csvReader) return *this;
        else {
            for (int i = 0; i < csvReader._dataGrid.size(); i++) {
                _dataGrid.push_back(csvReader._dataGrid[i]);
            }
            _csvFile.setFileName(csvReader._csvFile.fileName());
            return *this;
        }
    }

    //带一个参数的构造函数
    explicit CsvReader(QString filePath)
    {
        QFileInfo fileInfo(filePath);
        if (fileInfo.suffix() == QString("csv")) {
            _csvFile.setFileName(filePath);
        } else {
            _csvFile.setFileName("");
            qDebug() << "File format error !";
        }
    }

    //将csvReader中的内容保存到文件
    bool save()
    {
        if(_csvFile.open(QFile::WriteOnly))
        {
            QTextStream out(&_csvFile);
            for (int row = 0; row < _dataGrid.size(); row++) {
                for (int column = 0; column < _dataGrid[row].size(); column++) {
                    if (column == _dataGrid[row].size() - 1) {
                        out << _dataGrid[row][column];
                    } else {
                        out << _dataGrid[row][column] << ",";
                    }
                }
                out << endl;
            }
            _csvFile.close();
            return true;
        } else {
            qDebug() << QString("Can't save file(%1), it may be used by other program!").arg(_csvFile.fileName());
            return false;
        }
    }

    //向_dataGrid插入一列数据
    void pushColumnData(QString title, QStringList columnData)
    {
        if (_dataGrid.isEmpty()) {
            QStringList titleRow;
            titleRow << title;
            _dataGrid.push_back(titleRow);
            for (int row = 0; row < columnData.size(); row++)
            {
                QStringList dataRow;
                dataRow << columnData[row];
                _dataGrid.push_back(dataRow);
            }
        } else {
            _dataGrid[0] << title;
            for (int row = 0; row < columnData.size(); row++)
            {
                _dataGrid[row + 1] << columnData[row];
            }
        }
    }

    void setFilePath(QString filePath)
    {
        QFileInfo fileInfo(filePath);
        if (fileInfo.suffix() == QString("csv")) {
            _csvFile.setFileName(filePath);
        } else {
            _csvFile.setFileName("");
            qDebug() << "File format error !";
        }
    }


    //解析csv文件到_dataGrid
    bool analyze()
    {
        if (_csvFile.open(QFile::ReadOnly)) {
            QTextStream in(&_csvFile);
            while(!in.atEnd()) {
                QString dataStr = in.readLine();
                QStringList dataList = dataStr.split(',', QString::SkipEmptyParts);
                _dataGrid.push_back(dataList);
            }
            _csvFile.close();
            return true;
        }
        return false;
    }

    //主要是这个函数
    //获取csv文件的目标列
    /**
     * @brief getColumnByTitle
     * @param columnTitle： 列名称
     * @return
     */
    QStringList getColumnByTitle(QString columnTitle)
    {
        QStringList column;
        //判断二维数组是否有内容
        if (_dataGrid.size() != 0) {
            //读取第一行,二维数组的第一行全部都是列名称
            QStringList titleRow = _dataGrid[0];
            int targetColumn = -1;
            //遍历该行,寻找目标列
            for (int i = 0; i < titleRow.size(); i++) {
                //如果找到该列,则记下该列的列数字
                if (titleRow[i] == columnTitle) {
                    targetColumn = i;
                    break;
                }
            }

            //取得该列所有数据
            if (targetColumn != -1) {
                for (int row = 0; row < _dataGrid.size(); row++) {
                    column.push_back(_dataGrid[row][targetColumn]);
                }
            }
        }
        return column;
    }
};

#endif // CSVREADER_H
