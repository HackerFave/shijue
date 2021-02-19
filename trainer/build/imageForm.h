#pragma once

#include <QWidget>
#include "FileProjectManager.h"
#include "ui_imageForm.h"

class imageForm : public QWidget
{
    Q_OBJECT

public:
    imageForm(bool checked = true, QWidget *parent = Q_NULLPTR);
    ~imageForm();
    void initUi();
    void set_ismainshow(bool checked);
    void set_keepaspectratio(bool checked);
    bool Open(QString projectabspath, ainno_image curimg);
    void LabelRegionClear(); //�����ע����
    void LabelRegionUpdata(int idx);
    bool IsNoDefect();
    void SetLabelType(LabelRectFlag flag);
    QPixmap get_maskimage();
    QPixmap get_labelimage();
    void ResizePredictdefects(int width, int height);
    void DrawPredictEx(std::vector<float> ng_thrs, int scoretype);
    int getimageFormWidth();
    int getimageFormHeight();
    int getScrolWidth();
    int getScrolHeight();
    void UpdateColorMap(std::map<int, QColor> colorMap);//���������ɫ��
    void UpdateMaskGrayColorMap(std::map<int, QColor> colormap);//����mask�õĻҶ�ɫ��
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
    void SetPenColor(QColor color);//������ɫ
    void SetMaskPenColor(QColor color);
    ImageLabel *getImageForm();
    //���²˵�ѵ���Ĺ�ѡ
    void UpdateMenuTrainCheck(MenuOperate menuoperate, ImageMenu imagemenu);
    //��ʾͼƬ��Ϣ
    void ShowInfo();
    void ClearMenu();
    void ShowPredict() {
        ui.image->ShowPredict();
    }
    void  ShowLabel() {
        ui.image->ShowLabel();
    }
private slots:
    void OnSignalSendId(bool is, void* item);
signals:
    void signalImageSendId(bool, void* item,void* form);
private:
    Ui::imageForm ui;
    bool _isFillChecked = true; //�Ƿ����
    int _penSizeValue = 1;//��ʾ��עʱ���߿�
    bool _isKeepaspectratio = false;//�Ƿ񱣳�ԭ����
    bool _isBlabelregion;
};
