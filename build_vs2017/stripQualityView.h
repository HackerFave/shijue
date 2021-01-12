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
    void slotChangeVolumeNum(bool);//�޸ľ��
    void slotdeleteVolumeNum(bool);//ɾ�����
signals:
    void signalRefreshVolumeNum(bool,int);
private:
    void createRightMenu();  //����һ���Ҽ��˵�
private:
    Ui::stripQualityView ui;
    QStringList _listEdit;//�ɱ༭list
    QString _volumeNumStr;//��ǰitem��ʾ�ľ��
    QString _currentKey;//��ǰrecord_ID
    int _currentIndex;//��ǰ��������˵�combox����

    QMenu *_rightMenu = Q_NULLPTR;  //�Ҽ��˵�
    QAction *_cutAction = Q_NULLPTR;  //����
    QAction *_copyAction = Q_NULLPTR;  //����
    QAction *_pasteAction = Q_NULLPTR;  //ճ��
    QAction *_changeAction = Q_NULLPTR;  //�޸ľ��
    QAction *_deleteAction = Q_NULLPTR;  //ɾ�����
    QModelIndex _modelIndex;//��ǰ���index
};
