#pragma once

#include <QWidget>
#include <QStandardItemModel>
#include <QMutex>
#include <qlist.h>
#include <QHeaderView>
#include <QVariant>
#include <QDir>
#include <QAction>
#include <QMenu>
#include <QMouseEvent>
#include <QFileInfoList>
#include "qgridlayout.h"
#include "publicClass.h"
#include "imagelabel.h"
#include "imageForm.h"
#include "fileprojectmanager.h"
#include "ui_treeViewBaseWidget.h"
#include "publicStruct.h"
#pragma execution_character_set("utf-8")
Q_DECLARE_METATYPE(QStandardItemModel*)

struct ImageFileData
{
    QList<QStringList> _listFileData;
};
struct InfoMapData {
    int level;                                //0为第一层1为第二层依次类推
    QString key;                          //关键字
    QString displayText0;           //字段名
    QString displayText1;           //显示数据
    QString toolTip;                    //提示描述
    QVariant value;                     //对应数据
    QString unit;                         //单位
    QList<InfoMapData*>child;  //子类数据
};
Q_DECLARE_METATYPE(InfoMapData*) //声明
class treeViewBaseWidget : public QWidget
{
    Q_OBJECT

public:
    treeViewBaseWidget(QWidget *parent = Q_NULLPTR);
    ~treeViewBaseWidget();
    InfoMapData *addLevel(const QString &displayText0,
        const QString &displayText1 = "",
        const QString &toolTip = "",
        InfoMapData *parent = Q_NULLPTR,
        const QString &key = "",
        const QVariant &var = QVariant());
    QString getDecimalPlaces(double d, int countBit);
    void initTreeView();
    void rbkWarningCode(const QVariantList & list, QList<InfoMapData*>&infoMapDataList);
    void recursiveCollapseExpand(const QModelIndex index, bool isExpanded);
    void openImageFileDir(QString & filePath);
    QFileInfoList GetFileList(QString path);
    void addTreeView(QList<std::string> list);
    void addTreeView(QMap<int,std::string> map, const int indexStart, const int indexEnd, bool isOldFlag);
    void addTreeViewNoneHead(QList<std::string> list);
    QModelIndex getIndex(int index);
    void delTreeViewData();
    void setCurrentIndexNum(int index);
    QVariant getCurrentData(int index);
    int getCurrentRowCount();//获取当前行数
    QVariantList getCurrentData(QStringList list);
    void GetNode();//获取所有节点
    void GetItem(QStandardItem *item);//获所有item数据
    QStringList getAllSelectedRow();
    void getRowCounts();
    int getListIndex();
    void setCurrentImageData(QMap <std::string, int> imageData,QMap<std::string, trained_flag>);//当前映射表图片数据
    void LabelRegionUpdata(int idx, ImageLabel*label);
    void setColorMap(std::map<int, QColor> colorMap, std::map<int, QColor> maskgraycolormap);
protected:
   // void customContextMenuRequested(const QPoint &pos);
private slots:
    //表头
    void slotSectionClicked(int column);
    //扩展
    void slotExpandedClicked(bool checked);
    void slotTreeViewHeaderClicked(const QModelIndex &index);
    void slot_ImageFile_status_all1_data(const QString &ip, const ImageFileData *info);
    void OnlineTreeViewDoubleClick(const QModelIndex & index);
    void OnlineTreeViewClick(const QModelIndex & index);
    void slotLeftBtnClicked(const QPoint &pos);//左键点击列表
    void slotTreeMenu(const QPoint &pos);
    void triget_menu_delete();//删除
    void triget_menu_nolabel();//无缺陷
    void triget_menu_close_auto();//关闭工具条
    void triget_menu_open_auto();//打开工具条
    void triget_menu_test();//测试
    void triget_menu_train_always();//总是
    void triget_menu_train_auto();//自动
    void triget_menu_train_never();//从不
    //鼠标响应
    void OnSignalSendId(bool is, void* id);
    void OnSignalDelImage(bool ismain);

    //右键菜单
    void OnSignalInitMenu(bool is, void* item, int size);
    void OnSignalMenuOperate(bool is, int op, void* imagelabel);
signals:
    void signalIndexValue(QVariant, int);
    void signalDelImage(bool);
    void signalMenuOperate(bool ismain, int op, void*);
private:
    Ui::treeViewBaseWidget ui;
    QMutex _mutex;
    bool _isExpanded = true;
    bool _isPressedSection = true;
    int _indexSelect;
    QMap <std::string, int> _mapImageData;//当前显示的文件图片数据映射
    QMap<std::string, trained_flag> _flagMap;//训练标志
   //bool _load = false;
    std::vector<ImageLabel*> _VImageLabel;
    QMap<ImageLabel*, QModelIndex> _mapLabel;
    //QMap<imageForm*, QModelIndex> _mapLabel;
    std::map<int, QColor> _colorMap;
    std::map<int, QColor> _maskgraycolormap;
};
