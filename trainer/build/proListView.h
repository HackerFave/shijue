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
    void signalProName(QString &);//�򿪹���
    void signalFindPro(QString &);//�鿴����
private slots:
    void slotStatus(bool,QString,QString);
    void slotSelect(bool,QString,QString);
private:
    Ui::proListView ui;
    bool _isClear;
    QMap<QString, QVariant> _proMap;//�����б�
};
