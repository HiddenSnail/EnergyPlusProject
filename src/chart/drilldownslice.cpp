#include "./chart/drilldownslice.h"

QT_CHARTS_USE_NAMESPACE

DrilldownSlice::DrilldownSlice(qreal value, QString prefix, QAbstractSeries *drilldownSeries)
    : m_drilldownSeries(drilldownSeries),
      m_prefix(prefix)
{
    setValue(value);
    updateLabel();
    setLabelFont(QFont("Microsoft YaHei", 7));
    connect(this, SIGNAL(percentageChanged()), this, SLOT(updateLabel()));
    connect(this, SIGNAL(hovered(bool)), this, SLOT(showHighlight(bool)));
}

DrilldownSlice::~DrilldownSlice()
{

}

QAbstractSeries *DrilldownSlice::drilldownSeries() const
{
    return m_drilldownSeries;
}

void DrilldownSlice::updateLabel()
{
    QString label = m_prefix;
    label += ": ";
    label += QString::number(this->value() / (1000 * 3600), 'f', 2);
    label += "kWh, ";
    label += QString::number(this->percentage()*100, 'f', 1);
    label += "%";
    setLabel(label);
}

void DrilldownSlice::showHighlight(bool show)
{
    setLabelVisible(show);
    setExploded(show);
}
