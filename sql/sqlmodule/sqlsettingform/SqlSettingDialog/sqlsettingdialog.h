#pragma once

#ifndef BUILD_STATIC
# if defined(SQLSETTINGDIALOG_LIB)
#  define SQLSETTINGDIALOG_EXPORT Q_DECL_EXPORT
# else
#  define SQLSETTINGDIALOG_EXPORT Q_DECL_IMPORT
# endif
#else
# define SQLSETTINGDIALOG_EXPORT
#endif

#include <QtWidgets/QDialog>
#include "mrun_messagebox.h"
#include "treeviewbasemodel.h"
#include "../../sqlmethod/DatabaseInterface/mrun_sqlmethod.h"
#include "sqlsavesettingform.h"
#include "sqlshowsettingform.h"
#pragma execution_character_set("utf-8")

namespace Ui {
    class SqlSettingDialogClass;
}
class SQLSETTINGDIALOG_EXPORT SqlSettingDialog : public QDialog
{
    Q_OBJECT

public:
    SqlSettingDialog(QDialog *parent = Q_NULLPTR);
    ~SqlSettingDialog();
    void initmenu();
    QStringList mergelist(QList<QStringList> list);
    void SetItemsHide(QLayout *layout,bool checked);//隐藏控件
private slots:
    void SlotTrigerMenu(QAction* act);//action
private:
    Ui::SqlSettingDialogClass *ui;
    //TreeViewBaseModel *treeviewmodel_ = Q_NULLPTR;
    
    SqlSaveSettingForm *P_SqlSaveSettingForm_ = Q_NULLPTR;//数据库存储界面
    SqlShowSettingForm *P_SqlShowSettingForm_ = Q_NULLPTR;//数据库展示界面
};
