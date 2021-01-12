#include "TemplateMainForm.h"
#include "ui_TemplateMainForm.h"
#include "TemplateItemForm.h"
#include <qgridlayout.h>
#include <qdir.h>
#include "PublicClass.h"
#pragma execution_character_set("utf-8")
TemplateMainForm::TemplateMainForm(QWidget *parent)
    : QDialog(parent)
    ,ui( new Ui::TemplateMainFormClass)
{
    ui->setupUi(this);
    QDir::setCurrent(QCoreApplication::applicationDirPath());//设置当前路径为exe所在路径
    InitTabWidget();
    showMaximized();
    InitFilePath();
    //connect(ui->tabWidget_addplug, SIGNAL(currentChanged(int)), this, SLOT(slotUpdateForm(int)));
}
//清空tab
void TemplateMainForm::cleartab()
{
    int curTabCount = ui->tabWidget_addplug->count();
    // 4、删除tab页
    while (0 != curTabCount)
    {
        ui->tabWidget_addplug->removeTab(0);
        curTabCount = ui->tabWidget_addplug->count();
    }
}
void TemplateMainForm::InitTabWidget()
{
    int count = ui->tabWidget_addplug->count();
    for (int i = 0; i <count; i++) {
        TemplateItemForm *form = new TemplateItemForm(i);//添加每个界面的映射
        QGridLayout *layout = new QGridLayout;
        layout->addWidget(form);
        ui->tabWidget_addplug->setCurrentIndex(i);
        //form->SetCurrentFormObject(i);//添加每个界面的映射
        ui->tabWidget_addplug->currentWidget()->setLayout(layout);
        
   }
    ui->tabWidget_addplug->setCurrentIndex(0);
}
//初始化ini文件
void TemplateMainForm::InitFilePath()
{
    P_SetingIni_ = new QSettings(QString("%1/template/TemplateManager.ini").arg(QCoreApplication::applicationDirPath())
        , QSettings::IniFormat);
    QDir dir;
    dir.setPath(QString("%1/template").arg(QCoreApplication::applicationDirPath()));
    if (!dir.exists()) {
        dir.mkpath(QString("%1/template").arg(QCoreApplication::applicationDirPath()));
    }
    dir.setPath(QString("%1/template/algtem/").arg(QCoreApplication::applicationDirPath()));
    if (!dir.exists()) {
        dir.mkpath(QString("%1/template/algtem/").arg(QCoreApplication::applicationDirPath()));
    }
    dir.setPath(QString("%1/template/historicaldatatem/").arg(QCoreApplication::applicationDirPath()));
    if (!dir.exists()) {
        dir.mkpath(QString("%1/template/historicaldatatem/").arg(QCoreApplication::applicationDirPath()));
    }
    dir.setPath(QString("%1/template/defectfiltertem/").arg(QCoreApplication::applicationDirPath()));
    if (!dir.exists()) {
        dir.mkpath(QString("%1/template/defectfiltertem/").arg(QCoreApplication::applicationDirPath()));
    }
    dir.setPath(QString("%1/template/runformtem/").arg(QCoreApplication::applicationDirPath()));
    if (!dir.exists()) {
        dir.mkpath(QString("%1/template/runformtem/").arg(QCoreApplication::applicationDirPath()));
    }
    dir.setPath(QString("%1/template/plctem/").arg(QCoreApplication::applicationDirPath()));
    if (!dir.exists()) {
        dir.mkpath(QString("%1/template/plctem/").arg(QCoreApplication::applicationDirPath()));
    }
    P_SetingIni_->setValue("pluginpath/algtempath", QString("/template/algtem/"));
    P_SetingIni_->setValue("pluginpath/historicaldatatempath", QString("/template/historicaldatatem/"));
    P_SetingIni_->setValue("pluginpath/defectfiltertempath", QString("/template/defectfiltertem/"));
    P_SetingIni_->setValue("pluginpath/runformtempath", QString("/template/runformtem/"));
    P_SetingIni_->setValue("pluginpath/plctempath", QString("/template/plctem/"));

}
void TemplateMainForm::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Control) {
        PublicClass::instance()->SetKeyStatus(true);
    }
}
void TemplateMainForm::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Control) {
        PublicClass::instance()->SetKeyStatus(false);
    }
}
