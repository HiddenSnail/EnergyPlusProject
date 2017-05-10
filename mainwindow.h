#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QDialog>
#include <QProgressDialog>
#include <QProgressBar>
#include <QMessageBox>
#include <QTimer>
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

    void model_base_over(); //baseline模型处理完毕
    void model_nr_over(); //nr模型处理完毕
    void model_r_over(); //r模型处理完毕
    void model_rp_over(); //rp模型处理完毕
    void fetchResult(double result); //获取结果信号
private:
    //初始化函数
    void init();

    //清除之前的数据
    void clear();

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

    /* sourceFile && sourceNopeFile */
    //计算时间跨度
    QStringList calTimeSpan(QStringList oldDataList);

    /* proposedFile */
    QStringList calSchComCoolNr(QStringList oldDataList);
    QStringList calSchComHeatNr(QStringList oldDataList);

    QStringList calSchComCoolR(QStringList oldData);
    QStringList calSchComHeatR(QStringList oldData);

    QStringList calSchComCoolRp(QStringList oldDataList);
    QStringList calSchComHeatRp(QStringList oldDataList);

    //检查用户的输入
    bool checkUserInput();

    //准备关键数据
    void preImpData();

    //计算房间冷热负荷以及风机功率
    void calRoomLoadAndFanWatts(EnergyForm &baseForm, EnergyForm &proposedForm);

    //叠加负荷或功率
    QStringList compose(const QVector<int> &fourSizeRoomNum, const QVector<int> &perHourRoomNum, QVector<QStringList> &dataGrid);

    //根据档位修正风机功率
    QStringList fixFanWatts(const QStringList fanWattsList);

    //计算房间的设备能耗以及照明能耗
    void calDeviceAndLightEnergy(EnergyForm &baseForm, EnergyForm &proposedForm);

    //计算房间除设备能耗和照明能耗的其他能耗
    void calRoomRestEnergy(EnergyForm &form, bool isInheritCoreData = false);

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

    void on_radioButton_sec4_useCard_toggled(bool checked);

    void on_radioButton_sec4_useCardAndId_toggled(bool checked);

    void on_checkBox_sec6_keepHeat_toggled(bool checked);

    void on_checkBox_sec6_airconTempSet_toggled(bool checked);

    void on_checkBox_sec6_nightSETT_toggled(bool checked);

    void on_radioButton_sec6_ETM_toggled(bool checked);

    void on_radioButton_sec6_keepHeatNR_toggled(bool checked);

    void on_radioButton_sec6_newWind_toggled(bool checked);

    //更新房间总量
    void updateRoomNumber();
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QProgressDialog *pdlg;

private:
    QMap<QString, double> _cityMap; //城市与隔热系数的对应关系
    QMap<QString, QString> _cityNameMap; //在不同语言下的城市对应关系
    QString _city; //城市名称(英文)
    QVector<int> _roomSizeVec;

    int _roomSize; //房间面积
    int _sumRoomNum; //总房间数
    int _noRentRoomNum; //待租房间数
    QVector<int> _rentNoPeopleRoomNumVec; //24小时,每小时的已租无人房间数组
    QVector<int> _rentPeopleRoomNumVec; //24小时, 每小时的已租有人房间数组
};

#endif // MAINWINDOW_H
