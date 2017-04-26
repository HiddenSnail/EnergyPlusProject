#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include "global.h"
#include "handlemachine.h"

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
private:
    /* sourceFile 的计算操作 */
    //计算设备的能耗量
    QStringList calElecEqtWatts(QStringList oldDataList);
    //计算照明的能耗量
    QStringList calLightsWatts(QStringList oldDataList);

    /* nrFile 的计算操作 */
    QStringList calSchComCoolNr(QStringList oldDataList);
    QStringList calSchComHeatNr(QStringList oldDataList);

    /* rFile 的计算操作 */
    QStringList calSchComCoolR(QStringList oldData);
    QStringList calSchComHeatR(QStringList oldData);

    /* rpFile 的计算操作 */
    QStringList calSchComCoolRp(QStringList oldDataList);
    QStringList calSchComHeatRp(QStringList oldDataList);

    //计算结果
    void calResult();

    //开始调用Eplus输出文件
    void callEplus() { emit lSignal(); }

    //分离 models 的步骤
    void lStep();

    //models 分离完成后的步骤
    void zStep();
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

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void init();
};

typedef QStringList (MainWindow::*MF)(QStringList);

#endif // MAINWINDOW_H
