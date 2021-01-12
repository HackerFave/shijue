#pragma once

//#include <QTreeView>
#include <QtWidgets/qtreeview.h>
#include <QStandardItemModel>
#include <QIcon>
#include <QMap>
#include <qdialog.h>

namespace Ui {
    class TreeViewBaseModel;
}
class TreeViewBaseModel : public QTreeView
{
    Q_OBJECT

public:
    TreeViewBaseModel(QDialog *parent = Q_NULLPTR);
    ~TreeViewBaseModel();
    void InitTreeView();
    void init_icon_map();
    //��ȡ�����Ŀ���ݿ���Ϣ����
    void set_sql_all_table(QMap<QString, QMap<QString, QStringList>> mapprodata);
	void set_sql_all_table(QMap<QString, QStringList> mapprodata);
    Qt::CheckState checkSibling(QStandardItem * item);
    void delTreeViewData();
    void setCheckedAll(bool checked);
    void setCheckedRelated();
    void GetItem(QStandardItem *item,int flag);//��ȡ���нڵ�����
    QMap<QString, QStringList> getselecttabledata();//��ȡѡ������
    void setrelatedtablelist(const QStringList &list);//��������
private slots:
    void SlotTreeItemCheckChildChanged(QStandardItem * item);
    void SlotTreeItemChanged(QStandardItem * item);
    void SlotTreeItemCheckAllChild(QStandardItem * item, bool check);
    //void treeItem_checkAllChild(QStandardItem * item, bool check)
    void SlotTreeItemCheckAllChildRecursion(QStandardItem * item, bool check);
    //void treeItem_checkAllChild_recursion(QStandardItem * item, bool check);
    //void treeItem_CheckChildChanged(QStandardItem * item);
    
private:
    Ui::TreeViewBaseModel* ui;
    QMap<QString, QIcon> icon_map_;//ͼ��
    QMap<QString, QStringList> table_map_;//����ֶ�
    bool itemchecked = false;//itemѡ��״̬
    QStringList related_table_name_;//��������
};
