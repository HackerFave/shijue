#include "baseWidget.h"
#include <QDesktopWidget>
baseWidget::baseWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    connect(ui.toolButton_close, SIGNAL(clicked(bool)), this, SLOT(closeForm()));
    connect(ui.toolButton_min, SIGNAL(clicked(bool)), this, SLOT(showMin()));
    connect(ui.toolButton_normal, SIGNAL(clicked(bool)), this, SLOT(minOrMaxForm()));
    this->setAttribute(Qt::WA_ShowModal,true);
    QDesktopWidget *pDesk = QApplication::desktop();
    this->move((pDesk->width() - this->width()) / 3, (pDesk->height() - this->height()) / 2);
}

baseWidget::~baseWidget()
{
}
void baseWidget::mousePressEvent(QMouseEvent *event)
{
    if (ui.frame->geometry().contains(this->mapFromGlobal(QCursor::pos()))) {
        m_lastPos = event->globalPos();
        isPressedWidget = true; // 当前鼠标按下的即是QWidget而非界面上布局的其它控件
    }

}

void baseWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (isPressedWidget && !this->isMaximized()) {
        this->move(this->x() + (event->globalX() - m_lastPos.x()),
            this->y() + (event->globalY() - m_lastPos.y()));
        m_lastPos = event->globalPos();
    }
}

void baseWidget::mouseReleaseEvent(QMouseEvent *event)
{
    // 其实这里的mouseReleaseEvent函数可以不用重写
    m_lastPos = event->globalPos();
    isPressedWidget = false; // 鼠标松开时，置为false
}
void baseWidget::minOrMaxForm()
{
    if (this->isMaximized()) {
        this->showNormal();
    }
    else
    {
        this->showMaximized();
    }
}
void baseWidget::setWidget(QWidget *widget)
{
    ui.gridLayout_mainForm->addWidget(widget);
   // QDesktopWidget *pDesk = QApplication::desktop();
   // widget->move((pDesk->width() - widget->width()) / 2, (pDesk->height() - widget->height()) / 4);
    //this->move((pDesk->width() - this->width()) / 2, (pDesk->height() - this->height()) / 2);
}
void baseWidget::setHintHeight(int height)
{
    ui.frame->setFixedHeight(height);
}
void baseWidget::showMin()
{
    this->showMinimized();
}
void baseWidget::closeForm()
{
    this->close();
    emit signalFormStatus(0, true);
}