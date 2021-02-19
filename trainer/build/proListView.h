#pragma once

#include <QTableView>
#include <QStandardItemModel>
#include "ui_proListView.h"
#include "proListItem.h"
class proListView : public QTableView
{
    Q_OBJECT

public:
    proListView(QWidget *parent = Q_NULLPTR);
    ~proListView();
    void initTableView(const QMap<QString, QVariant> &map);
    void setListFile(const QStringList &list);
    void addItem(const QString & str);
    QStringList getListFile();
    void tableViewClear();
signals:
    void signalProName(QString &);//打开工程
    void signalFindPro(QString &);//查看工程
private slots:
    void slotStatus(bool,QString,QString);
    void slotSelect(bool,QString,QString);
private:
    Ui::proListView ui;
    bool _isClear;
    QMap<QString, QVariant> _proMap;//工程列表
};
