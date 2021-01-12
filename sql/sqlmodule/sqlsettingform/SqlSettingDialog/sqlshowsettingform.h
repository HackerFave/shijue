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
    //void SetItemsHide(QLayout *layout, bool checked);//���ؿؼ�
    QMap<QString, QString> fromstdmap(std::map<std::string, std::string> map);
    QStringList fromstdlist(std::list<std::string> list);
	void initComboxColor(QComboBox * comBox);
private slots:
    void SlotProChange(const QString &proname);
    void SlotCheckedContact(int);
    void on_toolButton_showdata_clicked();
    void SlotRelated(const QString &);//��������
    void SlotChangePageValue(int);//��ҳ
	void on_toolButton_left_clicked(bool checked);//���������ť
	void on_toolButton_right_clicked(bool checked);//���������ť
private:
    Ui::SqlShowSettingForm *ui;
    TreeViewBaseModel *treeviewmodel_ = Q_NULLPTR;

    QMap<QString, QDialog*> tabformmap_;//��̬�����Ľ���
    QMap<QString, QStringList> tablemap_;//����+�ֶ�
    QStringList mergelist_table_filed_;//�ֶ�+�ֶ�����
    QStringList related_table_list_;//��������+�����ֶ�+��������
    QStringList listtablerelatedname_;//�����ı���
    QString relatedid_;//����id
    QString strrelatedrecordid_;//��ǰ�������
    QMap<QString, QStringList> selecttablemap_;//�����ı��(�ֶ�|�ֶ�����);
    QString relatedlevelfirsttable_;//����������ߵı���
};
