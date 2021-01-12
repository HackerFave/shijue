#pragma once

#include <QWidget>
#include <QMouseEvent>
#include "ui_baseWidget.h"

class baseWidget : public QWidget
{
    Q_OBJECT

public:
    baseWidget(QWidget *parent = Q_NULLPTR);
    ~baseWidget();
    void setWidget(QWidget *widget);
    void setHintHeight(int height);
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
private slots:
    void minOrMaxForm();
    void showMin();
    void closeForm();
signals:
    void signalFormStatus(int,bool);
private:
    Ui::baseWidget ui;
    bool isPressedWidget = false;
    QPoint m_lastPos;
};
