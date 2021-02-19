#include "baseChartView.h"
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

#define RANGE 0.05
baseChartView::baseChartView(QWidget *parent)
    : QGraphicsView(new QGraphicsScene, parent),
    m_coordX(0),
    m_coordY(0),
    m_chart(0),
    m_tooltip(0)
{
    setDragMode(QGraphicsView::NoDrag);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//不出现滚动条垂直方向
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    m_chart = new QChart;
    //m_chart->margins().setBottom(4);
    //m_chart->margins().setTop(4);
    //m_chart->margins().setRight(4);
    //m_chart->margins().setLeft(0);
    //m_chart->setMaximumSize(500, 500);
    QFont font;
    font.setFamily("Tahoma"); // Tahoma 宋体
    m_chart->setTitleFont(font);
    m_chart->setTitle(tr("训练"));
    m_chart->legend()->hide();
    m_chart->setAcceptHoverEvents(true);
    setRenderHint(QPainter::Antialiasing);
    m_chart->setPos(0, 0-15);
    scene()->setSceneRect(0, 0, 400,350);

    this->setScene(scene());
    this->setFixedSize(400, 300);
    m_chart->setContentsMargins(0, 0, 0, 0);  //设置外边界全部为0
    m_chart->setMargins(QMargins(0, 0, 0, 0));//设置内边界全部为0
    m_chart->setBackgroundRoundness(0);
    // m_chart->setBackgroundBrush(QBrush(QColor(144, 144, 144)));
    scene()->addItem(m_chart);
    this->setStyleSheet("padding:0px;border:0px solid #014F84");
    _axisX = new QValueAxis;
    _axisX->setRange(0, _axisX_Range);    //设置范围
    _axisX->setLabelFormat("%.1f");   //设置刻度的格式
    //_axisX->setGridLineVisible(false);   //网格线可见
    _axisX->setTickCount(2);       //设置多少格
    _axisX->setMinorTickCount(1);   //设置每格小刻度线的数目
    _axisX->setTitleVisible(true);
    //_axisX->setTitleText(QStringLiteral("得分"));


    _axisY = new QValueAxis;
    _axisY->setRange(0, _axisY_Range);
    _axisY->setLabelFormat("%.1f");
    //_axisY->setGridLineVisible(false);
    _axisY->setTickCount(7);
    _axisY->setMinorTickCount(1);
    //_axisY->setTitleText(QStringLiteral("计数"));
    m_chart->addAxis(_axisX, Qt::AlignBottom);
    m_chart->addAxis(_axisY, Qt::AlignLeft);
    _axisY->setMinorGridLineVisible(false);
    _axisX->setMinorGridLineVisible(false);

    _series_LRed = new QLineSeries;//红色直线
    _series_LRed->setPen(QPen(QBrush(Qt::red), 3, Qt::DotLine));

    _series_LGreen = new QLineSeries;//绿色直线
    _series_LGreen->setPen(QPen(QBrush(Qt::green), 3, Qt::DotLine));
    m_coordX = new QGraphicsSimpleTextItem(m_chart);
    m_coordX->setPos(m_chart->size().width() / 2 + 50, m_chart->size().height());
    //m_coordX->setText("NG: 0.6");

    m_coordY = new QGraphicsSimpleTextItem(m_chart);
    m_coordY->setPos(m_chart->size().width() / 2 - 50, m_chart->size().height());
    //sm_coordY->setText("OK: 0.4");
    
    connect(_series_LRed, &QLineSeries::hovered, this, &baseChartView::tooltip);
    connect(_series_LGreen, &QLineSeries::hovered, this, &baseChartView::tooltip);
    QString str1 = tr("精度");
    QString str2 = tr("迭代次数");
    setX_Y_title(str2, str1);
    this->setMouseTracking(true);
}

void baseChartView::resizeEvent(QResizeEvent *event)
{
    if (scene()) {
        scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
        m_chart->resize(event->size());
        m_coordX->setPos(m_chart->size().width() / 2 + 80, m_chart->size().height() - 10);
        m_coordY->setPos(m_chart->size().width() / 2 - 50, m_chart->size().height() - 10);
        const auto callouts = m_callouts;
        for (Callout *callout : callouts)
            callout->updateGeometry();
    }
    this->setStyleSheet("padding:0px;border:0px solid #014F84");
    QGraphicsView::resizeEvent(event);
}

void baseChartView::mouseMoveEvent(QMouseEvent *event)
{
    qreal f_x = 0;
    qreal f_y = 0;
    if (event->buttons() & Qt::LeftButton &&/*_indexLine==0*/abs(m_chart->mapToValue(event->pos()).x() - x_str_R.toDouble()) <
        abs(m_chart->mapToValue(event->pos()).x() - x_str_G.toDouble())&&_isMove) {
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
       // m_coordX->setText(QString("NG: %1").arg(x_str_R));
        _series_LRed->clear();
        _series_LRed->append(m_chart->mapToValue(event->pos()).x(), 0);
        _series_LRed->append(m_chart->mapToValue(event->pos()).x(), _axisY_Range);
        m_chart->addSeries(_series_LRed);
        _series_LRed->attachAxis(_axisX);
        _series_LRed->attachAxis(_axisY);

        // m_coordY->setText(QString("Y: %1").arg(y_str));
    }
    if (event->buttons() & Qt::LeftButton &&/*_indexLine == 1*/abs(m_chart->mapToValue(event->pos()).x() - x_str_R.toDouble()) >
        abs(m_chart->mapToValue(event->pos()).x() - x_str_G.toDouble()) && _isMove) {
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
       // m_coordY->setText(QString("OK: %1").arg(x_str_G));
        _series_LGreen->clear();
        _series_LGreen->append(m_chart->mapToValue(event->pos()).x(), 0);
        _series_LGreen->append(m_chart->mapToValue(event->pos()).x(), _axisY_Range);
        m_chart->addSeries(_series_LGreen);
        _series_LGreen->attachAxis(_axisX);
        _series_LGreen->attachAxis(_axisY);
    }
    QGraphicsView::mouseMoveEvent(event);
}
void baseChartView::mouseReleaseEvent(QMouseEvent *event)
{
    this->setCursor(Qt::ArrowCursor);
    if (event->button() == Qt::LeftButton) {
        emit signalSendPoint_X(x_str_R.simplified().toDouble(), x_str_G.simplified().toDouble());
    }
}
void baseChartView::keepCallout()
{
    m_callouts.append(m_tooltip);
    m_tooltip = new Callout(m_chart);
}

void baseChartView::tooltip(QPointF point, bool state)
{
    if (m_tooltip == 0)
        m_tooltip = new Callout(m_chart); 
    if (state) {
        m_tooltip->setText(QString("X: %1 \nY: %2  ").arg(point.x()).arg(point.y()));
        m_tooltip->setAnchor(point);
        m_tooltip->setZValue(11);
        m_tooltip->updateGeometry();
        m_tooltip->show();
    }
    else {
        m_tooltip->hide();
    }
}
void baseChartView::addSeries(baseChartView::seriseStyel flag,const QList<QPointF> &listPoints )
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
    default:
        break;
    }

}

void baseChartView::addSeries(const QList<QPointF> &listPoints , QLineSeries * series)
{
    /*for (int i = 0; i < m_chart->series().size(); i++)
    {
        dynamic_cast<QLineSeries*>(m_chart->series().at(i))->clear();
    }*/
    if (listPoints.size() != 0) {
        for (int i = 0; i < listPoints.size(); ++i) {
            series->append(listPoints.at(i).x(), listPoints.at(i).y());
        }
    }
    m_chart->addSeries(series);
    series->attachAxis(_axisX);
    series->attachAxis(_axisY);
    QObject::connect(series, &QLineSeries::hovered, this, &baseChartView::tooltip);

}
void baseChartView::setIsMoveStatus(bool checked)
{
    _isMove = checked;
}
void baseChartView::setRange_X_Value(qreal xRange)
{
    _axisX_Range = xRange;
    _axisX->setRange(0, _axisX_Range);
}
void baseChartView::setRange_Y_Value(qreal yRange)
{
    _axisY_Range = yRange;
    _axisY->setRange(0, _axisY_Range);
}
void baseChartView::setX_Y_title(const QString &XStr, const QString &YStr)
{
    _axisXTitle = XStr;
    _axisYTitle = YStr;
    QFont font;
    font.setFamily("SimHei"); // 黑体
    _axisX->setTitleFont(font);
    _axisY->setTitleFont(font);
    _axisX->setTitleText(_axisXTitle);
    _axisY->setTitleText(_axisYTitle);
}
void baseChartView::clearSeries()
{
   
}
void baseChartView::dataClear()
{
    //for (int i = 0; i < m_chart->series().size(); i++)
    //{
    //    dynamic_cast<QLineSeries*>(m_chart->series().at(i))->clear();
    //}
    m_chart->removeAllSeries();
}