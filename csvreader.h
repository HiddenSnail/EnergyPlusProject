#ifndef CSVREADER_H
#define CSVREADER_H
#include <QtCore>

class CsvReader {
private:
    QVector<QStringList> _dataGrid;
    QFile _csvFile;
public:
    CsvReader() {}
    CsvReader(const CsvReader& csvReader)
    {
        for (int i = 0; i < csvReader._dataGrid.size(); i++) {
            _dataGrid.push_back(csvReader._dataGrid[i]);
        }
        _csvFile.setFileName(csvReader._csvFile.fileName());
    }

    explicit CsvReader(QString csvFilePath)
    {
        QFileInfo fileInfo(csvFilePath);
        if (fileInfo.suffix() == QString("csv")) {
            _csvFile.setFileName(csvFilePath);
            if (_csvFile.exists()) {
                qDebug() << "Csv instance success!";
            } else {
                _csvFile.setFileName("");
                qDebug() << "Csv instance fail!";
            }
        } else {
            qDebug() << "File format error !";
        }
    }

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

    QStringList getColumnByTitle(QString columnTitle)
    {
        QStringList column;
        if (_dataGrid.size() != 0) {
            QStringList titleRow = _dataGrid[0];
            int targetColumn = -1;
            //寻找目标列
            for (int i = 0; i < titleRow.size(); i++) {
                if (titleRow[i] == columnTitle) {
                    targetColumn = i;
                    break;
                }
            }

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
