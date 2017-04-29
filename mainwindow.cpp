#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
    connect(this, &MainWindow::lSignal, this, &MainWindow::lStep);
    connect(this, &MainWindow::zSignal, this, &MainWindow::zStep);
    connect(this, &MainWindow::mSignal, this, &MainWindow::mStep);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    QRegExp float0To1Reg("^1|([0]+(\\.[0-9]{1,2})?)$"); //匹配0.00~1的数字
    QRegExp float0To100Reg("^100|(([0]|([1-9][0-9]{0,1}))(\\.[0-9]{1,2})?)$"); //匹配0.00~100的数字
    QRegExp float0To1000Reg("^1000|(([0]|([1-9][0-9]{0,2}))(\\.[0-9]{1,2})?)$"); //识别0.00~1000的数字

    //section1
    //读取城市profile，获取城市信息和隔热系数
    QFile profile(PathManager::instance()->getPath("ProfileDir") + "/city_profile.json");
    if (!profile.open(QFile::ReadOnly)) { qFatal("When init, can't read the city profile!"); }
    QTextStream inStream(&profile);
    QJsonDocument doc = QJsonDocument::fromJson(inStream.readAll().toLatin1());
    profile.close();
    if (!doc.isObject() || doc.isNull()) { qFatal("The city profile file maybe broken!"); }
    QJsonObject root = doc.object();
    QJsonArray cityArray = root["cityArray"].toArray();
    for (int i = 0; i < cityArray.size(); i++) {
        QString cityName = cityArray[i].toString();
        double heatProNum = root[cityName].toDouble();
        _cityMap.insert(cityName, heatProNum);
    }
    QStringList items;
    for (int i = 0; i < _cityMap.keys().size(); i++) {
        items << _cityMap.keys()[i];
    }
    QComboBox *cityBox = ui->comboBox_sec1_city;
    cityBox->clear();
    cityBox->addItems(items);
    //设置隔热系数
    QLineEdit *heatProNum = ui->edit_sec1_heatProNum;
    heatProNum->setReadOnly(true);
    QString nowDisplayCity = ui->comboBox_sec1_city->currentText();
    heatProNum->setText(QString::number(_cityMap[nowDisplayCity]));
    connect(ui->comboBox_sec1_city, &QComboBox::currentTextChanged, [cityBox, heatProNum, this]() {
        heatProNum->setText(QString::number(_cityMap[cityBox->currentText()]));
    });
     ui->edit_sec1_size->setValidator(new QIntValidator(20, 1000000, this));
    ui->edit_sec1_roomNum->setValidator(new QIntValidator(1, 1000000, this));
    ui->edit_sec1_singleRoomNum->setValidator(new QIntValidator(1, 1000000, this));
    ui->edit_sec1_singleRoomNum->setDisabled(true);
    ui->edit_sec1_doubleRoomNum->setValidator(new QIntValidator(1, 1000000, this));
    ui->edit_sec1_doubleRoomNum->setDisabled(true);
    ui->edit_sec1_suiteRoomNum->setValidator(new QIntValidator(1, 1000000, this));
    ui->edit_sec1_suiteRoomNum->setDisabled(true);
    ui->edit_sec1_eastRoomNum->setValidator(new QIntValidator(1, 1000000, this));
    ui->edit_sec1_southRoomNum->setValidator(new QIntValidator(1, 1000000, this));
    ui->edit_sec1_westRoomNum->setValidator(new QIntValidator(1, 1000000, this));
    ui->edit_sec1_northRoomNum->setValidator(new QIntValidator(1, 1000000, this));

    //section2
    ui->radioButton_sec2_year->setChecked(true);
    ui->edit_sec2_year->setEnabled(true);
    ui->edit_sec2_quarter->setEnabled(false);
    ui->edit_sec2_month->setEnabled(false);
    ui->edit_sec2_year->setValidator(new QRegExpValidator(float0To100Reg, this));
    ui->edit_sec2_quarter->setValidator(new QRegExpValidator(float0To100Reg, this));
    ui->edit_sec2_month->setValidator(new QRegExpValidator(float0To100Reg, this));
    ui->edit_sec2_inRoomRate->setValidator(new QIntValidator(0, 24, this));

    //section3
    ui->radioButton_sec3_CO_keep->setChecked(true);
    ui->radioButton_sec3_LR_keep->setChecked(true);

    //section4
    ui->radioButton_sec4_noCard->setChecked(true);
    ui->edit_sec4_UC_noCardNum->setEnabled(false);
    ui->edit_sec4_UCI_noCardNum->setEnabled(false);
    ui->edit_sec4_UC_noCardNum->setValidator(new QRegExpValidator(float0To1Reg, this));
    ui->edit_sec4_UCI_noCardNum->setValidator(new QRegExpValidator(float0To1Reg, this));

    //section5
    ui->radioButton_sec5_4pip->setChecked(true);
    ui->radioButton_sec5_hmlMachine->setChecked(true);

    //section6
    ui->edit_sec6_keepHeatTempSet->setEnabled(false);
    ui->comboBox_sec6_keepHeatWindSet->setEnabled(false);
    ui->edit_sec6_lowTemp->setEnabled(false);
    ui->edit_sec6_highTemp->setEnabled(false);
    ui->timeEdit_sec6_nightStartTime->setEnabled(false);
    ui->edit_sec6_keepTime->setEnabled(false);
    ui->edit_sec6_nightTempOffset->setEnabled(false);

    ui->radioButton_sec6_keepHeatNR->setChecked(true);
    ui->radioButton_sec6_newWind->setDisabled(true);
    ui->edit_sec6_averUsingTime->setEnabled(false);

    ui->edit_sec6_keepHeatTempSet->setValidator(new QIntValidator(0, 100, this));
    ui->edit_sec6_highTemp->setValidator(new QIntValidator(0, 100, this));
    ui->edit_sec6_lowTemp->setValidator(new QIntValidator(0, 100, this));
    ui->edit_sec6_nightTempOffset->setValidator(new QIntValidator(0, 100, this));
    ui->edit_sec6_keepTime->setValidator(new QIntValidator(0, 24, this));
    ui->edit_sec6_keepHeatTempSetNR->setValidator(new QIntValidator(0, 24, this));
    ui->edit_sec6_averUsingTime->setValidator(new QIntValidator(0, 24, this));

    //section7
    ui->edit_sec7_light->setValidator(new QRegExpValidator(float0To1000Reg, this));
    ui->edit_sec7_lightUsingNum->setValidator(new QRegExpValidator(float0To1Reg, this));
    ui->edit_sec7_TV->setValidator(new QRegExpValidator(float0To1000Reg, this));
    ui->edit_sec7_TVUsingNum->setValidator(new QRegExpValidator(float0To1Reg, this));
    ui->edit_sec7_fridge->setValidator(new QRegExpValidator(float0To1000Reg, this));
    ui->edit_sec7_otherDevice->setValidator(new QRegExpValidator(float0To1000Reg, this));

}

/**----------------------------控件操作------------------------------**/

void MainWindow::on_btn_start_clicked()
{
    clear();
    bool isSec1Ready = ui->edit_sec1_size->hasAcceptableInput() &&
            ui->edit_sec1_eastRoomNum->hasAcceptableInput() &&
            ui->edit_sec1_southRoomNum->hasAcceptableInput() &&
            ui->edit_sec1_westRoomNum->hasAcceptableInput() &&
            ui->edit_sec1_northRoomNum->hasAcceptableInput();

    //待修改
    bool isSec2Ready = ui->edit_sec2_inRoomRate->hasAcceptableInput();

    bool isSec4Ready = true;
    if (ui->radioButton_sec4_useCard->isChecked()) {
        isSec4Ready = isSec4Ready && ui->edit_sec4_UC_noCardNum->hasAcceptableInput();
    } else if (ui->radioButton_sec4_useCardAndId->isChecked()) {
        isSec4Ready = isSec4Ready && ui->edit_sec4_UCI_noCardNum->hasAcceptableInput();
    } else {
        isSec4Ready = true;
    }

    bool isSec6Ready = true;
    if (!ui->radioButton_sec6_ETM->isChecked()) {
        if (ui->checkBox_sec6_keepHeat->isChecked()) {
            isSec6Ready = isSec6Ready && ui->edit_sec6_keepHeatTempSet->hasAcceptableInput();
        }

        if (ui->checkBox_sec6_airconTempSet->isChecked()) {
            isSec6Ready = isSec6Ready && true;
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

    bool isSec7Ready = ui->edit_sec7_light->hasAcceptableInput()
            && ui->edit_sec7_lightUsingNum->hasAcceptableInput()
            && ui->edit_sec7_TV->hasAcceptableInput()
            && ui->edit_sec7_TVUsingNum->hasAcceptableInput()
            && ui->edit_sec7_fridge->hasAcceptableInput()
            && ui->edit_sec7_otherDevice->hasAcceptableInput();

//    if (isSec1Ready && isSec2Ready && isSec4Ready && isSec6Ready && isSec7Ready) {
//        this->callEplus();
//    } else {
//        if (!isSec1Ready)
//        {
//            qDebug() << "Section1 not ready!";
//        }

//        if (!isSec2Ready)
//        {
//            qDebug() << "Section2 not ready!";
//        }

//        if (!isSec4Ready)
//        {
//            qDebug() << "Section4 not ready!";
//        }

//        if (!isSec6Ready)
//        {
//            qDebug() << "Section6 not ready!";
//        }

//        if (!isSec7Ready)
//        {
//            qDebug() << "Section7 not ready!";
//        }
//    }
    mStep();
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
    } else {
        ui->edit_sec2_quarter->setEnabled(false);
    }
}

void MainWindow::on_radioButton_sec2_month_toggled(bool checked)
{
    if (checked) {
        ui->edit_sec2_month->setEnabled(true);
    } else {
        ui->edit_sec2_month->setEnabled(false);
    }
}

void MainWindow::on_radioButton_sec4_useCard_toggled(bool checked)
{
    if (checked) {
        ui->edit_sec4_UC_noCardNum->setEnabled(true);
    } else {
        ui->edit_sec4_UC_noCardNum->setEnabled(false);
    }
}

void MainWindow::on_radioButton_sec4_useCardAndId_toggled(bool checked)
{
    if (checked) {
        ui->edit_sec4_UCI_noCardNum->setEnabled(true);
    } else {
        ui->edit_sec4_UCI_noCardNum->setEnabled(false);
    }
}

void MainWindow::on_checkBox_sec6_keepHeat_toggled(bool checked)
{
    if (checked) {
        ui->edit_sec6_keepHeatTempSet->setEnabled(true);
        ui->comboBox_sec6_keepHeatWindSet->setEnabled(true);
    } else {
        ui->edit_sec6_keepHeatTempSet->setEnabled(false);
        ui->comboBox_sec6_keepHeatWindSet->setEnabled(false);
    }
}

void MainWindow::on_checkBox_sec6_airconTempSet_toggled(bool checked)
{
    if (checked) {
        ui->edit_sec6_lowTemp->setEnabled(true);
        ui->edit_sec6_highTemp->setEnabled(true);
    } else {
        ui->edit_sec6_lowTemp->setEnabled(false);
        ui->edit_sec6_highTemp->setEnabled(false);
    }
}

void MainWindow::on_checkBox_sec6_nightSETT_toggled(bool checked)
{
    if (checked) {
        ui->timeEdit_sec6_nightStartTime->setEnabled(true);
        ui->edit_sec6_keepTime->setEnabled(true);
        ui->edit_sec6_nightTempOffset->setEnabled(true);
    } else {
        ui->timeEdit_sec6_nightStartTime->setEnabled(false);
        ui->edit_sec6_keepTime->setEnabled(false);
        ui->edit_sec6_nightTempOffset->setEnabled(false);
    }
}

void MainWindow::on_radioButton_sec6_ETM_toggled(bool checked)
{

    if (checked) {
        ui->checkBox_sec6_keepHeat->setEnabled(false);
        ui->checkBox_sec6_airconTempSet->setEnabled(false);
        ui->checkBox_sec6_nightSETT->setEnabled(false);

        ui->edit_sec6_keepHeatTempSet->setEnabled(false);
        ui->comboBox_sec6_keepHeatWindSet->setEnabled(false);
        ui->edit_sec6_lowTemp->setEnabled(false);
        ui->edit_sec6_highTemp->setEnabled(false);
        ui->timeEdit_sec6_nightStartTime->setEnabled(false);
        ui->edit_sec6_keepTime->setEnabled(false);
        ui->edit_sec6_nightTempOffset->setEnabled(false);
    } else {
        ui->checkBox_sec6_keepHeat->setEnabled(true);
        ui->checkBox_sec6_airconTempSet->setEnabled(true);
        ui->checkBox_sec6_nightSETT->setEnabled(true);
        if (ui->checkBox_sec6_keepHeat->isChecked()) {
            ui->edit_sec6_keepHeatTempSet->setEnabled(true);
            ui->comboBox_sec6_keepHeatWindSet->setEnabled(true);
        }
        if (ui->checkBox_sec6_airconTempSet->isChecked()) {
            ui->edit_sec6_lowTemp->setEnabled(true);
            ui->edit_sec6_highTemp->setEnabled(true);
        }
        if (ui->checkBox_sec6_nightSETT->isChecked()) {
            ui->timeEdit_sec6_nightStartTime->setEnabled(true);
            ui->edit_sec6_keepTime->setEnabled(true);
            ui->edit_sec6_nightTempOffset->setEnabled(true);
        }
    }
}

void MainWindow::on_radioButton_sec6_keepHeatNR_toggled(bool checked)
{
    if (checked) {
        ui->edit_sec6_keepHeatTempSetNR->setEnabled(true);
        ui->comboBox_sec6_keepHeatWindSetNR->setEnabled(true);
    } else {
        ui->edit_sec6_keepHeatTempSetNR->setEnabled(false);
        ui->comboBox_sec6_keepHeatWindSetNR->setEnabled(false);
    }
}

void MainWindow::on_radioButton_sec6_newWind_toggled(bool checked)
{
    if (checked) {
        ui->edit_sec6_averUsingTime->setEnabled(true);
    } else {
        ui->edit_sec6_averUsingTime->setEnabled(false);
    }
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
    //房间面积
    int roomSize = ui->edit_sec1_size->text().toInt();

    double averageWatts = (tvWatts + fdgWatts + otherWatts)*1000/roomSize;
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
    int roomSize = ui->edit_sec1_size->text().toInt();
    double averageWatts = lightWatts*1000/roomSize;

    QStringList dataList;
    dataList << QString::number(averageWatts, 'f', 2);
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
    qDebug() << dataList;
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

void MainWindow::checkUserInput()
{

    //@block: 计算各种状态的房间数量(包括:所有房间总数, 未租房间数量, 已租有人状态房间数组, 已租无人状态房间数组)
    //TODO:出租率(数字目前是假设的)
    double rentalRate = 0.3;
    //在室率
    double inRoomRate = ui->edit_sec2_inRoomRate->text().toInt()/24.0;

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
    _noRentRoomNum = _sumRoomNum*(1 - rentalRate);

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
        double realInRoomRate = inRoomRate*proInRoomRate/proAverInRoomRate;
        if (realInRoomRate > 1) realInRoomRate = 1;
        //每小时已租无人房间数
        int rentNoPeopleRoomNum = rentRoomNum*(1 - realInRoomRate)*(1 - keepOnRate);
        //每小时已租有人房间数
        int rentPeopleRoomNum = rentRoomNum - rentNoPeopleRoomNum;
        _rentNoPeopleRoomNumVec.push_back(rentNoPeopleRoomNum);
        _rentPeopleRoomNumVec.push_back(rentPeopleRoomNum);
    }

    isCheckRoomState = true;
}


void MainWindow::calRoomLoadAndFanWatts(EnergyForm &baseForm, EnergyForm &proposedForm)
{
    //房间面积
    int roomSize = ui->edit_sec1_size->text().toInt();

    int eastRoomNum = ui->edit_sec1_eastRoomNum->text().toInt();
    int southRoomNum = ui->edit_sec1_southRoomNum->text().toInt();
    int westRoomNum = ui->edit_sec1_westRoomNum->text().toInt();
    int northRoomNum = ui->edit_sec1_northRoomNum->text().toInt();

    //四面房间数组(用于函数调用)
    QVector<int> fourSizeRoomNum = { eastRoomNum, westRoomNum, southRoomNum, northRoomNum };

    //计算base model的房间负荷
    //每小时总房间数(用于函数调用)
    QVector<int> baseSumRoomNumVec(24, _sumRoomNum);
    //读取base.csv文件
    CsvReader baseReader(PathManager::instance()->getPath("OutPutDir") + QString("/%1/base.csv").arg(roomSize));
    baseReader.analyze();

    //房间热负荷序列
    QStringList eHeatListBase = baseReader.getColumnByTitle("BLOCK2:EAST FAN COIL:Fan Coil Heating Rate [W](Hourly)");
    QStringList wHeatListBase = baseReader.getColumnByTitle("BLOCK2:WEST FAN COIL:Fan Coil Heating Rate [W](Hourly)");
    QStringList sHeatListBase = baseReader.getColumnByTitle("BLOCK2:SOUTH FAN COIL:Fan Coil Heating Rate [W](Hourly)");
    QStringList nHeatListBase = baseReader.getColumnByTitle("BLOCK2:NORTH FAN COIL:Fan Coil Heating Rate [W](Hourly)");
    QStringList dsHeatListBase = baseReader.getColumnByTitle("DOAS SYSTEM HEATING COIL:Heating Coil Heating Rate [W](Hourly)");
    QVector<QStringList> heatGridBase = { eHeatListBase, wHeatListBase, sHeatListBase, nHeatListBase, dsHeatListBase };

    //房间冷负荷序列
    QStringList eCoolListBase = baseReader.getColumnByTitle("BLOCK2:EAST FAN COIL:Fan Coil Total Cooling Rate [W](Hourly)");
    QStringList wCoolListBase = baseReader.getColumnByTitle("BLOCK2:WEST FAN COIL:Fan Coil Total Cooling Rate [W](Hourly)");
    QStringList sCoolListBase = baseReader.getColumnByTitle("BLOCK2:SOUTH FAN COIL:Fan Coil Total Cooling Rate [W](Hourly)");
    QStringList nCoolListBase = baseReader.getColumnByTitle("BLOCK2:NORTH FAN COIL:Fan Coil Total Cooling Rate [W](Hourly)");
    QStringList dsCoolListBase = baseReader.getColumnByTitle("DOAS SYSTEM COOLING COIL:Cooling Coil Total Cooling Rate [W](Hourly)");
    QVector<QStringList> coolGridBase = { eCoolListBase, wCoolListBase, sCoolListBase, nCoolListBase, dsCoolListBase };

    //房间风机功率序列
    QStringList eFanListBase = baseReader.getColumnByTitle("BLOCK2:EAST SUPPLY FAN:Fan Electric Power [W](Hourly)");
    QStringList wFanListBase = baseReader.getColumnByTitle("BLOCK2:WEST SUPPLY FAN:Fan Electric Power [W](Hourly)");
    QStringList sFanListBase = baseReader.getColumnByTitle("BLOCK2:SOUTH SUPPLY FAN:Fan Electric Power [W](Hourly)");
    QStringList nFanListBase = baseReader.getColumnByTitle("BLOCK2:NORTH FAN COIL:Fan Coil Fan Electric Power [W](Hourly)");
    QVector<QStringList> fanGridBase = { eFanListBase, wFanListBase, sFanListBase, nFanListBase };

    baseForm._heatLoad = compose(fourSizeRoomNum, baseSumRoomNumVec, heatGridBase);
    baseForm._coolLoad = compose(fourSizeRoomNum, baseSumRoomNumVec, coolGridBase);
    baseForm._fanWatts = compose(fourSizeRoomNum, baseSumRoomNumVec, fanGridBase);


    //计算proposed models的房间负荷
    //每小时待租房间数组(用于函数调用)
    QVector<int> noRentRoomNumVec(24, _noRentRoomNum);

    //读取nr.csv r.csv rp.csv文件
    CsvReader nrReader(PathManager::instance()->getPath("OutPutDir") + QString("/%1/nr.csv").arg(roomSize));
    nrReader.analyze();
    CsvReader rReader(PathManager::instance()->getPath("OutPutDir") + QString("/%1/r.csv").arg(roomSize));
    rReader.analyze();
    CsvReader rpReader(PathManager::instance()->getPath("OutPutDir") + QString("/%1/rp.csv").arg(roomSize));
    rpReader.analyze();

    QVector<QVector<int> > modelsRoomNumVec = { noRentRoomNumVec, _rentNoPeopleRoomNumVec, _rentPeopleRoomNumVec };
    QVector<CsvReader> modelsReaderVec = { nrReader, rReader, rpReader };
    //modelsDataListVec[0] = nrDataListVec
    //modelsDataListVec[1] = rDataListVec
    //modelsDataListVec[2] = rpDataListVec
    QVector<QVector<QStringList> > modelsDataListVec;

    for (int j = 0; j < modelsRoomNumVec.size(); j++)
    {
        CsvReader csvReader = modelsReaderVec[j];
        QVector<int> perHourRoomNum = modelsRoomNumVec[j];
        //房间热负荷序列
        QStringList eHeatList = csvReader.getColumnByTitle("BLOCK2:EAST FAN COIL:Fan Coil Heating Rate [W](Hourly)");
        QStringList wHeatList = csvReader.getColumnByTitle("BLOCK2:WEST FAN COIL:Fan Coil Heating Rate [W](Hourly)");
        QStringList sHeatList = csvReader.getColumnByTitle("BLOCK2:SOUTH FAN COIL:Fan Coil Heating Rate [W](Hourly)");
        QStringList nHeatList = csvReader.getColumnByTitle("BLOCK2:NORTH FAN COIL:Fan Coil Heating Rate [W](Hourly)");
        QStringList dsHeatList = csvReader.getColumnByTitle("DOAS SYSTEM HEATING COIL:Heating Coil Heating Rate [W](Hourly)");
        QVector<QStringList> heatGrid = {eHeatList, wHeatList, sHeatList, nHeatList, dsHeatList};

        //房间冷负荷序列
        QStringList eCoolList = csvReader.getColumnByTitle("BLOCK2:EAST FAN COIL:Fan Coil Total Cooling Rate [W](Hourly)");
        QStringList wCoolList = csvReader.getColumnByTitle("BLOCK2:WEST FAN COIL:Fan Coil Total Cooling Rate [W](Hourly)");
        QStringList sCoolList = csvReader.getColumnByTitle("BLOCK2:SOUTH FAN COIL:Fan Coil Total Cooling Rate [W](Hourly)");
        QStringList nCoolList = csvReader.getColumnByTitle("BLOCK2:NORTH FAN COIL:Fan Coil Total Cooling Rate [W](Hourly)");
        QStringList dsCoolList = csvReader.getColumnByTitle("DOAS SYSTEM COOLING COIL:Cooling Coil Total Cooling Rate [W](Hourly)");
        QVector<QStringList> coolGrid = { eCoolList, wCoolList, sCoolList, nCoolList, dsCoolList };

        //房间风机功率序列
        QStringList eFanList = csvReader.getColumnByTitle("BLOCK2:EAST SUPPLY FAN:Fan Electric Power [W](Hourly)");
        QStringList wFanList = csvReader.getColumnByTitle("BLOCK2:WEST SUPPLY FAN:Fan Electric Power [W](Hourly)");
        QStringList sFanList = csvReader.getColumnByTitle("BLOCK2:SOUTH SUPPLY FAN:Fan Electric Power [W](Hourly)");
        QStringList nFanList = csvReader.getColumnByTitle("BLOCK2:NORTH FAN COIL:Fan Coil Fan Electric Power [W](Hourly)");
        QVector<QStringList> fanGrid = { eFanList, wFanList, sFanList, nFanList };

        QStringList heatList = compose(fourSizeRoomNum, perHourRoomNum, heatGrid);
        QStringList coolList = compose(fourSizeRoomNum, perHourRoomNum, coolGrid);
        QStringList fanList = compose(fourSizeRoomNum, perHourRoomNum, fanGrid);
        QVector<QStringList> dataListVec = { heatList, coolList, fanList };
        modelsDataListVec.push_back(dataListVec);
    }

    //proposed models所有类型房间的负荷、功率叠加
    //[proposedHeatLoad, proposedCoolLoad, proposedFanWatts];
    QVector<QStringList> proposedResultListVec;

    QVector<QStringList> nrDataListVec, rDataListVec, rpDataListVec;
    nrDataListVec = modelsDataListVec[0];
    rDataListVec = modelsDataListVec[1];
    rpDataListVec = modelsDataListVec[2];
    for (int k = 0; k < nrDataListVec.size(); k++)
    {
        QStringList dataListNr = nrDataListVec[k];
        QStringList dataListR = rDataListVec[k];
        QStringList dataListRp = rpDataListVec[k];
        QStringList sumResultList;
        for (int hour = 0; hour < dataListNr.size(); hour++)
        {
            double resultData = dataListNr[hour].toDouble() + dataListR[hour].toDouble()
                    + dataListRp[hour].toDouble();
            sumResultList << QString::number(resultData);
        }
        proposedResultListVec.push_back(sumResultList);
    }
    proposedForm._heatLoad = proposedResultListVec[0];
    proposedForm._coolLoad = proposedResultListVec[1];
    proposedForm._fanWatts = proposedResultListVec[2];


    //读取每间房的洗澡负荷
    QFile showerLoadProfile(PathManager::instance()->getPath("ProfileDir") + "/load/showerLoad_profile.json");
    if (!showerLoadProfile.open(QFile::ReadOnly)) { qFatal("Can't read the shower load profile!"); }
    QTextStream inStream(&showerLoadProfile);
    QJsonDocument doc = QJsonDocument::fromJson(inStream.readAll().toLatin1());
    showerLoadProfile.close();
    if (!doc.isObject() || doc.isNull()) { qFatal("The shower load profile file maybe broken!"); }
    QJsonObject root = doc.object();
    QStringList showerLoadList;
    for (int i = 0; i < root.size(); i++) {
        double data = root[QString::number(i+1)].toDouble();
        showerLoadList.push_back(QString::number(data));
    }

    //base model的冷热负荷修正
    for (int i = 0; i < baseForm._heatLoad.size(); i++) {
        int currentHour = i % 24;
        double realShowerLoad = showerLoadList[currentHour].toDouble()*_sumRoomNum;
        double heatLoad = baseForm._heatLoad[i].toDouble();
        double coolLoad = baseForm._coolLoad[i].toDouble();
        if (heatLoad > coolLoad) {
            baseForm._heatLoad[i] = QString::number(heatLoad - realShowerLoad);
        } else {
            baseForm._coolLoad[i] = QString::number(coolLoad + realShowerLoad);
        }
    }

    //proposed model的冷热负荷修正
    for (int i = 0; i < proposedForm._heatLoad.size(); i++)
    {
        int currentHour = i % 24;
        double realShowerLoad = showerLoadList[currentHour].toDouble()*_rentPeopleRoomNumVec[currentHour];
        double heatLoad = proposedForm._heatLoad[i].toDouble();
        double coolLoad = proposedForm._coolLoad[i].toDouble();
        if (heatLoad > coolLoad) {
            proposedForm._heatLoad[i] = QString::number(heatLoad - realShowerLoad);
        } else {
            proposedForm._coolLoad[i] = QString::number(coolLoad + realShowerLoad);
        }
    }

    qInfo() << "Calculate room heating load, cooling load and fans watts finish!";
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
         double heatSum = 0;
         for (int i = 0; i < dataGrid.size(); i++) {
             heatSum += dataGrid[i][hour].toDouble();
         }
         resultList << QString::number(heatSum);
     }

     return resultList;
 }


 void MainWindow::calDeviceAndLightEnergy(EnergyForm &baseForm, EnergyForm &proposedForm)
 {
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
         lightRatioList.push_back(rootDrp[QString::number(i+1)].toDouble());
     }

     //base model的设备能耗(J)
     for (int i = 0; i < 8760; i++)
     {
         double watts = 1000*(ui->edit_sec7_TV->text().toDouble() + ui->edit_sec7_fridge->text().toDouble()
                              + ui->edit_sec7_otherDevice->text().toDouble());
         double value = 3600*watts*deviceRatioList[i%24]*_sumRoomNum;
         baseForm._deviceEnergy.push_back(QString::number(value));
     }

     //base model的照明能耗(J)
     for (int i = 0; i < 8760; i++)
     {
         double watts = 1000*(ui->edit_sec7_light->text().toDouble());
         double value = 3600*watts*lightRatioList[i%24]*_sumRoomNum;
         baseForm._lightEnergy.push_back(QString::number(value));
     }

     //proposed model的设备能耗(J)
     for (int i = 0; i < 8760; i++)
     {
         double watts = 1000*(ui->edit_sec7_TV->text().toDouble() + ui->edit_sec7_fridge->text().toDouble()
                              + ui->edit_sec7_otherDevice->text().toDouble());
         double value =  3600*watts*deviceRatioList[i%24]*_rentPeopleRoomNumVec[i%24];
         proposedForm._deviceEnergy.push_back(QString::number(value));
     }

     //proposed model的照明能耗(J)
     for (int i = 0; i < 8760; i++)
     {
         double watts = 1000*(ui->edit_sec7_light->text().toDouble());
         double value = 3600*watts*lightRatioList[i%24]*_rentPeopleRoomNumVec[i%24];
         proposedForm._lightEnergy.push_back(QString::number(value));
     }
 }


 void MainWindow:: calRoomRestEnergy(EnergyForm &form)
 {
     double maxHeatLoad = 0;
     for (int i = 0; i < form._heatLoad.size(); i++) {
         double heatData = form._heatLoad[i].toDouble();
         if (heatData > maxHeatLoad) {
             maxHeatLoad = heatData;
         }
     }

     double maxCoolLoad = 0;
     for (int i = 0; i < form._coolLoad.size(); i++) {
         double coolData = form._coolLoad[i].toDouble();
         if (coolData > maxCoolLoad) {
             maxCoolLoad = coolData;
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

     //8760小时,每个时刻的锅炉能耗(J)
     for (int i = 0; i < boilerPLRList.size(); i++)
     {
         double plr = boilerPLRList[i].toDouble();
         double realEfficiency = 0.8*(0.97 + 0.0633*plr - 0.0333*plr*plr);
         double energy = form._heatLoad[i].toDouble()/realEfficiency*3600;
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

     qDebug() << "Energy calculate finnish!";
 }

/**---------------------------文件IO及Eplus调用操作---------------------------------**/

/**
 * @brief MainWindow::lStep >> 会在完成任务发射zSignal
 */
void MainWindow::lStep()
{
    //读取源idf文件
    int roomSize = ui->edit_sec1_size->text().toInt();
    QString sourceFilePath = QString(PathManager::instance()->getPath("SourceDir")+"/%1.idf").arg(roomSize);
    QString sourceNoPeFilePath = QString(PathManager::instance()->getPath("SourceNoPeDir")+"/%1.idf").arg(roomSize);
    HandleMachine *p_src = new HandleMachine(sourceFilePath);
    HandleMachine *p_srcNp = new HandleMachine(sourceNoPeFilePath);

    QThread *p_thread_src = new QThread();
    p_src->moveToThread(p_thread_src);

    //对源idf文件进行配置、操作及models分离
    //(注意：lambda表达式的捕捉参数,必须是通过值来捕捉,否则主线程在离开作用域后, 通过引用捕捉的话会导致程序崩溃)
    connect(p_thread_src, &QThread::started , [=](){
        QString cityName = ui->comboBox_sec1_city->currentText();
        p_src->initCityData(cityName);
        p_src->operate<MainWindow>(PathManager::instance()->getPath("BaseOpFile"), "opElectricEquipment", this, &MainWindow::calElecEqtWatts);
        p_src->operate<MainWindow>(PathManager::instance()->getPath("BaseOpFile"), "opLights", this, &MainWindow::calLightsWatts);
        p_srcNp->initCityData(cityName);
        p_srcNp->operate<MainWindow>(PathManager::instance()->getPath("BaseOpFile"), "opElectricEquipment", this, &MainWindow::calElecEqtWattsNope);
        p_srcNp->operate<MainWindow>(PathManager::instance()->getPath("BaseOpFile"), "opLights", this, &MainWindow::calLightsWattsNope);

        QStringList fileNameList, nopeFileNameList;
        fileNameList << "base" << "rp";
        nopeFileNameList << "nr" << "r";
        p_src->separate(fileNameList);
        p_srcNp->separate(nopeFileNameList);
    });

    connect(p_src, &HandleMachine::finishSep, p_thread_src, &QThread::quit);
    connect(p_thread_src, &QThread::finished, p_src, &QObject::deleteLater);
    connect(p_thread_src, &QThread::finished, p_srcNp, &QObject::deleteLater);
    connect(p_srcNp, &QObject::destroyed, this, &MainWindow::zSignal);
    p_thread_src->start();
}

void MainWindow::zStep()
{
    int roomSize = ui->edit_sec1_size->text().toInt();
    QString cityName = ui->comboBox_sec1_city->currentText();

    //优先运行base model
    QString baseFilePath = QString(PathManager::instance()->getPath("OutPutDir")+"/%1/base.idf").arg(roomSize);
    HandleMachine *p_base = new HandleMachine(baseFilePath);
    QThread *p_thread_base = new QThread();
    p_base->moveToThread(p_thread_base);

    connect(p_thread_base, &QThread::started, [p_base, cityName](){
        p_base->startMachine(cityName);
    });
    connect(p_base, &HandleMachine::finishExec, p_thread_base, &QThread::quit);
    connect(p_thread_base, &QThread::finished, p_base, &QObject::deleteLater);

    p_thread_base->start();

    //proposed model进行配置
    QString nrFilePath = QString(PathManager::instance()->getPath("OutPutDir")+"/%1/nr.idf").arg(roomSize);
    QString rFilePath = QString(PathManager::instance()->getPath("OutPutDir")+"/%1/r.idf").arg(roomSize);
    QString rpFilePath = QString(PathManager::instance()->getPath("OutPutDir")+"/%1/rp.idf").arg(roomSize);

    HandleMachine *p_nr = new HandleMachine(nrFilePath);
    HandleMachine *p_r = new HandleMachine(rFilePath);
    HandleMachine *p_rp = new HandleMachine(rpFilePath);

    QThread *p_thread_pro = new QThread();
    p_nr->moveToThread(p_thread_pro);
    p_r->moveToThread(p_thread_pro);
    p_rp->moveToThread(p_thread_pro);

    connect(p_thread_pro, &QThread::started, [=](){
        p_nr->configure(PathManager::instance()->getPath("NrConfigFile"));
        p_r->configure(PathManager::instance()->getPath("RConfigFile"));
        p_rp->configure(PathManager::instance()->getPath("RpConfigFile"));

        if (ui->radioButton_sec6_keepHeatNR->isChecked()) {
            p_nr->operate<MainWindow>(PathManager::instance()->getPath("NrOpFile"), "opSchComCool", this,
                                      &MainWindow::calSchComCoolNr);
            p_nr->operate<MainWindow>(PathManager::instance()->getPath("NrOpFile"), "opSchComHeat", this,
                                      &MainWindow::calSchComHeatNr);
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

        p_nr->startMachine(cityName);
        p_r->startMachine(cityName);
        p_rp->startMachine(cityName);
    });
    connect(p_rp, &HandleMachine::finishExec, [](){
        qInfo() << "Output finish!";
    });
    connect(p_rp, &HandleMachine::finishExec, p_thread_pro, &QThread::quit);
    connect(p_thread_pro, &QThread::finished, p_nr, &QObject::deleteLater);
    connect(p_thread_pro, &QThread::finished, p_r, &QObject::deleteLater);
    connect(p_thread_pro, &QThread::finished, p_rp, &QObject::deleteLater);
    connect(p_rp, &QObject::destroyed, this, &MainWindow::mSignal);
    p_thread_pro->start();
}

void MainWindow::mStep()
{
    checkUserInput();
    EnergyForm baseForm, proposedForm;
    calRoomLoadAndFanWatts(baseForm, proposedForm);
    calDeviceAndLightEnergy(baseForm, proposedForm);
    calRoomRestEnergy(baseForm);
    calRoomRestEnergy(proposedForm);
}

