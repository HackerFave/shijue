#pragma once

#include <QWidget>
#include <qevent.h>
#include <qpainter.h>
#include <qimage.h>
#include <qpixmap.h>
#include "qbitmap.h"
#include <qpoint.h>
#include <vector>
#include <QKeyEvent>
#include "trainer_common.h"
#include "filedatastream.h"
#include "qmutex.h"
enum PenType
{
    PEN, //����
    ERAS,//��Ƥ��
    COUNT_ // Ĭ��
};

enum LabelRectFlag
{
    NONE_,
    RED,
    GREEN
};

enum ShowFlag
{
    SHOWMASK,
    SHOWFLAG,
    SHOWPREDICTDEFECTS,
};

typedef struct
{
    void* menu_delet;
    void* menu_nolabel;
    void* menu_train_always;
    void* menu_train_never;
    void* menu_train_auto;
    void* menu_close_auto;
    void* menu_open_auto;
    void* menu_test;
    bool b_train_always;
    bool b_train_never;
    bool b_train_auto;

}ImageMenu;

enum MenuOperate
{
    O_NOLABEL,
    O_ALWAYS,
    O_NEVER,
    O_AUTO,
    O_CLOSE,
    O_OPEN,
    O_TEST,
};


typedef struct
{
    bool state;//0δѡ��1ѡ��
    QImage pix;
}MouseCheck;

enum TrainEnum
{
    T_ALWAYS,
    T_NEVER,
    T_AUTO,
    T_NONE
};

typedef struct
{
    TrainEnum trainstate;

}TrainFlag;


class ImageLabel : public QWidget
{
    Q_OBJECT

public:
    explicit ImageLabel(bool isBlabelregion = true,bool isTextShow = true,QWidget *parent = Q_NULLPTR);
    ~ImageLabel();

private:
    ainno_image* ainnoimage_;//����ͼƬ��Ϣ
    QImage srcimage_;       //���ļ���ȡ��ԭʼͼƬ
    QPixmap srcpix_;        //ԭͼ��ʾ�ؼ���λ����ײ�
    QPixmap maskpix_;       //���ڼ���ȱ��������ͼ
    //QPixmap maskoldpix_;    //���ݵ����ڼ���ȱ��������ͼ
    QPixmap labelpix_;      //��עͼ��ʾ�ؼ�����ʾ��ע��Ϣ
    //QPixmap labeloldpix_;   //���ݵı�עͼ��ʾ�ؼ�����ʾ��ע��Ϣ
    QPixmap labeltypepix_;  //��ע����ͼ��ʾ�ؼ������ϼ���б�ܣ���ɫΪ��ȱ�ݣ���ɫΪ��ȱ��
    QPixmap infopix_;       //��Ϣͼ��ʾ�ؼ�����ʾͼƬ��Ϣ
    QPixmap predictpix_;    //Ԥ��ͼ��ʾ�ؼ�����ʾ���Խ��
    QPixmap predicttypepix_;//Ԥ������ͼ��ʾ�ؼ���ͼ����Χ���ο򣬺�ɫΪ��ȱ�ݣ���ɫΪ��ȱ��
    QPixmap predictpixinfo_;//Ԥ��ͼ��ϸ��Ϣ��ʾ�ؼ�

    QPixmap predicttypepix_src_;     //Ԥ��ԭͼ
    cv::Mat predicttypepix_src_mat_;//Ԥ��ԭͼ

    PenType penoldtype_;
    PenType pentype_;       //��������
    QColor pencolor_;       //������ɫ
    QColor maskpencolor_;   //mask������ɫ
    int pensize_;           //���ʴ�С
    bool blabelregion;      //�Ƿ��Ǳ�ע����
    bool bshowlabel_;       //�Ƿ���ʾ��עͼ
    bool bshowpredict;      //�Ƿ���ʾԤ��ͼ
    bool bshowpredictpixinfo_;//�Ƿ���ʾԤ����Ϣ
    bool bkeepaspectratio_; //�Ƿ񱣳ֳ����
    MouseCheck mousecheck_; //�����״̬
    ImageMenu imagemenu_;   //����Ҽ�
    bool _isBlabelregion;//�Ƿ���������
    bool _isTextShow;//�б��Ƿ����ı���ʾ
    std::vector<QPoint> m_pointlist;
    QMutex mutex_;
    int x_;                 //����x
    int xptinterval_;
    int y_;                 //����y
    int yptinterval_;
    double scalex_;         //Ԥ�����ű���x
    double scaley_;         //Ԥ�����ű���y
    double zoomx_;          //ͼ�����ű���x
    double zoomy_;          //ͼ�����ű���y
    double zoomoldy_;       //ԭʼͼ�����ű���y
    double zoomoldx_;        //ԭʼͼ�����ű���x
    bool mousepressed_;     //��갴�±��
    QPoint mouseoldpos_;    //��갴��ʱ�ĵ�
    int scoretype_;         //�÷ֵıȽϷ�ʽ
    std::vector<float> score_;          //�÷�
    QPen predicttypepen_;   //Ԥ����򻭱�
    std::map<int, QColor> colormap_; //�����ɫ��
    std::map<int, QColor> maskgraycolormap_; //mask�õĻҶ�ɫ����ֵ����QColor

public:
    ainno_train_policy get_ainno_train_policy(){
        return ainnoimage_
            ->trainpolicy;
    }
    ainno_label_type get_ainnoimage_label_type() { return ainnoimage_
        ->labeltype;
    }
    ImageMenu get_imagemenu() { return imagemenu_; }

    void set_ainnoimage_label_type(ainno_label_type type) {
        ainnoimage_->labeltype
            = type;
    }
    void set_ainnoimage_labeled(bool is) { ainnoimage_->labeled = is; }
    void set_keepaspectratio(bool is) { bkeepaspectratio_ = is; }
    void set_ismainshow(bool is) { blabelregion = is; }
    void setLineSizeWidth(int value) { _lineSizeWidth = value; } //������ʾ��עʱ���߿�
    void setBlabelregion(bool checked) { _isBlabelregion = checked; }

    QPixmap get_maskimage() { return maskpix_; }
    QPixmap get_labelimage() { return labelpix_; }


signals:
    void signalSendId(bool is, void* id);
signals:
    void signalDelImage(bool ismain);
signals:
    void signalInitMenu(bool is, void* item, int size);
signals:
    void signalMenuOperate(bool ismain,int op, void* imagelabel);
    void signalSendPressPos(const QPoint &);//�Ҽ��������ź�
    void signalLeftBtnClicked(const QPoint &pos);//��������ź�
private slots:
    void triget_menu_delete();
    void triget_menu_nolabel();
    void triget_menu_train_always();
    void triget_menu_train_never();
    void triget_menu_train_auto();
    void triget_menu_close_auto();
    void triget_menu_open_auto();
    void triget_menu_test();

private:
    //�ָ�
    void ResetZoom();
    //��С
    void ZoomInImage(void);
    //�Ŵ�
    void ZoomOutImage(void);
    //�������¼�
    void wheelEvent(QWheelEvent *event);
    //������¼�
    void mousePressEvent(QMouseEvent * e);
    void keyPressEvent(QKeyEvent *event);
    //void contextMenuEvent(QContextMenuEvent *event);//�Ҽ��˵��¼�
    //����ƶ��¼�
    void mouseMoveEvent(QMouseEvent * e);
    //����ͷ��¼�
    void mouseReleaseEvent(QMouseEvent *e);
    //���ػ�ͼ
    void paintEvent(QPaintEvent *e);
    //����ע�Ͳ���
    void Draw();
    //��껭��
    void DrawLine(QPainter & p);
    ///���Ƶ�
    void DrawPoint(QPixmap * pix, const QPointF * point, int pointsize, QPen pen, bool fill);
    //����Ԥ����Ϣ����
    void DrawPredictInfo(QPoint pos);
    //���������ƶ�
    void DrawMoveZoom(QPoint pos);
    bool _status = false;
    bool _isFill = true;//��ע��ʾ�Ƿ����
    int _lineSizeWidth = 1;//��ʾ��עʱ���߿�
public:
    //���������ɫ��
    void UpdateColorMap(std::map<int, QColor> colormap) { colormap_ = colormap; score_.resize(colormap.size(), -1); };
	//����mask�õĻҶ�ɫ��
    void UpdateMaskGrayColorMap(std::map<int, QColor> colormap) { maskgraycolormap_ = colormap; };
    //ȥ��ͼ��
    bool ReadFileImage(QString projectabspath, ainno_image * curimg);
    //��
    bool Open(QString projectabspath, ainno_image curimg);
    //��ͼ�㸳ֵ
    bool ShowImage();
    //�����Ҽ�
    void SetMenu(bool isset = false);
    //��ʾ��ע
    void DrawLabel();
    void isFill(bool checked);
    //��ʾԤ�� scoretype 1:scoreavg 0:scoremax,3scoremin
    void DrawPredictEx(std::vector<float> ng_thrs, int scoretype);
    void DrawPredict();
    //��ʾͼƬ��Ϣ
    void ShowInfo();
    //���±����ͼƬ��Ϣ
    void UpdateImageInfo(ainno_image ainnoimage);
    //Ԥ������
    void ResizePredictdefects(int w, int h);
    //���û��ʻ�����Ƥ��
    void SetPenType(PenType type);
    //����maskͼ��
    void SaveMastImage(QString &path);
    //��ʾ��עͼ��
    void ShowLabel();
    //��ʾ/����Ԥ��ͼ��
    void ShowPredict();
    //�л������ʽ ��ݼ�z
    void SetPenStyle();
    //���û��ʴ�С
    void SetPenSize(int size);
    //���û�����ɫ
    void SetPenColor(QColor color);
    //����mask������ɫ
    void SetMaskPenColor(QColor color);
    //��ʾ���
    void SetPredictType(QPen pen);
    void SetPredictType(LabelRectFlag flag);
    //��ʾб��
    void SetLabelType(LabelRectFlag flag);
    //���±�ע���
    void UpdateLabeled();
    //���ô�С
    void ResizeWidetSize(int w,int h);
    //��ȡ�Ҽ��˵���Ϣ
    bool IsNoDefect();
    //�����Ҽ��˵�
    void ClearMenu();
    //��ʼ���Ҽ��˵�
    void InitMenu();
    //�����ע
    void ClearLabel();
    //���͵����ź� true�����false�Ҽ�
    void SendImageClicked(bool is);
    //ͼ������
    void ResetPix();
    //����ѡ��
    void SetCheck(bool is);
    //��ȡѡ��
    bool GetCheck();
    //������ȱ��
    void SetNoLabel(bool is);
    //��ȡ��ȱ��
    bool GetNoLabel();
    //������ȱ�ݲ˵�
    void SetMenuNoLabel(bool is);
    //���ǲ˵����߼�
    void MenuTrainAlways(bool is = false);
    //�Ӳ��˵����߼�
    void MenuTrainNever(bool is = false);
    //�Զ��˵����߼�
    void MenuTrainAuto(bool is = false);
    //���²˵�ѵ���Ĺ�ѡ
    void UpdateMenuTrainCheck(MenuOperate menuoperate, ImageMenu imagemenu);
    //�ж��Ƿ���Ч�ؼ�
    bool IsValid();

};
