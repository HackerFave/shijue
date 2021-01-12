#pragma once

#include <QTableView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QModelIndex>
#include <QMouseEvent>
#include "qualityStandardItem.h"
#include "qualityStandardComboBox.h"
#include "yellowDirtItem.h"
#include "ui_qualityStandardView.h"
#include "publicClass.h"
#include "tableViewEditItemForm.h"
#pragma execution_character_set("utf-8")
class qualityStandardView : public QTableView
{
    Q_OBJECT

public:
    qualityStandardView(QWidget *parent = Q_NULLPTR);
    ~qualityStandardView();
    void initTableView();
    QMap<QString, QMap<QString, QVariant>> getTableData();
    void setTableData(QMap<QString, QVariant> map);//≈‰÷√json ˝æ›
protected:
    void mousePressEvent(QMouseEvent *event);
private slots:
    void slotDoubleClicked(const QModelIndex & index);
    void slotEditItemData(int row, int column, QString text);
private:
    Ui::qualityStandardView ui;
    QString _filePath;
};
