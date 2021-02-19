#pragma once

#include <QWidget>
#include "ui_dataWidget.h"

class dataWidget : public QWidget
{
    Q_OBJECT

public:
    dataWidget(QWidget *parent = Q_NULLPTR);
    ~dataWidget();

private:
    Ui::dataWidget ui;
};
