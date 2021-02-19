#pragma once

#include <QWidget>
#include <qsettings.h>
#include <QVariant>
#include "publicClass.h"
#include "ui_proNameListForm.h"

class proNameListForm : public QWidget
{
    Q_OBJECT

public:
    proNameListForm(QWidget *parent = Q_NULLPTR);
    ~proNameListForm();
    void setIniFile(const QString key, QVariant value);//д��ini
    QVariant readIniFile(const QString key);//ini�ļ�
    QMap<QString, QVariant> getProData();
    void showForm();
private slots:
    void on_toolButton_openPro_clicked();
    void slotFileInfo(QString &textInfo);
signals:
    void signalOpenPro();//�򿪹����ź�
    void signalProPath(QString &);//���͹���·����������
private:
    Ui::proNameListForm ui;
    QSettings *_configIni = Q_NULLPTR;//ini�ļ�

};
