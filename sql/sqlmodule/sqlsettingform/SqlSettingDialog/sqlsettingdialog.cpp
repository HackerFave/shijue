#include "sqlsettingdialog.h"
#include "ui_sqlsettingdialog.h"
#include <qtreewidget.h>
#include <QtWidgets/qbuttongroup.h>
#include <qmenubar.h>
#include <qmenu.h>
#include "sqldatashowdialog.h"
#include "../../sqlmethod/DatabaseInterface/mrun_sqloperation_define.h"
#include "mrun_messagebox.h"
#include "sqlsavesettingform.h"

SqlSettingDialog::SqlSettingDialog(QDialog *parent)
    : QDialog(parent)
    , ui(new Ui::SqlSettingDialogClass)
{  
    ui->setupUi(this); 
    initmenu();//
    //this->showMaximized();
    P_SqlSaveSettingForm_ = new SqlSaveSettingForm;
    P_SqlShowSettingForm_ = new SqlShowSettingForm;
    ui->horizontalLayout_addsqlform->addWidget(P_SqlSaveSettingForm_);
    ui->horizontalLayout_addsqlform->addWidget(P_SqlShowSettingForm_);
    P_SqlSaveSettingForm_->setVisible(false);
	ui->frame_title->setVisible(false);
}
SqlSettingDialog::~SqlSettingDialog()
{
}
//初始化menu
void SqlSettingDialog::initmenu()
{
    QMenu *menu1 = new QMenu("数据&(S)");
    QAction *action1 = new QAction("数据库展示");
    action1->setCheckable(true);
    action1->setChecked(true);
    QAction *action2 = new QAction("用户存储");
    action2->setCheckable(true);
    action2->setChecked(false);
    menu1->addAction(action1);
    menu1->addSeparator();
    menu1->addAction(action2);
    QActionGroup *actiongroup = new QActionGroup(this);
    actiongroup->addAction(action1);
    actiongroup->addAction(action2);
    QMenuBar* menuBar = new QMenuBar(this);
	menuBar->setStyleSheet("color:white");
    menuBar->addMenu(menu1);
   // menuBar->addMenu(menu2);
  //  menuBar->setGeometry(0, 0, this->width(), 30);
    ui->horizontalLayout_menubar->addWidget(menuBar);
    connect(menuBar, SIGNAL(triggered(QAction*)), this, SLOT(SlotTrigerMenu(QAction*)));
}
QStringList SqlSettingDialog::mergelist(QList<QStringList> list)
{
    QStringList mergelist;
    if (list.size() > 1) {
        QStringList list0 = list.at(0);
        QStringList list1 = list.at(1);
        for (int i = 0; i < list0.size(); i++)
        {
            QString str = QString("%1|%2").arg(list0.at(i).simplified()).arg(list1.at(i).simplified());
            mergelist.append(str);
        }
    }
    return mergelist;
}


void SqlSettingDialog::SlotTrigerMenu(QAction* act)
{
    if (act->text() == "用户存储")
    {
        P_SqlSaveSettingForm_->setVisible(true);
        P_SqlShowSettingForm_->setVisible(false);
    }
    else if(act->text() == "数据库展示")
    {
        P_SqlSaveSettingForm_->setVisible(false);
        P_SqlShowSettingForm_->setVisible(true);
    }
 
    
}

void SqlSettingDialog::SetItemsHide(QLayout *layout ,bool checked)
{
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr)
    {
        ///setParent为NULL，防止删除之后界面不消失
        if (child->widget())
        {
            child->widget()->setParent(nullptr);
        }
        else if (child->layout()) {
            SetItemsHide(child->layout(), checked);
        }
        child->widget()->setVisible(checked);
    }
}