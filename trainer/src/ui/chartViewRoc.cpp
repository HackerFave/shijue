#include "chartViewRoc.h"
#include <QtGui/QResizeEvent>
#include <QtWidgets/QGraphicsScene>
#include <QtCharts/QChart>
#include <QtCharts/QSplineSeries>
#include <QtWidgets/QGraphicsTextItem>
#include "Callout.h"
#include <QtGui/QMouseEvent>
#include <QtCharts/QValueAxis>
#include <math.h>
#include "qdebug.h"

#define RANGE 0.01
chartViewRoc::chartViewRoc(QWidget *parent)
    : QGraphicsView(new QGraphicsScene, parent),
    m_coordX(0),
    m_coordY(0),
    m_chart(0),
    m_tooltip(0)
{
    //setDragMode(QGraphicsView::NoDrag);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//不出现滚动条垂直方向
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    m_chart = new QChart;
  //  m_chart->margins().setBottom(4);
   // m_chart->margins().setTop(4);
   // m_chart->margins().setRight(4);
   // m_chart->margins().setLeft(0);
    //m_chart->setMaximumSize(500, 500);
    QFont font;
    font.setFamily("Tahoma"); // Tahoma 宋体
    m_chart->setTitleFont(font);
    m_chart->setTitle("ROC曲线");
    m_chart->legend()->hide();
    m_chart->setAcceptHoverEvents(true);
    setRenderHint(QPainter::Antialiasing);
    m_chart->setPos(0, 0-15 );
    scene()->setSceneRect(0, 0, 400, 350);

    this->setScene(scene());
    this->setFixedSize(400, 350);
    //this->setScene(sc)
    _axisX = new QValueAxis;
    _axisX->setRange(0, _axisX_Range);    //设置范围
    _axisX->setLabelFormat("%.1f");   //设置刻度的格式
    //_axisX->setGridLineVisible(false);   //网格线可见
    _axisX->setTickCount(2);       //设置多少格
    _axisX->setMinorTickCount(1);   //设置每格小刻度线的数目
    _axisX->setTitleVisible(true);
   // _axisX->setTitleText(QStringLiteral("fpr"));


    _axisY = new QValueAxis;
    _axisY->setRange(0, _axisY_Range);
    _axisY->setLabelFormat("%.1f");
    //_axisY->setGridLineVisible(false);
    _axisY->setTickCount(2);
    _axisY->setMinorTickCount(1);
   // _axisY->setTitleText(QStringLiteral("tpr"));
    QString str1 = "fpr";
    QString str2 = "tpr";
    setX_Y_title(str1, str2);
    m_chart->addAxis(_axisX, Qt::AlignBottom);
    m_chart->addAxis(_axisY, Qt::AlignLeft);
    _axisY->setMinorGridLineVisible(false);
    _axisX->setMinorGridLineVisible(false);
    m_chart->setContentsMargins(0, 0, 0, 0);  //设置外边界全部为0
    m_chart->setMargins(QMargins(0, 0, 0, 0));//设置内边界全部为0
    m_chart->setBackgroundRoundness(0);
    // m_chart->setBackgroundBrush(QBrush(QColor(144, 144, 144)));
    scene()->addItem(m_chart);
    this->setStyleSheet("padding:0px;border:0px;");
    _series_LRed = new QLineSeries;//红色直线
    _series_LRed->setPen(QPen(QBrush(Qt::red), 3, Qt::DotLine));

    _series_LGreen = new QLineSeries;//绿色直线
    _series_LGreen->setPen(QPen(QBrush(Qt::green), 3, Qt::DotLine));


    _seriesRed = new QLineSeries;//红色曲线
   // QPen pen(QColor("red"));
   // pen.setWidth(3);
    _seriesRed->setPen(QPen(QBrush(Qt::red), 3, Qt::SolidLine));

    _seriesGreen = new QLineSeries;
    //QPen pen2(QColor("green"));
    //pen2.setWidth(3);
    _seriesGreen->setPen(QPen(QBrush(Qt::green), 3, Qt::SolidLine));

    _seriesROC = new QLineSeries;
    //QPen pen2(QColor("green"));
    //pen2.setWidth(3);
    _seriesROC->setPen(QPen(QBrush(Qt::black), 3, Qt::SolidLine));

    m_coordX = new QGraphicsSimpleTextItem(m_chart);
    m_coordX->setPos(m_chart->size().width() / 2 + 50, m_chart->size().height());
    m_coordX->setText("曲线下面积AUC:");

    m_coordY = new QGraphicsSimpleTextItem(m_chart);
    m_coordY->setPos(m_chart->size().width() / 2 - 50, m_chart->size().height());
    m_coordY->setText("最佳阈值:");

    connect(_seriesROC, &QLineSeries::hovered, this, &chartViewRoc::tooltip);
    _scatterROC = new QScatterSeries;
    _scatterROC->setMarkerShape(QScatterSeries::MarkerShapeCircle);//圆形的点
    _scatterROC->setBorderColor(Qt::green);//边框颜色
    _scatterROC->setBrush(QBrush(Qt::green));//背景颜色
    _scatterROC->setMarkerSize(6);
    connect(_scatterROC, &QScatterSeries::hovered, this, &chartViewRoc::scatterTooltip);
    this->setMouseTracking(true);
}

void chartViewRoc::resizeEvent(QResizeEvent *event)
{
    if (scene()) {
        scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
        m_chart->resize(event->size());
        m_coordX->setPos(m_chart->size().width() / 2-140 , m_chart->size().height() - 10);
        m_coordY->setPos(m_chart->size().width() / 2 +60, m_chart->size().height() - 10);
        const auto callouts = m_callouts;
        for (Callout *callout : callouts)
            callout->updateGeometry();
    }
    this->setStyleSheet("padding:0px;border:0px solid #014F84");
    QGraphicsView::resizeEvent(event);
}
void chartViewRoc::mousePressEvent(QMouseEvent *event)
{

    _x = m_chart->mapToValue(event->pos()).x();
    if (abs(_x - x_str_R.toDouble()) <
        abs(_x - x_str_G.toDouble())) {
        setIndex(0);
    }
    else
    {
        setIndex(1);
    }
}
void chartViewRoc::mouseMoveEvent(QMouseEvent *event)
{
    qreal f_x = 0;
    qreal f_y = 0;
    if (event->buttons() & Qt::LeftButton &&_indexLine == 0 && _isMove) {
        this->setCursor(Qt::SplitHCursor);
        f_x = m_chart->mapToValue(event->pos()).x();
        f_y = m_chart->mapToValue(event->pos()).y();
        if (QString::number(f_x, 'f', 3).simplified().toDouble()/* f_x */ < x_str_G.simplified().toDouble() + RANGE) {
            return;
        }
        if (f_x<0 || f_x>_axisX_Range)
            return;
        x_str_R = QString::number(f_x, 'f', 2);
        y_str_R = QString::number(f_y, 'f', 2);
       // m_coordX->setText(QString("曲线下面积AUC: %1").arg(x_str_R));
        _series_LRed->clear();
        _series_LRed->append(m_chart->mapToValue(event->pos()).x(), 0);
        _series_LRed->append(m_chart->mapToValue(event->pos()).x(), _axisY_Range);
        m_chart->addSeries(_series_LRed);
        _series_LRed->attachAxis(_axisX);
        _series_LRed->attachAxis(_axisY);

        // m_coordY->setText(QString("Y: %1").arg(y_str));
    }
    if (event->buttons() & Qt::LeftButton &&_indexLine == 1 && _isMove) {
        this->setCursor(Qt::SplitHCursor);
        f_x = m_chart->mapToValue(event->pos()).x();
        f_y = m_chart->mapToValue(event->pos()).y();
        if (QString::number(f_x, 'f', 3).simplified().toDouble()/*f_x*/ > x_str_R.simplified().toDouble() - RANGE) {
            return;
        }
        if (f_x<0 || f_x>_axisX_Range)
            return;
        x_str_G = QString::number(f_x, 'f', 2);
        y_str_G = QString::number(f_y, 'f', 2);
        // m_coordX->setText(QString("X: %1").arg(x_str_G));
        //m_coordY->setText(QString("最佳阈值: %1").arg(x_str_G));
        _series_LGreen->clear();
        _series_LGreen->append(m_chart->mapToValue(event->pos()).x(), 0);
        _series_LGreen->append(m_chart->mapToValue(event->pos()).x(), _axisY_Range);
        m_chart->addSeries(_series_LGreen);
        _series_LGreen->attachAxis(_axisX);
        _series_LGreen->attachAxis(_axisY);
    }
    QGraphicsView::mouseMoveEvent(event);
}
void chartViewRoc::mouseReleaseEvent(QMouseEvent *event)
{
    this->setCursor(Qt::ArrowCursor);
    if (event->button() == Qt::LeftButton) {
        emit signalSendPoint_X(x_str_R.simplified().toDouble(), x_str_G.simplified().toDouble());
    }
}
void chartViewRoc::keepCallout()
{
    m_callouts.append(m_tooltip);
    m_tooltip = new Callout(m_chart);
}
void chartViewRoc::scatterTooltip(QPointF point, bool state)
{
    if (m_tooltip == 0)
        m_tooltip = new Callout(m_chart);
    if (state) {
        m_tooltip->setText(QString("score:%0 \nX: %1 \nY: %2 ").arg(_thresholdValue).arg(_mapRoc.value(_thresholdValue).x()).arg(_mapRoc.value(_thresholdValue).y()));
        m_tooltip->setAnchor(point);
        m_tooltip->setZValue(11);
        m_tooltip->updateGeometry();
        m_tooltip->show();
    }
    else {
        m_tooltip->hide();
    }
}
void chartViewRoc::tooltip(QPointF point, bool state)
{
     QList<QPointF> pointsList;
    qreal temDistance = 0;
    float sc1 = 0;
    float sc2 = 0;
    float sc;
    qreal X1;
    qreal Y1;
    qreal X2;
    qreal Y2;
    if (m_tooltip == 0)
        m_tooltip = new Callout(m_chart);
    for(int i = 0; i < _mapRocPointsValue.size()-1; i++) {
            qreal dx = (_mapRocPointsValue.value(i).x() + _mapRocPointsValue.value(i + 1).x()) / 2;
            qreal dy = (_mapRocPointsValue.value(i).x() + _mapRocPointsValue.value(i + 1).x()) / 2;
            qreal distance = sqrt((dx - point.x())*(dx - point.x()) + (dy - point.y())*(dy - point.y())); 
            if (i == 0) {
                temDistance = distance;
            }            
            if (temDistance >= distance) {
                temDistance = distance;
                pointsList.clear();
                pointsList.append(QPointF(_mapRocPointsValue.value(i).x(), _mapRocPointsValue.value(i).y()));
                pointsList.append(QPointF(_mapRocPointsValue.value(i + 1).x(), _mapRocPointsValue.value(i + 1).y()));
            }
        }
    QMap<float, QPointF>::iterator it;
    for (it = _mapRoc.begin(); it != _mapRoc.end(); ++it) {
        if (it.value() == pointsList.at(0)) {
            sc1 = it.key();
            X1 = pointsList.at(0).x();
            Y1 = pointsList.at(0).y();
        }
        if (it.value() == pointsList.at(1)) {
            sc2 = it.key();
            X2 = pointsList.at(0).x();
            Y2 = pointsList.at(0).y();
        }
    }
    if ((sc1 + sc2) == 0) {
        sc = 0;
    }
    qreal diStanceAll = sqrt((X1 - X2)*(X1 - X2) + (Y1 - Y2)*(Y1 - Y2));
    qreal diStance1 = sqrt((X1 - point.x())*(X1 - point.x()) + (Y1 - point.y())*(Y1 - point.y()));
    qreal diStance2 = sqrt((point.x() - X2)*(point.x() - X2) + (point.y() - Y2)*(point.y() - Y2));
   // sc = (sc1*(diStance1/diStanceAll) + sc2*(diStance2/ diStanceAll)) / 2;
    sc = (sc1 + sc2) / 2;
    if (state) {
        m_tooltip->setText(QString("score:%0 \nX: %1 \nY: %2 ").arg(sc).arg(point.x()).arg(point.y()));
        m_tooltip->setAnchor(point);
        m_tooltip->setZValue(11);
        m_tooltip->updateGeometry();
        m_tooltip->show();
    }
    else {
        m_tooltip->hide();
    }
}
void chartViewRoc::addSeriesRoc(int key)
{
    addSeriesRoc(_mapRocAll.value(key));
}
void chartViewRoc::addSeriesRoc(const QMap<float, QPointF> &mapRoc)
{
    _seriesROC->clear();
    _mapRoc = mapRoc;
    int i = 0;
    QMap<float, QPointF>::const_iterator it;
    if (mapRoc.size() != 0) {
        for (it = mapRoc.begin(); it != mapRoc.end(); ++it) {
            _mapRocPointsValue.insert(i, QPointF(it.value().x(), it.value().y()));
            _seriesROC->append(it.value().x(), it.value().y());
            i++;
        }
    }
    m_chart->addSeries(_seriesROC);
    _seriesROC->attachAxis(_axisX);
    _seriesROC->attachAxis(_axisY);
}
void chartViewRoc::setIndex(int indexLine)
{
    _indexLine = indexLine;
}
void chartViewRoc::setIsMoveStatus(bool checked)
{
    _isMove = checked;
}
void chartViewRoc::setRange_X_Value(const qreal xRange)
{
    _axisX_Range = xRange;
    _axisX->setRange(0, _axisX_Range);
}
void chartViewRoc::setRange_Y_Value(const qreal yRange)
{
    _axisY_Range = yRange;
    _axisY->setRange(0, _axisY_Range);
}
void chartViewRoc::setX_Y_title(const QString &XStr,const QString &YStr)
{
    _axisXTitle = XStr;
    _axisYTitle = YStr;
    QFont font;
    font.setFamily("SimHei"); // Tahoma 宋体
    _axisX->setTitleFont(font);
    _axisY->setTitleFont(font);
    _axisX->setTitleText(_axisXTitle);
    _axisY->setTitleText(_axisYTitle);
}
void chartViewRoc::dataClear()
{
    _seriesROC->clear();
    _scatterROC->clear();
}
void chartViewRoc::setAuc_Bestthreshold(float m_coordXValue, float m_coordYValue)
{
    m_coordX->setText(tr("曲线下面积AUC: %1").arg(QString::number(m_coordXValue,'f',2)));
    m_coordY->setText(tr("最佳阈值: %1").arg(QString::number(m_coordYValue,'f',2)));
    _thresholdValue = m_coordYValue * 100;
    if (_mapRoc.size() > 0) {
        addScatterSeries(QPointF(_mapRoc.value(_thresholdValue).x(), _mapRoc.value(_thresholdValue).y()));
    }
   

}
void chartViewRoc::addScatterSeries(QPointF points)
{
    _scatterROC->clear();
    _scatterROC->append(points);
    m_chart->addSeries(_scatterROC);
    _scatterROC->attachAxis(_axisX);
    _scatterROC->attachAxis(_axisY);
}