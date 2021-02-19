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
    int level;                                //0Ϊ��һ��1Ϊ�ڶ�����������
    QString key;                          //�ؼ���
    QString displayText0;           //�ֶ���
    QString displayText1;           //��ʾ����
    QString toolTip;                    //��ʾ����
    QVariant value;                     //��Ӧ����
    QString unit;                         //��λ
    QList<InfoMapData*>child;  //��������
};
Q_DECLARE_METATYPE(InfoMapData*) //����
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
    int getCurrentRowCount();//��ȡ��ǰ����
    QVariantList getCurrentData(QStringList list);
    void GetNode();//��ȡ���нڵ�
    void GetItem(QStandardItem *item);//������item����
    QStringList getAllSelectedRow();
    void getRowCounts();
    int getListIndex();
    void setCurrentImageData(QMap <std::string, int> imageData,QMap<std::string, trained_flag>);//��ǰӳ���ͼƬ����
    void LabelRegionUpdata(int idx, ImageLabel*label);
    void setColorMap(std::map<int, QColor> colorMap, std::map<int, QColor> maskgraycolormap);
protected:
   // void customContextMenuRequested(const QPoint &pos);
private slots:
    //��ͷ
    void slotSectionClicked(int column);
    //��չ
    void slotExpandedClicked(bool checked);
    void slotTreeViewHeaderClicked(const QModelIndex &index);
    void slot_ImageFile_status_all1_data(const QString &ip, const ImageFileData *info);
    void OnlineTreeViewDoubleClick(const QModelIndex & index);
    void OnlineTreeViewClick(const QModelIndex & index);
    void slotLeftBtnClicked(const QPoint &pos);//�������б�
    void slotTreeMenu(const QPoint &pos);
    void triget_menu_delete();//ɾ��
    void triget_menu_nolabel();//��ȱ��
    void triget_menu_close_auto();//�رչ�����
    void triget_menu_open_auto();//�򿪹�����
    void triget_menu_test();//����
    void triget_menu_train_always();//����
    void triget_menu_train_auto();//�Զ�
    void triget_menu_train_never();//�Ӳ�
    //�����Ӧ
    void OnSignalSendId(bool is, void* id);
    void OnSignalDelImage(bool ismain);

    //�Ҽ��˵�
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
    QMap <std::string, int> _mapImageData;//��ǰ��ʾ���ļ�ͼƬ����ӳ��
    QMap<std::string, trained_flag> _flagMap;//ѵ����־
   //bool _load = false;
    std::vector<ImageLabel*> _VImageLabel;
    QMap<ImageLabel*, QModelIndex> _mapLabel;
    //QMap<imageForm*, QModelIndex> _mapLabel;
    std::map<int, QColor> _colorMap;
    std::map<int, QColor> _maskgraycolormap;
};
