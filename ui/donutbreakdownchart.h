
#ifndef DONUTBREAKDOWNCHART_H
#define DONUTBREAKDOWNCHART_H

#include <QtCharts/QChart>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieLegendMarker>
#include "chartslice.h"
QT_CHARTS_USE_NAMESPACE


class DonutBreakdownChart : public QChart
{
public:
    DonutBreakdownChart(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0);
	void addBreakdownSeries(QPieSeries *breakdownSeries, QColor color, ChartSlice *mainSlice);
	void updateLegendMarkers();
	void showMainLable(bool isshow);
private:
    void recalculateAngles();
	bool m_is_showmainlable;

private:
    QPieSeries *m_mainSeries;
};

#endif // DONUTBREAKDOWNCHART_H
