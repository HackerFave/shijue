#pragma once

#include <QWidget>
#include "ui_proCategoryItem.h"

class proCategoryItem : public QWidget
{
    Q_OBJECT

public:
    proCategoryItem(QWidget *parent = Q_NULLPTR);
    ~proCategoryItem();
    QStringList getProType();
signals:
    void signalSendProType(const QStringList &list);
private:
    Ui::proCategoryItem ui;

};
