#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    QRegExp percent0To100Reg("100|(([0]|[1-9]{1,2})[\\.][0-9]{1,2})"); //识别0.00~100.00的浮点数
    QRegExp percent0To1Reg("1|([0][\\.][0-9]{1,2})"); //识别0.00~1.00的浮点数
    //section1
    ui->edit_sec1_size->setValidator(new QIntValidator(20, 1000000, this));
    ui->edit_sec1_heatProNum->setValidator(new QRegExpValidator(percent0To1Reg, this));
    ui->edit_sec1_roomNum->setValidator(new QIntValidator(1, 1000000, this));
    ui->edit_sec1_singleRoomNum->setValidator(new QIntValidator(1, 1000000, this));
    ui->edit_sec1_doubleRoomNum->setValidator(new QIntValidator(1, 1000000, this));
    ui->edit_sec1_suiteRoomNum->setValidator(new QIntValidator(1, 1000000, this));
    ui->edit_sec1_eastRoomNum->setValidator(new QIntValidator(1, 1000000, this));
    ui->edit_sec1_southRoomNum->setValidator(new QIntValidator(1, 1000000, this));
    ui->edit_sec1_westRoomNum->setValidator(new QIntValidator(1, 1000000, this));
    ui->edit_sec1_northRoomNum->setValidator(new QIntValidator(1, 1000000, this));

    //section2
    ui->checkBox_sec2_year->setChecked(true);
    ui->edit_sec2_year->setEnabled(true);
    ui->edit_sec2_quarter->setEnabled(false);
    ui->edit_sec2_day->setEnabled(false);
    ui->edit_sec2_year->setValidator(new QRegExpValidator(percent0To100Reg, this));
    ui->edit_sec2_quarter->setValidator(new QRegExpValidator(percent0To100Reg, this));
    ui->edit_sec2_day->setValidator(new QRegExpValidator(percent0To100Reg, this));
    ui->edit_sec2_inRoomRate->setValidator(new QIntValidator(0, 24, this));

    //section3
    ui->checkBox_sec3_CO_keep->setChecked(true);
    ui->checkBox_sec3_LR_keep->setChecked(true);

    //section4
    ui->checkBox_sec4_noCard->setChecked(true);
    ui->edit_sec4_UC_noCardNum->setEnabled(false);
    ui->edit_sec4_UCI_noCardNum->setEnabled(false);
    ui->edit_sec4_UC_noCardNum->setValidator(new QRegExpValidator(percent0To1Reg, this));
    ui->edit_sec4_UCI_noCardNum->setValidator(new QRegExpValidator(percent0To1Reg, this));

    //section5
    ui->checkBox_sec5_4pip->setChecked(true);
    ui->checkBox_sec5_hmlMachine->setChecked(true);

    //section6
    ui->checkBox_sec6_keepHeatNR->setChecked(true);
    ui->edit_sec6_averUsingTime->setEnabled(false);
    ui->edit_sec6_keepHeatTempSetNR->setValidator(new QIntValidator(0, 100, this));
    ui->edit_sec6_averUsingTime->setValidator(new QIntValidator(0, 24, this));

    //section7
    ui->edit_sec7_light->setValidator(new QIntValidator(0, 1000000, this));
    ui->edit_sec7_lightUsingNum->setValidator(new QRegExpValidator(percent0To1Reg, this));
    ui->edit_sec7_TV->setValidator(new QIntValidator(0, 1000000, this));
    ui->edit_sec7_lightUsingNum->setValidator(new QRegExpValidator(percent0To1Reg, this));
    ui->edit_sec7_fridge->setValidator(new QIntValidator(0, 1000000, this));
    ui->edit_sec7_otherDevice->setValidator(new QIntValidator(0, 1000000, this));
}


void MainWindow::on_btn_start_clicked()
{
//    int size = ui->edit_sec1_size->;
}

void MainWindow::on_checkBox_sec2_year_stateChanged(int arg1)
{
    QLineEdit *edit = ui->edit_sec2_year;
    if (arg1 == Qt::Checked) {
        edit->setEnabled(true);
    } else {
        edit->setEnabled(false);
    }
}

void MainWindow::on_checkBox_sec2_quarter_stateChanged(int arg1)
{
    QLineEdit *edit = ui->edit_sec2_quarter;
    if (arg1 == Qt::Checked) {
        edit->setEnabled(true);
    } else {
        edit->setEnabled(false);
    }
}

void MainWindow::on_checkBox_sec2_day_stateChanged(int arg1)
{
    QLineEdit *edit = ui->edit_sec2_day;
    if (arg1 == Qt::Checked) {
        edit->setEnabled(true);
    } else {
        edit->setEnabled(false);
    }
}

void MainWindow::on_checkBox_sec4_useCard_stateChanged(int arg1)
{
    QLineEdit *edit = ui->edit_sec4_UC_noCardNum;
    if (arg1 == Qt::Checked) {
        edit->setEnabled(true);
    } else {
        edit->setEnabled(false);
    }
}

void MainWindow::on_checkBox_sec4_useCardAndId_stateChanged(int arg1)
{
    QLineEdit *edit = ui->edit_sec4_UCI_noCardNum;
    if (arg1 == Qt::Checked) {
        edit->setEnabled(true);
    } else {
        edit->setEnabled(false);
    }
}

void MainWindow::on_checkBox_sec6_keepHeatNR_stateChanged(int arg1)
{
    QLineEdit *edit = ui->edit_sec6_keepHeatTempSetNR;
    QComboBox *comboBox = ui->comboBox_sec6_keepHeatWindSetNR;
    QTimeEdit *sTimeEdit = ui->timeEdit_sec6_startTimeNR;
    QTimeEdit *eTimeEdit = ui->timeEdit_sec6_endTimeNR;
    if (arg1 == Qt::Checked) {
        edit->setEnabled(true);
        comboBox->setEnabled(true);
        sTimeEdit->setEnabled(true);
        eTimeEdit->setEnabled(true);
    } else {
        edit->setEnabled(false);
        comboBox->setEnabled(false);
        sTimeEdit->setEnabled(false);
        eTimeEdit->setEnabled(false);
    }
}

void MainWindow::on_checkBox_sec6_newWind_stateChanged(int arg1)
{
    QLineEdit *edit = ui->edit_sec6_averUsingTime;
    if (arg1 == Qt::Checked) {
        edit->setEnabled(true);
    } else {
        edit->setEnabled(false);
    }
}
