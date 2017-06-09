#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QDialog>
#include <QProgressDialog>
#include <QProgressBar>
#include <QMessageBox>
#include <QTimer>
#include "./global/stdafx.h"
#include "./utils/csvreader/csvreader.h"
#include "handlemachine.h"
#include "energyform.h"
#include "set_rate_diaglog.h"
#include "ephandler.h"

#define MIN_WIDTH 1200
#define MIN_HEIGHT 650

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

    void cancelCal(); //取消计算信号
private:
    //初始化函数
    void init();
    //初始化控件状态函数
    void initWidgetState();
    //初始化控件的验证器
    void initWidgetValidator();
    //初始化CoreData
    void initCoreData();
    //清除之前的数据
    void clear();



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

    void on_radioButton_sec6_keepTempOffset_toggled(bool checked);
    void on_radioButton_sec6_keepTempSet_toggled(bool checked);

    //更新房间总量
    void updateRoomNumber();


public:
    explicit MainWindow(QWidget *parent = 0);
    enum Language { Chinese, English };
    //分为4个地区(1->0.38, 2->0.5, 3->0.8, 4->1.5)
    enum City { Harbin = 1, Beijing = 2, Lanzhou = 2, XiAn = 2,
                Chengdu = 3, Chongqing = 3, Hangzhou = 3, Nanchang = 3,
                Nanjing = 3, Shanghai = 3, Wuhan = 3,
                Guangzhou = 4, Guiyang = 4, Shenzhen = 4, Xiamen = 4 };
    ~MainWindow();
    void setLanguage(Language lang = Chinese);

private:
    Ui::MainWindow *ui;
    QProgressDialog *pdlg;
    QTimer *timer;
    SetRateDialog *_pRateDialog;
    Language _language;

    //CoreData
private:
    QMap<int, double> _heatProNumMap; //区域与隔热系数的对应关系
    QMap<QString, City> _citySecMap; //城市区域对应关系(English-City)
    QMap<QString, QString> _cityNameMap; //在不同语言下的城市对应关系(other-English)
    QString _city; //城市英文名
    QVector<int> _roomSizeVec; //可选房间面积数组
    QMap<int, double> _coolLoadReducePercentMap; //空调制冷下限温度与冷负荷百分比缩减关系
    QMap<int, double> _heatLoadReducePercentMap; //空调供热上限温度与热负荷百分比缩减关系
    QStringList _noticeList; //所有已翻译的提示信息列表

    //ImportantData
private:
    int _roomSize; //房间面积
    int _sumRoomNum; //总房间数
    int _noRentRoomNum; //待租房间数
    QVector<int> _rentNoPeopleRoomNumVec; //24小时,每小时的已租无人房间数组
    QVector<int> _rentPeopleRoomNumVec; //24小时, 每小时的已租有人房间数组
};

#endif // MAINWINDOW_H
