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
    PEN, //画笔
    ERAS,//橡皮擦
    COUNT_ // 默认
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
    bool state;//0未选择，1选择
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
    ainno_image* ainnoimage_;//保存图片信息
    QImage srcimage_;       //从文件读取的原始图片
    QPixmap srcpix_;        //原图显示控件，位于最底层
    QPixmap maskpix_;       //用于计算缺陷轮廓的图
    //QPixmap maskoldpix_;    //备份的用于计算缺陷轮廓的图
    QPixmap labelpix_;      //标注图显示控件，显示标注信息
    //QPixmap labeloldpix_;   //备份的标注图显示控件，显示标注信息
    QPixmap labeltypepix_;  //标注类型图显示控件，右上键的斜杠，红色为有缺陷，绿色为无缺陷
    QPixmap infopix_;       //信息图显示控件，显示图片信息
    QPixmap predictpix_;    //预测图显示控件，显示测试结果
    QPixmap predicttypepix_;//预测类型图显示控件，图像外围矩形框，红色为有缺陷，绿色为无缺陷
    QPixmap predictpixinfo_;//预测图详细信息显示控件

    QPixmap predicttypepix_src_;     //预测原图
    cv::Mat predicttypepix_src_mat_;//预测原图

    PenType penoldtype_;
    PenType pentype_;       //画笔类型
    QColor pencolor_;       //画笔颜色
    QColor maskpencolor_;   //mask画笔颜色
    int pensize_;           //画笔大小
    bool blabelregion;      //是否是标注区域
    bool bshowlabel_;       //是否显示标注图
    bool bshowpredict;      //是否显示预测图
    bool bshowpredictpixinfo_;//是否显示预测信息
    bool bkeepaspectratio_; //是否保持长宽比
    MouseCheck mousecheck_; //鼠标点击状态
    ImageMenu imagemenu_;   //鼠标右键
    bool _isBlabelregion;//是否是主界面
    bool _isTextShow;//列表是否是文本显示
    std::vector<QPoint> m_pointlist;
    QMutex mutex_;
    int x_;                 //坐标x
    int xptinterval_;
    int y_;                 //坐标y
    int yptinterval_;
    double scalex_;         //预测缩放比例x
    double scaley_;         //预测缩放比例y
    double zoomx_;          //图像缩放比例x
    double zoomy_;          //图像缩放比例y
    double zoomoldy_;       //原始图像缩放比例y
    double zoomoldx_;        //原始图像缩放比例x
    bool mousepressed_;     //鼠标按下标记
    QPoint mouseoldpos_;    //鼠标按下时的点
    int scoretype_;         //得分的比较方式
    std::vector<float> score_;          //得分
    QPen predicttypepen_;   //预测外框画笔
    std::map<int, QColor> colormap_; //类别颜色表
    std::map<int, QColor> maskgraycolormap_; //mask用的灰度色表，但值还是QColor

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
    void setLineSizeWidth(int value) { _lineSizeWidth = value; } //设置显示标注时的线宽
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
    void signalSendPressPos(const QPoint &);//右键键单击信号
    void signalLeftBtnClicked(const QPoint &pos);//左键单击信号
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
    //恢复
    void ResetZoom();
    //缩小
    void ZoomInImage(void);
    //放大
    void ZoomOutImage(void);
    //鼠标滚轮事件
    void wheelEvent(QWheelEvent *event);
    //鼠标点击事件
    void mousePressEvent(QMouseEvent * e);
    void keyPressEvent(QKeyEvent *event);
    //void contextMenuEvent(QContextMenuEvent *event);//右键菜单事件
    //鼠标移动事件
    void mouseMoveEvent(QMouseEvent * e);
    //鼠标释放事件
    void mouseReleaseEvent(QMouseEvent *e);
    //重载绘图
    void paintEvent(QPaintEvent *e);
    //鼠标标注和擦除
    void Draw();
    //鼠标画线
    void DrawLine(QPainter & p);
    ///绘制点
    void DrawPoint(QPixmap * pix, const QPointF * point, int pointsize, QPen pen, bool fill);
    //绘制预测信息数据
    void DrawPredictInfo(QPoint pos);
    //绘制缩放移动
    void DrawMoveZoom(QPoint pos);
    bool _status = false;
    bool _isFill = true;//标注显示是否填充
    int _lineSizeWidth = 1;//显示标注时的线宽
public:
    //更新类别颜色表
    void UpdateColorMap(std::map<int, QColor> colormap) { colormap_ = colormap; score_.resize(colormap.size(), -1); };
	//更新mask用的灰度色表
    void UpdateMaskGrayColorMap(std::map<int, QColor> colormap) { maskgraycolormap_ = colormap; };
    //去读图像
    bool ReadFileImage(QString projectabspath, ainno_image * curimg);
    //打开
    bool Open(QString projectabspath, ainno_image curimg);
    //将图层赋值
    bool ShowImage();
    //设置右键
    void SetMenu(bool isset = false);
    //显示标注
    void DrawLabel();
    void isFill(bool checked);
    //显示预测 scoretype 1:scoreavg 0:scoremax,3scoremin
    void DrawPredictEx(std::vector<float> ng_thrs, int scoretype);
    void DrawPredict();
    //显示图片信息
    void ShowInfo();
    //更新保存的图片信息
    void UpdateImageInfo(ainno_image ainnoimage);
    //预测缩放
    void ResizePredictdefects(int w, int h);
    //设置画笔或者橡皮擦
    void SetPenType(PenType type);
    //保存mask图像
    void SaveMastImage(QString &path);
    //显示标注图像
    void ShowLabel();
    //显示/隐藏预测图像
    void ShowPredict();
    //切换鼠标样式 快捷键z
    void SetPenStyle();
    //设置画笔大小
    void SetPenSize(int size);
    //设置画笔颜色
    void SetPenColor(QColor color);
    //设置mask画笔颜色
    void SetMaskPenColor(QColor color);
    //显示外框
    void SetPredictType(QPen pen);
    void SetPredictType(LabelRectFlag flag);
    //显示斜杠
    void SetLabelType(LabelRectFlag flag);
    //更新标注标记
    void UpdateLabeled();
    //重置大小
    void ResizeWidetSize(int w,int h);
    //获取右键菜单信息
    bool IsNoDefect();
    //重置右键菜单
    void ClearMenu();
    //初始化右键菜单
    void InitMenu();
    //清除标注
    void ClearLabel();
    //发送单击信号 true左键，false右键
    void SendImageClicked(bool is);
    //图像重置
    void ResetPix();
    //设置选中
    void SetCheck(bool is);
    //获取选中
    bool GetCheck();
    //设置无缺陷
    void SetNoLabel(bool is);
    //获取无缺陷
    bool GetNoLabel();
    //设置无缺陷菜单
    void SetMenuNoLabel(bool is);
    //总是菜单的逻辑
    void MenuTrainAlways(bool is = false);
    //从不菜单的逻辑
    void MenuTrainNever(bool is = false);
    //自动菜单的逻辑
    void MenuTrainAuto(bool is = false);
    //更新菜单训练的勾选
    void UpdateMenuTrainCheck(MenuOperate menuoperate, ImageMenu imagemenu);
    //判断是否有效控件
    bool IsValid();

};
