#pragma once

#include <QWidget>
#include "ui_sqlsavesettingform.h"
#include "../../sqlmethod/DatabaseInterface/mrun_sqlmethod.h"
#include "sqltablegroupitem.h"
#include <QSettings>
namespace ui {
    class SqlSaveSettingForm;
}
class SqlSaveSettingForm : public QWidget
{
    Q_OBJECT

public:
    SqlSaveSettingForm(QWidget *parent = Q_NULLPTR);
    ~SqlSaveSettingForm();
    void InitSqlTableData(const QMap<QString, QMap<QString,QStringList>> &mapprodata);
    void AddItemsCombox(const std::list<std::string> list);
    QStringList mergelist(QList<QStringList> list);
    QMap<QString, QString> fromstdmap(std::map<std::string, std::string> map);
    QStringList fromstdlist(std::list<std::string> list);
    void ClearSqlTableGroup();
private slots:
    void SlotProChange(const QString &proname);
    void on_toolButton_savedatasql_clicked();
    void on_toolButton_addnewpro_clicked();
private:
    Ui::SqlSaveSettingForm* ui;
    QMap<QString, SqlTableGroupItem*> map_SqlTableGroupItem_;//存放当前数据库group
};
