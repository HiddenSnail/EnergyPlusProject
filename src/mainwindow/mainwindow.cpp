#include "./mainwindow/mainwindow.h"
#include "./utils/custom_widget.h"
#include "./utils/custom_validator.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    pdlg(nullptr),
    _pRateDialog(new SetRateDialog(this)),
    _pQuaterDialog(new QuarterDialog(this))

{
    ui->setupUi(this);
    init();
    connect(pdlg, &CustomProgressDialog::lSignal, this, &MainWindow::lStep);
    connect(pdlg, &CustomProgressDialog::zSignal, this, &MainWindow::zStep);
    connect(pdlg, &CustomProgressDialog::rSignal, this, &MainWindow::rStep);
    connect(pdlg, &CustomProgressDialog::showSignal, this, &MainWindow::showStep);
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
    initProcessDialog();
}


void MainWindow::initProcessDialog()
{
    pdlg = new CustomProgressDialog(ui->centralwidget);
    pdlg->close();
    pdlg->setWindowFlags(windowFlags()&~Qt::WindowCloseButtonHint);
    //pdlg->setWindowTitle(QString::fromLocal8Bit("正在进行计算..."));
    pdlg->setWindowTitle(_noticeList[2]);
    pdlg->setFixedSize(ui->centralwidget->width()/2, 150);
    pdlg->setCancelButton(nullptr);
    pdlg->setRange(0, 100);
    connect(this, &MainWindow::pSig, pdlg, &CustomProgressDialog::update);
}

/**
 * @brief MainWindow::initCoreData >> 初始化CoreData
 */
void MainWindow::initCoreData()
{
    _citySecMap.insert("Harbin", City::Harbin);
    _citySecMap.insert("Beijing", City::Beijing);
    _citySecMap.insert("Lanzhou", City::Lanzhou);
    _citySecMap.insert("XiAn", City::XiAn);
    _citySecMap.insert("Chengdu", City::Chengdu);
    _citySecMap.insert("Chongqing", City::Chongqing);
    _citySecMap.insert("Hangzhou", City::Hangzhou);
    _citySecMap.insert("Nanchang", City::Nanchang);
    _citySecMap.insert("Nanjing", City::Nanjing);
    _citySecMap.insert("Shanghai", City::Shanghai);
    _citySecMap.insert("Wuhan", City::Wuhan);
    _citySecMap.insert("Guangzhou", City::Guangzhou);
    _citySecMap.insert("Guiyang", City::Guiyang);
    _citySecMap.insert("Shenzhen", City::Shenzhen);
    _citySecMap.insert("Xiamen", City::Xiamen);

    _heatProNumMap.insert(1, 0.38);
    _heatProNumMap.insert(2, 0.5);
    _heatProNumMap.insert(3, 0.8);
    _heatProNumMap.insert(4, 1.5);

    for (auto key: _citySecMap.keys())
    {
        _cityNameMap.insert(key, key);
    }

    //获取房间可选面积数组
    QDir sourceDir(PathManager::instance()->getPath("SourceDir"));
    QFileInfoList fileInfoList = sourceDir.entryInfoList(QDir::Files);
    for (int i = 0; i < fileInfoList.size(); i++)
    {
        QFileInfo fileInfo = fileInfoList[i];
        int size = fileInfo.baseName().toInt();
        _roomSizeVec.push_back(size);
    }
    qSort(_roomSizeVec.begin(), _roomSizeVec.end(), [](int &a, int &b) {
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

    //section1
    ui->edit_sec1_size->setValidator(new IntValidator(1, 10000, this));
    ui->edit_sec1_roomNum->setValidator(new IntValidator(0, 10000, this));
    ui->edit_sec1_eastRoomNum->setValidator(new IntValidator(0, 10000, this));
    ui->edit_sec1_southRoomNum->setValidator(new IntValidator(0, 10000, this));
    ui->edit_sec1_westRoomNum->setValidator(new IntValidator(0, 10000, this));
    ui->edit_sec1_northRoomNum->setValidator(new IntValidator(0, 10000, this));

    //section2
    ui->edit_sec2_year->setValidator(new QRegExpValidator(float0To100Reg, this));

    //section4
    ui->edit_sec4_UC_noCardNum->setValidator(new QRegExpValidator(float0To1Reg, this));
    ui->edit_sec4_UCI_noCardNum->setValidator(new QRegExpValidator(float0To1Reg, this));

    //section6
    //已租状态：保温模式
    ui->edit_sec6_keepTempOffset->setValidator(new IntValidator(0, 10, this));
    int tempInitBottom = 18, tempInitTop = 28;
    IntValidator *summerTempValid = new IntValidator(tempInitBottom, tempInitTop, this);
    IntValidator *winterTempValid = new IntValidator(tempInitBottom, tempInitTop, this);
    ui->edit_sec6_keepCoolTemp->setValidator(summerTempValid); //已租状态:夏季温度18-28
    ui->edit_sec6_keepHeatTemp->setValidator(winterTempValid); //已租状态:冬季温度18-28
    //夏季温度 >= 冬季温度
    connect(ui->edit_sec6_keepCoolTemp, &QLineEdit::editingFinished, ui->edit_sec6_keepHeatTemp, [=]()
    {
        int summerTemp = ui->edit_sec6_keepCoolTemp->text().toInt();
        winterTempValid->setTop(summerTemp);
        QString winterTempStr = ui->edit_sec6_keepHeatTemp->text();
        int pos = 0;
        if (!winterTempStr.isEmpty() && winterTempValid->validate(winterTempStr, pos) != QValidator::Acceptable)
        {
            winterTempValid->fixup(winterTempStr);
            ui->edit_sec6_keepHeatTemp->setText(winterTempStr);
        }
    });

    //已租状态：空调面板可设定温度范围
    IntValidator *heatValid, *coolValid;
    int coolTempBottom = *_coolLoadReducePercentMap.keyBegin();
    int coolTempTop = *--_coolLoadReducePercentMap.keyEnd();
    int heatTempBottom = *_heatLoadReducePercentMap.keyBegin();
    int heatTempTop = *--_heatLoadReducePercentMap.keyEnd();
    coolValid = new IntValidator(coolTempBottom, coolTempTop, this);
    heatValid = new IntValidator(heatTempBottom, heatTempTop, this);
    ui->edit_sec6_lowTemp->setValidator(coolValid);
    ui->edit_sec6_highTemp->setValidator(heatValid);
    //制冷下限 <= 供热上限
    connect(ui->edit_sec6_lowTemp, &QLineEdit::editingFinished, ui->edit_sec6_highTemp, [=]()
    {
        int lowTemp = ui->edit_sec6_lowTemp->text().toInt();   
        if (lowTemp > heatValid->bottom())
        {
            heatValid->setBottom(lowTemp);
            QString heatTempStr = ui->edit_sec6_highTemp->text();
            //若heatTempStr不为空，则验证其是否符合要求并修改
            int pos = 0;
            if (!heatTempStr.isEmpty() &&  heatValid->validate(heatTempStr, pos) != QValidator::Acceptable)
            {
                heatValid->fixup(heatTempStr);
                ui->edit_sec6_highTemp->setText(heatTempStr);
            }
        }
        else
        {
            heatValid->setBottom(heatTempBottom);
        }
    });

    ui->edit_sec6_nightTempOffset->setValidator(new IntValidator(0, 30, this));
    ui->edit_sec6_keepTime->setValidator(new IntValidator(0, 24, this));
    //未租状态:保温模式
    int tempInitBottomNR = 18, tempInitTopNR = 28;
    IntValidator *summerTempValidNR = new IntValidator(tempInitBottomNR, tempInitTopNR, this);
    IntValidator *winterTempValidNR = new IntValidator(tempInitBottomNR, tempInitTopNR, this);
    ui->edit_sec6_keepCoolTempNR->setValidator(summerTempValidNR);
    ui->edit_sec6_keepHeatTempNR->setValidator(winterTempValidNR);
    //夏季温度 >= 冬季温度
    connect(ui->edit_sec6_keepCoolTempNR, &QLineEdit::editingFinished, ui->edit_sec6_keepHeatTempNR, [=]()
    {
        int summerTempNR = ui->edit_sec6_keepCoolTempNR->text().toInt();
        winterTempValidNR->setTop(summerTempNR);
        QString winterTempNRStr = ui->edit_sec6_keepHeatTempNR->text();
        int pos = 0;
        if (!winterTempNRStr.isEmpty() && winterTempValidNR->validate(winterTempNRStr, pos) != QValidator::Acceptable)
        {
            winterTempValidNR->fixup(winterTempNRStr);
            ui->edit_sec6_keepHeatTemp->setText(winterTempNRStr);
        }
    });
    ui->edit_sec6_averUsingTime->setValidator(new IntValidator(0, 24, this));

    //section7
    ui->edit_sec7_light->setValidator(new DoubleValidator(0, 5, 2, this));
    ui->edit_sec7_lightUsingNum->setValidator(new QRegExpValidator(float0To1Reg, this));
    ui->edit_sec7_TV->setValidator(new DoubleValidator(0, 5, 2, this));
    ui->edit_sec7_TVUsingNum->setValidator(new QRegExpValidator(float0To1Reg, this));
    ui->edit_sec7_fridge->setValidator(new DoubleValidator(0, 5, 2, this));
    ui->edit_sec7_otherDevice->setValidator(new DoubleValidator(0, 10, 2, this) );

}

/**
 * @brief MainWindow::initWidgetState >> 初始化控件的状态，并绑定相应的槽函数
 */
void MainWindow::initWidgetState()
{
    this->setMinimumSize(MIN_WIDTH, MIN_HEIGHT);

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
    heatProNum->setText(QString::number(_heatProNumMap[_citySecMap[_city]]));
    connect(cityBox, &QComboBox::currentTextChanged, [=]() {
        _city = _cityNameMap[cityBox->currentText()];
        heatProNum->setText(QString::number(_heatProNumMap[_citySecMap[_city]]));
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
    ui->btn_sec2_quarter->setEnabled(false);
    connect(ui->btn_sec2_quarter, &QPushButton::clicked, _pQuaterDialog, &QuarterDialog::exec);
    connect(ui->btn_sec2_inroomrate, &QPushButton::clicked, _pRateDialog, &SetRateDialog::exec);


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
    ui->radioButton_sec6_keepTempOffset->setChecked(true);
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

    //menubar
    QMenuBar *pMenuBar = ui->menuBar;
    QMenu *pLangMenu = new QMenu(QString::fromLocal8Bit("Language"), this);
    pMenuBar->addMenu(pLangMenu);
    QAction *pActionChi = new QAction(QString::fromLocal8Bit("中文"), pLangMenu);
    QAction *pActionEng = new QAction(QString::fromLocal8Bit("English"), pLangMenu);
    pLangMenu->addAction(pActionChi);
    pLangMenu->addAction(pActionEng);
    connect(pMenuBar, &QMenuBar::triggered, this, [=](QAction *pAction)
    {
        if (pAction->text() == QString::fromLocal8Bit("中文"))
        {
            this->setLanguage();
        }
        else if (pAction->text() == QString::fromLocal8Bit("English"))
        {
            this->setLanguage(English);
        }
        else
        {
            ;
        }
    });
}

void MainWindow::clear()
{
    _roomSize = 0;
    _result.clear();
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
    if (checkUserInput())
    {

        preImpData();
        callEplus();
        pdlg->setLabelText(_noticeList[3]);
        pdlg->exec();
    }
    else
    {
        QMessageBox::warning(this,_noticeList[0],_noticeList[1]);
        qInfo() << "Not ready!";
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
        isSec2Ready = isSec2Ready && true;
    }

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
            if (ui->radioButton_sec6_keepTempOffset->isChecked())
            {
                isSec6Ready = isSec6Ready && ui->edit_sec6_keepTempOffset->hasAcceptableInput();
            }
            else
            {
                isSec6Ready = isSec6Ready && ui->edit_sec6_keepCoolTemp->hasAcceptableInput()
                        && ui->edit_sec6_keepHeatTemp->hasAcceptableInput();
            }
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

    if (ui->radioButton_sec6_keepHeatNR->isChecked())
    {
        isSec6Ready = isSec6Ready && ui->edit_sec6_keepCoolTempNR->hasAcceptableInput()
                && ui->edit_sec6_keepHeatTempNR->hasAcceptableInput();
    }
    else
    {
        isSec6Ready = isSec6Ready && ui->edit_sec6_averUsingTime->hasAcceptableInput();
    }

    //section7
    bool isSec7Ready = ui->edit_sec7_light->hasAcceptableInput()
            && ui->edit_sec7_lightUsingNum->hasAcceptableInput()
            && ui->edit_sec7_TV->hasAcceptableInput()
            && ui->edit_sec7_TVUsingNum->hasAcceptableInput()
            && ui->edit_sec7_fridge->hasAcceptableInput()
            && ui->edit_sec7_otherDevice->hasAcceptableInput();

    QString styleOK = ".QFrame#%1 {"\
                      "background-color: rgb(255, 255, 255);"\
                      "border-radius: 2px;"\
                      "}";
    QString styleNot = ".QFrame#%1 {"\
                       "background-color: rgb(255, 246, 236);"\
                       "border-radius: 2px;"\
                       "}";

    if (!isSec1Ready)
    {
        ui->frame_1->setStyleSheet(styleNot.arg("frame_1"));
    }
    else
    {
        ui->frame_1->setStyleSheet(styleOK.arg("frame_1"));
    }

    if (!isSec2Ready)
    {
        ui->frame_2->setStyleSheet(styleNot.arg("frame_2"));
    }
    else
    {
        ui->frame_2->setStyleSheet(styleOK.arg("frame_2"));
    }

    if (!isSec4Ready)
    {
        ui->frame_4->setStyleSheet(styleNot.arg("frame_4"));
    }
    else
    {
        ui->frame_4->setStyleSheet(styleOK.arg("frame_4"));
    }

    if (!isSec6Ready)
    {
        ui->frame_6->setStyleSheet(styleNot.arg("frame_6"));
    }
    else
    {
        ui->frame_6->setStyleSheet(styleOK.arg("frame_6"));
    }

    if (!isSec7Ready)
    {
        ui->frame_7->setStyleSheet(styleNot.arg("frame_7"));
    }
    else
    {
        ui->frame_7->setStyleSheet(styleOK.arg("frame_7"));
    }

    bool isAllReady = isSec1Ready && isSec2Ready && isSec4Ready && isSec6Ready && isSec7Ready;
    return isAllReady;
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
    ui->btn_sec2_quarter->setEnabled(checked);
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
        ui->edit_sec4_UCI_noCardNum->setEnabled(checked);
        ui->lab_sec4_UCI_noCardNum->setEnabled(checked);
}

void MainWindow::on_radioButton_sec6_keepTempOffset_toggled(bool checked)
{
    ui->edit_sec6_keepTempOffset->setEnabled(checked);
}

void MainWindow::on_radioButton_sec6_keepTempSet_toggled(bool checked)
{
    ui->edit_sec6_keepCoolTemp->setEnabled(checked);
    ui->edit_sec6_keepHeatTemp->setEnabled(checked);
}

void MainWindow::on_checkBox_sec6_keepHeat_toggled(bool checked)
{
    ui->radioButton_sec6_keepTempOffset->setEnabled(checked);
    ui->lab_sec6_keepTempOffset->setEnabled(checked);

    ui->radioButton_sec6_keepTempSet->setEnabled(checked);
    ui->lab_sec6_keepCoolTemp->setEnabled(checked);
    ui->lab_sec6_keepHeatTemp->setEnabled(checked);
    ui->lab_sec6_offset_2->setEnabled(checked);
    if (checked)
    {
        if (ui->radioButton_sec6_keepTempOffset->isChecked())
        {
            on_radioButton_sec6_keepTempOffset_toggled(true);
        }
        if (ui->radioButton_sec6_keepTempSet->isChecked())
        {
            on_radioButton_sec6_keepTempSet_toggled(true);
        }
    }
    else
    {
        on_radioButton_sec6_keepTempOffset_toggled(false);
        on_radioButton_sec6_keepTempSet_toggled(false);
    }
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
    ui->lab_sec6_keepCoolTempNR->setEnabled(checked);
    ui->edit_sec6_keepCoolTempNR->setEnabled(checked);
    ui->lab_sec6_keepHeatTempNR->setEnabled(checked);
    ui->edit_sec6_keepHeatTempNR->setEnabled(checked);
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


//-------------------------------------计算操作----------------------------------------------//


void MainWindow::preImpData()
{
    //@block: 计算房间面积
    _roomSize = ui->edit_sec1_size->text().toInt();
    for (int i = 0; i < _roomSizeVec.size(); i++)
    {
        if (_roomSizeVec[i] >= _roomSize)
        {
            _roomSize = _roomSizeVec[i];
            break;
        }
        else
        {
            if (_roomSizeVec.size() - 1 == i)
            {
                _roomSize = _roomSizeVec[i];
            }
        }
    } 
}



/**---------------------------文件IO及Eplus调用操作---------------------------------**/

void MainWindow::callEplus()
{
    emit pSig(0, _noticeList[3]);
}


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
    //(注意：lambda表达式的捕捉参数,必须是通过值来捕捉,否则主线程在离开函数作用域后, 通过引用捕捉的函数内部变量会被销毁, 导致程序崩溃)
    connect(p_thread_src, &QThread::started , [=]()
    {
        QString initConfigFilePath(PathManager::instance()->getPath("ConfigDir") + QString("/init/init_sec%1_config.json").arg(_citySecMap[_city]));
        p_src->configure(initConfigFilePath);
        p_srcNp->configure(initConfigFilePath);

        double lightKW = ui->edit_sec7_light->text().toDouble();
        double tvKW = ui->edit_sec7_TV->text().toDouble();
        double fridgeKW = ui->edit_sec7_fridge->text().toDouble();
        double otherDevKW = ui->edit_sec7_otherDevice->text().toDouble();
        double tvUseNum = ui->edit_sec7_TVUsingNum->text().toDouble();
        double lightUseNum = ui->edit_sec7_lightUsingNum->text().toDouble();

        p_src->operate(OperateFactory::instance()->opElectricEquipment(_roomSize, tvKW, fridgeKW, otherDevKW, tvUseNum));
        p_src->operate(OperateFactory::instance()->opLights(_roomSize, lightKW, lightUseNum));

        p_srcNp->operate(OperateFactory::instance()->opElectricEquipment(0));
        p_srcNp->operate(OperateFactory::instance()->opLights(0));


        QStringList fileNameList, nopeFileNameList;
        fileNameList << "base" << "rp";
        //若使用新风模式,则不会去生成未租房间的idf
        if (ui->radioButton_sec6_newWind->isChecked())
        {
            nopeFileNameList << "r";
        } else {
            nopeFileNameList << "nr" << "r";
        }
        p_src->separate(fileNameList);
        p_srcNp->separate(nopeFileNameList);

        emit pSig(1, _noticeList[4]);

        p_thread_src->quit();
    });
    connect(p_thread_src, &QThread::finished, p_src, &QObject::deleteLater);
    connect(p_thread_src, &QThread::finished, p_srcNp, &QObject::deleteLater);
    connect(p_thread_src, &QThread::finished, p_thread_src, &QObject::deleteLater);
    p_thread_src->start();
}

void MainWindow::zStep()
{
    //尝试发送信号的函数, 1:base, 2:nr, 3:r, 4:rp
    static std::function<void(const unsigned int)> tryEmitMSig = [this](const unsigned int modelId)
    {
        static bool isBaseFinish = false;
        static bool isNrFinish = false;
        static bool isRFinish = false;
        static bool isRpFinish = false;
        switch (modelId) {
        case 1:
            isBaseFinish = true;
            break;
        case 2:
            isNrFinish = true;
            break;
        case 3:
            isRFinish = true;
            break;
        case 4:
            isRpFinish = true;
            break;
        default:
            break;
        }
        if (isBaseFinish && isNrFinish && isRFinish && isRpFinish)
        {
            isBaseFinish = false;
            isNrFinish = false;
            isRFinish = false;
            isRpFinish = false;
            emit pSig(2, _noticeList[5]);
        }
    };

    //要运行的文件路径模板
    QString excFilePathTemp = QString(PathManager::instance()->getPath("OutPutDir")+"/%1/%2/%2.idf").arg(_roomSize);

    //优先运行base model
    QString baseFilePath = excFilePathTemp.arg("base");
    QThread *p_thread_base = new QThread();
    connect(p_thread_base, &QThread::started, [=]()
    {
        EPHandler::instance()->callEplus(baseFilePath, _city);
        p_thread_base->quit();
        tryEmitMSig(1);
    });
    //当线程结束时，销毁线程
    connect(p_thread_base, &QThread::finished, p_thread_base, &QObject::deleteLater);
    //开始执行base model线程
    p_thread_base->start();


    //proposed model进行配置
    QString nrFilePath = excFilePathTemp.arg("nr");
    QString rFilePath = excFilePathTemp.arg("r");
    QString rpFilePath = excFilePathTemp.arg("rp");

    //若开启新风模式，则不使用Eplus调用nr.idf
    HandleMachine *p_nr = ui->radioButton_sec6_newWind->isChecked() ? nullptr : new HandleMachine(nrFilePath);
    HandleMachine *p_r = new HandleMachine(rFilePath);
    HandleMachine *p_rp = new HandleMachine(rpFilePath);
    QThread *p_thread_nr = (p_nr == nullptr) ? nullptr : new QThread();
    QThread *p_thread_r = new QThread();
    QThread *p_thread_rp = new QThread();
    //nr model线程执行过程
    if (p_nr != nullptr)
    {
        connect(p_thread_nr, &QThread::started, [=]()
        {
            //客人退房关闭窗帘
            if (ui->radioButton_sec3_CO_close->isChecked())
            {
                p_nr->configure(PathManager::instance()->getPath("ConfigDir") + QString("/shading/sh_sec%1_config.json").arg(_citySecMap[_city]));
                p_nr->configure(PathManager::instance()->getPath("ConfigDir") + QString("/global/nopeople_config.json"));
            }

            if (ui->radioButton_sec6_keepHeatNR->isChecked())
            {
                unsigned int coolTemp = ui->edit_sec6_keepCoolTempNR->text().toInt();
                unsigned int heatTemp = ui->edit_sec6_keepHeatTempNR->text().toInt();
                p_nr->operate(OperateFactory::instance()->opSchComByTemp(coolTemp, true));
                p_nr->operate(OperateFactory::instance()->opSchComByTemp(heatTemp, false));
            }
            p_nr->save();
            EPHandler::instance()->callEplus(nrFilePath, _city);
            p_thread_nr->quit();
            tryEmitMSig(2);
        });
        connect(p_thread_nr, &QThread::finished, p_nr, &QObject::deleteLater);
        connect(p_thread_nr, &QThread::finished, p_thread_nr, &QObject::deleteLater);
        p_thread_nr->start();
    }
    else
    {
        tryEmitMSig(2);
    }

    //r model线程执行过程
    connect(p_thread_r, &QThread::started, [=]()
    {
        //客人离房关闭窗帘
        if (ui->radioButton_sec3_LR_close->isChecked())
        {
            p_r->configure(PathManager::instance()->getPath("ConfigDir") + QString("/shading/sh_sec%1_config.json").arg(_citySecMap[_city]));
            p_r->configure(PathManager::instance()->getPath("ConfigDir") + QString("/global/nopeople_config.json"));
        }

        if (ui->checkBox_sec6_keepHeat->isChecked() && !ui->radioButton_sec6_ETM->isChecked())
        {
            if (ui->radioButton_sec6_keepTempOffset->isChecked())
            {
                unsigned int tempOffset = ui->edit_sec6_keepTempOffset->text().toInt();
                p_r->operate(OperateFactory::instance()->opSchComByTempOffset(tempOffset, true));
                p_r->operate(OperateFactory::instance()->opSchComByTempOffset(tempOffset, false));
            }
            else
            {
                unsigned int coolTemp = ui->edit_sec6_keepCoolTemp->text().toInt();
                unsigned int heatTemp = ui->edit_sec6_keepHeatTemp->text().toInt();
                p_r->operate(OperateFactory::instance()->opSchComByTemp(coolTemp, true));
                p_r->operate(OperateFactory::instance()->opSchComByTemp(heatTemp, false));
            }
        }

        p_r->save();
        EPHandler::instance()->callEplus(rFilePath, _city);
        p_thread_r->quit();
        tryEmitMSig(3);
    });
    connect(p_thread_r, &QThread::finished, p_r, &QObject::deleteLater);
    connect(p_thread_r, &QThread::finished, p_thread_r, &QObject::deleteLater);
    p_thread_r->start();

    //rp model线程执行过程
    connect(p_thread_rp, &QThread::started, [=]()
    {
        if (ui->checkBox_sec6_nightSETT->isChecked() && !ui->radioButton_sec6_ETM->isChecked())
        {
            QTime startTime = ui->timeEdit_sec6_nightStartTime->time();
            unsigned int keepHours = ui->edit_sec6_keepTime->text().toInt();
            unsigned int tempOffset = ui->edit_sec6_nightTempOffset->text().toInt();
            p_rp->operate(OperateFactory::instance()->opSchComByTempOffset(startTime, keepHours, tempOffset, true));
            p_rp->operate(OperateFactory::instance()->opSchComByTempOffset(startTime, keepHours, tempOffset, false));
        }
        p_rp->save();
        EPHandler::instance()->callEplus(rpFilePath, _city);
        p_thread_rp->quit();
        tryEmitMSig(4);
    });
    connect(p_thread_rp, &QThread::finished, p_rp, &QObject::deleteLater);
    connect(p_thread_rp, &QThread::finished, p_thread_rp, &QThread::deleteLater);
    p_thread_rp->start();
}

void MainWindow::rStep()
{
    std::function<double(double rate, RawDataSheet::TimeSpan timeSpan)> compFunc = [=](double rate, RawDataSheet::TimeSpan timeSpan)
    {
        QString filePathStr(PathManager::instance()->getPath("OutPutDir") + "/" + QString::number(_roomSize) + "/%1/%1.csv");
        int eRooms = ui->edit_sec1_eastRoomNum->text().toInt();
        int wRooms = ui->edit_sec1_westRoomNum->text().toInt();
        int sRooms = ui->edit_sec1_southRoomNum->text().toInt();
        int nRooms = ui->edit_sec1_northRoomNum->text().toInt();
        double rentalRate = rate;

        double keepOnRate;
        if (ui->radioButton_sec4_useCard->isChecked())
        {
            keepOnRate =  ui->edit_sec4_UC_noCardNum->text().toDouble();
        }
        else if (ui->radioButton_sec4_useCardAndId->isChecked())
        {
            keepOnRate = ui->edit_sec4_UCI_noCardNum->text().toDouble();
        }
        else
        {
            keepOnRate = 0;
        }
        D24hVector inRoomRateVec = _pRateDialog->getInRoomRateList().toVector();

        double deviceKW = ui->edit_sec7_TV->text().toDouble() * ui->edit_sec7_TVUsingNum->text().toDouble()
                + ui->edit_sec7_fridge->text().toDouble() + ui->edit_sec7_otherDevice->text().toDouble();
        double lightKW = ui->edit_sec7_light->text().toDouble() * ui->edit_sec7_lightUsingNum->text().toDouble();


        RoomState roomState(eRooms, wRooms, sRooms, nRooms, rentalRate, keepOnRate, inRoomRateVec, deviceKW, lightKW);
        if (!roomState.isCanAnalyze())
        {
            _P_ERR_OBJ_->addError("UNEXP_ERR", "RoomState can't analyze.");
        }
        RawDataSheet baseRawSheet(filePathStr.arg("base"), timeSpan);
        RawDataSheet rRawSheet(filePathStr.arg("r"), timeSpan);
        RawDataSheet rpRawSheet(filePathStr.arg("rp"), timeSpan);
        RawDataSheet nrRawSheet = ui->radioButton_sec6_newWind->isChecked() ?
                  RawDataSheet() : RawDataSheet(filePathStr.arg("nr"), timeSpan);

        RawDataSheetProcessor baseProc(baseRawSheet, roomState);
        RawDataSheetProcessor rProc(rRawSheet, roomState);
        RawDataSheetProcessor rpProc(rpRawSheet, roomState);
        RawDataSheetProcessor nrProc = nrRawSheet.isComplete() ?
                     RawDataSheetProcessor(nrRawSheet, roomState) : RawDataSheetProcessor();
        if (!baseProc.isReady())
        {
            _P_ERR_OBJ_->addError("UNEXP_ERR", "BaseProc not ready.");
        }
        if (!rProc.isReady())
        {
             _P_ERR_OBJ_->addError("UNEXP_ERR", "RProc not ready.");
        }
        if (!rpProc.isReady())
        {
             _P_ERR_OBJ_->addError("UNEXP_ERR", "RpProc not ready.");
        }
        if (nrRawSheet.isComplete())
        {
            if (!nrProc.isReady())
            {
                 _P_ERR_OBJ_->addError("UNEXP_ERR", "NrProc not ready.");
            }
        }

        if (ui->radioButton_sec5_hmlMachine->isChecked())
        {
            baseProc.fixFanWatts();
            rProc.fixFanWatts();
            rpProc.fixFanWatts();
            if (nrProc.isReady())
            {
                nrProc.fixFanWatts();
            }
        }

        HCFSheet baseHCF = baseProc.produceHCFSheet(RoomState::all);
        HCFSheet proposedHCF = rProc.produceHCFSheet(RoomState::rentedNoPe) + rpProc.produceHCFSheet(RoomState::rentedPe);
        proposedHCF = (nrProc.isReady()) ? proposedHCF + nrProc.produceHCFSheet(RoomState::noRented) : proposedHCF;

        HCFSheetProcessor::load();
        HCFSheetProcessor baseHCFProc(baseHCF, roomState);
        HCFSheetProcessor proposedHCFProc(proposedHCF, roomState);
        if (!baseHCFProc.isReady())
        {
            _P_ERR_OBJ_->addError("UNEXP_ERR", "BaseHCFProc not ready.");
        }
        if (!proposedHCFProc.isReady())
        {
             _P_ERR_OBJ_->addError("UNEXP_ERR", "ProposedHCFProc not ready.");
        }

        if (ui->radioButton_sec6_newWind->isChecked())
        {
            int hours = ui->edit_sec6_averUsingTime->text().toInt();
            proposedHCFProc.newWindConfig(_roomSize, hours);
        }

        if(ui->checkBox_sec6_airconTempSet->isChecked() && !ui->radioButton_sec6_ETM->isChecked())
        {
            int coolTemp, heatTemp;
            coolTemp = ui->edit_sec6_lowTemp->text().toInt();
            heatTemp = ui->edit_sec6_highTemp->text().toInt();
            proposedHCFProc.ACConfig(coolTemp, heatTemp);
        }

        if (ui->radioButton_sec5_2pip->isChecked())
        {
            proposedHCFProc.pip2Config();
        }

        EnergySheet baseEnergySheet = baseHCFProc.produceEnergySheet(RoomState::all);
        EnergySheet proposedEnergySheet = proposedHCFProc.produceEnergySheet(RoomState::rentedPe);
        double baseTotal = baseEnergySheet.sum();
        double proposedTotal = proposedEnergySheet.sum();
        return (baseTotal - proposedTotal)/baseTotal;
    };

    QThread *p_thread_res = new QThread();
    if (ui->radioButton_sec2_year->isChecked())
    {
        connect(p_thread_res, &QThread::started, [=]()
        {
            double rentalRate = ui->edit_sec2_year->text().toDouble() / 100.0;
            double percent = compFunc(rentalRate, RawDataSheet::TimeSpan::year);
            _result.push_back(QString::number(percent*100, 'f', 2));

            emit pSig(3, _noticeList[5]);

            p_thread_res->quit();
        });
    }
    else
    {
        connect(p_thread_res, &QThread::started, [=]()
        {
            QVector<double> rentalRateVec = _pQuaterDialog->getRentalRateVec();
            double percent1 = compFunc(rentalRateVec[0], RawDataSheet::TimeSpan::quarter1);
            double percent2 = compFunc(rentalRateVec[1], RawDataSheet::TimeSpan::quarter2);
            double percent3 = compFunc(rentalRateVec[2], RawDataSheet::TimeSpan::quarter3);
            double percent4 = compFunc(rentalRateVec[3], RawDataSheet::TimeSpan::quarter4);
            _result.push_back(QString::number(percent1*100, 'f', 2));
            _result.push_back(QString::number(percent2*100, 'f', 2));
            _result.push_back(QString::number(percent3*100, 'f', 2));
            _result.push_back(QString::number(percent4*100, 'f', 2));

            emit pSig(3, _noticeList[5]);

            p_thread_res->quit();
        });
    }
    connect(p_thread_res, &QThread::finished, p_thread_res, &QObject::deleteLater);
    p_thread_res->start();
}


void MainWindow::showStep()
{
    if (_result.size() == 1)
    {
        ui->edit_sec8_year->setText(_result[0]);
    }
    else
    {
        ui->edit_sec8_1To3Mon->setText(_result[0]);
        ui->edit_sec8_4To6Mon->setText(_result[1]);
        ui->edit_sec8_7To9Mon->setText(_result[2]);
        ui->edit_sec8_10To12Mon->setText(_result[3]);
    }
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
        this->setMinimumSize(MIN_WIDTH, MIN_HEIGHT);
        break;
    }
    case MainWindow::English:
    {
        fileName = "English";
        this->setMinimumSize(MIN_WIDTH+100, MIN_HEIGHT);
        break;
    }
    default:
    {
        fileName = "Chinese";
        this->setMinimumSize(MIN_WIDTH, MIN_HEIGHT);
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
    ui->btn_sec2_quarter->setText(sec2Arr[index++].toString());
    ui->btn_sec2_inroomrate->setText(sec2Arr[index++].toString());

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
    ui->lab_sec6_keepTempOffset->setText(sec6Arr[index++].toString());
    ui->lab_sec6_keepCoolTemp->setText(sec6Arr[index++].toString());
    ui->lab_sec6_keepHeatTemp->setText(sec6Arr[index++].toString());
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
    ui->lab_sec6_keepCoolTempNR->setText(sec6Arr[index++].toString());
    ui->lab_sec6_keepHeatTempNR->setText(sec6Arr[index++].toString());
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

    //city下拉框选项
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


    //用户提示选项
    _noticeList.clear();
    QJsonArray noticeList = lpRoot["noticeList"].toArray();
    for (auto notice: noticeList)
    {
        _noticeList << notice.toString();
    }

}
