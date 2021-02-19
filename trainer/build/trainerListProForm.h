#pragma once

#include <QWidget>
#include <imagelabel.h>
#include <QMap>
#include <QKeyEvent>
#include "ui_trainerListProForm.h"

class trainerListProForm : public QWidget
{
    Q_OBJECT

public:
    trainerListProForm(QWidget *parent = Q_NULLPTR);
    ~trainerListProForm();
    void initComboBoxStyle(QMap<int, QComboBox *> comBoxMap);
    void setLcdFileNumber(int value);
    int getSelectDisplayIndex();
    void addTreeViewData(QMap<int, std::string> map, const int indexStart, const int indexEnd, bool isOldFlag);
    void delTreeViewData();
    void setWidget_imageListVisible(bool checked);
    QString getWidget_imageListCurrentData(int index);
    QVariantList getWidget_imageListCurrentData(QStringList list);
    QStringList getAllSelectedRows();
    void setWidget_ImageListFixedSize(QSize size);
    QComboBox * getComboBox();
    void setCurrentIndexNum(int index);
    treeViewBaseWidget * getWidget_ImageForm();
    baseListWidget * getbaseListWidget();
    void FileListClear();//列表清理
    void setPageText(const QString &text);
    int getListIndex();
    int getCurrentRowCount();//获取文件列表当前显示的项数
    void setCurrentImageData(QMap <std::string, int> imageData, QMap<std::string, trained_flag> flag_map);//当前映射表图片数据
    void setColorMap(std::map<int, QColor> colorMap, std::map<int, QColor> maskgraycolormap);
    void setShiftStatus(bool isPressShit) {
        ui.listWidget->setShiftStatus(isPressShit);
    }
public slots:
    void OnFilterDisplay(int);//筛选combox
    void OnSignalDelImage(bool);//删除图片
    void OnSignalMenuOperate(bool, int, void*);
protected:
    //void keyPressEvent(QKeyEvent  *event);
signals:
    void signalFilterDisplay(int);//筛选combox index
    void signalMenuOperate(bool, int, void*);
    void signalDelImage(bool);
    void signalPreviousPage();
    void signalNextPage();
private:
    Ui::trainerListProForm ui;
    
};
