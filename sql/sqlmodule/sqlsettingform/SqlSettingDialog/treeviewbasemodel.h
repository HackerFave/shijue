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
    //获取相关项目数据库信息数据
    void set_sql_all_table(QMap<QString, QMap<QString, QStringList>> mapprodata);
	void set_sql_all_table(QMap<QString, QStringList> mapprodata);
    Qt::CheckState checkSibling(QStandardItem * item);
    void delTreeViewData();
    void setCheckedAll(bool checked);
    void setCheckedRelated();
    void GetItem(QStandardItem *item,int flag);//获取所有节点数据
    QMap<QString, QStringList> getselecttabledata();//获取选中数据
    void setrelatedtablelist(const QStringList &list);//关联表名
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
    QMap<QString, QIcon> icon_map_;//图标
    QMap<QString, QStringList> table_map_;//表格，字段
    bool itemchecked = false;//item选中状态
    QStringList related_table_name_;//关联表名
};
