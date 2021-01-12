#pragma once

#include <QWidget>
#include <QMap>
#include "baseWidget.h"
#include "ui_addTestReord.h"

class addTestReord : public QWidget
{
    Q_OBJECT

public:
    addTestReord(QWidget *parent = Q_NULLPTR);
    ~addTestReord();
private slots:
    void on_toolButton_ok_clicked();
    void on_toolButton_cancel_clicked();
    void slotFormStatus(int, bool);
    QMap<QString, QString> getEditData();
signals:
    void signalRowData(QMap<QString, QString>,bool);
private:
    Ui::addTestReord ui;
    baseWidget *P_baseWidget = Q_NULLPTR;
};
