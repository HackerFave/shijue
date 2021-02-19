#include "seriesMainForm.h"
#include <qdebug.h>
#include <QtCharts/QLineSeries>
#include <map>
#include <string>
#include <iostream>
using namespace std;
seriesMainForm::seriesMainForm(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    _btnGroup = new QButtonGroup;
   // ui.radioButton_LR->setChecked(true);
   // _btnGroup->addButton(ui.radioButton_LR, 0);
   // _btnGroup->addButton(ui.radioButton_LG, 1);
    ui.tabWidget->setCurrentIndex(0);
    connect(_btnGroup, SIGNAL(buttonClicked(int)), this, SLOT(slotRadioButtonStatus(int)));
    connect(ui.comboBox_rocLineSeries, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSetSeriesRoc(int)));
    connect(ui.comboBox_imageLineSeries, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSetSeriesImg(int)));
    connect(ui.comboBox_defectLineSeries, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSetSeriesDefect(int)));
    //ui.radioButton_LG->setVisible(false);
   // ui.radioButton_LR->setVisible(false);
    ui.graphicsView_Roc->setIsMoveStatus(false);
    ui.graphicsView_defect->setIsMoveStatus(false);
    ui.graphicsView_defect->titleClear(true);
    QString strImage = "图片级";
    ui.graphicsView_image->setTitleChart(strImage);
    QString strDefect = "缺陷级";
    ui.graphicsView_defect->setTitleChart(strDefect);

    QString str1 = "分数";
    QString str2 = "样本预测错误个数";
    ui.graphicsView_image->setX_Y_title(str1, str2);
    ui.graphicsView_defect->setX_Y_title(str1, str2);
    connect(ui.graphicsView_image, SIGNAL(signalSendPoint_X(qreal, qreal)), this, SLOT(slotSendPoints(qreal, qreal)));
    ui.graphicsView_training->setRange_X_Value(30);
    _series = new QLineSeries;
  //  ui.label->setVisible(false);
  //  ui.comboBox->setVisible(false);
    
}

seriesMainForm::~seriesMainForm()
{
}
QMap<int, QMap<float, QPointF>> seriesMainForm::getSeriesPointsRoc(std::map<int, std::map<float, std::pair<float, float>>> map)
{
    QMap<int, QMap<float, QPointF>> mapListAllPoints;   
    std::map<int, std::map<float, std::pair<float, float>>>::iterator iter;
    for(iter = map.begin(); iter != map.end(); ++iter) {
        //mapListPoints.insert(iter->first, QPointF(iter->second.first,iter->second.second));
        std::map<float, std::pair<float, float>>::iterator it;
        QMap<float, QPointF> mapListPoints;
        for (it = iter->second.begin(); it != iter->second.end(); ++it)
        {
            mapListPoints.insert(it->first, QPointF(it->second.first, it->second.second));
        }
      mapListAllPoints.insert(iter->first, mapListPoints);
    }
    return mapListAllPoints;
}
QList<QPointF> seriesMainForm::getSeriesPoints(int index,int value)
{
    switch (index)
    {
    case 0: {
        std::map<int, int> map = _pointsImgNg[value];
        QList<QPointF> listPoints;
        if (map.size() <= 0) {
            return listPoints;
        }
        std::map<int, int>::iterator it;
        for (it = map.begin(); it != map.end(); ++it)
        {
            listPoints.append(QPointF(it->first / 100.0, it->second));
        }
        return listPoints;
    }
            break;
    case 1: {
        std::map<int, int> map = _pointsImgOk[value];
        QList<QPointF> listPoints;
        if (map.size() <= 0) {
            return listPoints;
        }
        std::map<int, int>::iterator it;
        for (it = map.begin(); it != map.end(); ++it)
        {
            listPoints.append(QPointF(it->first / 100.0, it->second));
        }
        return listPoints;
    }
            break;
    case 2: {
        std::map<int, int> map = _pointsDefectNg[value];
        QList<QPointF> listPoints;
        if (map.size() <= 0) {
            return listPoints;
        }
        std::map<int, int>::iterator it;
        for (it = map.begin(); it != map.end(); ++it)
        {
            listPoints.append(QPointF(it->first / 100.0, it->second));
        }
        return listPoints;
    }
            break;
    case 3: {
        std::map<int, int> map = _pointsDefectOk[value];
        QList<QPointF> listPoints;
        if (map.size() <= 0) {
            return listPoints;
        }
        std::map<int, int>::iterator it;
        for (it = map.begin(); it != map.end(); ++it)
        {
            listPoints.append(QPointF(it->first / 100.0, it->second));
        }
        return listPoints;
    }
            break;
    default:
        break;
    }
    
    
}
//加载数据
void seriesMainForm::setSeriesPoints(std::map<int, std::map<int, int>> map,int index)
{ 
    switch (index)
    {
    case 0: {
        _pointsImgNg = map;
        addComboxItem(ui.comboBox_imageLineSeries, _pointsImgNg.size(), true);
    }
            break;
    case 1: {
        _pointsImgOk = map;
    }
            break;
    case 2: {
        _pointsDefectNg = map;
        addComboxItem(ui.comboBox_defectLineSeries, _pointsDefectNg.size(), false);
    }
            break;
    case 3: {
        _pointsDefectOk = map;
    }
            break;
    default:
        break;
    }

}
void seriesMainForm::addComboxItem(QComboBox *combox, int num, bool is_threshold)
{
        combox->clear();
        for (int i = 0; i < num; i++)
        {
            combox->addItem(QString::number(i));
            if (is_threshold) {
                _mapThreshold.insert(i, QPointF(0.6, 0.4));
            }
           
        }
        if (is_threshold) {
            QList<QPointF> listPoint2;
            listPoint2.append(QPointF(_mapThreshold.value(0).x(), 0));
            QList<QPointF> listPoint3;
            listPoint3.append(QPointF(_mapThreshold.value(0).y(), 0));
            addSeriesPoint(0, listPoint2);//ng (red)
            addSeriesPoint(1, listPoint3);//ok (green)
            ui.graphicsView_image->setX_RG_Str(QString::number(_mapThreshold.value(0).x()), QString::number(_mapThreshold.value(0).y()));
        }
}
//TC
void seriesMainForm::addSeriesPointRoc(const QMap<int, QMap<float, QPointF>> &map)
{
    _mapRocAll = map;
    ui.comboBox_rocLineSeries->clear();
    for (int i = 0; i < _mapRocAll.size(); i++)
    {
        ui.comboBox_rocLineSeries->addItem(QString::number(i));
    }
   // ui.comboBox_rocLineSeries->setCurrentIndex(1);
    ui.graphicsView_Roc->addSeriesRoc(_mapRocAll.value(0));
}
//Roc 多类别切换
void seriesMainForm::slotSetSeriesRoc(int index)
{
    ui.graphicsView_Roc->addSeriesRoc(_mapRocAll.value(index));
    setSeriesAuc_Bestthreshold(index);
}
//图片集多类别切换
void seriesMainForm::slotSetSeriesImg(int index)
{
    
   
    setYRange(dynamic_cast<chartView*>(setCurrentView(0)), _yRangeImage[index]);
    addSeriesPoint(2, getSeriesPoints(0, index));
    addSeriesPoint(3, getSeriesPoints(1, index));

    QList<QPointF> listPoint2;
    listPoint2.append(QPointF(_mapThreshold.value(index).x(), 0));
    QList<QPointF> listPoint3;
    listPoint3.append(QPointF(_mapThreshold.value(index).y(), 0));
    addSeriesPoint(0, listPoint2);//ng (red)
    addSeriesPoint(1, listPoint3);//ok (green)
    ui.graphicsView_image->setX_RG_Str(QString::number(_mapThreshold.value(index).x()), QString::number(_mapThreshold.value(index).y()));

}
//缺陷集多类别切换
void seriesMainForm::slotSetSeriesDefect(int index)
{
    setYRange(dynamic_cast<chartView*>(setCurrentView(1)), _yRangeDefect[index]);
    addSeriesPoint(dynamic_cast<chartView*>(setCurrentView(1)), 2, getSeriesPoints(2, index));
    addSeriesPoint(dynamic_cast<chartView*>(setCurrentView(1)), 3, getSeriesPoints(3, index));
}
//TC
void seriesMainForm::setSeriesAuc_Bestthreshold(std::map<int, float> auc, std::map<int, float> threshold)
{
    _auc = auc;
    _threshold = threshold;
    setSeriesAuc_Bestthreshold(0);
}
void seriesMainForm::setSeriesAuc_Bestthreshold(int index)
{
    ui.graphicsView_Roc->setAuc_Bestthreshold(_auc[index], _threshold[index]);
}
void seriesMainForm::addSeriesPoint(chartView * view,int index, const QList<QPointF> &listPoints)
{
    switch (index)
    {
    case 0: {
        view->addSeries(chartView::series_LR, listPoints);
    }
            break;
    case 1: {
        view->addSeries(chartView::series_LG, listPoints);
    }
            break;
    case 2: {
        view->addSeries(chartView::series_R, listPoints);
    }
            break;
    case 3: {
        view->addSeries(chartView::series_G, listPoints);
    }
            break;
    default:
        break;
    }
}
void seriesMainForm::addSeriesPoint(int index, const QList<QPointF> &listPoints)
{
    switch (index)
    {
    case 0: {
        ui.graphicsView_image->addSeries(chartView::series_LR, listPoints);
    }
            break;
    case 1: {
        ui.graphicsView_image->addSeries(chartView::series_LG, listPoints);
    }
            break;
    case 2: {
        ui.graphicsView_image->addSeries(chartView::series_R, listPoints);
    }
            break;
    case 3: {
        ui.graphicsView_image->addSeries(chartView::series_G, listPoints);
    }
            break;
    default:
        break;
    }
}
void seriesMainForm::slotRadioButtonStatus(int id)
{
    switch (id)
    {
    case 0: {
        ui.graphicsView_image->setIndex(0);
    }
            break;
    case 1: {
        ui.graphicsView_image->setIndex(1);
    }
            break;
    default:
        break;
    }
}
void seriesMainForm::slotSendPoints(qreal ng, qreal ok)
{
    _mapThreshold[ui.comboBox_imageLineSeries->currentIndex()] = QPointF(QString::number(ng, 'f', 2).toDouble(), QString::number(ok, 'f', 2).toDouble());
    emit signalSendPoints(_mapThreshold);
}
void seriesMainForm::setXRangeData(const std::map<int, int> xRange,int index)
{
    switch (index)
    {
        case 0: {
            _xRangeImage = xRange;//图片集
            setXRange(dynamic_cast<chartView*>(setCurrentView(0)), _xRangeImage[1]);
        }
          break;
        case 1: {
            _xRangeDefect = xRange;//缺陷集
            setXRange(dynamic_cast<chartView*>(setCurrentView(1)), _xRangeDefect[1]);
        }
          break;
    default:
        break;
    }
}
void seriesMainForm::setYRangeData(const std::map<int, int> yRange, int index)
{

    switch (index)
    {
    case 0: {
        _yRangeImage = yRange;//图片集
        setYRange(dynamic_cast<chartView*>(setCurrentView(0)), _yRangeImage[index]);
    }
            break;
    case 1: {
        _yRangeDefect = yRange;//缺陷集
        setYRange(dynamic_cast<chartView*>(setCurrentView(1)), _yRangeDefect[index]);
    }
    default:
        break;
    }
}
void seriesMainForm::setXRange(chartView *view, const qreal value)
{
    view->setRange_X_Value(value);
}

void seriesMainForm::setYRange(chartView *view, const qreal value)
{
    view->setRange_Y_Value(value);
}
void seriesMainForm::setXRange(const qreal xRange)
{
    ui.graphicsView_training->setRange_X_Value(xRange);
}
void seriesMainForm::setYRange(const qreal yRange)
{
    ui.graphicsView_training->setRange_Y_Value(yRange);
}
void seriesMainForm::setXRG_str(QString r, QString g)
{
    ui.graphicsView_image->setX_RG_Str(r, g);
}
QGraphicsView* seriesMainForm::setCurrentView(int index)
{
    switch (index)
    {
    case 0: {
        return ui.graphicsView_image;
    }
            break;
    case 1: {
        return ui.graphicsView_defect;
    }
            break;
    case 2: {
        return ui.graphicsView_Roc;
    }
            break;
    case 3: {
        return ui.graphicsView_training;
    }
            break;
    default:
        break;
    }
}
void seriesMainForm::setDataMatrix(std::vector<int> vector)
{
    if (vector.size() <= 0) {
        return;
    }
    QMap<int, int> map;
    for (int i = 0; i < vector.size(); i++)
    {
        map.insert(i, vector.at(i));
    }
    ui.widgetDataMatrix->initDataItem(map);

}
void seriesMainForm::setAreaIndexSeries(const QMap<QString, QMap<int, qreal>> &map, QStringList &keys,std::map<int, QColor> colormap)
{
    //_series->clear();
    ui.graphicsView_training->dataClear();
    for (size_t i = 0; i < keys.size(); i++)
    {
        QLineSeries * series = new QLineSeries;
        QPen pen;
        QStringList list = keys.at(i).split("_");
        int s = list.at(1).toInt();
        //qDebug() << s;
        if (list.size() >= 2) {
            if (list.at(1).toInt() == 0) {
                pen.setColor(Qt::black);
            }
            else {
                pen.setColor(colormap[(list.at(1).toInt())]);
            }
        }      
        pen.setWidth(3);
        series->setPen(pen);
        series->setName(keys.at(i));
        QMap<int, qreal> map_points;
        QList<QPointF> pointsList;

        map_points = map.value(keys.at(i).simplified());
        /*
        QMap<QString, QMap<int, qreal>> ::const_iterator it;
        for (it = map.constBegin(); it != map.constEnd(); ++it) {
            if (it.key().simplified() == keys.at(i).simplified()) {
                map_points = it.value();
                break;
            }
        }*/
        QMap<int, qreal> ::const_iterator it_points;
        pointsList.append(QPointF(0., 0.));
        for (it_points = map_points.constBegin(); it_points != map_points.constEnd(); it_points++)
        {
            pointsList.append(QPointF(it_points.key(), it_points.value()));
        }

        ui.graphicsView_training->addSeries(pointsList, series);
    }
   
}
void seriesMainForm::dataClear()
{
    ui.widgetDataMatrix->dataClear();
    ui.graphicsView_image->dataClear();//图片集
    ui.graphicsView_defect->dataClear();//权限集
    ui.graphicsView_Roc->dataClear();// roc
    ui.graphicsView_training->dataClear();// 训练

}
QList<QPixmap> seriesMainForm::getViewGrab()
{
    QList<QPixmap> listPix;
    listPix.push_back(ui.graphicsView_image->grab());
    listPix.push_back(ui.graphicsView_Roc->grab());
    listPix.push_back(ui.widgetDataMatrix->grab());
    return listPix;
}
QList<QByteArray> seriesMainForm::getGrabPixMap()
{
    QList<QByteArray> listByteData;
    QList<QPixmap> listPix = getViewGrab();

    for (int i = 0; i < listPix.size(); i++) {
        QImage tempImage = listPix.at(i).toImage();
        tempImage = tempImage.scaled(tempImage.width(), tempImage.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        QByteArray ba;
        QBuffer buf(&ba);
        tempImage.save(&buf, "PNG", 20);
        QByteArray hexed;
        hexed += "data:image/png;base64,";
        hexed += ba.toBase64();
        buf.close();
        listByteData.push_back(hexed);
    }
    return listByteData;
}