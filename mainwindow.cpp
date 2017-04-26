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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    QRegExp float0To100Reg("100|(([0]|[1-9]{1,2})[\\.][0-9]{1,2})"); //识别0.00~100.00的浮点数
    QRegExp float0To1Reg("1|([0][\\.][0-9]{1,2})"); //识别0.00~1.00的浮点数
    QRegExp float0To100000Reg("100000|(([0]|[1-9][0-9]{1,4})[\\.][0-9]{1,2})"); //识别0.00~100000.00的浮点数

    //section1
    ui->comboBox_sec1_city->clear();
    QStringList items;
    items << "Harbin" << "Beijing" << "Shanghai" << "Guangzhou";
    ui->comboBox_sec1_city->addItems(items);
    ui->edit_sec1_size->setValidator(new QIntValidator(20, 1000000, this));
    ui->edit_sec1_heatProNum->setValidator(new QRegExpValidator(float0To1Reg, this));
    ui->edit_sec1_roomNum->setValidator(new QIntValidator(1, 1000000, this));
    ui->edit_sec1_singleRoomNum->setValidator(new QIntValidator(1, 1000000, this));
    ui->edit_sec1_doubleRoomNum->setValidator(new QIntValidator(1, 1000000, this));
    ui->edit_sec1_suiteRoomNum->setValidator(new QIntValidator(1, 1000000, this));
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
    ui->edit_sec6_averUsingTime->setEnabled(false);

    ui->edit_sec6_keepHeatTempSet->setValidator(new QIntValidator(0, 100, this));
    ui->edit_sec6_highTemp->setValidator(new QIntValidator(0, 100, this));
    ui->edit_sec6_lowTemp->setValidator(new QIntValidator(0, 100, this));
    ui->edit_sec6_nightTempOffset->setValidator(new QIntValidator(0, 100, this));
    ui->edit_sec6_keepTime->setValidator(new QIntValidator(0, 24, this));
    ui->edit_sec6_keepHeatTempSetNR->setValidator(new QIntValidator(0, 24, this));
    ui->edit_sec6_averUsingTime->setValidator(new QIntValidator(0, 24, this));

    //section7
    ui->edit_sec7_light->setValidator(new QRegExpValidator(float0To100000Reg, this));
    ui->edit_sec7_lightUsingNum->setValidator(new QRegExpValidator(float0To1Reg, this));
    ui->edit_sec7_TV->setValidator(new QRegExpValidator(float0To100000Reg, this));
    ui->edit_sec7_TVUsingNum->setValidator(new QRegExpValidator(float0To1Reg, this));
    ui->edit_sec7_fridge->setValidator(new QRegExpValidator(float0To100000Reg, this));
    ui->edit_sec7_otherDevice->setValidator(new QRegExpValidator(float0To100000Reg, this));

}

/**----------------------------控件操作------------------------------**/

void MainWindow::on_btn_start_clicked()
{
    this->callEplus();
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
    //电视使用系数
    double tvNum = ui->edit_sec7_TVUsingNum->text().toDouble();
    //冰箱的总瓦特数(kW)
    double fdgWatts = ui->edit_sec7_fridge->text().toDouble();
    //其他设备的总平均瓦特数(kW)
    double otherWatts = ui->edit_sec7_otherDevice->text().toDouble();
    //房间面积
    int roomSize = ui->edit_sec1_size->text().toInt();

    double averageWatts = (tvWatts*tvNum + fdgWatts + otherWatts)*1000/roomSize;
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
    //照明系数
    double lightNum = ui->edit_sec7_light->text().toDouble();
    //房间面积
    int roomSize = ui->edit_sec1_size->text().toInt();
    double averageWatts = lightWatts*lightNum*1000/roomSize;

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

//void MainWindow::calResult()
//{
//    //出租率，先假设一个数字，之后修改
//    double rentalRate = 0.5;
//    double inRoomRate = ui->edit_sec2_inRoomRate->text().toInt()/24.0;

//    int eastRoomNum = ui->edit_sec1_eastRoomNum->text().toInt();
//    int southRoomNum = ui->edit_sec1_southRoomNum->text().toInt();
//    int westRoomNum = ui->edit_sec1_westRoomNum->text().toInt();
//    int northRoomNum = ui->edit_sec1_northRoomNum->text().toInt();
//    int sum = eastRoomNum + southRoomNum + westRoomNum + northRoomNum;

//    //不拔卡系数
//    double keepOnRate;
//    if (ui->radioButton_sec4_useCard->isChecked()) {
//        keepOnRate =  ui->edit_sec4_UC_noCardNum->text().toDouble();
//    }
//    else if (ui->radioButton_sec4_useCardAndId->isChecked()) {
//        keepOnRate = ui->edit_sec4_UCI_noCardNum->text().toDouble();
//    }
//    else {
//        keepOnRate = 1;
//    }

//}

/**---------------------------文件IO及Eplus调用操作---------------------------------**/

/**
 * @brief MainWindow::lStep >> 会在完成任务发射zSignal
 */
void MainWindow::lStep()
{
    //读取源idf文件
    int roomSize = ui->edit_sec1_size->text().toInt();
    QString sourceFilePath = QString(PathManager::instance()->getPath("SourceDir")+"/%1.idf").arg(roomSize);
    HandleMachine *p_src = new HandleMachine(sourceFilePath);
    QThread *p_thread_src = new QThread();
    p_src->moveToThread(p_thread_src);

    //对源idf文件进行配置、操作及models分离
    //(注意：lambda表达式的捕捉参数,必须是通过值来捕捉,否则主线程在离开作用域后, 通过引用捕捉的话会导致程序崩溃)
    connect(p_thread_src, &QThread::started , [=](){
        QString cityName = ui->comboBox_sec1_city->currentText();
        p_src->initCityData(cityName);
        p_src->operate<MainWindow>(PathManager::instance()->getPath("BaseOpFile"), "opElectricEquipment", this, &MainWindow::calElecEqtWatts);
        p_src->operate<MainWindow>(PathManager::instance()->getPath("BaseOpFile"), "opLights", this, &MainWindow::calLightsWatts);
        p_src->separate();

    });

    connect(p_src, &HandleMachine::finishSep, p_thread_src, &QThread::quit);
    connect(p_thread_src, &QThread::finished, p_src, &QObject::deleteLater);
    connect(p_src, &QObject::destroyed, this, &MainWindow::zSignal);

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

    p_thread_pro->start();
}

