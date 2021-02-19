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
    void setCurrentImageData(QMap <std::string, int> imageData, QMap<std::string, trained_flag>);//��ǰӳ���ͼƬ����
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
    void slotTreeMenu(const QPoint &pos);//��������˵�
    void triget_menu_delete();//ɾ��
    void triget_menu_nolabel();//��ȱ��
    void triget_menu_close_auto();//�رչ�����
    void triget_menu_open_auto();//�򿪹�����
    void triget_menu_test();//����
    void triget_menu_train_always();//����
    void triget_menu_train_auto();//�Զ�
    void triget_menu_train_never();//�Ӳ�
    //�����Ӧ
  //  void OnSignalSendId(bool is, void* id);
   // void OnSignalDelImage(bool ismain);

    //�Ҽ��˵�
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
    QMap <std::string, int> _mapImageData;//��ǰ��ʾ���ļ�ͼƬ����ӳ��
    bool _load = false;
    QMap<ImageLabel*, QListWidgetItem*> _mapLabel;
    QMap<std::string, trained_flag> _flagMap;//ѵ����־
    std::map<int, QColor> _colorMap;
    std::map<int, QColor> _maskgraycolormap;
    int _startRow;
    int _endRow;
};
