#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include "global.h"
#include "handlemachine.h"
#include "csvreader.h"
#include "energyform.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
signals:
    void dataPackage(int data);
    void lSignal(); //model分离前的配置动作信号
    void zSignal(); //model分离后的配置动作信号
    void mSignal(); //节能率计算信号
private:
    //初始化函数
    void init();

    //清除之前的数据
    void clear()
    {
        _sumRoomNum = 0;
        _noRentRoomNum = 0;
        _rentNoPeopleRoomNumVec.clear();
        _rentPeopleRoomNumVec.clear();
    }

    /* sourceFile */
    //计算设备的能耗量
    QStringList calElecEqtWatts(QStringList oldDataList);
    //计算照明的能耗量
    QStringList calLightsWatts(QStringList oldDataList);

    /* sourceNopeFile */
    QStringList calElecEqtWattsNope(QStringList oldDataList)
    {
        QStringList dataList;
        dataList << QString::number(0);
        return dataList;
    }

    QStringList calLightsWattsNope(QStringList oldDataList)
    {
        QStringList dataList;
        dataList << QString::number(0);
        return dataList;
    }

    /* proposedFile */
    QStringList calSchComCoolNr(QStringList oldDataList);
    QStringList calSchComHeatNr(QStringList oldDataList);

    QStringList calSchComCoolR(QStringList oldData);
    QStringList calSchComHeatR(QStringList oldData);

    QStringList calSchComCoolRp(QStringList oldDataList);
    QStringList calSchComHeatRp(QStringList oldDataList);

    //检查用户的输入并准备关键数据
    void checkUserInput();

    //计算房间冷热负荷以及风机功率
    void calRoomLoadAndFanWatts(EnergyForm &baseForm, EnergyForm &proposedForm);

    //叠加负荷或功率
    QStringList compose(const QVector<int> &fourSizeRoomNum, const QVector<int> &perHourRoomNum, QVector<QStringList> &dataGrid);

    //计算房间的设备能耗以及照明能耗
    void calDeviceAndLightEnergy(EnergyForm &baseForm, EnergyForm &proposedForm);

    //计算房间除设备能耗和照明能耗的其他能耗
    void calRoomRestEnergy(EnergyForm &form);

    //开始调用Eplus输出文件
    void callEplus() { emit lSignal(); }

    //分离 models 的步骤
    void lStep();

    //models 分离完成后的步骤
    void zStep();

    //节能率计算的步骤
    void mStep();
private slots:
    void on_btn_start_clicked();

    void on_radioButton_sec2_year_toggled(bool checked);

    void on_radioButton_sec2_quarter_toggled(bool checked);

    void on_radioButton_sec2_month_toggled(bool checked);

    void on_radioButton_sec4_useCard_toggled(bool checked);

    void on_radioButton_sec4_useCardAndId_toggled(bool checked);

    void on_checkBox_sec6_keepHeat_toggled(bool checked);

    void on_checkBox_sec6_airconTempSet_toggled(bool checked);

    void on_checkBox_sec6_nightSETT_toggled(bool checked);

    void on_radioButton_sec6_ETM_toggled(bool checked);

    void on_radioButton_sec6_keepHeatNR_toggled(bool checked);

    void on_radioButton_sec6_newWind_toggled(bool checked);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QMap<QString, double> _cityMap;

    bool isCheckRoomState;

    int _sumRoomNum; //总房间数
    int _noRentRoomNum; //待租房间数
    QVector<int> _rentNoPeopleRoomNumVec; //24小时,每小时的已租无人房间数组
    QVector<int> _rentPeopleRoomNumVec; //24小时, 每小时的已租有人房间数组
};

#endif // MAINWINDOW_H
