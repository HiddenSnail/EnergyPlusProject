#ifndef DRILLDOWNCHART_H
#define DRILLDOWNCHART_H

#include <QtCharts/QChart>

QT_CHARTS_BEGIN_NAMESPACE
class QAbstractSeries;
class QPieSlice;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class DrilldownChart : public QChart
{
    Q_OBJECT
private:
    void init();
public:
    explicit DrilldownChart(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0);
    ~DrilldownChart();
    void changeSeries(QAbstractSeries *series);
public Q_SLOTS:
    void handleSliceClicked(QPieSlice *slice);

private:
    QAbstractSeries *m_currentSeries;
};

#endif // DRILLDOWNCHART_H
