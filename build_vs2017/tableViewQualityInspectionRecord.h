#pragma once

#include <QTableView>
#include <QStandardItemModel>
#include "sqlOperation.h"
#include "ui_tableViewQualityInspectionRecord.h"
#pragma execution_character_set("utf-8")
class tableViewQualityInspectionRecord : public QTableView
{
    Q_OBJECT

public:
    tableViewQualityInspectionRecord(QWidget *parent = Q_NULLPTR);
    ~tableViewQualityInspectionRecord();
    void initTableView();
    void editTable(const QList<QVariantList> &list);
    void getInspectionFirst(const QString &recordID);
    void getInspectionEnd(const QString &recordID);
    void setInspectionFirst(const QString &recordID);
    void setInspectionEnd(const QString &recordID);
    void tableViewClear();
private:
    Ui::tableViewQualityInspectionRecord ui;
    sqlOperation *p_sqlOperation = Q_NULLPTR;//质检记录
};
