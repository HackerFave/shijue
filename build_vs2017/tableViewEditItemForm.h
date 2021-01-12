#pragma once

#include <QWidget>
#include "baseWidget.h"
#include "ui_tableViewEditItemForm.h"

class tableViewEditItemForm : public QWidget
{
    Q_OBJECT

public:
    tableViewEditItemForm(QWidget *parent = Q_NULLPTR);
    ~tableViewEditItemForm();
    void setTitle(const QString &text,const int &row,const int &column);
private slots:
    void on_toolButton_ok_clicked();
    void on_toolButton_cancel_clicked();
    void slotFormStatus(int, bool);
signals:
    void signalItemData(int, int, QString);
private:
    Ui::tableViewEditItemForm ui;
    baseWidget *P_baseWidget = Q_NULLPTR;
    int _row;
    int _column;
};
