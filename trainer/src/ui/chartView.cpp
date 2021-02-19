#include "chartView.h"
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
chartView::chartView(QWidget *parent)
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
   // m_chart->margins().setBottom(4);
   // m_chart->margins().setTop(4);
   // m_chart->margins().setRight(4);
   // m_chart->margins().setLeft(0);
    //m_chart->setMaximumSize(500, 500);
    m_chart->setTitle("Data");
    m_chart->legend()->hide();
    m_chart->setAcceptHoverEvents(true);
    setRenderHint(QPainter::Antialiasing);
    m_chart->setPos(0, 0-15);
    scene()->setSceneRect(0, 0, 400, 350);
    this->setScene(scene());
    this->setFixedSize(400,350);

   
    //this->setScene(sc)
    _axisX = new QValueAxis;
    _axisX->setRange(0, _axisX_Range);    //设置范围
    _axisX->setLabelFormat("%.1f");   //设置刻度的格式
    //_axisX->setGridLineVisible(true);   //网格线可见
    _axisX->setTickCount(2);       //设置多少格
    _axisX->setMinorTickCount(1);   //设置每格小刻度线的数目
    _axisX->setTitleVisible(true);
    _axisX->setTitleText(QStringLiteral("得分"));
    

    _axisY = new QValueAxis;
    _axisY->setRange(0, _axisY_Range);
    _axisY->setLabelFormat("%.1f");
    //_axisY->setGridLineVisible(false);
    _axisY->setTickCount(2);
    _axisY->setMinorTickCount(1);
    _axisY->setTitleText(QStringLiteral("计数"));
    m_chart->addAxis(_axisX, Qt::AlignBottom);
    m_chart->addAxis(_axisY, Qt::AlignLeft);
 
    m_chart->setContentsMargins(0, 0, 0, 0);  //设置外边界全部为0
    m_chart->setMargins(QMargins(0, 0, 0, 0));//设置内边界全部为0
    m_chart->setBackgroundRoundness(0);
   // m_chart->setBackgroundBrush(QBrush(QColor(144, 144, 144)));
    scene()->addItem(m_chart);
   this->setStyleSheet("padding:0px;border:0px solid #014F84");
       
    _axisY->setMinorGridLineVisible(false);
    _axisX->setMinorGridLineVisible(false);

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

    _scatterRed = new QScatterSeries;
    _scatterRed->setMarkerShape(QScatterSeries::MarkerShapeCircle);//圆形的点
    _scatterRed->setBorderColor(Qt::red);//边框颜色
    _scatterRed->setBrush(QBrush(Qt::red));//背景颜色
    _scatterRed->setMarkerSize(9);
    _scatterGreen = new QScatterSeries;
    _scatterGreen->setMarkerShape(QScatterSeries::MarkerShapeCircle);//圆形的点
    _scatterGreen->setBorderColor(Qt::green);//边框颜色
    _scatterGreen->setBrush(QBrush(Qt::green));//背景颜色
    _scatterGreen->setMarkerSize(9);

    m_coordX = new QGraphicsSimpleTextItem(m_chart);
    m_coordX->setPos(m_chart->size().width() / 2 + 50, m_chart->size().height());
    m_coordX->setText(tr("NG: %1").arg(x_str_R));

    m_coordY = new QGraphicsSimpleTextItem(m_chart);
    m_coordY->setPos(m_chart->size().width() / 2 - 50, m_chart->size().height());
    m_coordY->setText(tr("OK: %1").arg(x_str_G));

    // connect(series_L, &QLineSeries::clicked, this, &View::keepCallout);
    connect(_series_LRed, &QLineSeries::hovered, this, &chartView::tooltip);
    connect(_series_LGreen, &QLineSeries::hovered, this, &chartView::tooltip);
    //connect(series, &QSplineSeries::clicked, this, &View::keepCallout);
    connect(_seriesRed, &QLineSeries::hovered, this, &chartView::tooltip);

    //connect(series2, &QSplineSeries::clicked, this, &View::keepCallout);
    connect(_seriesGreen, &QLineSeries::hovered, this, &chartView::tooltip);
    connect(_seriesROC, &QLineSeries::hovered, this, &chartView::tooltip);
    connect(_scatterRed, &QScatterSeries::hovered, this, &chartView::tooltip);
    connect(_scatterGreen, &QScatterSeries::hovered, this, &chartView::tooltip);
    this->setMouseTracking(true);
}

void chartView::resizeEvent(QResizeEvent *event)
{
    if (scene()) {
        scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
        m_chart->resize(event->size());
        m_coordX->setPos(m_chart->size().width() / 2 + 80, m_chart->size().height()-10 );
        m_coordY->setPos(m_chart->size().width() / 2 - 50, m_chart->size().height()-10 );
        const auto callouts = m_callouts;
        for (Callout *callout : callouts)
            callout->updateGeometry();
    }
    this->setStyleSheet("padding:0px;border:0px solid #014F84");
    QGraphicsView::resizeEvent(event);
}
void chartView::mousePressEvent(QMouseEvent *event)
{
    
    _x= m_chart->mapToValue(event->pos()).x();
    if (abs(_x - x_str_R.toDouble()) <
        abs(_x - x_str_G.toDouble())) {
        setIndex(0);
    }
    else
    {
        setIndex(1);
    }
}
void chartView::mouseMoveEvent(QMouseEvent *event)
{
    qreal f_x = 0;
    qreal f_y = 0;
    if (event->buttons() & Qt::LeftButton &&_indexLine==0&& _isMove) {
        this->setCursor(Qt::SplitHCursor);
        f_x = m_chart->mapToValue(event->pos()).x();
        f_y = m_chart->mapToValue(event->pos()).y();
        if (QString::number(f_x, 'f', 3).simplified().toDouble()/* f_x */< x_str_G.simplified().toDouble()+ RANGE) {
            return;
        }
        if (f_x<0 || f_x>1)
            return;
        x_str_R = QString::number(f_x, 'f', 2);
        y_str_R = QString::number(f_y, 'f', 2);
        m_coordX->setText(QString("NG: %1").arg(x_str_R));
        _series_LRed->clear();
        QVector<QPointF> _vectorPoints;
        _vectorPoints.push_back(QPointF(m_chart->mapToValue(event->pos()).x(), 0));
        _vectorPoints.push_back(QPointF(m_chart->mapToValue(event->pos()).x(), _axisY_Range));
        _series_LRed->replace(_vectorPoints);

       
       // m_coordY->setText(QString("Y: %1").arg(y_str));
    }
    if (event->buttons() & Qt::LeftButton &&_indexLine == 1&&_isMove) {
        this->setCursor(Qt::SplitHCursor);
        f_x = m_chart->mapToValue(event->pos()).x();
        f_y = m_chart->mapToValue(event->pos()).y();
        if (QString::number(f_x, 'f', 3).simplified().toDouble()/*f_x*/> x_str_R.simplified().toDouble()- RANGE) {
            return;
        }
        if (f_x<0 || f_x>1)
            return;
        x_str_G = QString::number(f_x, 'f', 2);
        y_str_G = QString::number(f_y, 'f', 2);
       // m_coordX->setText(QString("X: %1").arg(x_str_G));
        m_coordY->setText(QString("OK: %1").arg(x_str_G));
        _series_LGreen->clear();
        QVector<QPointF> _vectorPoints;
        _vectorPoints.push_back(QPointF(m_chart->mapToValue(event->pos()).x(), 0));
        _vectorPoints.push_back(QPointF(m_chart->mapToValue(event->pos()).x(), _axisY_Range));
        _series_LGreen->replace(_vectorPoints);
    }
    QGraphicsView::mouseMoveEvent(event);
}
void chartView::mouseReleaseEvent(QMouseEvent *event)
{
    this->setCursor(Qt::ArrowCursor);
    if (event->button() == Qt::LeftButton) {
        emit signalSendPoint_X(x_str_R.simplified().toDouble(),x_str_G.simplified().toDouble());
    }
}
void chartView::keepCallout()
{
    m_callouts.append(m_tooltip);
    m_tooltip = new Callout(m_chart);
}

void chartView::tooltip(QPointF point, bool state)
{
    if (m_tooltip == 0)
        m_tooltip = new Callout(m_chart);
    if (state) {
            m_tooltip->setText(QString("X: %1 \nY: %2 ").arg(point.x()).arg(point.y()));
            m_tooltip->setAnchor(point);
            m_tooltip->setZValue(11);
            m_tooltip->updateGeometry();
            m_tooltip->show();       
    }
    else {
        m_tooltip->hide();
    }
}
void chartView::addSeries(chartView::seriseStyel flag, const QList<QPointF> &listPoints)
{
    switch (flag)
    {
    case series_LR: {
        _series_LRed->clear();
        if (listPoints.size() != 0) {
            _series_LRed->append(listPoints.at(0).x(), 0);
            _series_LRed->append(listPoints.at(0).x(), _axisY_Range);
        }
        m_chart->addSeries(_series_LRed);
        _series_LRed->attachAxis(_axisX);
        _series_LRed->attachAxis(_axisY);
    }
                    break;
    case series_LG:
    {
        _series_LGreen->clear();
        if (listPoints.size() != 0) {
            _series_LGreen->append(listPoints.at(0).x(), 0);
            _series_LGreen->append(listPoints.at(0).x(), _axisY_Range);
        }
        m_chart->addSeries(_series_LGreen);
        _series_LGreen->attachAxis(_axisX);
        _series_LGreen->attachAxis(_axisY);
    }
    break;
    case series_R:
    {
        _seriesRed->clear();
        _scatterRed->clear();
        if (listPoints.size() != 0) {
            if (listPoints.size() == 1) {
                _scatterRed->append(listPoints.at(0).x(), listPoints.at(0).y());
                m_chart->addSeries(_scatterRed);
                _scatterRed->attachAxis(_axisX);
                _scatterRed->attachAxis(_axisY);
            }
            else
            {
                for (int i = 0; i < listPoints.size(); ++i) {
                    _seriesRed->append(listPoints.at(i).x(), listPoints.at(i).y());
                }
                m_chart->addSeries(_seriesRed);
                _seriesRed->attachAxis(_axisX);
                _seriesRed->attachAxis(_axisY);
            }
           
        }
    }
    break;
    case series_G:
    {
        _seriesGreen->clear();
        _scatterGreen->clear();
        if (listPoints.size() != 0) {
            if (listPoints.size() == 1) {
                _scatterGreen->append(listPoints.at(0).x(), listPoints.at(0).y());
                m_chart->addSeries(_scatterGreen);
                _scatterGreen->attachAxis(_axisX);
                _scatterGreen->attachAxis(_axisY);
            }
            else
            {
                for (int i = 0; i < listPoints.size(); ++i) {
                    _seriesGreen->append(listPoints.at(i).x(), listPoints.at(i).y());
                }
                m_chart->addSeries(_seriesGreen);
                _seriesGreen->attachAxis(_axisX);
                _seriesGreen->attachAxis(_axisY);
            }
           
        }
    }
    break;
    }

}
void chartView::addSeriesRoc(const QMap<float, QPointF> &mapRoc)
{  
    _seriesROC->clear();
    QMap<float, QPointF>::const_iterator it;
    if (mapRoc.size() != 0) {
        for (it = mapRoc.begin(); it!= mapRoc.end();++it) {
            _seriesROC->append(it.value().x(),it.value().y());
        }
    }
    m_chart->addSeries(_seriesROC);
    _seriesROC->attachAxis(_axisX);
    _seriesROC->attachAxis(_axisY);
}
void chartView::setIndex(int indexLine)
{
    _indexLine = indexLine;
}
void chartView::setIsMoveStatus(bool checked)
{
    _isMove = checked;
}
void chartView::setRange_X_Value(const qreal xRange)
{
    _axisX_Range = xRange;
    _axisX->setRange(0, _axisX_Range);
}
void chartView::setRange_Y_Value(const qreal yRange)
{
    _axisY_Range = yRange;
    _axisY->setRange(0, _axisY_Range);
}
void chartView::setX_Y_title(const QString &XStr, const QString &YStr)
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
void chartView::setX_RG_Str(QString x, QString y)
{
    x_str_R = x;
    x_str_G = y;
    m_coordX->setText(tr("NG: %1").arg(x_str_R));
    m_coordY->setText(tr("OK: %1").arg(x_str_G));
}
void chartView::titleClear(bool checked)
{
    if (checked) {
        m_coordX->setVisible(false);
        m_coordY->setVisible(false);
    }
}
void chartView::dataClear()
{
    _seriesRed->clear();
    _seriesGreen->clear();
    _series_LRed->clear();
    _series_LGreen->clear();
    setX_RG_Str("", "");
}
void chartView::setTitleChart(QString &str)
{
    QFont font;
    font.setFamily("Tahoma"); // Tahoma 宋体
    m_chart->setTitleFont(font);
    m_chart->setTitle(str);
   
}