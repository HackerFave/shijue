#pragma once

#include <QGraphicsView>

#include <QtWidgets/QGraphicsView>
#include <QtCharts/QChartGlobal>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QMouseEvent;
class QResizeEvent;
QT_END_NAMESPACE

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
QT_CHARTS_END_NAMESPACE

class Callout;

QT_CHARTS_USE_NAMESPACE
#pragma execution_character_set("utf-8")
class chartViewRoc : public QGraphicsView
{
    Q_OBJECT


public:
    enum seriseStyel {

        series_LR = 0,
        series_LG = 1,
        series_R = 2,
        series_G = 3

    }; 
    chartViewRoc(QWidget *parent);
    void addSeriesRoc(int key);
    void addSeriesRoc(const QMap<float, QPointF> &map);
    void addScatterSeries(QPointF points);
    void setIndex(int indexLine);
    void setIsMoveStatus(bool checked);
    void setRange_X_Value(const qreal xRange);
    void setRange_Y_Value(const qreal yRange);
    void setX_Y_title(const QString &XStr,const QString &YStr);
    void dataClear();
    void setAuc_Bestthreshold(float m_coordXValue, float m_coordYValue);
public slots:
    void keepCallout();
    void tooltip(QPointF point, bool state);
    void scatterTooltip(QPointF point, bool state);
protected:
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
signals:
    void signalSendPoint_X(qreal NG, qreal OK);
private:
    QGraphicsSimpleTextItem *m_coordX = Q_NULLPTR;
    QGraphicsSimpleTextItem *m_coordY = Q_NULLPTR;
    QChart *m_chart;//表格
    Callout *m_tooltip;
    QList<Callout *> m_callouts;
    QLineSeries *_series_LRed = Q_NULLPTR;//红色直线
    QLineSeries *_series_LGreen = Q_NULLPTR;//绿色直线
    QValueAxis *_axisX = Q_NULLPTR;//x轴
    QValueAxis *_axisY = Q_NULLPTR;//y轴
    QString _axisXTitle = "";
    QString _axisYTitle = "";
    qreal _axisX_Range = 1.2;//x轴最大值
    qreal _axisY_Range = 1.2;//y轴最大值
    QLineSeries *_seriesRed = Q_NULLPTR;//红色曲线
    QLineSeries *_seriesGreen = Q_NULLPTR;//绿色曲线
    QLineSeries *_seriesROC = Q_NULLPTR;//ROC曲线
    int _indexLine = 0;
    QString x_str_R = "";//红色分界线x坐标;
    QString y_str_R;
    QString x_str_G = "";//绿色分界线x坐标;
    QString y_str_G;
    bool _isMove = true;
    qreal _x;
    QMap<float, QPointF> _mapRoc;
    QMap<int, QMap<float, QPointF>> _mapRocAll;
    QMap<int, QPointF> _mapRocPointsValue;
    QScatterSeries *_scatterROC = Q_NULLPTR;
    float _thresholdValue;
};

