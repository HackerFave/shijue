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
    void LabelRegionClear(); //清除标注区域
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
    void UpdateColorMap(std::map<int, QColor> colorMap);//更新类别颜色表
    void UpdateMaskGrayColorMap(std::map<int, QColor> colormap);//更新mask用的灰度色表
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
    void SetPenColor(QColor color);//画笔颜色
    void SetMaskPenColor(QColor color);
    ImageLabel *getImageForm();
    //更新菜单训练的勾选
    void UpdateMenuTrainCheck(MenuOperate menuoperate, ImageMenu imagemenu);
    //显示图片信息
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
    bool _isFillChecked = true; //是否填充
    int _penSizeValue = 1;//显示标注时的线宽
    bool _isKeepaspectratio = false;//是否保持原比例
    bool _isBlabelregion;
};
