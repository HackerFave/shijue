#pragma once

#include <QTableView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QList>
#include <QStringList>
#include <QSqlQuery>
#include <databaseSqlInset.h>
#include <QDateTime>
#include <QMessageBox>
#include <qtooltip.h>
#include <QMenu>
#include <QAction>
#include "tableViewEditItemForm.h"
#include "ui_stripQualityView.h"
#pragma execution_character_set("utf-8")
class stripQualityView : public QTableView
{
    Q_OBJECT

public:
    stripQualityView(QWidget *parent = Q_NULLPTR);
    ~stripQualityView();
    void initTableView();
    void editTable(const QList<QVariantList> &list,QStandardItemModel *model);
    void setItemData(int row, int column, const QString &text,QStandardItemModel* model);
    void setCurrentRecord_ID(const QString &currentKey, const QString &volumeNumStr,const int &currentIndex);
    void tableViewClear();
    QStandardItemModel * initModel();
private slots:
    void slotDoubleClicked(const QModelIndex &);
    void slotEditItemData(int row, int column, QString text);
    void showToolTip(const QModelIndex &index);
    void slotRightMenu(QPoint);
    void slotChangeVolumeNum(bool);//修改卷号
    void slotdeleteVolumeNum(bool);//删除卷号
signals:
    void signalRefreshVolumeNum(bool,int);
private:
    void createRightMenu();  //创建一个右键菜单
private:
    Ui::stripQualityView ui;
    QStringList _listEdit;//可编辑list
    QString _volumeNumStr;//当前item显示的卷号
    QString _currentKey;//当前record_ID
    int _currentIndex;//当前卷号下拉菜单combox索引

    QMenu *_rightMenu = Q_NULLPTR;  //右键菜单
    QAction *_cutAction = Q_NULLPTR;  //剪切
    QAction *_copyAction = Q_NULLPTR;  //复制
    QAction *_pasteAction = Q_NULLPTR;  //粘贴
    QAction *_changeAction = Q_NULLPTR;  //修改卷号
    QAction *_deleteAction = Q_NULLPTR;  //删除卷号
    QModelIndex _modelIndex;//当前点击index
};
