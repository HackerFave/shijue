#pragma once

#include <QWidget>
#include <qcolordialog.h>
#include <qcolor.h>
#include "ui_yellowDirtItem.h"

class yellowDirtItem : public QWidget
{
    Q_OBJECT

public:
    yellowDirtItem(QWidget *parent = Q_NULLPTR);
    ~yellowDirtItem();
    void setLabelColor(QColor color);
    QColor getLabelColor();
private slots:
    void slotColorBtn(int);
    void on_toolButton_color_clicked();
    void slotSetColor(QColor);
    void slotSetColor_1();
    void slotShowColor(QColor);
signals:
    void signalLabelColor(QColor);
private:
    Ui::yellowDirtItem ui;
    QColor m_color;//记录线条初始颜色
    QColor m_selectedColor;//记录按OK键后选中的颜色
};
