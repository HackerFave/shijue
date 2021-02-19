#pragma once

#include <QWidget>
#include "imageForm.h"
#include "controlBtnForm.h"
#include "parameterConfigForm.h"
#include "seriesMainForm.h"
#include "trainerListProForm.h"
#include "ui_imageShowForm.h"
#include "resultanalysis_multiclass.h"
#include "encrypt.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "yaml-cpp/yaml.h"
#include "CiWaMsgDialog.h"
#include <QTimer>
#include <iostream> 
#include <Windows.h>
#include <cstdlib>
#include <tchar.h>
#include "sysStatusFrame.h"
#define SHOW_NUM_FILE 30
#define SHOW_NUM_IMAGE 4
using namespace std;
class imageShowForm : public QWidget
{
    Q_OBJECT

public:
    imageShowForm(QWidget *parent = Q_NULLPTR);
    ~imageShowForm();
    void initUi();
    void imageListClear();
    void setFormVisible(int id, bool checked);
    bool frameIsVisible(int id);
    void UpdateColorMap(std::map<int, QColor> colorMap);
    void UpdateMaskGrayColorMap(std::map<int, QColor> colormap);
    void setWidth(int w);
    void setHeight(int h);
    int getScrolWidth()
    {
        return P_imageForm->getScrolWidth();
    }
    int getScrolHeight()
    {
        return P_imageForm->getScrolHeight();
    }
    void ShowPredict() {
        P_imageForm->ShowPredict();
    }
    void  ShowLabel() {
        P_imageForm->ShowLabel();
    }
   
    void setprogressBarValue(int value);
    void setprogressBarMaxValue(int value);
    void LabelRegionClear(); //清除标注区域
    bool IsNoDefect();//有无缺陷
    void SetLabelType(LabelRectFlag flag);
    QPixmap get_maskimage();
    QPixmap get_labelimage();
    QVariant getCurrentData(int index);
    QVariantList getCurrentData(QStringList list);
    void set_keepaspectratio(bool checked);//保持原比例
    void set_ismainshow(bool checked);
    //更新保存的图片信息
    void UpdateImageInfo(ainno_image ainnoimage);
    void setLineSizeWidth(int value);  //设置显示标注时的线宽
    void isFill(bool checked);
    //绘图显示标注
    void DrawLabel();
    //重置大小
    void ResizeWidetSize(int w, int h);
    //设置画笔或者橡皮擦
    void SetPenType(PenType type);
    bool setPropertyObj(const char *name, const QVariant &value);
    void SetPenSize(int value);
    void ClearLabel();//清除标记
    int getSelectDisplayIndex();
    void delTreeViewData();
    void addTreeViewData(QMap<int, std::string> map, const int indexStart, const int indexEnd, bool isOldFlag);
    void setLcdFileNumber(int value);
    //预测缩放
    void ResizePredictdefects(int w, int h);
    void DrawPredictEx(std::vector<float> ng_thrs, int scoretype);
    int getImageWidth();
    int getImageHeight();
    //设置画笔颜色
    void SetPenColor(QColor color);
    void SetMaskPenColor(QColor color);
    bool OpenImage(QString projectabspath, ainno_image curimg);
    //void LabelRegionUpdata(int idx);
    ImageLabel *getImageForm()
    {
        return P_imageForm->getImageForm();
    }
    treeViewBaseWidget * getWidget_ImageForm()
    {
        return P_trainerListProForm->getWidget_ImageForm();
    }
    baseListWidget* getbaseListWidget() {
        return P_trainerListProForm->getbaseListWidget();
    }
    void statisticalAreaDataClear();//统计数据清除
    bool TrainCfgLoad();//加载训练配置文件（训练参数）
     //修改训练配置文件中的工作目录
    bool TrainCfgSetRootPath();
    //加载测试配置文件
    void TestCfgLoad();
    //修改测试配置文件中的工作目录
    bool TestCfgSetRootPath();
    void setCurrentIndexNum(int index);
    QStringList getAllSelectedRows();//获取所有选中的行
    void UpdateMenuTrainCheck(MenuOperate menuoperate, ImageMenu imagemenu);
    float geteditThresholdValue();//阈值
    void statisticalAreaInit();//曲线图初始化
    QString getedit_epochText();
    void setTimerStatus(bool checked);
    void setbtnText(const QString &btn, const QString &btnText);
    void LabelRegionUpdateTrainFlag(int idx);
    void btnGroupEnabled(int id, bool checked);//按钮状态
    int getprogressBarValue();//获取进度条value
    int getcom_keeptraindataCurrentIndex();
    QString getedit_train_ratioText();
    YAML::Node getTestconfig();
    YAML::Node getTrainconfig();
    std::map<int, Sthreshold> getSthreshold();
    void ClearMenu();
    bool Open(QString projectabspath, ainno_image curimg);
    void addFileListData(QMap <int, std::string> mapFileData);
    int getCurrentRowCount();
    void setCurrentImageData(QMap <std::string, int> imageData, QMap<std::string, trained_flag> flag_map);//当前映射表图片数据
    bool getLoadMode() {
        return publicClass::instance()->readIniFile("switchBtnFileLoadMode").toBool();
    }
    void setPageindex(int page, int index);
    QList<QByteArray> getGrabPixMap();
    void setShiftStatus(bool isPressShit) {
        P_trainerListProForm->setShiftStatus(isPressShit);
    }
    //void OnChangeNgOk(std::vector<float> ng_thrs, int scoretype);
private slots:
    //void slotsignalbtnThreshold();
    //训练
    void slotupData();
    void slotPreviousPage();
    void slotNextPage();
    void slotPageIndex(const QString &);
    void slotChangeNgOk(QMap<int, QPointF>);
protected:
   // void keyPressEvent(QKeyEvent  *event);
   // void keyReleaseEvent(QKeyEvent *event);
signals:
    void signalbtnThreshold();
    void signalbtnTrain();
    void signalbtnTest();
    void signalbtnConvert();
    //筛选当前显示的图片
    void signalSelectDisplay(int);
    void signalPreviousPage();//上一页
    void signalNextPage();//下一页
    void signalOpenImage(QVariant, int);
   // void signalLabelStatus(int,bool);
private:
    Ui::imageShowForm ui;
    controlBtnForm * P_controlBtnForm = Q_NULLPTR;//控制按钮
    imageForm * P_imageForm = Q_NULLPTR;//图片显示区域
    parameterConfigForm * P_parameterConfigForm = Q_NULLPTR;//参数配置
    seriesMainForm * P_seriesMainForm = Q_NULLPTR;//曲线图
    trainerListProForm * P_trainerListProForm = Q_NULLPTR;//文件列表
    Analysiser * analysiser = NULL;
    QTimer *_timer = Q_NULLPTR;
    //std::string _errcode;//模型转换中的错误信息
    //YAML::Node trainconfig_;
    //YAML::Node testconfig_;
    std::map<int, Sthreshold> sthresholdMap;
    std::map<int, QColor> _colorMap;
    std::map<int, QColor> _maskgraycolormap;
    int _pageIndex = 0;//当前显示页
    int _pageSize;//总页数
    QMap <int, std::string> _mapFileData;
   // bool _isCtrl = false;
   // bool _isSpace = false;
    int show_image_file;
};
