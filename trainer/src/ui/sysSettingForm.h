#pragma once

#include <QWidget>
#include <QCloseEvent>
#include <QSpacerItem>
#include <qmap.h>
#include <QVariantList>
#include <QStandardItemModel>
#include "SwitchButton.h"
#include "publicClass.h"
#include "moreTypeForm.h"
#include "ui_sysSettingForm.h"

#define TYPE_NUM 20
class sysSettingForm : public QWidget
{
    Q_OBJECT

public:
    sysSettingForm(QWidget *parent = Q_NULLPTR);
    ~sysSettingForm();
protected:
    void closeEvent(QCloseEvent *event);
    void getInitStatus();
    void setSysIni();
    void initColor();
    void initMoreTypeMap();
    void initComboBoxStyle(QMap<int, QComboBox *> comBoxMap);
public slots:
    void show();
private slots:
    void slotValueChangePen(int);
    void slotCheckedChange(bool);
    void on_pushButton_ok_clicked();
    void on_pushButton_cancel_clicked();
    void on_toolButton_addMoreType_clicked();
    void on_toolButton_del_clicked();
    void onStateChanged(int);

signals:
    void signalFormChecked(bool);
    void signalValuePen(int);
    void signalSwitchChacked(int,bool);
    void signalMoreTypeData(QList<QVariantList>);
    void signalLoadMode(bool);//文件加载方式
    void signalFileExportMode(QString);//文件导出格式
private:
    Ui::sysSettingForm ui;
    bool _switchBtn_fillChecked;
    bool _switchBtn_inproportionChecked;
    bool _widget_testChecked;
    bool _switchBtn_changeImageNameChecked;
    bool _switchBtnFileLoadMode;//图片加载模式
    int _PenSize;
    QString _fileExportMode;//文件导出格式
    publicClass *_publicClass = Q_NULLPTR;
    QMap<int, moreTypeForm*> _mapMoreTypeForm;
    bool _checked = false;
    QMap<int, QColor> _mapColorDefault;
};
