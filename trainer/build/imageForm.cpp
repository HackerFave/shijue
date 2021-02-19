#include "imageForm.h"

imageForm::imageForm(bool checked, QWidget *parent)
    : QWidget(parent),_isBlabelregion(checked)
{
    ui.setupUi(this);
    ui.image->setBlabelregion(checked);
   // connect(ui.image, SIGNAL(signalSendId(bool, void *)), this, SLOT(OnSignalSendId(bool, void*)));
    //connect(ui.image,SIGNAL())
}

imageForm::~imageForm()
{
}
void imageForm::initUi()
{

}
void imageForm::set_ismainshow(bool checked)
{
    ui.image->set_ismainshow(checked);
}
void imageForm::set_keepaspectratio(bool checked)
{
    ui.image->set_keepaspectratio(checked);//�Ƿ񱣳ֳ����
}
bool imageForm::Open(QString projectabspath, ainno_image curimg)
{
    return ui.image->Open(projectabspath, curimg);
}
//�����ע����
void imageForm::LabelRegionClear() 
{
    ui.image->ResetPix();
    ui.image->repaint();
}
//����б���ĳ��ͼƬʱ�����±�ע����
void imageForm::LabelRegionUpdata(int idx)
{

    QString imageabspath = FileProjectManager::Instance()->ProjectGetImageAbsPath();
    ainno_image curimg = FileProjectManager::Instance()->StreamGetImageInfo(idx);
    ui.image->ClearMenu();
    ui.image->setLineSizeWidth(_penSizeValue);
    ui.image->isFill(_isFillChecked);
    ui.image->set_keepaspectratio(_isKeepaspectratio);

    ui.image->Open(imageabspath, curimg);
    ui.image->ResizeWidetSize(ui.scrollArea->width() - 2, ui.scrollArea->height() - 2);
    //ui.image->DrawPredict();
    ui.image->ResizePredictdefects(ui.scrollArea->width() - 2, ui.scrollArea->height() - 2);
}
void imageForm::ClearMenu()
{
    ui.image->ClearMenu();
}
bool imageForm::IsNoDefect()
{
    return ui.image->IsNoDefect();
}
void imageForm::SetLabelType(LabelRectFlag flag)
{
    ui.image->SetLabelType(flag);
}
QPixmap imageForm::get_maskimage()
{
    return ui.image->get_maskimage();
}
QPixmap imageForm::get_labelimage()
{
    return ui.image->get_labelimage();
}
int imageForm::getScrolWidth()
{
    return ui.scrollArea->width();
}
int imageForm::getScrolHeight()
{
    return ui.scrollArea->height();
}
void imageForm::ResizePredictdefects(int width, int height)
{
    ui.image->ResizePredictdefects(width, height);
}
void imageForm::DrawPredictEx(std::vector<float> ng_thrs, int scoretype)
{
    ui.image->DrawPredictEx(ng_thrs, scoretype);
}
int imageForm::getimageFormWidth()
{
    return ui.image->width();
}
int imageForm::getimageFormHeight()
{
    return ui.image->height();
}
void imageForm::UpdateColorMap(std::map<int, QColor> colorMap)
{
    ui.image->UpdateColorMap(colorMap);
}
void imageForm::UpdateMaskGrayColorMap(std::map<int, QColor> colormap)
{
    ui.image->UpdateMaskGrayColorMap(colormap);
}
//���±����ͼƬ��Ϣ
void imageForm::UpdateImageInfo(ainno_image ainnoimage)
{
    ui.image->UpdateImageInfo(ainnoimage);
}
void imageForm::setLineSizeWidth(int value)  //������ʾ��עʱ���߿�
{
    ui.image->setLineSizeWidth(value);
}
// trainer����boolֵ���Ƿ������ʾ
void imageForm::isFill(bool checked)
{
    ui.image->isFill(checked);
}
//��ͼ��ʾ��ע
void imageForm::DrawLabel()
{
    ui.image->DrawLabel();
}
//���ô�С
void imageForm::ResizeWidetSize(int w, int h)
{
    ui.image->ResizeWidetSize(w,h);
}
//���û��ʻ�����Ƥ��
void imageForm::SetPenType(PenType type)
{
    ui.image->SetPenType(type);
}
bool imageForm::setPropertyObj(const char *name, const QVariant &value)
{
    return ui.image->setProperty(name, value);
}
void imageForm::SetPenSize(int value)
{
    ui.image->SetPenSize(value);
}
void imageForm::ClearLabel()//������
{
    ui.image->ClearLabel();
}
void imageForm::SetPenColor(QColor color)
{
    ui.image->SetPenColor(color);
}
void imageForm::SetMaskPenColor(QColor color)
{
    ui.image->SetMaskPenColor(color);
}
ImageLabel *imageForm::getImageForm()
{
    return ui.image;
}
void imageForm::UpdateMenuTrainCheck(MenuOperate menuoperate, ImageMenu imagemenu)
{
    ui.image->UpdateMenuTrainCheck(menuoperate, imagemenu);
}
//��ʾͼƬ��Ϣ
void imageForm::ShowInfo()
{
    ui.image->ShowInfo();
}
void imageForm::OnSignalSendId(bool is, void* item)
{
    ImageLabel* label = (ImageLabel*)item;
    emit signalImageSendId(is, label,this);
}