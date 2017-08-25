#include "./chart/datastage.h"

QHash<QString, int> DataStage::_monthRange =
{
    {"Jue", 744},
    {"Feb", 672},
    {"Mar", 744},
    {"Apr", 720},
    {"May", 744},
    {"Jun", 720},
    {"Jul", 744},
    {"Aug", 744},
    {"Sep", 720},
    {"Oct", 744},
    {"Nov", 720},
    {"Dec", 744}
};

QHash<QString, QString> DataStage::_langHash =
{
    {"Light", QString::fromLocal8Bit("照明能耗")},
    {"Device", QString::fromLocal8Bit("设备能耗")},
    {"CooMachine", QString::fromLocal8Bit("冷机能耗")},
    {"BoilerFuelUse", QString::fromLocal8Bit("锅炉能耗")},
    {"CooTower", QString::fromLocal8Bit("冷却塔能耗")},
    {"FreWaterPump", QString::fromLocal8Bit("冷冻水泵能耗")},
    {"CooWaterPump", QString::fromLocal8Bit("冷却水泵能耗")},
    {"HotWaterPump", QString::fromLocal8Bit("热水泵能耗")},
    {"Fan", QString::fromLocal8Bit("风机能耗")}
};

QList<QString> DataStage::getMonths(int startMon, int endMon)
{
    static QList<QString> months = {
        "Jue", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    QList<QString> realMonths;
    if (startMon <= 0 || endMon <= 0 || endMon < startMon || endMon > 12)
    {
        realMonths = months;
    }
    else
    {
        for (int i = startMon - 1; i < endMon; i++) {
            realMonths << months[i];
        }
    }
    return realMonths;
}

void DataStage::resizeEvent(QResizeEvent *event)
{
    int btnWidth = btnL->width(), btnHeight = btnL->height();
    chart1->setGeometry(btnWidth, 0, (event->size().width()- 2*btnL->width())/2, event->size().height());
    chart2->setGeometry((event->size().width()- 2*btnL->width())/2 + btnWidth, 0, (event->size().width()- 2*btnL->width())/2, event->size().height());

    barChart->setGeometry(btnWidth, 0, event->size().width() - btnWidth*2, event->size().height());
    btnL->setGeometry(0, event->size().height()/2 - btnHeight, btnWidth, btnHeight);
    btnR->setGeometry(event->size().width() - btnWidth, event->size().height()/2 - btnHeight, btnWidth, btnHeight);
}

DataStage::DataStage()
{
    scene = new QGraphicsScene();
    this->setWindowTitle("Chart");
    this->setRenderHint(QPainter::Antialiasing);
    this->setSceneRect(0, 0, 1400, 800);
    this->setMinimumSize(1400, 800);
    scene->setBackgroundBrush(QBrush(QColor(240, 240, 240)));

    btnL = new QPushButton();
    btnR = new QPushButton();
    btnL->setIcon(QIcon(":/res/icons/L.png"));
    btnL->setIconSize(QSize(50, 100));
    btnR->setIcon(QIcon(":/res/icons/R.png"));
    btnR->setIconSize(QSize(50, 100));
    btnL->setStyleSheet("background-color: rgb(240, 240, 240);");
    btnR->setStyleSheet("background-color: rgb(240, 240, 240);");
    btnL->setFlat(true);
    btnR->setFlat(true);

    L = scene->addWidget(btnL);
    R = scene->addWidget(btnR);
    int btnWidth = 50, btnHeight = 100;
    btnL->setGeometry(0, this->height()/2 - btnHeight, btnWidth, btnHeight);
    btnR->setGeometry(this->width() - btnWidth, this->height()/2 - btnHeight, btnWidth, btnHeight);
    this->setScene(scene);

    QObject::connect(btnL, &QPushButton::clicked, this, [=](){
        showAsBarChart();
        btnL->setEnabled(false);
        btnR->setEnabled(true);
    });

    QObject::connect(btnR, &QPushButton::clicked, this, [=](){
        showAsPieChart();
        btnR->setEnabled(false);
        btnL->setEnabled(true);
    });

    chart1 = new DrilldownChart();
    chart1->setAnimationOptions(QChart::AllAnimations);
    chart1->legend()->setVisible(true);
    chart1->legend()->setAlignment(Qt::AlignRight);
    chart2 = new DrilldownChart();
    chart2->setAnimationOptions(QChart::AllAnimations);
    chart2->legend()->setVisible(true);
    chart2->legend()->setAlignment(Qt::AlignRight);

    barChart = new DrilldownChart();
    barChart->setAnimationOptions(QChart::SeriesAnimations);
    barChart->legend()->setVisible(true);
    barChart->legend()->setAlignment(Qt::AlignBottom);
}

DataStage::DataStage(EnergySheet s1, EnergySheet s2)
    :DataStage()
{
    setData(s1, s2);
}

DataStage::~DataStage()
{
    delete scene;
    delete chart1;
    delete chart2;
    delete barChart;
}

void DataStage::setData(EnergySheet s1, EnergySheet s2)
{
    this->s1 = s1;
    this->s2 = s2;
}

void DataStage::setPieChartData(DrilldownChart *chart, EnergySheet sheet, QString chartName, int quarter)
{
    QPieSeries *mainSeries = new QPieSeries();
    mainSeries->setName(chartName);
    QList<QString> kinds = EnergySheet::_idMap.keys();
    QList<QString> months;
    switch (quarter) {
    case 1:
        months = getMonths(1, 3);
        break;
    case 2:
        months = getMonths(4, 6);
        break;
    case 3:
        months = getMonths(7, 9);
        break;
    case 4:
        months = getMonths(10, 12);
        break;
    default:
        months = getMonths(1, 12);
        break;
    }
    for (auto kind: kinds)
    {
        QPieSeries *detailSeries = new QPieSeries(mainSeries);
        detailSeries->setName(chartName + " - " + _langHash[kind]);
        int head = 0;
        for (auto month: months)
        {
            int range = _monthRange[month];
            *detailSeries << new DrilldownSlice(sheet[kind].sum(head, range), month, mainSeries);
            head += range;
        }
        *mainSeries << new DrilldownSlice(sheet[kind].sum(), _langHash[kind], detailSeries);
        QObject::connect(detailSeries, SIGNAL(clicked(QPieSlice*)), chart, SLOT(handleSliceClicked(QPieSlice*)));
    }
    QObject::connect(mainSeries, SIGNAL(clicked(QPieSlice*)), chart, SLOT(handleSliceClicked(QPieSlice*)));
    chart->changeSeries(mainSeries);
}

void DataStage::newPieChartData(int quarter)
{
    setPieChartData(chart1, s1, "Base Model", quarter);
    setPieChartData(chart2, s2, "Proposed Model", quarter);
    int btnWidth = btnL->width();
    chart1->setGeometry(btnWidth, 0, (this->size().width()- 2*btnL->width())/2 , this->size().height());
    chart2->setGeometry((this->size().width()- 2*btnL->width())/2 + btnWidth, 0, (this->size().width()- 2*btnL->width())/2, this->size().height());
}

void DataStage::showAsPieChart()
{
    if (cur_item == 2)
    {
        scene->removeItem(barChart);
    }
    scene->addItem(chart1);
    scene->addItem(chart2);
    scene->removeItem(L);
    scene->removeItem(R);
    scene->addItem(L);
    scene->addItem(R);
    cur_item = 1;
    this->viewport()->repaint();
    this->show();
}

void DataStage::newBarChartData(int quarter)
{
    QBarSet *baseSet = new QBarSet("Base Model");
    QBarSet *proSet = new QBarSet("Proposed Model");
    QList<QString> months;
    switch (quarter) {
    case 1:
        months = getMonths(1, 3);
        break;
    case 2:
        months = getMonths(4, 6);
        break;
    case 3:
        months = getMonths(7, 9);
        break;
    case 4:
        months = getMonths(10, 12);
        break;
    default:
        months = getMonths(1, 12);
        break;
    }

    int head = 0;
    for (auto month: months)
    {
        int range = _monthRange[month];
        *baseSet << s1.sum(head, range) / (1000 * 3600);
        *proSet << s2.sum(head, range) / (1000 * 3600);
        head += range;
    }

    barChart->removeAxis(barChart->axisX());
    barChart->removeAxis(barChart->axisY());

    QBarSeries *barSeries = new QBarSeries();
    barSeries->append(baseSet);
    barSeries->append(proSet);
    barChart->changeSeries(barSeries);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(months);
    axisX->setRange(months.first(), months.back());
    axisX->setTitleFont(QFont("Microsoft YaHei",8));
    axisX->setLabelsFont(QFont("Microsoft YaHei",8));
    barChart->setAxisX(axisX, barSeries);

    QValueAxis *axisY = new QValueAxis();
    axisY->setMin(0);
    axisY->setLabelFormat("%.0f");
    axisY->setTitleText("kWh");
    axisY->setTitleFont(QFont("Microsoft YaHei",8));
    axisY->setLabelsFont(QFont("Microsoft YaHei",8));
    barChart->setAxisY(axisY, barSeries);
    barChart->setGeometry(btnL->width(), 0, this->width() - 2*btnL->width() , this->height());
}

void DataStage::showAsBarChart()
{
    if (cur_item == 1)
    {
        scene->removeItem(chart1);
        scene->removeItem(chart2);
    }
    scene->addItem(barChart);
    cur_item = 2;
    scene->removeItem(L);
    scene->removeItem(R);
    scene->addItem(L);
    scene->addItem(R);
    this->viewport()->repaint();
    this->show();
}

void DataStage::showChart()
{
    btnL->setEnabled(false);
    btnR->setEnabled(true);
    showAsBarChart();
}

double DataStage::getSumPercent()
{
    return (s1.sum() - s2.sum())/s1.sum();
}

void DataStage::setChartInfo(int quarter)
{
    newBarChartData(quarter);
    newPieChartData(quarter);
}
