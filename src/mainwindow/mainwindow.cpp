#include "./include/mainwindow/mainwindow.h"
#include "./utils/custom_widget.hpp"
#include "./utils/custom_validator.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    pdlg(nullptr)

{
    connect(this, &MainWindow::lSignal, this, &MainWindow::lStep);
    connect(this, &MainWindow::zSignal, this, &MainWindow::zStep);
    connect(this, &MainWindow::mSignal, this, &MainWindow::mStep);
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    initCoreData();
    initWidgetState();
    initWidgetValidator();
    setLanguage();
}

/**
 * @brief MainWindow::initCoreData >> 初始化CoreData
 */
void MainWindow::initCoreData()
{
    //获取城市信息和隔热系数
    QFile cityProfile(PathManager::instance()->getPath("ProfileDir") + "/city_profile.json");
    if (!cityProfile.open(QFile::ReadOnly)) { qFatal("Can't read the city profile!"); }
    QTextStream cpStream(&cityProfile);
    cpStream.setCodec("UTF-8");
    QJsonDocument cpDoc = QJsonDocument::fromJson(cpStream.readAll().toUtf8());
    cityProfile.close();

    if (!cpDoc.isObject() || cpDoc.isEmpty()) { qFatal("The city profile file maybe broken!"); }
    QJsonObject cpRoot = cpDoc.object();
    QJsonArray cityArray = cpRoot["cityArray"].toArray();
    for (int i = 0; i < cityArray.size(); i++) {
        QString cityName = cityArray[i].toString();
        double heatProNum = cpRoot[cityName].toDouble();
        _cityMap.insert(cityName, heatProNum);
        _cityNameMap.insert(cityName, cityName);
    }



    //初始化_fanLevelMap
    _fanLevelMap.insert("high", "high");
    _fanLevelMap.insert("middle", "middle");
    _fanLevelMap.insert("low", "low");



    //获取房间可选面积数组
    QDir sourceDir(PathManager::instance()->getPath("SourceDir"));
    QFileInfoList fileInfoList = sourceDir.entryInfoList(QDir::Files);
    for (int i = 0; i < fileInfoList.size(); i++)
    {
        QFileInfo fileInfo = fileInfoList[i];
        int size = fileInfo.baseName().toInt();
        _roomSizeVec.push_back(size);
    }
    qSort(_roomSizeVec.begin(), _roomSizeVec.end(), [](int &a, int &b){
        return a < b;
    });



    //获取冷热负荷缩减百分比关系
    QFile loadReduceProfile(PathManager::instance()->getPath("ProfileDir") + "/load/loadReducePercent_profile.json");
    if (!loadReduceProfile.open(QFile::ReadOnly)) { qFatal("Can't read the loadReducePercent profile profile!"); }
    //lrp == loadReduceProfile的缩写
    QTextStream lrpStream(&loadReduceProfile);
    lrpStream.setCodec("UTF-8");
    QJsonDocument lrpDoc = QJsonDocument::fromJson(lrpStream.readAll().toUtf8());
    loadReduceProfile.close();

    if (!lrpDoc.isObject() || lrpDoc.isEmpty()) { qFatal("The loadReducePercent profile file maybe broken!"); }
    QJsonObject lrpRoot = lrpDoc.object();
    int coolTempTop = lrpRoot["coolTempTop"].toInt();
    int coolTempBottom = lrpRoot["coolTempBottom"].toInt();
    QJsonArray coolLoadReducePercentArray = lrpRoot["coolReducePercent"].toArray();
    for (int temp = coolTempBottom; temp <= coolTempTop; temp++)
    {
        double percent = coolLoadReducePercentArray[temp - coolTempBottom].toDouble();
        _coolLoadReducePercentMap.insert(temp, percent);
    }
    int heatTempTop = lrpRoot["heatTempTop"].toInt();
    int heatTempBottom = lrpRoot["heatTempBottom"].toInt();
    QJsonArray heatLoadReducePercentArray = lrpRoot["heatReducePercent"].toArray();
    for (int temp = heatTempBottom; temp <= heatTempTop; temp++)
    {
        double percent = heatLoadReducePercentArray[temp - heatTempBottom].toDouble();
        _heatLoadReducePercentMap.insert(temp, percent);
    }


}

/**
 * @brief MainWindow::initWidgetValidator >> 初始化控件的验证器
 */
void MainWindow::initWidgetValidator()
{
    //准备所需验证器正则表达式
    QRegExp float0To1Reg("^1|([0]+(\\.[0-9]{1,2})?)$"); //匹配0.00~1的数字
    QRegExp float0To100Reg("^100|(([0]|([1-9][0-9]{0,1}))(\\.[0-9]{1,2})?)$"); //匹配0.00~100的数字
    QRegExp float0To1000Reg("^1000|(([0]|([1-9][0-9]{0,2}))(\\.[0-9]{1,2})?)$"); //识别0.00~1000的数字



    //section1
    ui->edit_sec1_size->setValidator(new IntValidator(1, 10000, this));
    ui->edit_sec1_roomNum->setValidator(new IntValidator(0, 10000, this));
    ui->edit_sec1_eastRoomNum->setValidator(new IntValidator(0, 10000, this));
    ui->edit_sec1_southRoomNum->setValidator(new IntValidator(0, 10000, this));
    ui->edit_sec1_westRoomNum->setValidator(new IntValidator(0, 10000, this));
    ui->edit_sec1_northRoomNum->setValidator(new IntValidator(0, 10000, this));

    //section2
    ui->edit_sec2_year->setValidator(new QRegExpValidator(float0To100Reg, this));
    ui->edit_sec2_quarter->setValidator(new QRegExpValidator(float0To100Reg, this));
    ui->edit_sec2_inRoomRate->setValidator(new IntValidator(0, 24, this));

    //section4
    ui->edit_sec4_UC_noCardNum->setValidator(new QRegExpValidator(float0To1Reg, this));
    ui->edit_sec4_UCI_noCardNum->setValidator(new QRegExpValidator(float0To1Reg, this));

    //section6
    ui->edit_sec6_keepHeatTempSet->setValidator(new IntValidator(0, 100, this));
    IntValidator *heatValid, *coolValid;
    int coolTempBottom = *_coolLoadReducePercentMap.keyBegin();
    int coolTempTop = *--_coolLoadReducePercentMap.keyEnd();
    int heatTempBottom = *_heatLoadReducePercentMap.keyBegin();
    int heatTempTop = *--_heatLoadReducePercentMap.keyEnd();

    coolValid = new IntValidator(coolTempBottom, coolTempTop, this);
    heatValid = new IntValidator(heatTempBottom, heatTempTop, this);
    ui->edit_sec6_lowTemp->setValidator(coolValid);
    ui->edit_sec6_highTemp->setValidator(heatValid);
    connect(ui->edit_sec6_lowTemp, &QLineEdit::editingFinished, ui->edit_sec6_highTemp, [=](){
        int lowTemp = ui->edit_sec6_lowTemp->text().toInt();
        if (lowTemp > heatValid->bottom())
        {
            heatValid->setBottom(lowTemp);
            QString heatTempStr = ui->edit_sec6_highTemp->text();
            heatValid->fixup(heatTempStr);
            ui->edit_sec6_highTemp->setText(heatTempStr);
        }
        else { heatValid->setBottom(heatTempBottom); }
    });

    ui->edit_sec6_nightTempOffset->setValidator(new IntValidator(0, 30, this));
    ui->edit_sec6_keepTime->setValidator(new IntValidator(0, 24, this));
    ui->edit_sec6_keepHeatTempSetNR->setValidator(new IntValidator(0, 24, this));
    ui->edit_sec6_averUsingTime->setValidator(new IntValidator(0, 24, this));

    //section7
    ui->edit_sec7_light->setValidator(new QRegExpValidator(float0To1000Reg, this));
    ui->edit_sec7_lightUsingNum->setValidator(new QRegExpValidator(float0To1Reg, this));
    ui->edit_sec7_TV->setValidator(new QRegExpValidator(float0To1000Reg, this));
    ui->edit_sec7_TVUsingNum->setValidator(new QRegExpValidator(float0To1Reg, this));
    ui->edit_sec7_fridge->setValidator(new QRegExpValidator(float0To1000Reg, this));
    ui->edit_sec7_otherDevice->setValidator(new QRegExpValidator(float0To1000Reg, this));

}

/**
 * @brief MainWindow::initWidgetState >> 初始化控件的状态，并绑定相应的槽函数
 */
void MainWindow::initWidgetState()
{
    //section1
    QStringList items;
    for (auto cityName: _cityNameMap.keys()) {
        items << cityName;
    }
    QComboBox *cityBox = ui->comboBox_sec1_city;
    cityBox->clear();
    cityBox->addItems(items);
    _city = _cityNameMap[cityBox->currentText()];

    //设置隔热系数
    QLineEdit *heatProNum = ui->edit_sec1_heatProNum;
    heatProNum->setReadOnly(true);
    heatProNum->setText(QString::number(_cityMap[_city]));
    connect(cityBox, &QComboBox::currentTextChanged, [=]() {
        _city = _cityNameMap[cityBox->currentText()];
        heatProNum->setText(QString::number(_cityMap[_city]));
    });

    ui->edit_sec1_roomNum->setReadOnly(true);
    ui->edit_sec1_roomNum->setText("0");
    connect(ui->edit_sec1_eastRoomNum, &QLineEdit::editingFinished, this, &MainWindow::updateRoomNumber);
    connect(ui->edit_sec1_westRoomNum, &QLineEdit::editingFinished, this, &MainWindow::updateRoomNumber);
    connect(ui->edit_sec1_southRoomNum, &QLineEdit::editingFinished, this, &MainWindow::updateRoomNumber);
    connect(ui->edit_sec1_northRoomNum, &QLineEdit::editingFinished, this, &MainWindow::updateRoomNumber);


    //section2
    ui->radioButton_sec2_year->setChecked(true);
    ui->edit_sec2_year->setEnabled(true);
    ui->edit_sec2_quarter->setEnabled(false);
    ui->comboBox_sec2_quarter->setEnabled(false);


    //section3
    ui->radioButton_sec3_CO_keep->setChecked(true);
    ui->radioButton_sec3_LR_keep->setChecked(true);


    //section4
    ui->radioButton_sec4_noCard->setChecked(true);
    ui->edit_sec4_UC_noCardNum->setEnabled(false);
    ui->lab_sec4_UC_noCardNum->setEnabled(false);
    ui->edit_sec4_UCI_noCardNum->setEnabled(false);
    ui->lab_sec4_UCI_noCardNum->setEnabled(false);


    //section5
    ui->radioButton_sec5_4pip->setChecked(true);
    ui->radioButton_sec5_hmlMachine->setChecked(true);


    //section6
    on_checkBox_sec6_keepHeat_toggled(false);
    on_checkBox_sec6_airconTempSet_toggled(false);
    on_checkBox_sec6_nightSETT_toggled(false);
    ui->radioButton_sec6_keepHeatNR->setChecked(true);
    on_radioButton_sec6_keepHeatNR_toggled(true);
    on_radioButton_sec6_newWind_toggled(false);

    //section8
    ui->edit_sec8_year->setReadOnly(true);
    ui->edit_sec8_1To3Mon->setReadOnly(true);
    ui->edit_sec8_4To6Mon->setReadOnly(true);
    ui->edit_sec8_7To9Mon->setReadOnly(true);
    ui->edit_sec8_10To12Mon->setReadOnly(true);

}

void MainWindow::clear()
{
    _roomSize = 0;
    _sumRoomNum = 0;
    _noRentRoomNum = 0;
    _rentNoPeopleRoomNumVec.clear();
    _rentPeopleRoomNumVec.clear();
    ui->edit_sec8_1To3Mon->clear();
    ui->edit_sec8_4To6Mon->clear();
    ui->edit_sec8_7To9Mon->clear();
    ui->edit_sec8_10To12Mon->clear();
    ui->edit_sec8_year->clear();
}


/**----------------------------控件操作------------------------------**/

void MainWindow::on_btn_start_clicked()
{
    clear();
    if (checkUserInput()) {
        preImpData();
        callEplus();

        if (pdlg == nullptr) {
            pdlg = new CustomProgressDialog(ui->centralwidget);
            pdlg->setWindowFlags(windowFlags()&~Qt::WindowCloseButtonHint);
            //pdlg->setWindowTitle(QString::fromLocal8Bit("正在进行计算..."));
            pdlg->setWindowTitle(_noticeList[2]);
            pdlg->setFixedSize(ui->centralwidget->width()/2, 150);
            pdlg->setCancelButton(nullptr);
            pdlg->setMinimum(0);
            pdlg->setMaximum(100);
            timer = new QTimer(ui->centralwidget);
            connect(timer, &QTimer::timeout, [=](){
                int curValue = pdlg->value();
                switch (curValue) {
                case 0:
                {
                    //pdlg->setLabelText(QString::fromLocal8Bit("正在构建、配置模型..."));
                    pdlg->exec();
                    pdlg->setLabelText(_noticeList[3]);
                    pdlg->setValue(curValue+1);
                    break;
                }
                case 30:
                {
                    //pdlg->setLabelText(QString::fromLocal8Bit("正在处理模型..."));
                     pdlg->setLabelText(_noticeList[4]);
                     pdlg->setValue(curValue+1);
                     break;
                }
                case 99:
                {
                    //pdlg->setLabelText(QString::fromLocal8Bit("正在计算结果..."));
                     timer->stop();
                     pdlg->setLabelText(_noticeList[5]);
                     break;

                }
                default:
                {
                    pdlg->setValue(curValue+1);
                    break;
                }
                }
            });
            connect(this, &MainWindow::mSignal, [=](){
                timer->stop();
                pdlg->setValue(99);
            });
            connect(this, &MainWindow::fetchResult, pdlg, &QProgressDialog::close);
        }
        else
        {
            pdlg->setValue(0);
        }

        timer->start(500);
        pdlg->exec();
    }
    else
    {
        QMessageBox::warning(this,_noticeList[0],_noticeList[1]);
        qDebug() << "Not ready!";
    }
}


bool MainWindow::checkUserInput()
{
    //section1
    bool isSec1Ready = ui->edit_sec1_size->hasAcceptableInput() &&
            ui->edit_sec1_eastRoomNum->hasAcceptableInput() &&
            ui->edit_sec1_southRoomNum->hasAcceptableInput() &&
            ui->edit_sec1_westRoomNum->hasAcceptableInput() &&
            ui->edit_sec1_northRoomNum->hasAcceptableInput();

    //section2
    bool isSec2Ready = true;
    if (ui->radioButton_sec2_year->isChecked()) {
        isSec2Ready = isSec2Ready && ui->edit_sec2_year->hasAcceptableInput();
    } else {
        isSec2Ready = isSec2Ready && ui->edit_sec2_quarter->hasAcceptableInput();
    }
    isSec2Ready = isSec2Ready && ui->edit_sec2_inRoomRate->hasAcceptableInput();

    //section4
    bool isSec4Ready = true;
    if (ui->radioButton_sec4_useCard->isChecked()) {
        isSec4Ready = isSec4Ready && ui->edit_sec4_UC_noCardNum->hasAcceptableInput();
    } else if (ui->radioButton_sec4_useCardAndId->isChecked()) {
        isSec4Ready = isSec4Ready && ui->edit_sec4_UCI_noCardNum->hasAcceptableInput();
    } else {
        isSec4Ready = true;
    }

    //section6
    bool isSec6Ready = true;
    if (!ui->radioButton_sec6_ETM->isChecked()) {
        if (ui->checkBox_sec6_keepHeat->isChecked()) {
            isSec6Ready = isSec6Ready && ui->edit_sec6_keepHeatTempSet->hasAcceptableInput();
        }

        if (ui->checkBox_sec6_airconTempSet->isChecked()) {
            isSec6Ready = isSec6Ready && ui->edit_sec6_lowTemp->hasAcceptableInput()
                    && ui->edit_sec6_highTemp->hasAcceptableInput();
        }

        if (ui->checkBox_sec6_nightSETT->isChecked()) {
            isSec6Ready = isSec6Ready && ui->edit_sec6_keepTime->hasAcceptableInput()
                    && ui->edit_sec6_nightTempOffset->hasAcceptableInput();
        }
    } else {
        isSec6Ready = isSec6Ready && true;
    }

    if (ui->radioButton_sec6_keepHeatNR->isChecked()) {
        isSec6Ready = isSec6Ready && ui->edit_sec6_keepHeatTempSetNR->hasAcceptableInput();
    } else {
        isSec6Ready = isSec6Ready && ui->edit_sec6_averUsingTime->hasAcceptableInput();
    }

    //section7
    bool isSec7Ready = ui->edit_sec7_light->hasAcceptableInput()
            && ui->edit_sec7_lightUsingNum->hasAcceptableInput()
            && ui->edit_sec7_TV->hasAcceptableInput()
            && ui->edit_sec7_TVUsingNum->hasAcceptableInput()
            && ui->edit_sec7_fridge->hasAcceptableInput()
            && ui->edit_sec7_otherDevice->hasAcceptableInput();

    if (isSec1Ready && isSec2Ready && isSec4Ready && isSec6Ready && isSec7Ready) {
        ui->frame_1->setStyleSheet(".QFrame#frame_1{"\
                                   "background-color: rgb(255, 255, 255);"
                                   "}");
        ui->frame_2->setStyleSheet(".QFrame#frame_2{"\
                                   "background-color: rgb(255, 255, 255);"
                                   "}");
        ui->frame_4->setStyleSheet(".QFrame#frame_4{"\
                                   "background-color: rgb(255, 255, 255);"
                                   "}");
        ui->frame_6->setStyleSheet(".QFrame#frame_6{"\
                                   "background-color: rgb(255, 255, 255);"
                                   "}");
        ui->frame_7->setStyleSheet(".QFrame#frame_7{"\
                                   "background-color: rgb(255, 255, 255);"
                                   "}");
        return true;
    } else {
        if (!isSec1Ready)
        {
            ui->frame_1->setStyleSheet(".QFrame#frame_1{"\
                                       "background-color: rgb(255, 246, 236);"
                                       "}");
        }
        else
        {
            ui->frame_1->setStyleSheet(".QFrame#frame_1{"\
                                       "background-color: rgb(255, 255, 255);"
                                       "}");
        }

        if (!isSec2Ready)
        {
            ui->frame_2->setStyleSheet(".QFrame#frame_2{"\
                                       "background-color: rgb(255, 246, 236);"
                                       "}");
        }
        else
        {
            ui->frame_2->setStyleSheet(".QFrame#frame_2{"\
                                       "background-color: rgb(255, 255, 255);"
                                       "}");
        }

        if (!isSec4Ready)
        {
            ui->frame_4->setStyleSheet(".QFrame#frame_4{"\
                                       "background-color: rgb(255, 246, 236);"
                                       "}");
        }
        else
        {
            ui->frame_4->setStyleSheet(".QFrame#frame_4{"\
                                       "background-color: rgb(255, 255, 255);"
                                       "}");
        }

        if (!isSec6Ready)
        {
            ui->frame_6->setStyleSheet(".QFrame#frame_6{"\
                                       "background-color: rgb(255, 246, 236);"
                                       "}");
        }
        else
        {
            ui->frame_6->setStyleSheet(".QFrame#frame_6{"\
                                       "background-color: rgb(255, 255, 255);"
                                       "}");
        }

        if (!isSec7Ready)
        {
            ui->frame_7->setStyleSheet(".QFrame#frame_7{"\
                                       "background-color: rgb(255, 246, 236);"
                                       "}");
        }
        else
        {
            ui->frame_7->setStyleSheet(".QFrame#frame_7{"\
                                       "background-color: rgb(255, 255, 255);"
                                       "}");
        }
        return false;
    }
}


void MainWindow::on_radioButton_sec2_year_toggled(bool checked)
{
    if (checked) {
        ui->edit_sec2_year->setEnabled(true);
    } else {
        ui->edit_sec2_year->setEnabled(false);
    }
}

void MainWindow::on_radioButton_sec2_quarter_toggled(bool checked)
{
    if (checked) {
        ui->edit_sec2_quarter->setEnabled(true);
        ui->comboBox_sec2_quarter->setEnabled(true);
    } else {
        ui->edit_sec2_quarter->setEnabled(false);
        ui->comboBox_sec2_quarter->setEnabled(false);
    }
}

void MainWindow::on_radioButton_sec4_useCard_toggled(bool checked)
{
    if (checked) {
        ui->edit_sec4_UC_noCardNum->setEnabled(true);
        ui->lab_sec4_UC_noCardNum->setEnabled(true);
    } else {
        ui->edit_sec4_UC_noCardNum->setEnabled(false);
        ui->lab_sec4_UC_noCardNum->setEnabled(false);
    }
}

void MainWindow::on_radioButton_sec4_useCardAndId_toggled(bool checked)
{
    if (checked) {
        ui->edit_sec4_UCI_noCardNum->setEnabled(true);
        ui->lab_sec4_UCI_noCardNum->setEnabled(true);
    } else {
        ui->edit_sec4_UCI_noCardNum->setEnabled(false);
        ui->lab_sec4_UCI_noCardNum->setEnabled(false);
    }
}

void MainWindow::on_checkBox_sec6_keepHeat_toggled(bool checked)
{
    ui->lab_sec6_keepHeatTempSet->setEnabled(checked);
    ui->lab_sec6_keepHeatWindSet->setEnabled(checked);
    ui->edit_sec6_keepHeatTempSet->setEnabled(checked);
    ui->comboBox_sec6_keepHeatWindSet->setEnabled(checked);
    ui->lab_sec6_offset_2->setEnabled(checked);
}

void MainWindow::on_checkBox_sec6_airconTempSet_toggled(bool checked)
{
    ui->lab_sec6_lowTemp->setEnabled(checked);
    ui->lab_sec6_highTemp->setEnabled(checked);
    ui->edit_sec6_lowTemp->setEnabled(checked);
    ui->edit_sec6_highTemp->setEnabled(checked);
}

void MainWindow::on_checkBox_sec6_nightSETT_toggled(bool checked)
{
    ui->lab_sec6_nightStartTime->setEnabled(checked);
    ui->lab_sec6_keepTime->setEnabled(checked);
    ui->lab_sec6_nightTempOffset->setEnabled(checked);
    ui->timeEdit_sec6_nightStartTime->setEnabled(checked);
    ui->edit_sec6_keepTime->setEnabled(checked);
    ui->edit_sec6_nightTempOffset->setEnabled(checked);
    ui->lab_sec6_offset->setEnabled(checked);
}

void MainWindow::on_radioButton_sec6_ETM_toggled(bool checked)
{
    ui->checkBox_sec6_keepHeat->setEnabled(!checked);
    ui->checkBox_sec6_airconTempSet->setEnabled(!checked);
    ui->checkBox_sec6_nightSETT->setEnabled(!checked);
    if (checked) {
        on_checkBox_sec6_keepHeat_toggled(false);
        on_checkBox_sec6_airconTempSet_toggled(false);
        on_checkBox_sec6_nightSETT_toggled(false);
    } else {
        if (ui->checkBox_sec6_keepHeat->isChecked()) {
            on_checkBox_sec6_keepHeat_toggled(true);
        }
        if (ui->checkBox_sec6_airconTempSet->isChecked()) {
            on_checkBox_sec6_airconTempSet_toggled(true);
        }
        if (ui->checkBox_sec6_nightSETT->isChecked()) {
            on_checkBox_sec6_nightSETT_toggled(true);
        }
    }
}

void MainWindow::on_radioButton_sec6_keepHeatNR_toggled(bool checked)
{
    ui->lab_sec6_keepHeatTempSetNR->setEnabled(checked);
    ui->lab_sec6_keepHeatWindSetNR->setEnabled(checked);
    ui->lab_sec6_offset_3->setEnabled(checked);
    ui->edit_sec6_keepHeatTempSetNR->setEnabled(checked);
    ui->comboBox_sec6_keepHeatWindSetNR->setEnabled(checked);
}

void MainWindow::on_radioButton_sec6_newWind_toggled(bool checked)
{
    ui->lab_sec6_averUsingTime->setEnabled(checked);
    ui->edit_sec6_averUsingTime->setEnabled(checked);
}

void MainWindow::updateRoomNumber()
{
    int number = 0;
    if (!ui->edit_sec1_eastRoomNum->text().isEmpty()) {
        number += ui->edit_sec1_eastRoomNum->text().toInt();
    }
    if (!ui->edit_sec1_westRoomNum->text().isEmpty()) {
        number += ui->edit_sec1_westRoomNum->text().toInt();
    }
    if (!ui->edit_sec1_southRoomNum->text().isEmpty()) {
        number += ui->edit_sec1_southRoomNum->text().toInt();
    }
    if (!ui->edit_sec1_northRoomNum->text().isEmpty()) {
        number += ui->edit_sec1_northRoomNum->text().toInt();
    }
    ui->edit_sec1_roomNum->setText(QString::number(number));
}


/**----------------------------计算操作------------------------------**/

/**
 * @brief MainWindow::calElecEqtWatts
 * @param oldDataList: [每平米设备功率]
 * @return
 */
QStringList MainWindow::calElecEqtWatts(QStringList oldDataList)
{   
    //电视的总瓦特数(kW)
    double tvWatts = ui->edit_sec7_TV->text().toDouble();
    //电视使用系数(占时不使用)
    //double tvNum = ui->edit_sec7_TVUsingNum->text().toDouble();
    //冰箱的总瓦特数(kW)
    double fdgWatts = ui->edit_sec7_fridge->text().toDouble();
    //其他设备的总平均瓦特数(kW)
    double otherWatts = ui->edit_sec7_otherDevice->text().toDouble();

    double averageWatts = (tvWatts + fdgWatts + otherWatts)*1000.0/_roomSize;
    QStringList dataList;
    dataList << QString::number(averageWatts, 'f', 2);
    return dataList;
}

/**
 * @brief MainWindow::calLightsWatts
 * @param oldDataList: [每平米光照功率]
 * @return
 */
QStringList MainWindow::calLightsWatts(QStringList oldDataList)
{
    //照明总瓦特数(kW)
    double lightWatts = ui->edit_sec7_light->text().toDouble();
    //照明系数(占时不使用)
    //double lightNum = ui->edit_sec7_light->text().toDouble();
    //房间面积
    double averageWatts = lightWatts*1000.0/_roomSize;

    QStringList dataList;
    dataList << QString::number(averageWatts, 'f', 2);
    return dataList;
}

/**
 * @brief MainWindow::calTimeSpan
 * @param oldDataList: [起始月份，起始日期，结束月份，结束日期]
 * @return
 */
QStringList MainWindow::calTimeSpan(QStringList oldDataList)
{
    QStringList dataList;
    if (ui->radioButton_sec2_year->isChecked()) {
        dataList << QString::number(1) << QString::number(1) << QString::number(12) << QString::number(31);
    } else {
        int quarter = ui->comboBox_sec2_quarter->currentText().toInt();
        switch (quarter) {
        case 1:
        {
            dataList << QString::number(1) << QString::number(1) << QString::number(3) << QString::number(31);
            break;
        }
        case 2:
        {
            dataList << QString::number(4) << QString::number(1) << QString::number(6) << QString::number(30);
            break;
        }
        case 3:
        {
            dataList << QString::number(7) << QString::number(1) << QString::number(9) << QString::number(30);
            break;
        }
        default:
        {
            dataList << QString::number(10) << QString::number(1) << QString::number(12) << QString::number(31);
            break;
        }
        }
    }
    return dataList;
}


/**
 * @brief MainWindow::calSchComCoolNr
 * @param oldDataList: [保温温度，保温温度，保温温度]
 * @return
 */
QStringList MainWindow::calSchComCoolNr(QStringList oldDataList)
{
    QStringList dataList(oldDataList);
    if (ui->radioButton_sec6_keepHeatNR->isChecked()) {
        int tempOffset = ui->edit_sec6_keepHeatTempSetNR->text().toInt();
        for (int i = 0; i < dataList.size(); i++) {
            int temp = dataList[i].toInt();
            dataList[i] = QString::number(temp + tempOffset);
        }
    }
    return dataList;
}

/**
 * @brief MainWindow::calSchComHeatNr
 * @param oldDataList: [保温温度，保温温度，保温温度]
 * @return
 */
QStringList MainWindow::calSchComHeatNr(QStringList oldDataList)
{
    QStringList dataList(oldDataList);
    if (ui->radioButton_sec6_keepHeatNR->isChecked()) {
        int tempOffset = ui->edit_sec6_keepHeatTempSetNR->text().toInt();
        for (int i = 0; i < dataList.size(); i++) {
            int temp = dataList[i].toInt();
            dataList[i] = QString::number(temp - tempOffset);
        }
    }
    return dataList;
}

/**
 * @brief MainWindow::calSchComCoolR
 * @param oldDataList: [保温温度，保温温度，保温温度]
 * @return
 */
QStringList MainWindow::calSchComCoolR(QStringList oldDataList)
{
    QStringList dataList(oldDataList);
    if (ui->checkBox_sec6_keepHeat->isChecked()) {
        int tempOffset = ui->edit_sec6_keepHeatTempSet->text().toInt();
        for (int i = 0; i < dataList.size(); i++) {
            int temp = dataList[i].toInt();
            dataList[i] = QString::number(temp + tempOffset);
        }
    }
    return dataList;
}

/**
 * @brief MainWindow::calSchComHeatR
 * @param oldDataList: [保温温度，保温温度，保温温度]
 * @return
 */
QStringList MainWindow::calSchComHeatR(QStringList oldDataList)
{
    QStringList dataList(oldDataList);
    if (ui->checkBox_sec6_keepHeat->isChecked()) {
        int tempOffset = ui->edit_sec6_keepHeatTempSet->text().toInt();
        for (int i = 0; i < dataList.size(); i++) {
            int temp = dataList[i].toInt();
            dataList[i] = QString::number(temp - tempOffset);
        }
    }
    return dataList;
}

/**
 * @brief MainWindow::calSchComCoolRp
 * @param oldList: [Until: 时间(headTime)，温度，Until: 时间(tailTime)，温度，Until: 24， 温度]
 * @return
 */
QStringList MainWindow::calSchComCoolRp(QStringList oldDataList)
{
    QStringList dataList(oldDataList);
    if (ui->checkBox_sec6_nightSETT->isChecked()) {
        int tempOffset = ui->edit_sec6_nightTempOffset->text().toInt();
        int temp = dataList[1].toInt();
        //获取headTime和tailTime
        QRegularExpression reg("([2]{1}[0-3]{1}:[0-6]{1}[0-9]{1}|[1]{0,1}[0-9]{1}:[0-6]{1}[0-9]{1})");
        QRegularExpressionMatch result1, result2;
        QString headTimeStr, tailTimeStr;
        if (dataList[0].contains(reg, &result1) && dataList[2].contains(reg, &result2)) {
            headTimeStr = result1.captured(1);
            tailTimeStr = result2.captured(1);
        } else {
            qFatal("Some mistake happen when handle rp model!");
        }
        QTime headTime = QTime::fromString(headTimeStr, "H:mm");
        QTime tailTime = QTime::fromString(tailTimeStr, "H:mm");
        QTime sTime = ui->timeEdit_sec6_nightStartTime->time();
        int keepTime = ui->edit_sec6_keepTime->text().toInt();
        if (keepTime < 24)
        {
            if (QTime(0, 0) == sTime)
            {
                QTime startTime = sTime;
                QTime endTime(keepTime, 0);
                if (endTime < tailTime)
                {
                    dataList[0] = QString("Until: %1").arg(endTime.toString("H:mm"));
                    dataList[1] = QString::number(temp + tempOffset);
                }
                else
                {
                    dataList[1] = QString::number(temp + tempOffset);
                    dataList[2] = QString("Until: %1").arg(endTime.toString("H:mm"));
                    dataList[3] = QString::number(temp + tempOffset);
                }
            }
            else
            {
                int totalHours = sTime.hour() + keepTime;
                QTime startTime = sTime;
                QTime endTime((startTime.hour()+keepTime)%24, startTime.minute());
                if (totalHours > 24 || (totalHours == 24 && sTime.minute() > 0))
                {
                    dataList[0] = QString("Until: %1").arg(endTime.toString("H:mm"));
                    dataList[1] = QString::number(temp + tempOffset);
                    dataList[2] = QString("Until: %1").arg(startTime.toString("H:mm"));
                    dataList[5] = QString::number(temp + tempOffset);
                }
                else if (totalHours < 24)
                {
                    dataList[0] = QString("Until: %1").arg(startTime.toString("H:mm"));
                    dataList[2] = QString("Until: %1").arg(endTime.toString("H:mm"));
                    dataList[3] = QString::number(temp + tempOffset);
                }
                else
                {
                    if (startTime > headTime)
                    {
                        dataList[2] = QString("Until: %1").arg(startTime.toString("H:mm"));
                        dataList[5] = QString::number(temp + tempOffset);
                    }
                    else
                    {
                        dataList[0] = QString("Until: %1").arg(startTime.toString("H:mm"));
                        dataList[3] = QString::number(temp + tempOffset);
                        dataList[5] = QString::number(temp + tempOffset);
                    }
                }
            }
        }
        else if (24 == keepTime)
        {
            dataList[1] = QString::number(temp + tempOffset);
            dataList[3] = QString::number(temp + tempOffset);
            dataList[5] = QString::number(temp + tempOffset);
        }
        else
        {
            ;
        }
    }
    qDebug() << dataList;
    return dataList;
}

/**
 * @brief MainWindow::calSchComHeatRp
 * @param oldDataList: [Until: 时间(headTime)，温度，Until: 时间(tailTime)，温度，Until: 24， 温度]
 * @return
 */
QStringList MainWindow::calSchComHeatRp(QStringList oldDataList)
{
    QStringList dataList(oldDataList);
    if (ui->checkBox_sec6_nightSETT->isChecked()) {
        int tempOffset = ui->edit_sec6_nightTempOffset->text().toInt();
        int temp = dataList[1].toInt();
        //获取headTime和tailTime
        QRegularExpression reg("([2]{1}[0-3]{1}:[0-6]{1}[0-9]{1}|[1]{0,1}[0-9]{1}:[0-6]{1}[0-9]{1})");
        QRegularExpressionMatch result1, result2;
        QString headTimeStr, tailTimeStr;
        if (dataList[0].contains(reg, &result1) && dataList[2].contains(reg, &result2)) {
            headTimeStr = result1.captured(1);
            tailTimeStr = result2.captured(1);
        } else {
            qFatal("Some mistake happen when handle rp model!");
        }
        QTime headTime = QTime::fromString(headTimeStr, "H:mm");
        QTime tailTime = QTime::fromString(tailTimeStr, "H:mm");
        QTime sTime = ui->timeEdit_sec6_nightStartTime->time();
        int keepTime = ui->edit_sec6_keepTime->text().toInt();
        if (keepTime < 24)
        {
            if (QTime(0, 0) == sTime)
            {
                QTime startTime = sTime;
                QTime endTime(keepTime, 0);
                if (endTime < tailTime)
                {
                    dataList[0] = QString("Until: %1").arg(endTime.toString("H:mm"));
                    dataList[1] = QString::number(temp - tempOffset);
                }
                else
                {
                    dataList[1] = QString::number(temp - tempOffset);
                    dataList[2] = QString("Until: %1").arg(endTime.toString("H:mm"));
                    dataList[3] = QString::number(temp - tempOffset);
                }
            }
            else
            {
                int totalHours = sTime.hour() + keepTime;
                QTime startTime = sTime;
                QTime endTime((startTime.hour()+keepTime)%24, startTime.minute());
                if (totalHours > 24 || (totalHours == 24 && sTime.minute() > 0))
                {
                    dataList[0] = QString("Until: %1").arg(endTime.toString("H:mm"));
                    dataList[1] = QString::number(temp - tempOffset);
                    dataList[2] = QString("Until: %1").arg(startTime.toString("H:mm"));
                    dataList[5] = QString::number(temp - tempOffset);
                }
                else if (totalHours < 24)
                {
                    dataList[0] = QString("Until: %1").arg(startTime.toString("H:mm"));
                    dataList[2] = QString("Until: %1").arg(endTime.toString("H:mm"));
                    dataList[3] = QString::number(temp - tempOffset);
                }
                else
                {
                    if (startTime > headTime)
                    {
                        dataList[2] = QString("Until: %1").arg(startTime.toString("H:mm"));
                        dataList[5] = QString::number(temp - tempOffset);
                    }
                    else
                    {
                        dataList[0] = QString("Until: %1").arg(startTime.toString("H:mm"));
                        dataList[3] = QString::number(temp - tempOffset);
                        dataList[5] = QString::number(temp - tempOffset);
                    }
                }
            }
        }
        else if (24 == keepTime)
        {
            dataList[1] = QString::number(temp - tempOffset);
            dataList[3] = QString::number(temp - tempOffset);
            dataList[5] = QString::number(temp - tempOffset);
        }
        else
        {
            ;
        }
    }
    return dataList;
}


void MainWindow::preImpData()
{
    //@block: 计算房间面积
    _roomSize = ui->edit_sec1_size->text().toInt();
    for (int i = 0; i < _roomSizeVec.size(); i++) {
        if (_roomSizeVec[i] >= _roomSize) {
            _roomSize = _roomSizeVec[i];
            break;
        } else {
            if (_roomSizeVec.size() - 1 == i) {
                _roomSize = _roomSizeVec[i];
            }
        }
    } 

    //@block: 计算各种状态的房间数量(包括:所有房间总数, 未租房间数量, 已租有人状态房间数组, 已租无人状态房间数组)
    //出租率
    double rentalRate = 0;
    if (ui->radioButton_sec2_year->isChecked()) {
        rentalRate = ui->edit_sec2_year->text().toDouble()/100;
    } else {
        rentalRate = ui->edit_sec2_quarter->text().toDouble()/100;
    }

    //在室率
    //暂时不需要使用在室率
//    double inRoomRate = ui->edit_sec2_inRoomRate->text().toInt()/24.0;
    int eastRoomNum = ui->edit_sec1_eastRoomNum->text().toInt();
    int southRoomNum = ui->edit_sec1_southRoomNum->text().toInt();
    int westRoomNum = ui->edit_sec1_westRoomNum->text().toInt();
    int northRoomNum = ui->edit_sec1_northRoomNum->text().toInt();
    _sumRoomNum = eastRoomNum + westRoomNum + southRoomNum + northRoomNum;

    //不拔卡系数
    double keepOnRate;
    if (ui->radioButton_sec4_useCard->isChecked()) {
        keepOnRate =  ui->edit_sec4_UC_noCardNum->text().toDouble();
    }
    else if (ui->radioButton_sec4_useCardAndId->isChecked()) {
        keepOnRate = ui->edit_sec4_UCI_noCardNum->text().toDouble();
    }
    else {
        keepOnRate = 1;
    }

    //待租房间数
    _noRentRoomNum = (int)ceil(_sumRoomNum*(1 - rentalRate));

    //已租房间数
    int rentRoomNum = _sumRoomNum - _noRentRoomNum;

    //读取profile文件
    QFile profile(PathManager::instance()->getPath("ProfileDir") + "/gudu_profile.json");
    if (!profile.open(QFile::ReadOnly)) { qFatal("Can't read the room state profile!"); }
    QTextStream inStream(&profile);
    QJsonDocument doc = QJsonDocument::fromJson(inStream.readAll().toLatin1());
    profile.close();
    if (!doc.isObject() || doc.isNull()) { qFatal("The room state profile file maybe broken!"); }
    QJsonObject root = doc.object();

    //profile中的平均在室率
    double proAverInRoomRate = root["average"].toDouble();
    for (int i = 0; i < 24; i++)
    {
        double proInRoomRate = root[QString::number(i+1)].toDouble();
//        double realInRoomRate = inRoomRate*proInRoomRate/proAverInRoomRate;
        //所使用的在在室率暂时是profile中的
        double realInRoomRate = proInRoomRate;

        if (realInRoomRate > 1) realInRoomRate = 1;
        //每小时已租无人房间数
        int rentNoPeopleRoomNum = ceil(rentRoomNum*(1 - realInRoomRate)*(1 - keepOnRate));

        //每小时已租有人房间数
        int rentPeopleRoomNum = rentRoomNum - rentNoPeopleRoomNum;
        _rentNoPeopleRoomNumVec.push_back(rentNoPeopleRoomNum);
        _rentPeopleRoomNumVec.push_back(rentPeopleRoomNum);
    }
}


void MainWindow::calRoomLoadAndFanWatts(EnergyForm &baseForm, EnergyForm &proposedForm)
{
    int eastRoomNum = ui->edit_sec1_eastRoomNum->text().toInt();
    int westRoomNum = ui->edit_sec1_westRoomNum->text().toInt();
    int southRoomNum = ui->edit_sec1_southRoomNum->text().toInt();
    int northRoomNum = ui->edit_sec1_northRoomNum->text().toInt();

    //四面房间数组(用于函数调用)
    QVector<int> fourSizeRoomNum = { eastRoomNum, westRoomNum, southRoomNum, northRoomNum };

    //计算base model的房间负荷
    //每小时总房间数(用于函数调用)
    QVector<int> baseSumRoomNumVec(24, _sumRoomNum);
    //读取base.csv文件
    CsvReader baseReader(PathManager::instance()->getPath("OutPutDir") + QString("/%1/base.csv").arg(_roomSize));
    if (!baseReader.analyze()) { qFatal("BaseReader can't analyze!"); }

    //房间热负荷序列
    QStringList eHeatListBase = baseReader.getColumnByTitle("BLOCK2:EAST HEATING COIL:Heating Coil Heating Rate [W](Hourly)");
    QStringList wHeatListBase = baseReader.getColumnByTitle("BLOCK2:WEST HEATING COIL:Heating Coil Heating Rate [W](Hourly)");
    QStringList sHeatListBase = baseReader.getColumnByTitle("BLOCK2:SOUTH HEATING COIL:Heating Coil Heating Rate [W](Hourly)");
    QStringList nHeatListBase = baseReader.getColumnByTitle("BLOCK2:NORTH HEATING COIL:Heating Coil Heating Rate [W](Hourly)");
    QStringList dsHeatListBase = baseReader.getColumnByTitle("DOAS SYSTEM HEATING COIL:Heating Coil Heating Rate [W](Hourly)");
    QVector<QStringList> heatGridBase = { eHeatListBase, wHeatListBase, sHeatListBase, nHeatListBase, dsHeatListBase };

    //房间冷负荷序列
    QStringList eCoolListBase = baseReader.getColumnByTitle("BLOCK2:EAST COOLING COIL:Cooling Coil Total Cooling Rate [W](Hourly)");
    QStringList wCoolListBase = baseReader.getColumnByTitle("BLOCK2:WEST COOLING COIL:Cooling Coil Total Cooling Rate [W](Hourly)");
    QStringList sCoolListBase = baseReader.getColumnByTitle("BLOCK2:SOUTH COOLING COIL:Cooling Coil Total Cooling Rate [W](Hourly)");
    QStringList nCoolListBase = baseReader.getColumnByTitle("BLOCK2:NORTH COOLING COIL:Cooling Coil Total Cooling Rate [W](Hourly)");
    QStringList dsCoolListBase = baseReader.getColumnByTitle("DOAS SYSTEM COOLING COIL:Cooling Coil Total Cooling Rate [W](Hourly)");
    QVector<QStringList> coolGridBase = { eCoolListBase, wCoolListBase, sCoolListBase, nCoolListBase, dsCoolListBase };

    //房间风机功率序列
    QStringList eFanListBase =  baseReader.getColumnByTitle("BLOCK2:EAST SUPPLY FAN:Fan Electric Power [W](Hourly)");
    QStringList wFanListBase = baseReader.getColumnByTitle("BLOCK2:WEST SUPPLY FAN:Fan Electric Power [W](Hourly)");
    QStringList sFanListBase = baseReader.getColumnByTitle("BLOCK2:SOUTH SUPPLY FAN:Fan Electric Power [W](Hourly)");
    QStringList nFanListBase = baseReader.getColumnByTitle("BLOCK2:NORTH FAN COIL:Fan Coil Fan Electric Power [W](Hourly)");
    if (ui->radioButton_sec5_hmlMachine->isChecked()) {
        eFanListBase = fixFanWatts(eFanListBase);
        wFanListBase = fixFanWatts(wFanListBase);
        sFanListBase = fixFanWatts(sFanListBase);
        nFanListBase = fixFanWatts(nFanListBase);
    }

    QVector<QStringList> fanGridBase = { eFanListBase, wFanListBase, sFanListBase, nFanListBase };
    baseForm._heatLoad = compose(fourSizeRoomNum, baseSumRoomNumVec, heatGridBase);
    baseForm._coolLoad = compose(fourSizeRoomNum, baseSumRoomNumVec, coolGridBase);
    baseForm._fanWatts = compose(fourSizeRoomNum, baseSumRoomNumVec, fanGridBase);

    //计算proposed models的房间负荷
    //每小时待租房间数组(用于函数调用)
    QVector<int> noRentRoomNumVec(24, _noRentRoomNum);

    //读取nr.csv r.csv rp.csv文件
    CsvReader nrReader;
    if (ui->radioButton_sec6_keepHeatNR->isChecked()) {
        nrReader = CsvReader(PathManager::instance()->getPath("OutPutDir") + QString("/%1/nr.csv").arg(_roomSize));
        if (!nrReader.analyze()) { qFatal("NrReader can't analyze!"); }
    }
    CsvReader rReader(PathManager::instance()->getPath("OutPutDir") + QString("/%1/r.csv").arg(_roomSize));
    if (!rReader.analyze()) { qFatal("RReader can't analyze!"); }
    CsvReader rpReader(PathManager::instance()->getPath("OutPutDir") + QString("/%1/rp.csv").arg(_roomSize));
    if (!rpReader.analyze()) { qFatal("RpReader can't analyze!"); }

    QVector<QVector<int> > modelsRoomNumVec;
    if (ui->radioButton_sec6_keepHeatNR->isChecked()) modelsRoomNumVec.push_back(noRentRoomNumVec);
    modelsRoomNumVec.push_back(_rentNoPeopleRoomNumVec);
    modelsRoomNumVec.push_back(_rentPeopleRoomNumVec);

    QVector<CsvReader> modelsReaderVec;
    if (ui->radioButton_sec6_keepHeatNR->isChecked()) modelsReaderVec.push_back(nrReader);
    modelsReaderVec.push_back(rReader);
    modelsReaderVec.push_back(rpReader);

    /* modelsDataListVec[0] = nrDataListVec
       modelsDataListVec[1] = rDataListVec
       modelsDataListVec[2] = rpDataListVec
       或
       modelsDataListVec[0] = rDataListVec
       modelsDataListVec[1] = rpDataListVec
    */
    QVector<QVector<QStringList> > modelsDataListVec;

    for (int j = 0; j < modelsRoomNumVec.size(); j++)
    {
        CsvReader csvReader = modelsReaderVec[j];
        QVector<int> perHourRoomNum = modelsRoomNumVec[j];
        //房间热负荷序列
        QStringList eHeatList = csvReader.getColumnByTitle("BLOCK2:EAST HEATING COIL:Heating Coil Heating Rate [W](Hourly)");
        QStringList wHeatList = csvReader.getColumnByTitle("BLOCK2:WEST HEATING COIL:Heating Coil Heating Rate [W](Hourly)");
        QStringList sHeatList = csvReader.getColumnByTitle("BLOCK2:SOUTH HEATING COIL:Heating Coil Heating Rate [W](Hourly)");
        QStringList nHeatList = csvReader.getColumnByTitle("BLOCK2:NORTH HEATING COIL:Heating Coil Heating Rate [W](Hourly)");
        QStringList dsHeatList = csvReader.getColumnByTitle("DOAS SYSTEM HEATING COIL:Heating Coil Heating Rate [W](Hourly)");
        QVector<QStringList> heatGrid = {eHeatList, wHeatList, sHeatList, nHeatList, dsHeatList};

        //房间冷负荷序列
        QStringList eCoolList = csvReader.getColumnByTitle("BLOCK2:EAST COOLING COIL:Cooling Coil Total Cooling Rate [W](Hourly)");
        QStringList wCoolList = csvReader.getColumnByTitle("BLOCK2:WEST COOLING COIL:Cooling Coil Total Cooling Rate [W](Hourly)");
        QStringList sCoolList = csvReader.getColumnByTitle("BLOCK2:SOUTH COOLING COIL:Cooling Coil Total Cooling Rate [W](Hourly)");
        QStringList nCoolList = csvReader.getColumnByTitle("BLOCK2:NORTH COOLING COIL:Cooling Coil Total Cooling Rate [W](Hourly)");
        QStringList dsCoolList = csvReader.getColumnByTitle("DOAS SYSTEM COOLING COIL:Cooling Coil Total Cooling Rate [W](Hourly)");
        QVector<QStringList> coolGrid = { eCoolList, wCoolList, sCoolList, nCoolList, dsCoolList };

        //房间风机功率序列
        QStringList eFanList = csvReader.getColumnByTitle("BLOCK2:EAST SUPPLY FAN:Fan Electric Power [W](Hourly)");
        QStringList wFanList = csvReader.getColumnByTitle("BLOCK2:WEST SUPPLY FAN:Fan Electric Power [W](Hourly)");
        QStringList sFanList = csvReader.getColumnByTitle("BLOCK2:SOUTH SUPPLY FAN:Fan Electric Power [W](Hourly)");
        QStringList nFanList = csvReader.getColumnByTitle("BLOCK2:NORTH FAN COIL:Fan Coil Fan Electric Power [W](Hourly)");
        if (ui->radioButton_sec5_hmlMachine->isChecked()) {
            eFanList = fixFanWatts(eFanList);
            wFanList = fixFanWatts(wFanList);
            sFanList = fixFanWatts(sFanList);
            nFanList = fixFanWatts(nFanList);
        }
        QVector<QStringList> fanGrid = { eFanList, wFanList, sFanList, nFanList };

        QStringList heatList = compose(fourSizeRoomNum, perHourRoomNum, heatGrid);
        QStringList coolList = compose(fourSizeRoomNum, perHourRoomNum, coolGrid);
        QStringList fanList = compose(fourSizeRoomNum, perHourRoomNum, fanGrid);
        //0:heatList, 1:coolList, 2:fanList
        QVector<QStringList> dataListVec = { heatList, coolList, fanList };
        modelsDataListVec.push_back(dataListVec);
    }

    //proposed models所有类型房间的负荷、功率叠加
    QStringList sumHeatLoad, sumCoolLoad, sumFanWatts;
    for (int k = 0; k < modelsDataListVec[0][0].size(); k++)
    {
        sumHeatLoad << "0";
        sumCoolLoad << "0";
        sumFanWatts << "0";
    }

    for (int i = 0; i < modelsDataListVec.size(); i++)
    {
        for (int hour = 0; hour < modelsDataListVec[i][0].size(); hour++)
        {
            //叠加 model i 的热负荷序列
            sumHeatLoad[hour] = QString::number(sumHeatLoad[hour].toDouble() + modelsDataListVec[i][0][hour].toDouble());
            //叠加 model i 的冷负荷序列
            sumCoolLoad[hour] = QString::number(sumCoolLoad[hour].toDouble() + modelsDataListVec[i][1][hour].toDouble());
            //叠加 model i 的风机功率序列
            sumFanWatts[hour] = QString::number(sumFanWatts[hour].toDouble() + modelsDataListVec[i][2][hour].toDouble());
        }
    }

    if (ui->radioButton_sec6_keepHeatNR->isChecked()) {
        proposedForm._heatLoad = sumHeatLoad;
        proposedForm._coolLoad = sumCoolLoad;
        proposedForm._fanWatts = sumFanWatts;
    } else {
        proposedForm._heatLoad = sumHeatLoad;
        proposedForm._coolLoad = sumCoolLoad;
        //换气次数
        int changeTimes = 1;
        //流量
        double airFlow = _roomSize*changeTimes*3/3600.0;
        //压头(Pa)
        double pumpHead = 75.0;
        //效率
        double efficiency = 0.7;
        //24小时，每小时的风机功率
        double nrFanWatts = airFlow*pumpHead/efficiency;
        //时长
        int usingTime = ui->edit_sec6_averUsingTime->text().toInt();

        QStringList nrFanWattsList;
        for (int i = 0; i < 24; i++) {
            if (i < usingTime) {
                nrFanWattsList.push_back(QString::number(nrFanWatts));
            } else {
                nrFanWattsList.push_back("0");
            }
        }

        for (int i = 0; i< sumFanWatts.size(); i++) {
            sumFanWatts[i] = QString::number(sumFanWatts[i].toDouble() + nrFanWattsList[i%24].toDouble());
        }
        proposedForm._fanWatts = sumFanWatts;
    }


//    //读取每间房的洗澡负荷
//    QFile showerLoadProfile(PathManager::instance()->getPath("ProfileDir") + "/load/showerLoad_profile.json");
//    if (!showerLoadProfile.open(QFile::ReadOnly)) { qFatal("Can't read the shower load profile!"); }
//    QTextStream inStream(&showerLoadProfile);
//    QJsonDocument doc = QJsonDocument::fromJson(inStream.readAll().toLatin1());
//    showerLoadProfile.close();
//    if (!doc.isObject() || doc.isNull()) { qFatal("The shower load profile file maybe broken!"); }
//    QJsonObject root = doc.object();
//    QStringList showerLoadList;
//    for (int i = 0; i < root.size(); i++) {
//        double data = root[QString::number(i+1)].toDouble();
//        showerLoadList.push_back(QString::number(data));
//    }

//    //base model的冷热负荷修正
//    for (int i = 0; i < baseForm._heatLoad.size(); i++) {
//        int currentHour = i % 24;
//        double realShowerLoad = showerLoadList[currentHour].toDouble()*_sumRoomNum;
//        double heatLoad = baseForm._heatLoad[i].toDouble();
//        double coolLoad = baseForm._coolLoad[i].toDouble();
//        if (heatLoad > coolLoad) {
//            baseForm._heatLoad[i] = QString::number(heatLoad - realShowerLoad);
//        } else {
//            baseForm._coolLoad[i] = QString::number(coolLoad + realShowerLoad);
//        }
//    }

//    //proposed model的冷热负荷修正
//    for (int i = 0; i < proposedForm._heatLoad.size(); i++)
//    {
//        int currentHour = i % 24;
//        double realShowerLoad = showerLoadList[currentHour].toDouble()*_rentPeopleRoomNumVec[currentHour];
//        double heatLoad = proposedForm._heatLoad[i].toDouble();
//        double coolLoad = proposedForm._coolLoad[i].toDouble();
//        if (heatLoad > coolLoad) {
//            proposedForm._heatLoad[i] = QString::number(heatLoad - realShowerLoad);
//        } else {
//            proposedForm._coolLoad[i] = QString::number(coolLoad + realShowerLoad);
//        }
//    }

    //两管或四管制的负荷修正
    if (ui->radioButton_sec5_2pip->isChecked()) {
        qDebug() << "Pip load fix can use....";
        for (int i = 0; i < baseForm.getFormDataSize(); i++) {
            if (baseForm._heatLoad[i].toDouble() > baseForm._coolLoad[i].toDouble())
            {
                double load = baseForm._heatLoad[i].toDouble() - baseForm._coolLoad[i].toDouble();
                baseForm._heatLoad[i] = QString::number(load);
                baseForm._coolLoad[i] = QString::number(0);
            } else {
                double load = baseForm._coolLoad[i].toDouble() - baseForm._heatLoad[i].toDouble();
                baseForm._coolLoad[i] = QString::number(load);
                baseForm._heatLoad[i] = QString::number(0);
            }
        }

        for (int i = 0; i < proposedForm.getFormDataSize(); i++) {
            if (proposedForm._heatLoad[i].toDouble() > proposedForm._coolLoad[i].toDouble())
            {
                double load = proposedForm._heatLoad[i].toDouble() - proposedForm._coolLoad[i].toDouble();
                proposedForm._heatLoad[i] = QString::number(load);
                proposedForm._coolLoad[i] = QString::number(0);
            } else {
                double load = proposedForm._coolLoad[i].toDouble() - proposedForm._heatLoad[i].toDouble();
                proposedForm._coolLoad[i] = QString::number(load);
                proposedForm._heatLoad[i] = QString::number(0);
            }
        }
    }

    //空调面板温度负荷修正
    if (ui->checkBox_sec6_airconTempSet->isChecked() && !ui->radioButton_sec6_ETM->isChecked())
    {
        int coolTemp, heatTemp;
        coolTemp = ui->edit_sec6_lowTemp->text().toInt();
        heatTemp = ui->edit_sec6_highTemp->text().toInt();
        double coolLoadPercent = 0, heatLoadPercent = 0;
        coolLoadPercent = 1 - _coolLoadReducePercentMap[coolTemp];
        heatLoadPercent = 1 - _heatLoadReducePercentMap[heatTemp];
        for (int index = 0; index < baseForm.getFormDataSize(); index++)
        {
            proposedForm._coolLoad[index] = QString::number(proposedForm._coolLoad[index].toDouble()*coolLoadPercent);
            proposedForm._heatLoad[index] = QString::number(proposedForm._heatLoad[index].toDouble()*heatLoadPercent);
        }
    }

    qInfo() << "Calculate room heating load, cooling load and fans watts finish!";
}

/**
 * @brief MainWindow::fixFanWatts >> 根据档位修正风机功率
 * @param fanWattsList
 * @return
 */
QStringList MainWindow::fixFanWatts(const QStringList fanWattsList)
{
    QStringList realFanWattsList;
    double maxWatts = 0;
    for (int i = 0; i < fanWattsList.size(); i++)
    {
        if (fanWattsList[i].toDouble() > maxWatts)
        {
            maxWatts = fanWattsList[i].toDouble();
        }
    }

    QList<double> realDataList = {maxWatts*0.1, maxWatts*0.45, maxWatts};
    for (int i = 0; i < fanWattsList.size(); i++)
    {
        double data = fanWattsList[i].toDouble();

        for (int k = 0; k < realDataList.size(); k++)
        {
            if (data < realDataList[k]) {
                realFanWattsList.push_back(QString::number(realDataList[k]));
                break;
            } else {
                if (k == realDataList.size() - 1) {
                    realFanWattsList.push_back(QString::number(realDataList[k]));
                }
            }
        }
    }

    return realFanWattsList;
}

/**
  * @brief MainWindow::compose >> 数据叠加计算
  * @param fourSizeRoomNum: [east, west, south, north]
  * @param perHourRoomNum
  * @param dataGrid: [east, west, south, north, doas system]
  * @return
  */
 QStringList MainWindow::compose(const QVector<int> &fourSizeRoomNum, const QVector<int> &perHourRoomNum, QVector<QStringList> &dataGrid)
 {
     //从第50行开始计算
     for (int i = 0; i < dataGrid.size(); i++) {
         if (dataGrid[i].size() > 0) {
             QStringList::iterator begin = dataGrid[i].begin();
             QStringList::iterator end = begin + 49;
             dataGrid[i].erase(begin, end);
         } else {
             dataGrid.remove(i);
         }
     }

     //计算实际负荷
     int eastRoomNum = fourSizeRoomNum[0];
     int westRoomNum = fourSizeRoomNum[1];
     int southRoomNum = fourSizeRoomNum[2];
     int northRoomNum = fourSizeRoomNum[3];
     int sum = eastRoomNum + westRoomNum + southRoomNum + northRoomNum;

     for (int i = 0; i < dataGrid.size(); i++) {
         switch (i) {
         case 0:
         {
             for (int hour = 0; hour < dataGrid[i].size(); hour++) {
                 double data = dataGrid[i][hour].toDouble();
                 double newData = data*perHourRoomNum[hour%24]*eastRoomNum/sum;
                 dataGrid[i][hour] = QString::number(newData);
             }
             break;
         }
         case 1:
         {

             for (int hour = 0; hour < dataGrid[i].size(); hour++) {
                 double data = dataGrid[i][hour].toDouble();
                 double newData = data*perHourRoomNum[hour%24]*westRoomNum/sum;
                 dataGrid[i][hour] = QString::number(newData);
             }
             break;
         }
         case 2:
         {
             for (int hour = 0; hour < dataGrid[i].size(); hour++) {
                 double data = dataGrid[i][hour].toDouble();
                 double newData = data*perHourRoomNum[hour%24]*southRoomNum/sum;
                 dataGrid[i][hour] = QString::number(newData);
             }
             break;
         }
         case 3:
         {
             for (int hour = 0; hour < dataGrid[i].size(); hour++) {
                 double data = dataGrid[i][hour].toDouble();
                 double newData = data*perHourRoomNum[hour%24]*northRoomNum/sum;
                 dataGrid[i][hour] = QString::number(newData);
             }
             break;
         }
         case 4:
         {
             for (int hour = 0; hour < dataGrid[i].size(); hour++) {
                 double data = dataGrid[i][hour].toDouble();
                 double newData = data*perHourRoomNum[hour%24]/4;
                 dataGrid[i][hour] = QString::number(newData);
             }
             break;
         }
         default:
             break;
         }
     }

     QStringList resultList;
     for (int hour = 0; hour < dataGrid[0].size(); hour++) {
         double result = 0;
         for (int i = 0; i < dataGrid.size(); i++) {
             result += dataGrid[i][hour].toDouble();
         }
         resultList << QString::number(result);
     }

     return resultList;
 }


 void MainWindow::calDeviceAndLightEnergy(EnergyForm &baseForm, EnergyForm &proposedForm)
 {
     if (!baseForm.isReadyToCalEnergy() || !proposedForm.isReadyToCalEnergy()) {
         qFatal("Form data seems not ready, some mistake happen!");
     }

     //24小时的设备使用系数
     QFile deviceRatioProfile(PathManager::instance()->getPath("ProfileDir") + "/ratio/deviceRatio_profile.json");
     if (!deviceRatioProfile.open(QFile::ReadOnly)) { qFatal("Can't read the device ratio profile!"); }
     QTextStream inStreamDrp(&deviceRatioProfile);
     QJsonDocument docDrp = QJsonDocument::fromJson(inStreamDrp.readAll().toLatin1());
     if (!docDrp.isObject() || docDrp.isNull()) { qFatal("The device ratio profile maybe broken!"); }
     QJsonObject rootDrp = docDrp.object();

     QVector<double> deviceRatioList;
     for (int i = 0; i < rootDrp.size(); i++)
     {
         deviceRatioList.push_back(rootDrp[QString::number(i+1)].toDouble());
     }

     //24小时的照明使用系数
     QFile lightRatioProfile(PathManager::instance()->getPath("ProfileDir") + "/ratio/lightRatio_profile.json");
     if (!lightRatioProfile.open(QFile::ReadOnly)) { qFatal("Can't read the light ratio profile!"); }
     QTextStream inStreamLrp(&lightRatioProfile);
     QJsonDocument docLrp = QJsonDocument::fromJson(inStreamLrp.readAll().toLatin1());
     if (!docLrp.isObject() || docLrp.isNull()) { qFatal("The light ratio profile maybe broken!"); }
     QJsonObject rootLrp = docLrp.object();

     QVector<double> lightRatioList;
     for (int i = 0; i < rootLrp.size(); i++)
     {
         lightRatioList.push_back(rootLrp[QString::number(i+1)].toDouble());
     }

     //base model的设备能耗(J)
     for (int i = 0; i < baseForm.getFormDataSize(); i++)
     {
         double watts = 1000*(ui->edit_sec7_TV->text().toDouble() + ui->edit_sec7_fridge->text().toDouble()
                              + ui->edit_sec7_otherDevice->text().toDouble());
         double value = 3600*watts*deviceRatioList[i%24]*_sumRoomNum;
         baseForm._deviceEnergy.push_back(QString::number(value));
     }

     for (int i = 0; i < baseForm.getFormDataSize(); i++)
     {
         double watts = 1000*(ui->edit_sec7_light->text().toDouble());
         double value = 3600*watts*lightRatioList[i%24]*_sumRoomNum;
         baseForm._lightEnergy.push_back(QString::number(value));
     }

     //proposed model的设备能耗(J)
     for (int i = 0; i < baseForm.getFormDataSize(); i++)
     {
         double watts = 1000*(ui->edit_sec7_TV->text().toDouble() + ui->edit_sec7_fridge->text().toDouble()
                              + ui->edit_sec7_otherDevice->text().toDouble());
         double value =  3600*watts*deviceRatioList[i%24]*_rentPeopleRoomNumVec[i%24];
         proposedForm._deviceEnergy.push_back(QString::number(value));
     }

     //proposed model的照明能耗(J)
     for (int i = 0; i < baseForm.getFormDataSize(); i++)
     {
         double watts = 1000*(ui->edit_sec7_light->text().toDouble());
         double value = 3600*watts*lightRatioList[i%24]*_rentPeopleRoomNumVec[i%24];
         proposedForm._lightEnergy.push_back(QString::number(value));
     }
 }


 void MainWindow:: calRoomRestEnergy(EnergyForm &form, bool isInheritCoreData)
 {
     if (!form.isReadyToCalEnergy()) {
         qFatal("Form data seems not ready, some mistake happen!");
     }

     static double maxHeatLoad = 0;
     static double maxCoolLoad = 0;
     if (isInheritCoreData == false)
     {
         for (int i = 0; i < form._heatLoad.size(); i++) {
             double heatData = form._heatLoad[i].toDouble();
             if (heatData > maxHeatLoad) {
                 maxHeatLoad = heatData;
             }
         }
         for (int i = 0; i < form._coolLoad.size(); i++) {
             double coolData = form._coolLoad[i].toDouble();
             if (coolData > maxCoolLoad) {
                 maxCoolLoad = coolData;
             }
         }
     }

     //螺杆机台数
     int screwMachineNum;
     if (maxCoolLoad*1.15/3 > 150000) {
         screwMachineNum = 3;
     } else if (maxCoolLoad*1.15/2 > 150000) {
         screwMachineNum = 2;
     } else {
         screwMachineNum = 1;
     }

     //读取螺杆机profile
     QFile screwProfile(PathManager::instance()->getPath("ProfileDir") + "/screw_profile.json");
     if (!screwProfile.open(QFile::ReadOnly)) { qFatal("Can't read the screw profile!"); }
     QTextStream inStream(&screwProfile);
     QJsonDocument doc = QJsonDocument::fromJson(inStream.readAll().toLatin1());
     screwProfile.close();
     if (!doc.isObject() || doc.isNull()) { qFatal("The screw profile file maybe broken!"); }
     QJsonArray dataArray = doc.object()["data"].toArray();

     //螺杆机的制冷量(kW)和COP数组
     QVector<QPair<double, double> > screwWattsAndCOP;
     for (int i = 0; i < dataArray.size(); i++) {
         QJsonArray object = dataArray[i].toArray();
         QPair<double, double> pair(object[0].toDouble(), object[1].toDouble());
         screwWattsAndCOP.push_back(pair);
     }

     //对screwWattsAndCOP进行排序，按制冷量从小到大排序
     qSort(screwWattsAndCOP.begin(), screwWattsAndCOP.end(),[](QPair<double, double>&a, QPair<double, double>&b){
         return a.first < b.first;
     });

     //计算螺杆机(冷机)容量(W)以及COP并修正
     double screwCapacity = maxCoolLoad/screwMachineNum*1.15;
     double cop = 0;
     for (int i = 0; i < screwWattsAndCOP.size(); i++) {
         if (screwWattsAndCOP[i].first >= screwCapacity/1000) {
             screwCapacity = screwWattsAndCOP[i].first*1000;
             cop = screwWattsAndCOP[i].second;
             break;
         } else {
             if (i == screwWattsAndCOP.size()-1) {
                 cop = screwWattsAndCOP[i].second;
             }
         }
     }

     //锅炉容量(W)
     double boilerCapacity = maxHeatLoad*1.05;
     qDebug() << "boilerCapacity: " << boilerCapacity;

     //(螺杆机)冷冻水泵额定流量(m³/s)
     double freWaterPumpFlow = maxCoolLoad/(5*1000*4200*screwMachineNum);
     //(螺杆机)冷冻泵额定功率(W)
     double freWaterPumpWatts = freWaterPumpFlow*(100000+200*200)/0.6;

     //(螺杆机)冷却水泵额定流量(m³/s)
     double cooWaterPumpFlow = freWaterPumpFlow*1.5;
     //(螺杆机)冷却水泵额定功率(W)
     double cooWaterPumpWatts = cooWaterPumpFlow*(100000+200*200)/0.6;

     //热水泵额定流量(m³/s)
     double hotWaterPumpFlow = maxHeatLoad/(10*1000*4200);
     //热水泵额定功率(W)
     double hotWaterPumpWatts = hotWaterPumpFlow*(100000+200*200)/0.6;


     //列计算
     //8760小时,每个时刻运行的冷机台数
     QStringList runningCooMachineNumList;
     for (int i = 0; i < form._coolLoad.size(); i++)
     {
         //count上取整, 所以会在后面+1
         int count = 0;
         double t = form._coolLoad[i].toDouble()/(0.95*screwCapacity);
         if (0 == t) {
             count = 0;
         } else {
             count = (int)t + 1;
         }
         runningCooMachineNumList.push_back(QString::number(count));
     }

     //8760小时,每个时刻运行的冷机的负荷(W)
     QStringList runningCooMachineLoadList;
     for (int i = 0; i < runningCooMachineNumList.size(); i++)
     {
         double load = 0;
         if (0 == runningCooMachineNumList[i].toInt()) {
             load = 0;
         } else {
             load = form._coolLoad[i].toDouble()/runningCooMachineNumList[i].toInt();
         }
         runningCooMachineLoadList.push_back(QString::number(load));
     }

     //8760小时,每个时刻的部分负荷率PLR
     QStringList partLoadPercentPLRList;
     for (int i = 0; i < runningCooMachineLoadList.size(); i++)
     {
         double plr = runningCooMachineLoadList[i].toDouble()/screwCapacity;
         partLoadPercentPLRList.push_back(QString::number(plr));
     }


     //8760小时,每个时刻的修正曲线值
     QStringList correctedValueList;
     for (int i = 0; i < partLoadPercentPLRList.size(); i++)
     {
         double plr = partLoadPercentPLRList[i].toDouble();
         double correctedValue = 0.5958 + 1.5534*plr - 1.1552*plr*plr;
         correctedValueList.push_back(QString::number(correctedValue));
     }

     //8760小时,每个时刻的COP值
     QStringList currentCopList;
     for (int i = 0; i < correctedValueList.size(); i++)
     {
         double realCop = cop*correctedValueList[i].toDouble();
         currentCopList.push_back(QString::number(realCop));
     }

     //8760小时,每个时刻运行的冷机的功率(W)
     QStringList runningCooMachineWattsList;
     for (int i = 0; i < currentCopList.size(); i++)
     {
         double watts = runningCooMachineLoadList[i].toDouble()/currentCopList[i].toDouble();
         runningCooMachineWattsList.push_back(QString::number(watts));
     }

     //8760小时,每个时刻运行的冷机的能耗(J)
     for (int i = 0; i < runningCooMachineWattsList.size(); i++)
     {
         double energy = runningCooMachineWattsList[i].toDouble()*runningCooMachineNumList[i].toInt()*3600;
         form._cooMachineEnergy.push_back(QString::number(energy));
     }

     //8760小时,每个时刻的锅炉PLR
     QStringList boilerPLRList;
     for (int i = 0; i < form._heatLoad.size(); i++)
     {
         double plr = form._heatLoad[i].toDouble()/boilerCapacity;
         boilerPLRList.push_back(QString::number(plr));
     }

     QStringList realEff;
     //8760小时,每个时刻的锅炉能耗(J)
     for (int i = 0; i < boilerPLRList.size(); i++)
     {
         double plr = boilerPLRList[i].toDouble();
         double realEfficiency = 0.8*(0.97 + 0.0633*plr - 0.0333*plr*plr);
         double energy = form._heatLoad[i].toDouble()/realEfficiency*3600;
         realEff.push_back(QString::number(realEfficiency));
         form._boilerFuelUse.push_back(QString::number(energy));
     }

     //8760小时,每个时刻每台冷却塔的总散热量(W)
     QStringList perCooTowerLoseHeatList;
     for (int i = 0; i < currentCopList.size(); i++)
     {
         double value = (1 + 1/currentCopList[i].toDouble())*runningCooMachineLoadList[i].toDouble();
         perCooTowerLoseHeatList.push_back(QString::number(value));
     }

     //获取冷却塔的最大散热量(W)
     double maxCooTowerLoseHeat = 0;
     for (int i = 0; i < perCooTowerLoseHeatList.size(); i++)
     {
         if (perCooTowerLoseHeatList[i].toDouble() > maxCooTowerLoseHeat) {
             maxCooTowerLoseHeat = perCooTowerLoseHeatList[i].toDouble();
         }
     }

     //计算冷却塔风机额定功率(W)
     double cooTowerFanWatts = 0.0088*maxCooTowerLoseHeat - 1.1766;

     //8760小时,每个时刻每台冷却塔风机的功率(W)
     QStringList cooTowerFanWattsList;
     for (int i = 0; i < partLoadPercentPLRList.size(); i++)
     {
         double plr = partLoadPercentPLRList[i].toDouble();
         double value = (0.35071223 + 0.30850535*plr - 0.54137364*plr*plr + 0.87198823*plr*plr*plr)*cooTowerFanWatts;
         cooTowerFanWattsList.push_back(QString::number(value));
     }

     //8760小时,每个时刻冷却塔的能耗(J)
     for (int i = 0; i < cooTowerFanWattsList.size(); i++)
     {
         double energy = 3600*cooTowerFanWattsList[i].toDouble()*runningCooMachineNumList[i].toInt();
         form._cooTowerEnergy.push_back(QString::number(energy));
     }

     //8760小时,每个时刻的冷冻水泵当前功率(W)
     QStringList freWaterPumpWattsList;
     for (int i = 0; i < partLoadPercentPLRList.size(); i++)
     {
         double plr = partLoadPercentPLRList[i].toDouble();
         double value = plr*freWaterPumpWatts;
         freWaterPumpWattsList.push_back(QString::number(value));
     }

     //8760小时,每个时刻的冷冻水泵能耗(J)
     for (int i = 0; i < freWaterPumpWattsList.size(); i++)
     {
         double energy = 3600*freWaterPumpWattsList[i].toDouble()*runningCooMachineNumList[i].toInt();
         form._freWaterPumpEnergy.push_back(QString::number(energy));
     }

     //8760小时,每个时刻的冷却水泵当前功率(W)
     QStringList cooWaterPumpWattsList;
     for (int i = 0; i < partLoadPercentPLRList.size(); i++)
     {
         double plr = partLoadPercentPLRList[i].toDouble();
         double value = plr*cooWaterPumpWatts;
         cooWaterPumpWattsList.push_back(QString::number(value));
     }

     //8760小时,每个时刻的冷却水泵能耗(J)
     for (int i = 0; i < cooWaterPumpWattsList.size(); i++)
     {
         double energy = 3600*cooWaterPumpWattsList[i].toDouble()*runningCooMachineNumList[i].toInt();
         form._cooWaterPumpEnergy.push_back(QString::number(energy));
     }

     //8760小时,每个时刻的热水泵当前功率(W)
     QStringList hotWaterPumpWattsList;
     for (int i = 0; i < boilerPLRList.size(); i++)
     {
         double plr =  boilerPLRList[i].toDouble();
         double value = plr*hotWaterPumpWatts;
         hotWaterPumpWattsList.push_back(QString::number(value));
     }

     //8760小时,每个时刻的热水泵能耗(J)
     for (int i = 0; i < hotWaterPumpWattsList.size(); i++)
     {
         double energy = 3600*hotWaterPumpWattsList[i].toDouble();
         form._hotWaterPumpEnergy.push_back(QString::number(energy));
     }

     //8760小时,每个时刻的风机能耗(J)
     for (int i = 0; i < form._fanWatts.size(); i++)
     {
         double energy = 3600*form._fanWatts[i].toDouble();
         form._fanEnergy.push_back(QString::number(energy));
     }

     qInfo() << "Energy calculate finnish!";
 }

/**---------------------------文件IO及Eplus调用操作---------------------------------**/

/**
 * @brief MainWindow::lStep >> 会在完成任务发射zSignal
 */
void MainWindow::lStep()
{
    //读取源idf文件
    QString sourceFilePath = QString(PathManager::instance()->getPath("SourceDir")+"/%1.idf").arg(_roomSize);
    QString sourceNoPeFilePath = QString(PathManager::instance()->getPath("SourceNoPeDir")+"/%1.idf").arg(_roomSize);
    HandleMachine *p_src = new HandleMachine(sourceFilePath);
    HandleMachine *p_srcNp = new HandleMachine(sourceNoPeFilePath);
    QThread *p_thread_src = new QThread();

    //对源idf文件进行配置、操作及models分离
    //(注意：lambda表达式的捕捉参数,必须是通过值来捕捉,否则主线程在离开作用域后, 通过引用捕捉的话会导致程序崩溃)
    connect(p_thread_src, &QThread::started , [=](){
        p_src->initCityData(_city);
        p_src->operate<MainWindow>(PathManager::instance()->getPath("BaseOpFile"), "opElectricEquipment", this, &MainWindow::calElecEqtWatts);
        p_src->operate<MainWindow>(PathManager::instance()->getPath("BaseOpFile"), "opLights", this, &MainWindow::calLightsWatts);
        p_src->operate<MainWindow>(PathManager::instance()->getPath("BaseOpFile"), "opTimeSpan", this, &MainWindow::calTimeSpan);
        p_srcNp->initCityData(_city);
        p_srcNp->operate<MainWindow>(PathManager::instance()->getPath("BaseOpFile"), "opElectricEquipment", this, &MainWindow::calElecEqtWattsNope);
        p_srcNp->operate<MainWindow>(PathManager::instance()->getPath("BaseOpFile"), "opLights", this, &MainWindow::calLightsWattsNope);
        p_srcNp->operate<MainWindow>(PathManager::instance()->getPath("BaseOpFile"), "opTimeSpan", this, &MainWindow::calTimeSpan);

        QStringList fileNameList, nopeFileNameList;
        fileNameList << "base" << "rp";
        //若使用新风模式,则不会去生成未租房间的idf
        if (ui->radioButton_sec6_newWind->isChecked()) {
            nopeFileNameList << "r";
        } else {
            nopeFileNameList << "nr" << "r";
        }
        p_src->separate(fileNameList);
        p_srcNp->separate(nopeFileNameList);
    });
    connect(p_src, &HandleMachine::finishSep, p_thread_src, &QThread::quit);
    connect(p_thread_src, &QThread::finished, p_src, &QObject::deleteLater);
    connect(p_thread_src, &QThread::finished, p_srcNp, &QObject::deleteLater);
    connect(p_thread_src, &QThread::finished, p_thread_src, &QObject::deleteLater);
    connect(p_srcNp, &QObject::destroyed, this, &MainWindow::zSignal);
    p_thread_src->start();
}

void MainWindow::zStep()
{
    //优先运行base model
    QString baseFilePath = QString(PathManager::instance()->getPath("OutPutDir")+"/%1/base.idf").arg(_roomSize);
    HandleMachine *p_base = new HandleMachine(baseFilePath);
    QThread *p_thread_base = new QThread();
    p_base->moveToThread(p_thread_base);

    connect(p_thread_base, &QThread::started, [=](){
        p_base->startMachine(_city);
    });
    connect(p_base, &HandleMachine::finishExec, p_thread_base, &QThread::quit);
    connect(p_base, &HandleMachine::finishExec, this, &MainWindow::model_base_over);
    connect(p_thread_base, &QThread::finished, p_base, &QObject::deleteLater);
    //当线程结束时，销毁线程
    connect(p_thread_base, &QThread::finished, p_thread_base, &QObject::deleteLater);
    //接收到停止计算信号立即停止线程
    connect(this, &MainWindow::cancelCal, p_thread_base, &QThread::quit);
    p_thread_base->start();

    //proposed model进行配置
    QString nrFilePath = QString(PathManager::instance()->getPath("OutPutDir")+"/%1/nr.idf").arg(_roomSize);
    QString rFilePath = QString(PathManager::instance()->getPath("OutPutDir")+"/%1/r.idf").arg(_roomSize);
    QString rpFilePath = QString(PathManager::instance()->getPath("OutPutDir")+"/%1/rp.idf").arg(_roomSize);

    HandleMachine *p_nr = NULL;
    //若开启新风模式，则不使用Eplus调用未租房间的idf
    if (ui->radioButton_sec6_newWind->isChecked()) {
        p_nr = NULL;
    } else {
        p_nr = new HandleMachine(nrFilePath);
    }
    HandleMachine *p_r = new HandleMachine(rFilePath);
    HandleMachine *p_rp = new HandleMachine(rpFilePath);

    QThread *p_thread_pro = new QThread();
    if (p_nr != NULL) p_nr->moveToThread(p_thread_pro);
    p_r->moveToThread(p_thread_pro);
    p_rp->moveToThread(p_thread_pro);

    connect(p_thread_pro, &QThread::started, [=](){
        //客人离房关闭窗帘
        if (ui->radioButton_sec3_LR_close->isChecked()) {
            p_r->configure(PathManager::instance()->getPath("ShadingDir") + QString("/%1.json").arg(_city));
            p_r->configure(PathManager::instance()->getPath("RConfigFile"));
        }

        if (p_nr != NULL) {
            //客人退房关闭窗帘
            if (ui->radioButton_sec3_CO_close->isChecked()) {
                p_nr->configure(PathManager::instance()->getPath("ShadingDir") + QString("/%1.json").arg(_city));
                p_nr->configure(PathManager::instance()->getPath("NrConfigFile"));
            }

            if (ui->radioButton_sec6_keepHeatNR->isChecked()) {
                p_nr->operate<MainWindow>(PathManager::instance()->getPath("NrOpFile"), "opSchComCool", this,
                                          &MainWindow::calSchComCoolNr);
                p_nr->operate<MainWindow>(PathManager::instance()->getPath("NrOpFile"), "opSchComHeat", this,
                                          &MainWindow::calSchComHeatNr);
            }
        }

        if (ui->checkBox_sec6_keepHeat->isChecked() && !ui->radioButton_sec6_ETM->isChecked()) {
            p_r->operate<MainWindow>(PathManager::instance()->getPath("ROpFile"), "opSchComCool", this,
                                     &MainWindow::calSchComCoolR);
            p_r->operate<MainWindow>(PathManager::instance()->getPath("ROpFile"), "opSchComHeat", this,
                                     &MainWindow::calSchComHeatR);
        }

        if (ui->checkBox_sec6_nightSETT->isChecked() && !ui->radioButton_sec6_ETM->isChecked()) {
            p_rp->operate<MainWindow>(PathManager::instance()->getPath("RpOpFile"), "opSchComCool", this,
                                      &MainWindow::calSchComCoolRp);
            p_rp->operate<MainWindow>(PathManager::instance()->getPath("RpOpFile"), "opSchComHeat", this,
                                     &MainWindow::calSchComHeatRp);
        }

        if (p_nr != NULL) p_nr->startMachine(_city);
        p_r->startMachine(_city);
        p_rp->startMachine(_city);
    });
    connect(p_rp, &HandleMachine::finishExec, [](){
        qInfo() << "Output finish!";
    });
    connect(p_rp, &HandleMachine::finishExec, p_thread_pro, &QThread::quit);
    if (p_nr != NULL)
    {
        connect(p_nr, &HandleMachine::finishExec, this, &MainWindow::model_nr_over);
        connect(p_thread_pro, &QThread::finished, p_nr, &QObject::deleteLater);
    } else {
        emit model_base_over();
    }

    connect(p_r, &HandleMachine::finishExec, this, &MainWindow::model_r_over);
    connect(p_thread_pro, &QThread::finished, p_r, &QObject::deleteLater);

    connect(p_rp, &HandleMachine::finishExec, this, &MainWindow::model_rp_over);
    connect(p_thread_pro, &QThread::finished, p_rp, &QObject::deleteLater);

    connect(p_rp, &QObject::destroyed, this, &MainWindow::mSignal);
    //当线程结束时，销毁线程
    connect(p_thread_pro, &QThread::finished, p_thread_pro, &QObject::deleteLater);
    p_thread_pro->start();
}

void MainWindow::mStep()
{
    QThread *p_thread_res = new QThread();
    connect(p_thread_res, &QThread::started, [=](){
        EnergyForm baseForm, proposedForm;
        this->calRoomLoadAndFanWatts(baseForm, proposedForm);
        this->calDeviceAndLightEnergy(baseForm, proposedForm);
        this->calRoomRestEnergy(baseForm);
        this->calRoomRestEnergy(proposedForm, true);
        double baseTotal = 0, proposedTotal = 0;
        QVector<double> baseVec, proposedVec;
        for (int i = 0; i < baseForm._lightEnergy.size(); i++) {
            double value = baseForm._lightEnergy[i].toDouble() +
                    baseForm._deviceEnergy[i].toDouble() +
                    baseForm._cooMachineEnergy[i].toDouble() +
                    baseForm._boilerFuelUse[i].toDouble() +
                    baseForm._cooMachineEnergy[i].toDouble() +
                    baseForm._freWaterPumpEnergy[i].toDouble() +
                    baseForm._cooWaterPumpEnergy[i].toDouble() +
                    baseForm._hotWaterPumpEnergy[i].toDouble() +
                    baseForm._fanEnergy[i].toDouble();
            baseVec.push_back(value);
        }

        for (int i = 0; i < proposedForm._lightEnergy.size(); i++) {
            double value = proposedForm._lightEnergy[i].toDouble() +
                    proposedForm._deviceEnergy[i].toDouble() +
                    proposedForm._cooMachineEnergy[i].toDouble() +
                    proposedForm._boilerFuelUse[i].toDouble() +
                    proposedForm._cooMachineEnergy[i].toDouble() +
                    proposedForm._freWaterPumpEnergy[i].toDouble() +
                    proposedForm._cooWaterPumpEnergy[i].toDouble() +
                    proposedForm._hotWaterPumpEnergy[i].toDouble() +
                    proposedForm._fanEnergy[i].toDouble();
            proposedVec.push_back(value);
        }

        for (int i = 0; i < baseVec.size(); i++) {
            baseTotal += baseVec[i];
            proposedTotal += proposedVec[i];
        }

//        //!<debug>
//        baseForm.outToFile("./base.txt", _roomSize, _sumRoomNum);
//        proposedForm.outToFile("./proposed.txt", _roomSize, _sumRoomNum);
//        CsvReader baseCsv("baseData.csv");
//        baseCsv.pushColumnData("HeatLoad", baseForm._heatLoad);
//        baseCsv.pushColumnData("CoolLoad", baseForm._coolLoad);
//        baseCsv.save();
//        //!</debug>

        double percent = (baseTotal - proposedTotal)/baseTotal;
        emit fetchResult(percent);
    });

    connect(this, &MainWindow::fetchResult, p_thread_res, &QThread::quit);
    connect(p_thread_res, &QThread::finished, p_thread_res, &QObject::deleteLater);
    connect(this, &MainWindow::fetchResult, [=](double percent){
        if (ui->radioButton_sec2_year->isChecked()) {
            ui->edit_sec8_year->setText(QString::number(percent*100, 'f', 2));
        } else {
            if (ui->comboBox_sec2_quarter->currentText() == "1") {
                ui->edit_sec8_1To3Mon->setText(QString::number(percent*100, 'f', 2));
            } else if (ui->comboBox_sec2_quarter->currentText() == "2") {
                ui->edit_sec8_4To6Mon->setText(QString::number(percent*100, 'f', 2));
            } else if (ui->comboBox_sec2_quarter->currentText() == "3") {
                ui->edit_sec8_7To9Mon->setText(QString::number(percent*100, 'f', 2));
            } else {
                ui->edit_sec8_10To12Mon->setText(QString::number(percent*100, 'f', 2));
            }
        }
    });
    p_thread_res->start();
}

/**
 * @brief MainWindow::setLanguage >> 设置语言
 * @param lang
 */
void MainWindow::setLanguage(Language lang)
{
    QString fileName;
    _language = lang;
    switch (lang) {
    case MainWindow::Chinese:
    {
        fileName = "Chinese";
        break;
    }
    case MainWindow::English:
    {
        fileName = "English";
        break;
    }
    default:
    {
        fileName = "Chinese";
        break;
    }
    }
    QFile langProfile(PathManager::instance()->getPath("ProfileDir") + QString("/lang/%1.json").arg(fileName));
    if (!langProfile.open(QIODevice::ReadOnly)) { qFatal("Can't read the language profile!"); }
    QTextStream lpStream(&langProfile);
    lpStream.setCodec("UTF-8");
    QJsonDocument lpDoc = QJsonDocument::fromJson(lpStream.readAll().toUtf8());
    langProfile.close();
    if (!lpDoc.isObject() || lpDoc.isEmpty()) { qFatal("The language profile maybe broken!"); }
    QJsonObject lpRoot = lpDoc.object();
    int index = 0;

    //section1
    QJsonArray sec1Arr = lpRoot["section_1"].toArray();
    index = 0;
    ui->lab_sec1_title->setText(sec1Arr[index++].toString());
    ui->lab_sec1_city->setText(sec1Arr[index++].toString());
    ui->lab_sec1_heatProNum->setText(sec1Arr[index++].toString());
    ui->lab_sec1_size->setText(sec1Arr[index++].toString());
    ui->lab_sec1_eastRoomNum->setText(sec1Arr[index++].toString());
    ui->lab_sec1_southRoomNum->setText(sec1Arr[index++].toString());
    ui->lab_sec1_westRoomNum->setText(sec1Arr[index++].toString());
    ui->lab_sec1_northRoomNum->setText(sec1Arr[index++].toString());
    ui->lab_sec1_roomNum->setText(sec1Arr[index++].toString());

    //section2
    QJsonArray sec2Arr = lpRoot["section_2"].toArray();
    index = 0;
    ui->lab_sec2_title->setText(sec2Arr[index++].toString());
    ui->lab_sec2_rentRate->setText(sec2Arr[index++].toString());
    ui->lab_sec2_year->setText(sec2Arr[index++].toString());
    ui->lab_sec2_quarter->setText(sec2Arr[index++].toString());
    ui->lab_sec2_inRoomRate->setText(sec2Arr[index++].toString());

    //section3
    QJsonArray sec3Arr = lpRoot["section_3"].toArray();
    index = 0;
    ui->lab_sec3_title->setText(sec3Arr[index++].toString());
    ui->lab_sec3_leaveRoom->setText(sec3Arr[index++].toString());
    ui->radioButton_sec3_LR_keep->setText(sec3Arr[index++].toString());
    ui->radioButton_sec3_LR_close->setText(sec3Arr[index++].toString());
    ui->lab_sec3_checkOut->setText(sec3Arr[index++].toString());
    ui->radioButton_sec3_CO_keep->setText(sec3Arr[index++].toString());
    ui->radioButton_sec3_CO_close->setText(sec3Arr[index++].toString());

    //section4
    QJsonArray sec4Arr = lpRoot["section_4"].toArray();
    index = 0;
    ui->lab_sec4_title->setText(sec4Arr[index++].toString());
    ui->radioButton_sec4_useCard->setText(sec4Arr[index++].toString());
    ui->lab_sec4_UC_noCardNum->setText(sec4Arr[index++].toString());
    ui->radioButton_sec4_useCardAndId->setText(sec4Arr[index++].toString());
    ui->lab_sec4_UCI_noCardNum->setText(sec4Arr[index++].toString());
    ui->radioButton_sec4_noCard->setText(sec4Arr[index++].toString());

    //section5
    QJsonArray sec5Arr = lpRoot["section_5"].toArray();
    index = 0;
    ui->lab_sec5_title->setText(sec5Arr[index++].toString());
    ui->radioButton_sec5_4pip->setText(sec5Arr[index++].toString());
    ui->radioButton_sec5_2pip->setText(sec5Arr[index++].toString());
    ui->radioButton_sec5_hmlMachine->setText(sec5Arr[index++].toString());
    ui->radioButton_sec5_0to10Machine->setText(sec5Arr[index++].toString());

    //section6
    QJsonArray sec6Arr = lpRoot["section_6"].toArray();
    index = 0;
    ui->lab_sec6_title->setText(sec6Arr[index++].toString());
    ui->lab_sec6_ranted->setText(sec6Arr[index++].toString());
    ui->checkBox_sec6_keepHeat->setText(sec6Arr[index++].toString());
    ui->lab_sec6_keepHeatTempSet->setText(sec6Arr[index++].toString());
    ui->lab_sec6_keepHeatWindSet->setText(sec6Arr[index++].toString());
    ui->checkBox_sec6_airconTempSet->setText(sec6Arr[index++].toString());
    ui->lab_sec6_lowTemp->setText(sec6Arr[index++].toString());
    ui->lab_sec6_highTemp->setText(sec6Arr[index++].toString());
    ui->checkBox_sec6_nightSETT->setText(sec6Arr[index++].toString());
    ui->lab_sec6_nightStartTime->setText(sec6Arr[index++].toString());
    ui->lab_sec6_keepTime->setText(sec6Arr[index++].toString());
    ui->lab_sec6_nightTempOffset->setText(sec6Arr[index++].toString());
    ui->radioButton_sec6_ETM->setText(sec6Arr[index++].toString());
    ui->lab_sec6_noRanted->setText(sec6Arr[index++].toString());
    ui->radioButton_sec6_keepHeatNR->setText(sec6Arr[index++].toString());
    ui->lab_sec6_keepHeatTempSetNR->setText(sec6Arr[index++].toString());
    ui->lab_sec6_keepHeatWindSetNR->setText(sec6Arr[index++].toString());
    ui->radioButton_sec6_newWind->setText(sec6Arr[index++].toString());
    ui->lab_sec6_averUsingTime->setText(sec6Arr[index++].toString());

    //section7
    QJsonArray sec7Arr = lpRoot["section_7"].toArray();
    index = 0;
    ui->lab_sec7_title->setText(sec7Arr[index++].toString());
    ui->lab_sec7_light->setText(sec7Arr[index++].toString());
    ui->lab_sec7_lightUsingNum->setText(sec7Arr[index++].toString());
    ui->lab_sec7_TV->setText(sec7Arr[index++].toString());
    ui->lab_sec7_TVUsingNum->setText(sec7Arr[index++].toString());
    ui->lab_sec7_fridge->setText(sec7Arr[index++].toString());
    ui->lab_sec7_otherDevice->setText(sec7Arr[index++].toString());

    //section8
    QJsonArray sec8Arr = lpRoot["section_8"].toArray();
    index = 0;
    ui->lab_sec8_title->setText(sec8Arr[index++].toString());
    ui->lab_sec8_year->setText(sec8Arr[index++].toString());
    ui->lab_sec8_1To3Mon->setText(sec8Arr[index++].toString());
    ui->lab_sec8_4To6Mon->setText(sec8Arr[index++].toString());
    ui->lab_sec8_7To9Mon->setText(sec8Arr[index++].toString());
    ui->lab_sec8_10To12Mon->setText(sec8Arr[index++].toString());
    ui->btn_start->setText(sec8Arr[index++].toString());

    //city,fanLevel 等下拉框选项
    QJsonObject cityRoot = lpRoot["city"].toObject();
    QList<QString> cityNameValues = _cityNameMap.values();
    _cityNameMap.clear();
    for (auto cityName: cityNameValues)
    {
        QString cityTransName = cityRoot[cityName].toString();
        if (!cityTransName.isEmpty())
        {
            _cityNameMap.insert(cityTransName, cityName);
        }
    }
    QStringList cityItems;
    for (auto cityTransName: _cityNameMap.keys())
    {
        cityItems << cityTransName;
    }

    ui->comboBox_sec1_city->clear();
    ui->comboBox_sec1_city->addItems(cityItems);
    _city = _cityNameMap[ui->comboBox_sec1_city->currentText()];


    QJsonObject levelRoot = lpRoot["fanLevel"].toObject();
    QList<QString> fanLevelValues = _fanLevelMap.values();
    _fanLevelMap.clear();
    for (auto fanLevel: fanLevelValues)
    {
        QString fanLevelTrans = levelRoot[fanLevel].toString();
        if (!fanLevelTrans.isEmpty())
        {
            _fanLevelMap.insert(fanLevelTrans, fanLevel);
        }
    }
    QStringList fanItems;
    for (auto fanLevelTrans: _fanLevelMap.keys())
    {
        fanItems << fanLevelTrans;
    }
    ui->comboBox_sec6_keepHeatWindSet->clear();
    ui->comboBox_sec6_keepHeatWindSet->addItems(fanItems);
    ui->comboBox_sec6_keepHeatWindSetNR->clear();
    ui->comboBox_sec6_keepHeatWindSetNR->addItems(fanItems);



    //用户提示选项
    _noticeList.clear();
    QJsonArray noticeList = lpRoot["noticeList"].toArray();
    for (auto notice: noticeList)
    {
        _noticeList << notice.toString();
    }

}
