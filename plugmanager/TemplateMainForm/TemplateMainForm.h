#pragma once

#ifndef BUILD_STATIC
# if defined(TEMPLATEMAINFORM_LIB)
#  define TEMPLATEMAINFORM_EXPORT Q_DECL_EXPORT
# else
#  define TEMPLATEMAINFORM_EXPORT Q_DECL_IMPORT
# endif
#else
# define TEMPLATEMAINFORM_EXPORT
#endif

#include <QtWidgets/QDialog>
#include <qsettings.h>
#include <qevent.h>
#pragma execution_character_set("utf-8")
namespace Ui {
    class TemplateMainFormClass;
}
class TEMPLATEMAINFORM_EXPORT TemplateMainForm : public QDialog
{
    Q_OBJECT

public:
    TemplateMainForm(QWidget *parent = Q_NULLPTR);
    void cleartab();//清除tab
    void InitTabWidget();//初始化tab页
    void InitFilePath();//初始化ini
protected:
    void keyPressEvent(QKeyEvent *event) ;
    void keyReleaseEvent(QKeyEvent *event) ;
private:
    Ui::TemplateMainFormClass *ui;
    QSettings * P_SetingIni_ = Q_NULLPTR;//第一级ini文件，用于管理各个组件存放在哪个目录
};
