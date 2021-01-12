#pragma once

#include <QWidget>
#include "ui_TemplateItemForm.h"
#include "TemplateRegisterForm.h"
#include <qsettings.h>
#include <QMouseEvent>
#include <QWheelEvent>
class TemplateItemForm : public QWidget
{
    Q_OBJECT

public:
    TemplateItemForm(int index, QWidget *parent = Q_NULLPTR);
    ~TemplateItemForm();
    void InitItemLabel();
    void SetCurrentFormObject(int index);
    QString ReadIni(const QString &key);//
protected:
    bool eventFilter(QObject *target, QEvent *event);
private slots:
    void on_toolButton_addTemp_clicked();
    void slotRefreshList();//刷新列表
    void on_toolButton_updateform_clicked();//更新界面
private:
    Ui::TemplateItemForm ui;
    TemplateRegisterForm* P_TemplateRegisterForm = Q_NULLPTR;//注册界面
    QSettings *P_templateIni_ = Q_NULLPTR;
    int index_;//属性页索引
};
