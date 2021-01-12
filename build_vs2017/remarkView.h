#pragma once

#include <QTableView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QModelIndex>
#include "tableViewEditItemForm.h"
#include "ui_remarkView.h"
#pragma execution_character_set("utf-8")
class remarkView : public QTableView
{
    Q_OBJECT

public:
    remarkView(QWidget *parent = Q_NULLPTR);
    ~remarkView();
    void initTableView();
    void editTable(const QList<QVariantList> &list);
    void tableViewClear();
private slots:
    void slotDoubleClicked(const QModelIndex &);
    void slotEditItemData(int row, int column, QString text);
private:
    Ui::remarkView ui;
};
