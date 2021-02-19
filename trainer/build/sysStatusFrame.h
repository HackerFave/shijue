#pragma once

#include <QWidget>
#include <qdebug.h>
#include "proNameListForm.h"
#include "ui_sysStatusFrame.h"
#pragma execution_character_set("utf-8")
class sysStatusFrame : public QWidget
{
    Q_OBJECT

public:
    enum ststusID
    {
       Loading,
       PageNum,
       Pro
    };
    sysStatusFrame(QWidget *parent = Q_NULLPTR);
    ~sysStatusFrame();
    static sysStatusFrame *instance();
    static void Release() {
        if (P_sysStatusFrame!=NULL) {
            delete P_sysStatusFrame;
            P_sysStatusFrame = NULL;
        }
    }
    void close() {
        _proListForm->close();
    }
    void comboBox_pageAddItem(int size);
    void setCurrentIndex(int index);
    void setProIni(const QString key, QVariant value);
    proNameListForm * getProListP() {
        return _proListForm;
    }
signals:
    void signalStatusStr(ststusID flag, const QString &statusStr);
    void signalPageIndex(const QString &);
private slots:
    void slotStatusStr(ststusID flag, const QString &statusStr);
    void slotLabelProList(const QString&);
private:
    Ui::sysStatusFrame ui;
    static sysStatusFrame * P_sysStatusFrame;
    proNameListForm * _proListForm = Q_NULLPTR;//解决方案列表
};
