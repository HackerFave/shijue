#pragma once

#include <QListWidget>
#include <QListWidgetItem>
#include <QMutex>
#include <QMap>
#include <QMenu>
#include <QAction>
#include <QPoint>
#include <QColor>
#include <QContextMenuEvent>
#include "imagelabel.h"
#include "fileprojectmanager.h"
#include "ui_baseListWidget.h"
#include "publicClass.h"
#include "publicStruct.h"
using namespace std;
class baseListWidget : public QListWidget
{
    Q_OBJECT

public:
    baseListWidget(QWidget *parent = Q_NULLPTR);
    ~baseListWidget();
    void initUi();
    void addListWidget(QMap<int, std::string> map, const int indexStart, const int indexEnd, bool isOldFlag);
    void LabelRegionUpdata(int idx, ImageLabel *label);
    void setCurrentImageData(QMap <std::string, int> imageData, QMap<std::string, trained_flag>);//当前映射表图片数据
    void setColorMap(std::map<int, QColor> colorMap, std::map<int, QColor> maskgraycolormap);
    void clearListWidget();
    QVariant getCurrentData(int index);
    QVariantList getCurrentData(QStringList list);
    QStringList getAllSelectedRow();
    void setCurrentIndexNum(int index);
    int getCurrentRowCount();
    int getListIndex();
    void setShiftStatus(bool isPressShit) {
        _isPressShift = isPressShit;
        _startRow = this->currentIndex().row();
    }
protected:
private slots:
    void OnSignalSendId(bool is, void* item);
    void slotListWidgetClick(const QModelIndex &);
    void slotTreeMenu(const QPoint &pos);//左键弹出菜单
    void triget_menu_delete();//删除
    void triget_menu_nolabel();//无缺陷
    void triget_menu_close_auto();//关闭工具条
    void triget_menu_open_auto();//打开工具条
    void triget_menu_test();//测试
    void triget_menu_train_always();//总是
    void triget_menu_train_auto();//自动
    void triget_menu_train_never();//从不
    //鼠标响应
  //  void OnSignalSendId(bool is, void* id);
   // void OnSignalDelImage(bool ismain);

    //右键菜单
   // void OnSignalInitMenu(bool is, void* item, int size);
   // void OnSignalMenuOperate(bool is, int op, void* imagelabel);
signals:
    void signalIndexValue(QVariant, int);
    void signalDelImage(bool);
    void signalMenuOperate(bool ismain, int op, void*);
private:
    Ui::baseListWidget ui;
    std::map<int, QColor> colormap_;
    std::map<int, QColor> maskgraycolormap_;
    QMutex _mutex;
    bool _isExpanded = true;
    bool _isPressedSection = true;
    int _indexSelect;
    bool _isPressShift = false;
    QMap <std::string, int> _mapImageData;//当前显示的文件图片数据映射
    bool _load = false;
    QMap<ImageLabel*, QListWidgetItem*> _mapLabel;
    QMap<std::string, trained_flag> _flagMap;//训练标志
    std::map<int, QColor> _colorMap;
    std::map<int, QColor> _maskgraycolormap;
    int _startRow;
    int _endRow;
};
