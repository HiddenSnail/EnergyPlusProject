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
    void weahter(QString weather);

private slots:
    void on_btn_start_clicked();

    void on_checkBox_sec2_year_stateChanged(int arg1);

    void on_checkBox_sec2_quarter_stateChanged(int arg1);

    void on_checkBox_sec2_day_stateChanged(int arg1);

    void on_checkBox_sec4_useCard_stateChanged(int arg1);

    void on_checkBox_sec4_useCardAndId_stateChanged(int arg1);

    void on_checkBox_sec6_keepHeatNR_stateChanged(int arg1);

    void on_checkBox_sec6_newWind_stateChanged(int arg1);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void init();
};

#endif // MAINWINDOW_H
