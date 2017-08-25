#ifndef DATASTAGE_H
#define DATASTAGE_H
#include <QHash>
#include <QPair>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChartView>
#include <QtCharts/QLegend>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include "./core/sheet.h"
#include "drilldownchart.h"
#include "drilldownslice.h"

class DataStage: public QGraphicsView {
    Q_OBJECT
    //QPair<开始位置，跨度>
    static QHash<QString, int> _monthRange;
    static QVector<QString> _months;
    static QHash<QString, QString> _langHash;
private:
    int cur_item = 0;
    EnergySheet s1, s2;
    DrilldownChart *chart1, *chart2;
    DrilldownChart *barChart;
    QGraphicsScene *scene;
    QPushButton *btnL;
    QPushButton *btnR;
    QGraphicsProxyWidget *L, *R;
private:
    static QList<QString> getMonths(int startMon, int endMon);
    void setPieChartData(DrilldownChart *chart, EnergySheet sheet, QString chartName, int quarter = 0);
    void newPieChartData(int quarter = 0);
    void newBarChartData(int quarter = 0);
protected:
    void resizeEvent(QResizeEvent *event);
public:
    DataStage();
    ~DataStage();
    DataStage(EnergySheet s1, EnergySheet s2);
    void setData(EnergySheet s1, EnergySheet s2);
    void setChartInfo(int quarter = 0);
    void showAsPieChart();
    void showAsBarChart();
    void showChart();
    double getSumPercent();
};

#endif // DATASTAGE_H
