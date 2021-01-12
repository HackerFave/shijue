#pragma once

#include <QWidget>
#include "treeviewbasemodel.h"
#include "ui_sqlshowsettingform.h"
#include "../../sqlmethod/DatabaseInterface/mrun_sqlmethod.h"
#define DATA_COUNT_PAGE 18
namespace ui {
    class SqlShowSettingForm;
}
class SqlShowSettingForm : public QWidget
{
    Q_OBJECT

public:
    SqlShowSettingForm(QWidget *parent = Q_NULLPTR);
    ~SqlShowSettingForm();
    void cleartab();
    void AddTabWidget(QString head, QStringList list);
    void initsql();
    void AddItemsCombox(const std::list<std::string> list);
    QStringList mergelist(QList<QStringList> list);
    //void SetItemsHide(QLayout *layout, bool checked);//隐藏控件
    QMap<QString, QString> fromstdmap(std::map<std::string, std::string> map);
    QStringList fromstdlist(std::list<std::string> list);
	void initComboxColor(QComboBox * comBox);
private slots:
    void SlotProChange(const QString &proname);
    void SlotCheckedContact(int);
    void on_toolButton_showdata_clicked();
    void SlotRelated(const QString &);//启动关联
    void SlotChangePageValue(int);//翻页
	void on_toolButton_left_clicked(bool checked);//左侧收缩按钮
	void on_toolButton_right_clicked(bool checked);//左侧收缩按钮
private:
    Ui::SqlShowSettingForm *ui;
    TreeViewBaseModel *treeviewmodel_ = Q_NULLPTR;

    QMap<QString, QDialog*> tabformmap_;//动态创建的界面
    QMap<QString, QStringList> tablemap_;//表名+字段
    QStringList mergelist_table_filed_;//字段+字段描述
    QStringList related_table_list_;//关联表名+关联字段+关联级别
    QStringList listtablerelatedname_;//关联的表名
    QString relatedid_;//关联id
    QString strrelatedrecordid_;//当前关联卷号
    QMap<QString, QStringList> selecttablemap_;//关联的表跟(字段|字段描述);
    QString relatedlevelfirsttable_;//关联级别最高的表名
};
