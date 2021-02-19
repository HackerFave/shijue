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
    void LabelRegionClear(); //�����ע����
    bool IsNoDefect();//����ȱ��
    void SetLabelType(LabelRectFlag flag);
    QPixmap get_maskimage();
    QPixmap get_labelimage();
    QVariant getCurrentData(int index);
    QVariantList getCurrentData(QStringList list);
    void set_keepaspectratio(bool checked);//����ԭ����
    void set_ismainshow(bool checked);
    //���±����ͼƬ��Ϣ
    void UpdateImageInfo(ainno_image ainnoimage);
    void setLineSizeWidth(int value);  //������ʾ��עʱ���߿�
    void isFill(bool checked);
    //��ͼ��ʾ��ע
    void DrawLabel();
    //���ô�С
    void ResizeWidetSize(int w, int h);
    //���û��ʻ�����Ƥ��
    void SetPenType(PenType type);
    bool setPropertyObj(const char *name, const QVariant &value);
    void SetPenSize(int value);
    void ClearLabel();//������
    int getSelectDisplayIndex();
    void delTreeViewData();
    void addTreeViewData(QMap<int, std::string> map, const int indexStart, const int indexEnd, bool isOldFlag);
    void setLcdFileNumber(int value);
    //Ԥ������
    void ResizePredictdefects(int w, int h);
    void DrawPredictEx(std::vector<float> ng_thrs, int scoretype);
    int getImageWidth();
    int getImageHeight();
    //���û�����ɫ
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
    void statisticalAreaDataClear();//ͳ���������
    bool TrainCfgLoad();//����ѵ�������ļ���ѵ��������
     //�޸�ѵ�������ļ��еĹ���Ŀ¼
    bool TrainCfgSetRootPath();
    //���ز��������ļ�
    void TestCfgLoad();
    //�޸Ĳ��������ļ��еĹ���Ŀ¼
    bool TestCfgSetRootPath();
    void setCurrentIndexNum(int index);
    QStringList getAllSelectedRows();//��ȡ����ѡ�е���
    void UpdateMenuTrainCheck(MenuOperate menuoperate, ImageMenu imagemenu);
    float geteditThresholdValue();//��ֵ
    void statisticalAreaInit();//����ͼ��ʼ��
    QString getedit_epochText();
    void setTimerStatus(bool checked);
    void setbtnText(const QString &btn, const QString &btnText);
    void LabelRegionUpdateTrainFlag(int idx);
    void btnGroupEnabled(int id, bool checked);//��ť״̬
    int getprogressBarValue();//��ȡ������value
    int getcom_keeptraindataCurrentIndex();
    QString getedit_train_ratioText();
    YAML::Node getTestconfig();
    YAML::Node getTrainconfig();
    std::map<int, Sthreshold> getSthreshold();
    void ClearMenu();
    bool Open(QString projectabspath, ainno_image curimg);
    void addFileListData(QMap <int, std::string> mapFileData);
    int getCurrentRowCount();
    void setCurrentImageData(QMap <std::string, int> imageData, QMap<std::string, trained_flag> flag_map);//��ǰӳ���ͼƬ����
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
    //ѵ��
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
    //ɸѡ��ǰ��ʾ��ͼƬ
    void signalSelectDisplay(int);
    void signalPreviousPage();//��һҳ
    void signalNextPage();//��һҳ
    void signalOpenImage(QVariant, int);
   // void signalLabelStatus(int,bool);
private:
    Ui::imageShowForm ui;
    controlBtnForm * P_controlBtnForm = Q_NULLPTR;//���ư�ť
    imageForm * P_imageForm = Q_NULLPTR;//ͼƬ��ʾ����
    parameterConfigForm * P_parameterConfigForm = Q_NULLPTR;//��������
    seriesMainForm * P_seriesMainForm = Q_NULLPTR;//����ͼ
    trainerListProForm * P_trainerListProForm = Q_NULLPTR;//�ļ��б�
    Analysiser * analysiser = NULL;
    QTimer *_timer = Q_NULLPTR;
    //std::string _errcode;//ģ��ת���еĴ�����Ϣ
    //YAML::Node trainconfig_;
    //YAML::Node testconfig_;
    std::map<int, Sthreshold> sthresholdMap;
    std::map<int, QColor> _colorMap;
    std::map<int, QColor> _maskgraycolormap;
    int _pageIndex = 0;//��ǰ��ʾҳ
    int _pageSize;//��ҳ��
    QMap <int, std::string> _mapFileData;
   // bool _isCtrl = false;
   // bool _isSpace = false;
    int show_image_file;
};
