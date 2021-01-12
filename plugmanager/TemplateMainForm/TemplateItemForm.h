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
    void slotRefreshList();//ˢ���б�
    void on_toolButton_updateform_clicked();//���½���
private:
    Ui::TemplateItemForm ui;
    TemplateRegisterForm* P_TemplateRegisterForm = Q_NULLPTR;//ע�����
    QSettings *P_templateIni_ = Q_NULLPTR;
    int index_;//����ҳ����
};
