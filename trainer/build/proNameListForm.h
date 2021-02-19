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
    void setIniFile(const QString key, QVariant value);//写入ini
    QVariant readIniFile(const QString key);//ini文件
    QMap<QString, QVariant> getProData();
    void showForm();
private slots:
    void on_toolButton_openPro_clicked();
    void slotFileInfo(QString &textInfo);
signals:
    void signalOpenPro();//打开工程信号
    void signalProPath(QString &);//发送工程路径到主界面
private:
    Ui::proNameListForm ui;
    QSettings *_configIni = Q_NULLPTR;//ini文件

};
