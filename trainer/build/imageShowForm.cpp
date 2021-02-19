#include "imageShowForm.h"

imageShowForm::imageShowForm(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    initUi();
}

imageShowForm::~imageShowForm()
{
   // FileListClear();
    P_trainerListProForm->FileListClear();
    if (analysiser) {
        delete analysiser;
    }
}
void imageShowForm::initUi()
{
    P_controlBtnForm = new controlBtnForm;//上部控制按钮
    P_imageForm = new imageForm;//显示图片
    P_parameterConfigForm = new parameterConfigForm;//参数配置
    P_seriesMainForm = new seriesMainForm;//曲线图
    P_trainerListProForm = new trainerListProForm;//文件列表
    this->setFocusPolicy(Qt::StrongFocus);
    analysiser = new Analysiser(); //界面启动时新建类指针
    _timer = new QTimer;
    connect(_timer, SIGNAL(timeout()), this, SLOT(slotupData()));

    ui.verticalLayout_image->addWidget(P_controlBtnForm);
    ui.verticalLayout_image->addWidget(P_imageForm);

    ui.gridLayout_filelist->addWidget(P_trainerListProForm);
    ui.gridLayout_parameter->addWidget(P_parameterConfigForm);
    ui.gridLayout_statisticalArea->addWidget(P_seriesMainForm);

    connect(P_parameterConfigForm, SIGNAL(signal_btnParamDefault()), this, SLOT(on_btnParamDefault_clicked()));
    connect(P_parameterConfigForm, SIGNAL(signal_btnParamApply()), this, SLOT(on_btnParamApply_clicked()));
    connect(P_controlBtnForm, SIGNAL(signalbtnThreshold()), this, SIGNAL(signalbtnThreshold()));//分析
    connect(P_controlBtnForm, SIGNAL(signalbtnTrain()), this, SIGNAL(signalbtnTrain()));//训练
    connect(P_controlBtnForm, SIGNAL(signalbtnTest()), this, SIGNAL(signalbtnTest()));//测试
    connect(P_controlBtnForm, SIGNAL(signalbtnConvert()), this, SIGNAL(signalbtnConvert()));//导出
    connect(P_trainerListProForm, SIGNAL(signalFilterDisplay(int)), this, SIGNAL(signalSelectDisplay(int)));
    QObject::connect(P_seriesMainForm, SIGNAL(signalSendPoints(QMap<int, QPointF>)), this, SLOT(slotChangeNgOk(QMap<int, QPointF>)));
    //上一页
    connect(P_trainerListProForm, SIGNAL(signalPreviousPage()), this, SLOT(slotPreviousPage()));
    //下一页
    connect(P_trainerListProForm, SIGNAL(signalNextPage()), this, SLOT(slotNextPage()));
    //指定页
    connect(sysStatusFrame::instance(), SIGNAL(signalPageIndex(const QString &)), this, SLOT(slotPageIndex(const QString &)));
}
void imageShowForm::imageListClear()
{
    P_trainerListProForm->FileListClear();
}
void imageShowForm::setFormVisible(int id, bool checked)
{
    switch (id)
    {
    case 0: {
        ui.frame_filelist->setVisible(checked);
    }
            break;
    case 1: {
        ui.frame_imageArea->setVisible(checked);
    }
            break;
    case 2: {
        ui.frame_parameter->setVisible(checked);
    }
            break;
    case 3: {
        ui.frame_statisticalArea->setVisible(checked);
    }
            break;
    default:
        break;
    }
}
bool imageShowForm::frameIsVisible(int id)
{
    switch (id)
    {
    case 0: {
       return ui.frame_filelist->isVisible();
    }
            break;
    case 1: {
       return ui.frame_imageArea->isVisible();
    }
            break;
    case 2: {
       return ui.frame_parameter->isVisible();
    }
            break;
    case 3: {
       return ui.frame_statisticalArea->isVisible();
    }
            break;
    default:
        break;
    }
}
void imageShowForm::UpdateColorMap(std::map<int, QColor> colorMap)
{
    P_imageForm->UpdateColorMap(colorMap);
    _colorMap = colorMap;
}
void imageShowForm::UpdateMaskGrayColorMap(std::map<int, QColor> colormap)
{
    P_imageForm->UpdateMaskGrayColorMap(colormap);
    _maskgraycolormap = colormap;
}
void imageShowForm::set_keepaspectratio(bool checked)
{
    P_imageForm->set_keepaspectratio(checked);//是否保持长宽比
}
//是否是标注区域
void imageShowForm::set_ismainshow(bool checked)
{
    P_imageForm->set_ismainshow(checked);
}
//更新保存的图片信息
void imageShowForm::UpdateImageInfo(ainno_image ainnoimage)
{
    P_imageForm->UpdateImageInfo(ainnoimage);
}
void imageShowForm::setLineSizeWidth(int value)  //设置显示标注时的线宽
{
    P_imageForm->setLineSizeWidth(value);
}
// trainer传回bool值，是否填充显示
void imageShowForm::isFill(bool checked)
{
    P_imageForm->isFill(checked);
}
//绘图显示标注
void imageShowForm::DrawLabel()
{
    P_imageForm->DrawLabel();
}
//重置大小
void imageShowForm::ResizeWidetSize(int w, int h)
{
    P_imageForm->ResizeWidetSize(w, h);
}
//获取当前list文件列表item数据
QVariant imageShowForm::getCurrentData(int index)
{
   return P_trainerListProForm->getWidget_imageListCurrentData(index);
}
QVariantList imageShowForm::getCurrentData(QStringList list)
{
    return P_trainerListProForm->getWidget_imageListCurrentData(list);
}
//设置画笔或者橡皮擦
void imageShowForm::SetPenType(PenType type)
{
    P_imageForm->SetPenType(type);
}
bool imageShowForm::setPropertyObj(const char *name, const QVariant &value)
{
    return P_imageForm->setPropertyObj(name, value);
}
void imageShowForm::SetPenSize(int value)
{
    P_imageForm->SetPenSize(value);
}
//清除标注
void imageShowForm::ClearLabel()
{
    P_imageForm->ClearLabel();
}
bool imageShowForm::IsNoDefect()
{
    return P_imageForm->IsNoDefect();
}
void imageShowForm::SetLabelType(LabelRectFlag flag)
{
    P_imageForm->SetLabelType(flag);
}
QPixmap imageShowForm::get_maskimage()
{
    return P_imageForm->get_maskimage();
}
QPixmap imageShowForm::get_labelimage()
{
    return P_imageForm->get_labelimage();
}
int imageShowForm::getSelectDisplayIndex()
{
    return P_trainerListProForm->getSelectDisplayIndex();
}
void imageShowForm::delTreeViewData()
{
    P_trainerListProForm->delTreeViewData();
}
void imageShowForm::addTreeViewData(QMap<int, std::string> map, const int indexStart, const int indexEnd, bool isOldFlag)
{
    P_trainerListProForm->addTreeViewData(map,indexStart,indexEnd,isOldFlag);
}
void imageShowForm::setLcdFileNumber(int value)
{
    P_trainerListProForm->setLcdFileNumber(value);
}
//预测缩放
void imageShowForm::ResizePredictdefects(int w, int h)
{
    P_imageForm->ResizePredictdefects(w, h);
}
//绘图显示预测结果
void imageShowForm::DrawPredictEx(std::vector<float> ng_thrs, int scoretype)
{
    P_imageForm->DrawPredictEx(ng_thrs, scoretype);
}
int imageShowForm::getImageWidth()
{
    return P_imageForm->getimageFormWidth();
}
int imageShowForm::getImageHeight()
{
    return P_imageForm->getimageFormHeight();
}
void imageShowForm::SetPenColor(QColor color)
{
    P_imageForm->SetPenColor(color);
}
void imageShowForm::SetMaskPenColor(QColor color)
{
    P_imageForm->SetMaskPenColor(color);
}
void imageShowForm::LabelRegionClear()
{
    P_imageForm->LabelRegionClear();
}
void imageShowForm::setprogressBarValue(int value)
{
    P_controlBtnForm->setprogressBarValue(value);
}
void imageShowForm::setprogressBarMaxValue(int value)
{
    P_controlBtnForm->setprogressBarMaxValue(value);
}
bool imageShowForm::OpenImage(QString projectabspath, ainno_image curimg)
{
    return P_imageForm->Open(projectabspath, curimg);
}
/*void imageShowForm::LabelRegionUpdata(int idx)
{
    P_imageForm->LabelRegionUpdata(idx);
}*/
bool imageShowForm::Open(QString projectabspath, ainno_image curimg)
{
    return P_imageForm->Open(projectabspath, curimg);
}
void imageShowForm::ClearMenu()
{
    P_imageForm->ClearMenu();
}
void imageShowForm::statisticalAreaDataClear()
{
    P_seriesMainForm->dataClear();
}
bool imageShowForm::TrainCfgLoad()
{
   return P_parameterConfigForm->TrainCfgLoad();
}
bool imageShowForm::TrainCfgSetRootPath()
{
    return P_parameterConfigForm->TrainCfgSetRootPath();
}
//加载测试配置文件
void imageShowForm::TestCfgLoad()
{
    P_parameterConfigForm->TestCfgLoad();
}
//修改测试配置文件中的工作目录
bool imageShowForm::TestCfgSetRootPath()
{
   return P_parameterConfigForm->TestCfgSetRootPath();
}
void imageShowForm::setCurrentIndexNum(int index)
{
    P_trainerListProForm->setCurrentIndexNum(index);
}
QStringList imageShowForm::getAllSelectedRows()
{
   return P_trainerListProForm->getAllSelectedRows();
}
void imageShowForm::UpdateMenuTrainCheck(MenuOperate menuoperate, ImageMenu imagemenu)
{
   P_imageForm->UpdateMenuTrainCheck(menuoperate, imagemenu);
}
float imageShowForm::geteditThresholdValue()
{
    return P_controlBtnForm->geteditThresholdValue();
}
void imageShowForm::statisticalAreaInit()
{
    int64 ts = cv::getTickCount();
    float ng_thr = 0.6;
    float ok_thr = 0.4;
    analysiser->SetNClass(_colorMap.size() + 1);
    QTime time;
    time.start();
    bool path_exist = analysiser->UpdatePath(); //更新gt和pred图片路径 380张图约5-6ms
    qDebug() << time.elapsed() / 1000.0 << "更新gt和pred图片路径";
    if (!path_exist)
    {
        return; //若更新路径时报错，则直接返回
    }
    P_seriesMainForm->setXRG_str("0.6", "0.4");
 
    //计算roc曲线，计算图片级及缺陷级信息
    P_seriesMainForm->addSeriesPointRoc(P_seriesMainForm->getSeriesPointsRoc(
        analysiser->get_roc(P_controlBtnForm->geteditThresholdText().toFloat(),
            P_controlBtnForm->getcomboBoxCurrentIndex(), false))); //类别0
    P_seriesMainForm->setSeriesAuc_Bestthreshold(analysiser->get_auc(), analysiser->get_bestthreshold());
    for (int i = 0; i < _colorMap.size() + 1; i++) {
        Sthreshold tmpthr;
        tmpthr.global_ng_thr = ng_thr;
        tmpthr.global_ok_thr = ok_thr;
        sthresholdMap[i] = tmpthr;
    }
    P_seriesMainForm->setDataMatrix(analysiser->get_confusion_matrix(sthresholdMap)); //根据roc的数据计算混淆矩阵
    P_seriesMainForm->setYRangeData(analysiser->get_max_number_img(), 0);//y轴最大值
    P_seriesMainForm->setYRangeData(analysiser->get_max_number_defect(), 1);//最大值
    QList<QPointF> listPoint2;
    listPoint2.append(QPointF(ng_thr, 0));
    listPoint2.append(QPointF(ng_thr, 0));
    QList<QPointF> listPoint3;
    listPoint3.append(QPointF(ok_thr, 0));
    listPoint3.append(QPointF(ok_thr, 0));
    P_seriesMainForm->addSeriesPoint(0, listPoint2);//ng竖线（red）
    P_seriesMainForm->addSeriesPoint(1, listPoint3);//ok竖线（green）

    P_seriesMainForm->setSeriesPoints(analysiser->get_histogram_ng_img(), 0);//ng数据
    P_seriesMainForm->setSeriesPoints(analysiser->get_histogram_ok_img(), 1);//ok数据

    P_seriesMainForm->addSeriesPoint(2, P_seriesMainForm->getSeriesPoints(0, 1));
    P_seriesMainForm->addSeriesPoint(3, P_seriesMainForm->getSeriesPoints(1, 1));

    P_seriesMainForm->setSeriesPoints(analysiser->get_histogram_ng_defect(), 2);
    P_seriesMainForm->setSeriesPoints(analysiser->get_histogram_ok_defect(), 3);
    P_seriesMainForm->addSeriesPoint(dynamic_cast<chartView*>(P_seriesMainForm->setCurrentView(1)), 2, P_seriesMainForm->getSeriesPoints(2, 1));
    P_seriesMainForm->addSeriesPoint(dynamic_cast<chartView*>(P_seriesMainForm->setCurrentView(1)), 3, P_seriesMainForm->getSeriesPoints(3, 1));
    //ui.editThreshold->setValue(ng_thr);
    int64 te = cv::getTickCount();
    std::cout << "staticticalAreaInit time: " << (te - ts) / cv::getTickFrequency() << " second, with pixelscore = " << P_controlBtnForm->geteditThresholdText().toFloat() << std::endl;
}

//训练曲线图
void imageShowForm::slotupData()
{
   //QString filePath = "D:/manuvision-platform/build/shoes0616/setting/train_log.csv";
    QMap<QString, QMap<int, qreal>> map = publicClass::instance()
        ->getAllPointsMap(FileProjectManager::Instance()->ProjectGetTrainLogAbsPath());
    // publicClass::instance()->addData();
    P_seriesMainForm->setXRange(publicClass::instance()->getNumLine());
    QStringList keysList;
    QMap<QString, QMap<int, qreal>>::iterator it;
    for (it = map.begin(); it != map.end(); it++) {
        if (it.key().contains("F1")) {
            keysList.append(it.key());
        }
    }
    P_seriesMainForm->setAreaIndexSeries(map, keysList, _colorMap);

}
QString imageShowForm::getedit_epochText()
{
    return P_parameterConfigForm->getedit_epochText();
}
void imageShowForm::setTimerStatus(bool checked)
{
    if (checked) {
        _timer->start(1000);
    }
    else
    {
        _timer->stop();
    }
}
void imageShowForm::setbtnText(const QString &btn, const QString &btnText)
{
    if (btn.simplified() == "btnTrain") {
        P_controlBtnForm->setbtnTrainText(btnText);
    }
    else if (btn.simplified() == "btnTest") {
        P_controlBtnForm->setbtnTestText(btnText);
    }
}
void imageShowForm::LabelRegionUpdateTrainFlag(int idx)
{
    //没有图片时，训练会崩溃，这里加一个保护
    if (idx < 0) {
        return;
    }
    ainno_image curimg = FileProjectManager::Instance()->StreamGetImageInfo(idx);
    P_imageForm->UpdateImageInfo(curimg);
    P_imageForm->ShowInfo();
}
void imageShowForm::btnGroupEnabled(int id, bool checked)
{
    P_controlBtnForm->btnGroupEnabled(id, checked);
}
int imageShowForm::getprogressBarValue()
{
    return P_controlBtnForm->getprogressBarValue();
}
int imageShowForm::getcom_keeptraindataCurrentIndex()
{
    return P_parameterConfigForm->getcom_keeptraindataCurrentIndex();
}
QString imageShowForm::getedit_train_ratioText()
{
    return P_parameterConfigForm->getedit_train_ratioText();
}
YAML::Node imageShowForm::getTestconfig()
{
    return P_parameterConfigForm->getTestconfig();
}
YAML::Node imageShowForm::getTrainconfig()
{
    return P_parameterConfigForm->getTrainconfig();
}
std::map<int, Sthreshold> imageShowForm::getSthreshold()
{
    return sthresholdMap;
}
//当前可见文件列表数据映射
void imageShowForm::setCurrentImageData(QMap <std::string, int> imageData, QMap<std::string, trained_flag> flag_map)
{
    P_trainerListProForm->setCurrentImageData(imageData, flag_map);
}
//当前可见文件列表数据
void imageShowForm::addFileListData(QMap <int, std::string> mapFileData)
{
    int w = P_trainerListProForm->width()-20;
    P_trainerListProForm->setColorMap(_colorMap, _maskgraycolormap);
    _mapFileData.clear();
    delTreeViewData();
    QMap <int, std::string> ::iterator it;
    int i = 0;
    for (it  = mapFileData.begin(); it != mapFileData.end(); it++)
    {
        i++;
        _mapFileData.insert(i-1, it.value());
    }
    if (_mapFileData.isEmpty()) {
        _pageSize = 0;
            _pageIndex = 0;
        //P_trainerListProForm->setPageText(QString("%1/%2 页").arg(_pageIndex).arg(_pageSize));
        emit sysStatusFrame::instance()->signalStatusStr(sysStatusFrame::PageNum, 
            QString("%1/%2 页").arg(_pageIndex).arg(_pageSize));
        sysStatusFrame::instance()->comboBox_pageAddItem(0);
        return;
    }
    //_mapFileData = mapFileData;
    show_image_file = _mapFileData.size();
    _pageSize = _mapFileData.size() / (getLoadMode() == true ? show_image_file : SHOW_NUM_IMAGE); 
    if (_mapFileData.size() % (getLoadMode() == true ? show_image_file : SHOW_NUM_IMAGE) != 0) {
        _pageSize = _pageSize + 1;
    }
    sysStatusFrame::instance()->comboBox_pageAddItem(_pageSize);//页数
    _pageIndex = 1;
    //P_trainerListProForm->setPageText(QString("%1/%2 页").arg(_pageIndex).arg(_pageSize));
    emit sysStatusFrame::instance()->signalStatusStr(sysStatusFrame::PageNum, QString("%1/%2 页").arg(_pageIndex).arg(_pageSize));
    sysStatusFrame::instance()->setCurrentIndex(_pageIndex);
    if (_mapFileData.size() < (getLoadMode() == true ? show_image_file : SHOW_NUM_IMAGE)) {
        addTreeViewData(_mapFileData, (_pageIndex - 1)*(getLoadMode() == true ? show_image_file : SHOW_NUM_IMAGE), _mapFileData.size(), false);
    }
    else
    {
        addTreeViewData(_mapFileData, (_pageIndex - 1)*(getLoadMode() == true ? show_image_file : SHOW_NUM_IMAGE), _pageIndex*(getLoadMode() == true ? show_image_file : SHOW_NUM_IMAGE), false);
    }
    emit signalOpenImage(QVariant(), 0);
}
//上一页
void imageShowForm::slotPreviousPage()
{
    if (_pageIndex == 0) {
        return;
    }
    if (_pageIndex > 1) {
        _pageIndex = _pageIndex - 1;
    }
    //if (!getLoadMode()) {
        delTreeViewData();
    //}
    //P_trainerListProForm->setPageText(QString("%1/%2 页").arg(_pageIndex).arg(_pageSize));
    emit sysStatusFrame::instance()->signalStatusStr(sysStatusFrame::PageNum, QString("%1/%2 页").arg(_pageIndex).arg(_pageSize));
    sysStatusFrame::instance()->setCurrentIndex(_pageIndex);
    addTreeViewData(_mapFileData, (_pageIndex - 1)*(getLoadMode()==true? show_image_file :SHOW_NUM_IMAGE), _pageIndex*(getLoadMode() == true ? show_image_file : SHOW_NUM_IMAGE), false);
    //P_trainerListProForm->setCurrentIndexNum(0);
    emit signalOpenImage(QVariant(),0);
}
//下一页
void imageShowForm::slotNextPage()
{
    if (_pageIndex >= _pageSize) {
        return;
    }
    if (_pageIndex < _pageSize) {
        _pageIndex = _pageIndex + 1;
    }
   // if (!getLoadMode()) {
        delTreeViewData();
   // }
    //P_trainerListProForm->setPageText(QString("%1/%2 页").arg(_pageIndex).arg(_pageSize));
    emit sysStatusFrame::instance()->signalStatusStr(sysStatusFrame::PageNum, QString("%1/%2 页").arg(_pageIndex).arg(_pageSize));
    sysStatusFrame::instance()->setCurrentIndex(_pageIndex);
    if (_pageIndex == _pageSize) {
        addTreeViewData(_mapFileData, (_pageIndex - 1)*(getLoadMode() == true ? show_image_file : SHOW_NUM_IMAGE),
            (_pageIndex-1)*(getLoadMode() == true ? show_image_file : SHOW_NUM_IMAGE) + (_mapFileData.size()- (_pageIndex - 1)*(getLoadMode() == true ? show_image_file : SHOW_NUM_IMAGE)), false);
    }
    else
    {
        addTreeViewData(_mapFileData, (_pageIndex - 1)*(getLoadMode() == true ? show_image_file : SHOW_NUM_IMAGE),
            _pageIndex*(getLoadMode() == true ? show_image_file : SHOW_NUM_IMAGE), false);
    }
    P_trainerListProForm->setCurrentIndexNum(0);
    emit signalOpenImage(QVariant(), 0);
}
void imageShowForm::slotPageIndex(const QString &indexText)
{
    int index = indexText.toInt();
    _pageIndex = index;
    delTreeViewData();
    // }
     //P_trainerListProForm->setPageText(QString("%1/%2 页").arg(_pageIndex).arg(_pageSize));
    emit sysStatusFrame::instance()->signalStatusStr(sysStatusFrame::PageNum, QString("%1/%2 页").arg(index).arg(_pageSize));
    if (index == _pageSize) {
        addTreeViewData(_mapFileData, (index - 1)*(getLoadMode() == true ? show_image_file : SHOW_NUM_IMAGE),
            (index - 1)*(getLoadMode() == true ? show_image_file : SHOW_NUM_IMAGE) +
            (_mapFileData.size() - (index - 1)*(getLoadMode() == true ? show_image_file : SHOW_NUM_IMAGE)), false);
    }
    else
    {
        addTreeViewData(_mapFileData, (index - 1)*(getLoadMode() == true ? show_image_file : SHOW_NUM_IMAGE),
            index*(getLoadMode() == true ? show_image_file : SHOW_NUM_IMAGE), false);
    }
    P_trainerListProForm->setCurrentIndexNum(0);
    emit signalOpenImage(QVariant(), 0);
}
int imageShowForm::getCurrentRowCount()
{
    return P_trainerListProForm->getCurrentRowCount();
}
void imageShowForm::setWidth(int w)
{

}
void imageShowForm::setHeight(int h)
{

}
void imageShowForm::setPageindex(int page, int index)
{
    _pageIndex = page;

}
QList<QByteArray> imageShowForm::getGrabPixMap()
{
    return P_seriesMainForm->getGrabPixMap();
}
void imageShowForm::slotChangeNgOk(QMap<int, QPointF> map)
{
    if (analysiser->run_normal())
    {
        //更新混淆矩阵
        for (int i = 0; i < map.size(); i++) {
            Sthreshold tmpthr;
            tmpthr.global_ng_thr = map.value(i).x();
            tmpthr.global_ok_thr = map.value(i).y();
            sthresholdMap[i] = tmpthr;
        }
        P_seriesMainForm->setDataMatrix(analysiser->get_confusion_matrix(sthresholdMap));

        //更新预测结果显示
        std::vector<float> tmp_ng_thrs;
        for (int i = 0; i < sthresholdMap.size(); i++) {
            tmp_ng_thrs.push_back(sthresholdMap[i].global_ng_thr);
        }
        DrawPredictEx(tmp_ng_thrs, P_controlBtnForm->getcomboBoxCurrentIndex());
        repaint();
    }
}