#pragma once

#include <QGraphicsView>

/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Charts module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/
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
class chartView : public QGraphicsView
{
    Q_OBJECT
  

public:
    enum seriseStyel {

        series_LR = 0,
        series_LG = 1,
        series_R = 2,
        series_G = 3

    };
    chartView(QWidget *parent = 0);
    void addSeries(chartView::seriseStyel, const QList<QPointF> &listPoints);
    void addSeriesRoc(const QMap<float,QPointF> &map);
    void setIndex(const int indexLine);
    void setIsMoveStatus(bool checked);
    void setRange_X_Value(const qreal xRange);
    void setRange_Y_Value(const qreal yRange); 
    void setX_Y_title(const QString &XStr,const QString &YStr);
    void setX_RG_Str(QString x, QString y);
    void titleClear(bool checked);
    void dataClear();
    void setTitleChart(QString &str);
public slots:
    void keepCallout();
    void tooltip(QPointF point, bool state);
protected:
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
signals:
    void signalSendPoint_X(qreal NG,qreal OK);
private:
    QGraphicsSimpleTextItem *m_coordX;
    QGraphicsSimpleTextItem *m_coordY;
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
    QScatterSeries *_scatterRed = Q_NULLPTR;
    QScatterSeries *_scatterGreen = Q_NULLPTR;
    QLineSeries *_seriesRed = Q_NULLPTR;//红色曲线
    QLineSeries *_seriesGreen = Q_NULLPTR;//绿色曲线
    QLineSeries *_seriesROC = Q_NULLPTR;//ROC曲线
    int _indexLine = 0;
    QString x_str_R ="" ;//红色分界线x坐标;
    QString y_str_R;
    QString x_str_G = "" ;//绿色分界线x坐标;
    QString y_str_G;
    bool _isMove = true;
    qreal _x;
};

