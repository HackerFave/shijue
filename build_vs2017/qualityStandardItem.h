#pragma once

#include <QWidget>
#include <qdebug.h>
#include "ui_qualityStandardItem.h"

class qualityStandardItem : public QWidget
{
    Q_OBJECT

public:
    qualityStandardItem(QWidget *parent = Q_NULLPTR);
    ~qualityStandardItem();
    void setTitleRange(int id, const QString &title, const QString &range);
    void setVisibleFrame(int id, bool checked);
    void setLabelFixSize(int id, QSize size);
    void setSpinBoxValue(const QString & value1, const QString & value2);
    double getSpinBoxValue(int index);
private:
    Ui::qualityStandardItem ui;
};
