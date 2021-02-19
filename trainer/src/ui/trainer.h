#pragma once
#include <map>
#include <vector>
#include <QWidget>
#include <QToolBar>
#include "ui_trainer.h"
#include "trainer_common.h"
#include "manuvisioninterface.h"
#include "yaml-cpp/yaml.h"
#include <QtConcurrent>
#include <QFuture>
#include "argsParser.h"
#include <cstdlib>
#include <fstream>
#include <sstream>
#include "alg.h"
#include "seriesMainForm.h"
//#include "resultanalysis.h"
#include "resultanalysis_multiclass.h"
#include <qtimer.h>
#include "trainprogress.h"
#include "qthread.h"
#include "btnDrogForm.h"
#include "proNameListForm.h"
#include <QStandardItemModel>
#include <sysSettingForm.h>
#include "parameterConfigForm.h"
#include <QMenuBar>
#include <QMenu>
#include <qhash.h>
#include <qaction.h>
#include "fileprojectmanager.h"
#include "publicClass.h"
#include "CiWaMsgDialog.h"
#include "imageShowForm.h"
#include <QButtonGroup>
#include <qpixmap.h>
#include "qfiledialog.h"
#include "imagelabel.h"
#include <iostream> 
#include "qdir.h"
#include "qmessagebox.h"
#include <QTime>
#include "encrypt.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <QDesktopWidget>
#include <QCursor>
#include <qdebug.h>
#include <Windows.h>
#include <tchar.h>
#include <map>
#include <QFuture>
#include "argsParser.h"
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <QKeyEvent>
#include "seriesMainForm.h"
//#include "resultanalysis.h"
#include "resultanalysis_multiclass.h"
#include <qtimer.h>
#include "trainprogress.h"
#include "qthread.h"
#include <QStandardItemModel>
#include "parameterConfigForm.h"
#include "publicStruct.h"
#include "sysStatusFrame.h"
#pragma execution_character_set("utf-8")
using namespace std;
class Trainer : public QWidget, ManuvisionInterface
{
    Q_OBJECT
        Q_PLUGIN_METADATA(IID "org.qt-project.Qt.Examples.ManuvisionInterface" FILE "manuvisionInterface.json")
        Q_INTERFACES(ManuvisionInterface)

public:
    Trainer(QWidget *parent = Q_NULLPTR);
    ~Trainer();
    void* GetWidgetPoint();
public:
    void initUi();
    void initIniSysSetting();
    void FileListAddFileName();
signals:
    void signalThreshold();
    void signalRefrush();
    void DoWorkSignal(QString);//线程
    void signalPreviousPage();//上一页
    void signalNextPage();//下一页
    void signalLabelStatus(int, bool);//显示/隐藏
private slots:
    void slotOpenPro(QString &proPath);
    void on_btnNew_clicked();//新建
    void on_btnOpen_clicked();
    void on_btnAdd_clicked();
    void on_btnSave_clicked();
    void on_btnLoadLabel_clicked();
    void on_btnAddOkImage_clicked();
    void slotActionCalloutShow();//参数配置
    void slotActionOpenPro();//打开工程
    void TrainerInitFail(QString shead, QString msg);//提示框
    void slotFormChecked(bool checked);
    void slotSysSettingSwitchChacked(int, bool);//标注配置
    void slotSysPenValue(int);//标注显示画笔宽度
    //更新类别颜色映射表
    void UpdateColorMap(QList<QVariantList>);
    void slotFileExportMode(QString);//系统配置设置文件导出模式
    void slotFileloadMode(bool);//文件加载方式
    void slotActionTrainerReport();//导出训练报告
    void slotToolSet(int);
    void slotSetPenSize(int size);// 画笔大小滑块处理函数
    //设置画笔颜色
    void slotSetPenColor(QColor);
    void slotOpenImage(QVariant data, int row);
    //鼠标响应
    void OnSignalDelImage(bool ismain);
    void OnSignalMenuOperate(bool is, int op, void* imagelabel);
    void slotActionSysSetting();//系统设置
    //阈值
    void on_btnThreshold_clicked();
    void slotUpdataForm();
    //进度条
    void OnReadTrainCsvFinished();
    void OnReadTestCsvFinished();
    //接受进度条信号
    void OnSignalResultReady(int n);
    //训练
    void on_btnTrain_clicked();//训练
    void on_btnTest_clicked();//测试
    void on_btnConvert_clicked();//导出
    //右键点击
    void on_menutest_clicked(bool islabelregion);//测试
    void slotActionChartShow();//曲线区域
    void slotActionToolShow();
    void OnFilterDisplay(int index);//筛选当前显示图片
    void openUrl(QString);//打开工程
    void LabelRegionUpdata(int idx);//刷新图片显示

    void on_btnHideP_clicked();
    void on_btnHide_clicked();
    void slotLabelSatus(int id, bool checked);
private:
    //更新类别信息
    bool TrainCfgSetClassAndLabel();
    //更新类别颜色表
    void LabelRegionUpdateColorMap();
    //更新标注
    void LabelRegionLabeldefects(int idx);
    //更新类别颜色表
    void FileListUpdateColorMap();
    //更新标注信息
    void FileListUpdateLabeldefects();
    //菜单应用
    void MenuApply();
    //点击应用按钮，更新标注区域，返回状态
    labelinfo LabelRegionApply();
    void FileListUpdataImage();//更新当前选择图片状态，返回图片索引
    //文件列表过滤
    void FileListFilter(int index);
    //按类别找Contours
    void FindContoursByClass(cv::Mat maskimg, std::vector<std::vector<cv::Point>> &labeldefects, std::vector<cv::Vec4i> hierarchy);
    //按照文件名添加图片
    void updateFileListAddFileName();
    //“应用”按钮处理函数，批量处理右键“无缺陷”以及缺陷标注
    void ListMenuApply(int index, ImageLabel* imagelabel);
    //更新训练标志
    void UpdataTrainFlag(bool trained);
    //初始化并启动进度条线程 1:test ,2:train
    void ProcessInit(QString & csv, int len, int processtype);
    //进度条对象释放
    void ProcessRelease();
    //模型序列化及加密
    bool serialize_encrypt(std::string model_type);
    //模型转换后处理，把模型拷贝到指定目录，并修改配置文件
    void ConvertPostprocess(QString modelabspath);
    //键盘事件
    void keyPressEvent(QKeyEvent *ev);
    void keyReleaseEvent(QKeyEvent *ev);
    //bool getKeySwitchImageNeedIndex(const int itemIndex);
private:
    Ui::Trainer ui;
    //工程操作按钮
    QAction *actionOpen = Q_NULLPTR;// "&打开工程"
    QAction *actionNew = Q_NULLPTR; //"&新建工程"
    QAction *actionAdd = Q_NULLPTR; //"&添加工程"
    QAction *actionSave = Q_NULLPTR; //"&保存工程";
    QAction *actionSet = Q_NULLPTR; //"&加载标注";
    QAction *actionAddOther = Q_NULLPTR; //"&添加无缺陷图"
    QAction *actionCallout = Q_NULLPTR; //"&参数配置"
    QAction *actionChart = Q_NULLPTR; //"&统计分析"
    QAction *actionTool = Q_NULLPTR; //"&标注工具"
    QAction *actionSysSetting = Q_NULLPTR; //系统设置
    QAction *actionDoc = Q_NULLPTR;//文档
    QAction *actionExportTrainerReport = Q_NULLPTR;//导出训练报告
    QAction *actionjishuzhichi = Q_NULLPTR;//技术支持
    QAction *actionLog = Q_NULLPTR;//日志
    QAction *actionguanyu = Q_NULLPTR;//关于
    QAction *_actionSpace = Q_NULLPTR;
    QAction *_actionCtrl = Q_NULLPTR;
    int _indexImageFileName = -1; // 文件名加载模式时当前索引
    QMap <std::string, int> _mapTableFile;//map映射表
    QMap<std::string, trained_flag> _flag_map;//训练标志
    //QButtonGroup *_btnGroup = Q_NULLPTR;//按钮组
    btnDrogForm *_btnDrog = Q_NULLPTR;//拖拽工具条
    sysSettingForm * _sysSettingform = Q_NULLPTR; // 系统设置模块
    std::string export_mode_ = "onnx";//导出模式
    bool _isFillChecked = true; //是否填充
    bool _isKeepaspectratio = false;//是否保持原比例
    int _penSizeValue = 1;//显示标注时的线宽
    int _selectId = 2;
    std::map<int, QColor> colormap_;
    std::map<int, QColor> maskgraycolormap_;
    std::map<int, QString> namemap_;
    //QMap<QString, ainno_image> _mapImageStruct;//文件名+图片数据
    int _valueSize = 3;
    QString _fileExportMode;
    bool _fileLoadType = true;//文件加载方式
    bool _oldFileLoadType = false;//打开新工程之前的老工程加载方式
    bool _isOpenNewPro = true;//是否打开新的工程
    QString _proPath;//工程路径
    QMap<QString, QString> _mapImageNamePath;//文件名+路径
    QMap<QString, ainno_image> _mapImageStruct;//文件名+图片数据
    //QList<std::string> _listFileName;//文件名
    QString _currentListFileName;//当前编辑图片
    QMap <int, std::string> _mapFileName;//全部文件名
    QMap <int, std::string> _mapFileNameShow;//列表显示list
     //训练按钮状态
    bool bistrainopen_ = false;
    //测试按钮状态
    bool bistestopen_ = false;
    bool ispressctrl_ = false;          //是否按住了ctrl键
    bool ispressshift_ = false;         //是否按住了shift键
    TrainProgress* trainprogress_ = Q_NULLPTR; //进度条线程
    QThread* workerthread_ = Q_NULLPTR;     //进度条线程
    std::string _errcode;//模型转换中的错误信息
    std::vector<int> testindexes_;
    imageShowForm * P_imageShowForm = Q_NULLPTR;//展示区主界面
    bool _isCtrl = false;
    bool _isSpace = false;
};
