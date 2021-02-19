#pragma once

#include <QWidget>
#include "ui_dataMatrixGroup.h"

class dataMatrixGroup : public QWidget
{
    Q_OBJECT

public:
    dataMatrixGroup(QWidget *parent = Q_NULLPTR);
    ~dataMatrixGroup();
    void initItem();
    void initDataItem(QMap<int, int> map);
    void dataClear();
private:
    Ui::dataMatrixGroup ui;
    QMap<int, QLabel*> _mapLabelItem;
};
