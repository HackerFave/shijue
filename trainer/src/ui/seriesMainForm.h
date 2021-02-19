#pragma once

#include <QWidget>
#include "chartView.h"
#include "ui_seriesMainForm.h"
#include <qpoint.h>
#include <qlist.h>
#include <qbuttongroup.h>
#include <map>
#include <iostream>
#include <vector>
#include "publicClass.h"
class seriesMainForm : public QWidget
{
    Q_OBJECT

public:
    seriesMainForm(QWidget *parent = Q_NULLPTR);
    ~seriesMainForm();
    void addSeriesPoint(int index, const QList<QPointF> &listPoints);
    void addSeriesPoint(chartView * view,int index,const QList<QPointF> &listPoints);
    void addSeriesPointRoc(const QMap<int, QMap<float, QPointF>> &map);
    void setSeriesAuc_Bestthreshold(std::map<int, float> auc, std::map<int, float> threshold);
    void setSeriesAuc_Bestthreshold(int index);
    QList<QPointF> getSeriesPoints(int index, int value);
    void  setSeriesPoints(std::map<int, std::map<int, int>> map, int index);
    void addComboxItem(QComboBox *combox, int num,bool is_threshold);
    QMap<int, QMap<float, QPointF>> getSeriesPointsRoc(std::map<int, std::map<float, std::pair<float, float>>> map);
    void setXRangeData(const std::map<int, int> xRange, int index);
    void setYRangeData(const std::map<int, int> yRange, int index);
    void setXRange(chartView *view, const qreal value);
    void setYRange(chartView *view, const qreal value);
    void setXRange(const qreal xRange);
    void setYRange(const qreal yRange);
    void setXRG_str(QString r, QString g);
    QGraphicsView* setCurrentView(int index);
    void setDataMatrix(std::vector<int> vector);
    void setAreaIndexSeries(const QMap<QString, QMap<int, qreal>> &map, QStringList &keys, std::map<int, QColor> colormap);
    void dataClear();
    QList<QByteArray> getGrabPixMap();
    QList<QPixmap> getViewGrab();
public slots:
    void slotRadioButtonStatus(int id);
    void slotSendPoints(qreal, qreal);
    void slotSetSeriesRoc(int);
    void slotSetSeriesImg(int);
    void slotSetSeriesDefect(int);
signals:
    void signalSendPoints(QMap<int,QPointF>);
    void signalTest(QString, QString);

private:
    Ui::seriesMainForm ui;
    chartView _Chart;
    QButtonGroup * _btnGroup = Q_NULLPTR;
    QLineSeries * _series;
    QMap<int, QMap<float, QPointF>> _mapRocAll;//roc多类别点集
    std::map<int, float> _auc; //
    std::map<int, float> _threshold;//
    std::map<int, int> _yRangeImage;//图片集Y轴最大值
    std::map<int, int> _yRangeDefect;//缺陷集Y轴最大值
    std::map<int, int> _xRangeImage;
    std::map<int, int> _xRangeDefect;
    std::map<int, std::map<int, int>> _pointsImgNg;
    std::map<int, std::map<int, int>> _pointsImgOk;
    std::map<int, std::map<int, int>> _pointsDefectNg;
    std::map<int, std::map<int, int>> _pointsDefectOk;
    QGraphicsView * _viewImg = Q_NULLPTR;
    QMap<int,QPointF> _mapThreshold;
};
