#include "trainer.h"
#include "qtmat.h"
#define TIME_OUT 1000
Trainer::Trainer(QWidget *parent)
: QWidget(parent)
{
    ui.setupUi(this);
    initUi();
}

Trainer::~Trainer()
{
    alg::deinitialize();
}
void * Trainer::GetWidgetPoint()
{
    return (void*)this;
}
void Trainer::initUi()
{
    alg::initialize();
   // QCoreApplication::processEvents();//强制刷新一下
    this->setFocusPolicy(Qt::StrongFocus);
    P_imageShowForm = new imageShowForm;//主显示界面
    ui.horizontalLayout_addStatusBar->addWidget(sysStatusFrame::instance());//状态栏
    ui.gridLayout_mainForm->addWidget(P_imageShowForm);
    connect(this, SIGNAL(signalRefrush()), this, SLOT(slotUpdataForm()), Qt::QueuedConnection);//刷新
    connect(this, SIGNAL(signalThreshold()), this, SLOT(on_btnThreshold_clicked()), Qt::QueuedConnection);
    connect(P_imageShowForm, SIGNAL(signalbtnThreshold()), this, SLOT(on_btnThreshold_clicked()));//点击“分析”曲线图分析数据
    connect(P_imageShowForm, SIGNAL(signalbtnTrain()), this, SLOT(on_btnTrain_clicked()));
    connect(P_imageShowForm, SIGNAL(signalbtnTest()), this, SLOT(on_btnTest_clicked()));
    connect(P_imageShowForm, SIGNAL(signalbtnConvert()), this, SLOT(on_btnConvert_clicked()));
    connect(P_imageShowForm, SIGNAL(signalSelectDisplay(int)), this, SLOT(OnFilterDisplay(int)));
    connect(P_imageShowForm, SIGNAL(signalOpenImage(QVariant, int)), this, SLOT(slotOpenImage(QVariant, int)));
    connect(this, SIGNAL(signalLabelStatus(int, bool)), this, SLOT(slotLabelSatus(int, bool)));
    connect(this, SIGNAL(signalPreviousPage()), P_imageShowForm, SLOT(slotPreviousPage()));
    connect(this, SIGNAL(signalNextPage()), P_imageShowForm, SLOT(slotNextPage()));
    P_imageShowForm->set_ismainshow(true);//是否是标注区域
    //P_imageShowForm->set_keepaspectratio(_isKeepaspectratio);//是否保持长宽比

    connect(P_imageShowForm->getImageForm(), SIGNAL(signalDelImage(bool)), this, SLOT(OnSignalDelImage(bool)));
    connect(P_imageShowForm->getImageForm(), SIGNAL(signalMenuOperate(bool, int, void*)), this, SLOT(OnSignalMenuOperate(bool, int, void*)));
    connect(P_imageShowForm->getbaseListWidget(), SIGNAL(signalDelImage(bool)), this, SLOT(OnSignalDelImage(bool)));
    connect(P_imageShowForm->getbaseListWidget(), SIGNAL(signalMenuOperate(bool, int, void*)), this, SLOT(OnSignalMenuOperate(bool, int, void*)));

    QList<QVariantList> list;
    UpdateColorMap(list);
    P_imageShowForm->setFormVisible(3, false);//隐藏曲线图区域
    P_imageShowForm->setFormVisible(2, false);//隐藏参数配置区域
    P_imageShowForm->setFormVisible(0, false);//文件列表
    P_imageShowForm->setFormVisible(1, false);//显示区
    //P_imageShowForm->setFormVisible(1, false);
    _btnDrog = new btnDrogForm(this);
    _btnDrog->setVisibleForm(false);
    _sysSettingform = new sysSettingForm();

    //隐藏系统配置界面
    connect(_sysSettingform, SIGNAL(signalFormChecked(bool)), this, SLOT(slotFormChecked(bool)));
    //切换按钮
    connect(_sysSettingform, SIGNAL(signalSwitchChacked(int, bool)), this, SLOT(slotSysSettingSwitchChacked(int, bool)));
    //画笔
    connect(_sysSettingform, SIGNAL(signalValuePen(int)), this, SLOT(slotSysPenValue(int)));
    //多类别与工具条
    connect(_sysSettingform, SIGNAL(signalMoreTypeData(QList<QVariantList>)), _btnDrog, SLOT(slotBtnDrogMoreType(QList<QVariantList>)));
    //更新多类别颜色表
    connect(_sysSettingform, SIGNAL(signalMoreTypeData(QList<QVariantList>)), this, SLOT(UpdateColorMap(QList<QVariantList>)));
    //文件导出格式
    connect(_sysSettingform, SIGNAL(signalFileExportMode(QString)), this, SLOT(slotFileExportMode(QString)));
    //文件加载方式，后期可能取消
    connect(_sysSettingform, SIGNAL(signalLoadMode(bool)), this, SLOT(slotFileloadMode(bool)));

    connect(_btnDrog, SIGNAL(signalBtnId(int)), this, SLOT(slotToolSet(int)));
    connect(_btnDrog, SIGNAL(signalValue(int)), this, SLOT(slotSetPenSize(int)));
    connect(_btnDrog, SIGNAL(signalLineColor(QColor)), this, SLOT(slotSetPenColor(QColor)));

    QMenuBar * menuBar = new QMenuBar;
    menuBar->setFixedHeight(30);
    QMenu *menuFile = menuBar->addMenu(tr("&文件"));
    QMenu *menuForm = menuBar->addMenu(tr("&窗口"));
    QMenu *menuHelp = menuBar->addMenu(tr("&帮助"));
    menuBar->setStyleSheet("QMenuBar{background-color:transparent;color:white;}");/*设置背景色，跟随背景色*/
     //   "QMenuBar::selected{background-color:transparent;}"/*设置菜单栏选中背景色*/
       // "QMenuBar::item{font-size:12px;font-family:Microsoft YaHei;color:rgba(255,255,255,1);}"/*设置菜单栏字体为白色，透明度为1（取值范围0.0-255）*/
    actionOpen = new QAction(tr("&打开工程"));
    //actionOpen->setShortcut(QKeySequence::Open);

    actionNew = new QAction(tr("&新建工程"));
    // actionNew->setShortcuts(QKeySequence::New);

    actionAdd = new QAction(tr("&添加图片"));
    //actionAdd->setStatusTip(tr("添加一个新的文件"));

    actionSave = new QAction(tr("&保存工程"));
    // actionSave->setShortcuts(QKeySequence::Save);

    actionSet = new QAction(tr("&添加标注"));
    //actionSet->setShortcuts(QKeySequence("ctrl+e"));

    actionAddOther = new QAction(tr("&添加OK图"));
    // actionAddOther->setShortcuts(QKeySequence::Save);
    actionCallout = new QAction(tr("&参数配置"));
    actionChart = new QAction(tr("&统计分析"));
    actionTool = new QAction(tr("&标注工具"));
    actionSysSetting = new QAction(tr("&系统设置"));
    // actionAddOther->setShortcuts(QKeySequence::Save);
    actionSysSetting->setEnabled(false);
    actionDoc = new QAction(tr("&文档"));//文档
    actionExportTrainerReport = new QAction(tr("&导出训练报告"));
    actionjishuzhichi = new QAction(tr("&技术支持"));//技术支持
    actionLog = new QAction(tr("&日志"));//日志
    actionguanyu = new QAction(tr("&关于"));//关于
    connect(actionOpen, SIGNAL(triggered()), this, SLOT(slotActionOpenPro())); //New点击事件调用test (打开工程列表)   
    connect(sysStatusFrame::instance()->getProListP(), SIGNAL(signalOpenPro()), this, SLOT(on_btnOpen_clicked()));//打开新工程，不存在记录里
    connect(sysStatusFrame::instance()->getProListP(), SIGNAL(signalProPath(QString &)), this, SLOT(slotOpenPro(QString &)));
    connect(actionNew, SIGNAL(triggered()), this, SLOT(on_btnNew_clicked())); //新建
    connect(actionAdd, SIGNAL(triggered()), this, SLOT(on_btnAdd_clicked())); //增加
    connect(actionSave, SIGNAL(triggered()), this, SLOT(on_btnSave_clicked())); //保存
    connect(actionSet, SIGNAL(triggered()), this, SLOT(on_btnLoadLabel_clicked())); //“加载标注”按钮处理函数
    connect(actionAddOther, SIGNAL(triggered()), this, SLOT(on_btnAddOkImage_clicked()));//“增加无缺陷图”按钮处理函数
    connect(actionCallout, SIGNAL(triggered()), this, SLOT(slotActionCalloutShow())); //参数配置
    connect(actionChart, SIGNAL(triggered()), this, SLOT(slotActionChartShow())); //图表区
    connect(actionTool, SIGNAL(triggered()), this, SLOT(slotActionToolShow()));
    connect(actionSysSetting, SIGNAL(triggered()), this, SLOT(slotActionSysSetting()));
    //帮助
    connect(actionDoc, SIGNAL(triggered()), this, SLOT(slotActionDoc())); //文档
    connect(actionExportTrainerReport, SIGNAL(triggered()), this, SLOT(slotActionTrainerReport()));  //导出训练报告 
    connect(actionguanyu, SIGNAL(triggered()), this, SLOT(slotActionguanyu())); //关于
    connect(actionjishuzhichi, SIGNAL(triggered()), this, SLOT(slotActionjishuzhichi()));//技术支持
    connect(actionLog, SIGNAL(triggered()), this, SLOT(slotActionLog()));//日志

    menuFile->addAction(actionNew);
    menuFile->addSeparator();
    menuFile->addAction(actionOpen);
    menuFile->addSeparator();
    menuFile->addAction(actionSave);
    menuFile->addSeparator();
    menuFile->addAction(actionAdd);
    menuFile->addSeparator();
    menuFile->addAction(actionSet);
    menuFile->addSeparator();
    menuFile->addAction(actionAddOther);
    menuFile->addSeparator();
    menuForm->addAction(actionCallout);
    actionCallout->setCheckable(true);
    actionCallout->setChecked(false);
    menuForm->addSeparator();
    menuForm->addAction(actionChart);
    actionChart->setCheckable(true);
    actionChart->setChecked(false);
    menuForm->addSeparator();
    menuForm->addAction(actionTool);
    menuForm->addSeparator();
    menuForm->addAction(actionSysSetting);
    actionSysSetting->setCheckable(true);
    actionSysSetting->setChecked(false);
    actionTool->setVisible(false);
    menuHelp->addAction(actionDoc);//文档
    menuHelp->addSeparator();
    menuHelp->addAction(actionExportTrainerReport);//导出训练报告
    menuHelp->addSeparator();
    menuHelp->addAction(actionjishuzhichi);//技术支持
    menuHelp->addSeparator();
    menuHelp->addAction(actionLog);//日志
    menuHelp->addSeparator();
    menuHelp->addAction(actionguanyu);//关于
    // actionTool->setCheckable(true);
     //actionTool->setChecked(false);
    ui.horizontalLayout_menuBar->addWidget(menuBar);
}
/*工程操作*/
//“新建”按钮处理函数
void Trainer::on_btnNew_clicked()
{
    if (!FileProjectManager::Instance()->FileOperation(FileOperationType::NEW)) {
        QApplication::restoreOverrideCursor();
        return;
    }
    QString proName = FileProjectManager::Instance()->ProjectGetProjectAbsPath().simplified();
    slotOpenPro(proName);
    QApplication::restoreOverrideCursor();
    CiWaMsgDialog::MessageSignal("操作", "工程新建成功！");//提示框
    return;
}
void Trainer::slotOpenPro(QString &proPath)
{
    _isOpenNewPro = false;
    _proPath = proPath;
    on_btnOpen_clicked();
    _isOpenNewPro = true;
    _proPath = "";
}
//“打开”按钮处理函数(重构模块选择同一种加载方式)
void Trainer::on_btnOpen_clicked()
{

    if (!FileProjectManager::Instance()->FileOperation(FileOperationType::OPEN, -1, _isOpenNewPro, _proPath)) {
        QApplication::restoreOverrideCursor();
        return;
    }
    initIniSysSetting();
    //获取项目绝对路径
    QString proName = FileProjectManager::Instance()->ProjectGetProjectAbsPath().simplified();
    emit sysStatusFrame::instance()->signalStatusStr(sysStatusFrame::Loading, QString("加载中..."));
    int last = proName.lastIndexOf("/");
    emit sysStatusFrame::instance()->signalStatusStr(sysStatusFrame::Pro, proName.right(proName.length() - last - 1));
    sysStatusFrame::instance()->setProIni(proName.right(proName.length() - last - 1), proName);
    QList<QVariantList> list;
    UpdateColorMap(list);
    P_imageShowForm->imageListClear();
    _mapImageNamePath.clear();
    _mapImageStruct.clear();
    _mapFileName.clear();
    _mapFileNameShow.clear();
    _mapTableFile.clear();
    _flag_map.clear();
    P_imageShowForm->LabelRegionClear();
    FileListAddFileName();//加载图片数据
    P_imageShowForm->setCurrentIndexNum(0);
    //统计清除
    P_imageShowForm->statisticalAreaDataClear();
    update();
    //加载配置文件
    P_imageShowForm->TrainCfgLoad();
    P_imageShowForm->TrainCfgSetRootPath();
    P_imageShowForm->TestCfgLoad();
    P_imageShowForm->TestCfgSetRootPath();
    _btnDrog->setVisibleForm(true);
    // P_imageShowForm->setFormVisible(2, true);//隐藏参数配置区域
    P_imageShowForm->setFormVisible(0, true);//文件列表
    P_imageShowForm->setFormVisible(1, true);//显示区
   // emit signalThreshold();//加载预测结果
    emit signalRefrush();
    CiWaMsgDialog::MessageSignal("操作", "工程打开成功！");
    QApplication::restoreOverrideCursor();
    sysStatusFrame::instance()->close();
    emit sysStatusFrame::instance()->signalStatusStr(sysStatusFrame::Loading, QString(""));
    return;

}
//“增加”按钮处理函数
void Trainer::on_btnAdd_clicked()
{
    //添加图像
    if (!FileProjectManager::Instance()->FileOperation(FileOperationType::ADDIMAGE)) {
        QApplication::restoreOverrideCursor();
        return;
    }
    emit sysStatusFrame::instance()->signalStatusStr(sysStatusFrame::Loading, "正在添加图片...");
    QCoreApplication::processEvents();
    FileListAddFileName();
    emit sysStatusFrame::instance()->signalStatusStr(sysStatusFrame::Loading, "");
    CiWaMsgDialog::MessageSignal("操作", "添加图片成功！");
    QApplication::restoreOverrideCursor();
    return;
}
//“保存”按钮处理函数
void Trainer::on_btnSave_clicked()
{
    //保存
    if (!FileProjectManager::Instance()->FileOperation(FileOperationType::SAVE)) {
        QApplication::restoreOverrideCursor();
        return;
    }
    P_imageShowForm->setCurrentIndexNum(_mapTableFile.value(_currentListFileName.toStdString()));
    CiWaMsgDialog::MessageSignal("操作", "工程保存成功！");
    QApplication::restoreOverrideCursor();
    return;
}
//“加载标注”按钮处理函数
void Trainer::on_btnLoadLabel_clicked()
{
    //保存
    if (!FileProjectManager::Instance()->FileOperation(FileOperationType::LOADLABEL)) {
        QApplication::restoreOverrideCursor();
        return;
    }
    QString dataStr = P_imageShowForm->getCurrentData(_indexImageFileName).toString().simplified();
    LabelRegionLabeldefects(_mapTableFile.value(dataStr.toStdString()));
    update();

    CiWaMsgDialog::MessageSignal("操作", "加载标注成功！");
    QApplication::restoreOverrideCursor();
    return;
}

//“增加无缺陷图”按钮处理函数
void Trainer::on_btnAddOkImage_clicked()
{
    //添加图像
    if (!FileProjectManager::Instance()->FileOperation(FileOperationType::ADDOKIMAGE)) {
        QApplication::restoreOverrideCursor();
        return;
    }
    
     FileListAddFileName();
    CiWaMsgDialog::MessageSignal("操作", "添加图片成功！");

    QApplication::restoreOverrideCursor();
    return;
}
void Trainer::slotActionCalloutShow()
{
    if (_mapImageStruct.size() <= 0) {
        CiWaMsgDialog::MessageSignal("提示:", "工程未打开");
        actionCallout->setChecked(false);
        return;
    }
    if (P_imageShowForm->frameIsVisible(2)) {//参数配置区域
        P_imageShowForm->setFormVisible(2, false);//隐藏参数配置区域
        //ui.scrollArea->update();
        emit signalRefrush();
        return;
    }
    P_imageShowForm->setFormVisible(2, true);//参数配置
    actionCallout->setChecked(true);
    emit signalRefrush();
    //ui.parameterConfigFrame->setStyleSheet("background-color:rgb(111, 111, 111);");
}
void Trainer::slotActionOpenPro()
{
    //工程列表界面
    //_proListForm->showForm();
    on_btnOpen_clicked();
}
void Trainer::initIniSysSetting()
{

    //FileProjectManager::Instance()->ProjectGetProjectAbsPath().isEmpty();
    publicClass::instance()->iniSystemConfigFile(FileProjectManager::Instance()->ProjectGetProjectAbsPath().simplified());
    actionSysSetting->setEnabled(true);
    _isFillChecked = publicClass::instance()->readIniFile("switchBtnFill").toBool();//是否填充
    _isKeepaspectratio = publicClass::instance()->readIniFile("switchBtnInproportion").toBool();//是否保持原比例
    _penSizeValue = publicClass::instance()->readIniFile("spinBoxPenSize").toInt();
    export_mode_ = std::string(publicClass::instance()->readIniFile("fileExportMode").toString().toLocal8Bit());//文件导出模式
    _btnDrog->setVisibleForm(true);
}
//接受线程中弹出对话框失败
void Trainer::TrainerInitFail(QString shead, QString msg)
{
    CiWaMsgDialog::MessageT(shead, msg);
}
//文件名加载方式，
void Trainer::FileListAddFileName()
{
    slotFileloadMode(_fileLoadType);
    int startidx = _mapImageStruct.size();
    //int w = ui.listWidget->width() - 20;
    int totalimagenum = FileProjectManager::Instance()->StreamGetImageNum();//获取图片总数
    //设置进度条值
    P_imageShowForm->setprogressBarMaxValue(totalimagenum - startidx);
    //设置当前进度值
    P_imageShowForm->setprogressBarValue(0);
    for (int i = startidx; i < totalimagenum; i++)
    {
        ainno_image curimg = FileProjectManager::Instance()->StreamGetImageInfo(i);
        if (curimg.labeled) {
            if (curimg.labeltype == 0) {
                _flag_map.insert(curimg.filename, trained_flag::good);
            }
            else if(curimg.labeltype == 1)
            {
                _flag_map.insert(curimg.filename, trained_flag::bad);
            }
        }
        else
        {
            _flag_map.insert(curimg.filename, trained_flag::all);
        }
        QString imageabspath = FileProjectManager::Instance()->ProjectGetImageAbsPath();
        _mapImageNamePath.insert(QString(curimg.filename.c_str()), imageabspath);
        _mapImageStruct.insert(QString(curimg.filename.c_str()), curimg);
        // _listFileName.append(curimg.filename);
        _mapFileName.insert(i, curimg.filename);//全部文件名与索引
        _mapTableFile.insert(curimg.filename, i);//文件名与索引映射
    }
    _mapFileNameShow = _mapFileName;
    QTime timeL;
    timeL.start();
    //更新列表统计
    FileListFilter(P_imageShowForm->getSelectDisplayIndex());
    qDebug() << timeL.elapsed() / 1000.0 << "更新列表统计";
    QTime time;
    time.start();
    //P_imageShowForm->addTreeViewData(_mapFileName);
    //如果之前没有图片，默认点击第一个图片
    if (startidx == 0 && _mapImageStruct.size() > 0) {
        if (!P_imageShowForm->OpenImage(_mapImageNamePath.value(QString(_mapFileName.value(startidx).c_str())), _mapImageStruct.value(QString(_mapFileName.value(startidx).c_str())))) {
            return;
        }
        _indexImageFileName = startidx;
        LabelRegionUpdata(startidx);
    }
   // QObject::connect(P_imageShowForm->getWidget_ImageForm(), SIGNAL(signalIndexValue(QVariant, int)), this, SLOT(slotOpenImage(QVariant, int)));
    QObject::connect(P_imageShowForm->getbaseListWidget(), SIGNAL(signalIndexValue(QVariant, int)), this, SLOT(slotOpenImage(QVariant, int)));
    qDebug() << time.elapsed() / 1000.0 << "添加图片列表时间";
}
//系统设置
void Trainer::slotFormChecked(bool checked)
{
    if (_sysSettingform->isVisible()) {
        actionSysSetting->setChecked(true);
    }
    else
    {
        actionSysSetting->setChecked(false);
    }
}
//系统配置界面switch
void Trainer::slotSysSettingSwitchChacked(int index, bool checked)
{

    switch (index)
    {
    case 1: { //标注显示是否填充   
        if (_isFillChecked != checked) {
            _isFillChecked = checked;
            // P_imageShowForm->isFill(_isFillChecked);
        }

    }
            break;
    case 2: { //是否保持按照原比例
        if (_isKeepaspectratio != checked) {
            _isKeepaspectratio = checked;
            // P_imageShowForm->set_keepaspectratio(_isKeepaspectratio);
        }

    }
            break;
    case 3: { //是否保存测试结果
        if (checked) {
            FileProjectManager::Instance()->SetTestResultFlag(true);
        }
        else {
            FileProjectManager::Instance()->SetTestResultFlag(false);
        }
    }
            break;
    case 4: { //增加图片时是否修改图片名
        if (checked) {

        }
    }
            break;
    default:
        break;
    }
}
//标注显示画笔宽度
void Trainer::slotSysPenValue(int value)
{
    _penSizeValue = value;
}
//更新类别颜色映射表
void Trainer::UpdateColorMap(QList<QVariantList> list) {
    int moreTypeNum = publicClass::instance()->readIniFile("moreTypeNum").toUInt();
    colormap_.clear();
    maskgraycolormap_.clear();
    namemap_.clear();
    if (moreTypeNum <= 0) {
        return;
    }
    int step = 255 / moreTypeNum;
    for (int i = 0; i < moreTypeNum; i++) {
        QVariantList list;
        auto labelinfo = publicClass::instance()->readIniFile(QString::number(i + 1)).toString().split(":");
        QString name = labelinfo.at(0);
        QString color = labelinfo.at(1);
        qreal r = color.split(",").at(0).toInt();
        qreal g = color.split(",").at(1).toInt();
        qreal b = color.split(",").at(2).toInt();
        colormap_[i + 1] = QColor(r, g, b);
        namemap_[i + 1] = name;
        maskgraycolormap_[i + 1] = QColor(step*(i + 1), step*(i + 1), step*(i + 1));
    }

    FileProjectManager::Instance()->ProjectSetClassNum(colormap_.size() + 1);
    TrainCfgSetClassAndLabel();//更新类别信息
    LabelRegionUpdateColorMap();//更新类别颜色表
    FileListUpdateColorMap();//更新类别颜色表
}
//更新类别信息
bool Trainer::TrainCfgSetClassAndLabel()
{
    QString yamlfile = FileProjectManager::Instance()->ProjectGetTrainCfgAbsPath();
    QFile file(yamlfile);
    if (!file.exists())
    {
        //不存在
        return false;
    }
    std::string fs = yamlfile.toLocal8Bit();
    YAML::Node config = YAML::LoadFile(fs);
    QString data;
    QStringList datalist;

    YAML::Node tmpnode = config["DATASETS"]["CLASSES"];
    if (tmpnode.size() <= (namemap_.size())) {
        for (int i = tmpnode.size(); i <= namemap_.size(); i++) {
            config["DATASETS"]["CLASSES"][i] = std::string(namemap_[i].toLocal8Bit());
            config["DATASETS"]["LABELS"][i] = i;
            config["LOSS"]["CrossEntropyLoss"]["WEIGHT"][i] = 1;
            config["VAL"]["METRIC"]["THRESHOLD"][i] = 0.1;
        }
    }

    std::ofstream fout(fs);
    fout << config;
    fout.close();
    return true;
}
//更新类别颜色表
void Trainer::LabelRegionUpdateColorMap()
{

    P_imageShowForm->UpdateColorMap(colormap_);
    P_imageShowForm->UpdateMaskGrayColorMap(maskgraycolormap_);
    /* if (_indexImageFileName >= 0) {
         QString dataStr = P_imageShowForm->getCurrentData(_indexImageFileName).toString().simplified();
         LabelRegionLabeldefects(_mapTableFile.value(dataStr.toStdString()));
     }
     LabelRegionLabeldefects(0);*/
}
void Trainer::LabelRegionLabeldefects(int idx)
{
    //没有图片时，加载标注会奔溃，这里加一个保护
    if (idx < 0) {
        return;
    }
    ainno_image curimg = FileProjectManager::Instance()->StreamGetImageInfo(idx);
    P_imageShowForm->UpdateImageInfo(curimg);
    P_imageShowForm->setLineSizeWidth(_penSizeValue);
    P_imageShowForm->isFill(_isFillChecked);
    P_imageShowForm->set_keepaspectratio(_isKeepaspectratio);
    P_imageShowForm->DrawLabel();
    P_imageShowForm->ResizeWidetSize(P_imageShowForm->getScrolWidth() - 2, P_imageShowForm->getScrolHeight() - 2);

}
void Trainer::FileListUpdateColorMap()
{
    if (_fileLoadType) {

    }
    else
    {
        /* for (int i = 0; i < imagelist_.size(); i++) {
             imagelist_[i]->UpdateColorMap(colormap_);
             imagelist_[i]->UpdateMaskGrayColorMap(maskgraycolormap_);
         }
         FileListUpdateLabeldefects();
         */
    }
}
//更新标注信息
void Trainer::FileListUpdateLabeldefects()
{
    /* ainno_image curimg;
     int w = ui.listWidget->width() - 20;
     for (int i = 0; i < imagelist_.size(); i++) {
         curimg = FileProjectManager::Instance()->StreamGetImageInfo(i);
         imagelist_[i]->UpdateImageInfo(curimg);
         imagelist_[i]->setLineSizeWidth(_penSizeValue);//设置线宽
         imagelist_[i]->isFill(_isFillChecked);
         imagelist_[i]->set_keepaspectratio(_isKeepaspectratio);
         imagelist_[i]->DrawLabel();
         imagelist_[i]->ResizeWidetSize(w, w);
         FileListFilterCount(i);
     }*/
}
//文件导出模式
void Trainer::slotFileExportMode(QString exportMode)
{
    export_mode_ = std::string(exportMode.toLocal8Bit());
}
//文件加载模式
void Trainer::slotFileloadMode(bool checked)
{
    _fileLoadType = checked;
}
//拖拽条响应
void Trainer::slotToolSet(int id)
{
    switch (id)
    {
    case 1: {
        //选择
        P_imageShowForm->SetPenType(PenType::COUNT_);
        P_imageShowForm->setPropertyObj("cursor", QVariant(QCursor(Qt::ArrowCursor)));
    }
            break;
    case 2: {
        //on_btnDrawLine_clicked();//画笔
        QPixmap pixmap(tr("%1/icons/penCusor.png").arg(publicClass::instance()->getAppPath()));
        QPixmap scaledPixmap = pixmap.scaled(_btnDrog->getCursorSize() + 2, _btnDrog->getCursorSize() + 2, Qt::KeepAspectRatio);
        QCursor myCursor(scaledPixmap, -1, -1);    //-1,-1表示热点位于图片中心
        P_imageShowForm->SetPenType(PenType::PEN);
        P_imageShowForm->SetPenSize(_valueSize);
        P_imageShowForm->setPropertyObj("cursor", QVariant(myCursor));
        _selectId = 0;
    }
            break;
    case 3: {
        QPixmap pixmap(tr("%1/icons/penCusor.png").arg(publicClass::instance()->getAppPath()));
        QPixmap scaledPixmap = pixmap.scaled(_btnDrog->getCursorSize() + 2, _btnDrog->getCursorSize() + 2, Qt::KeepAspectRatio);
        QCursor myCursor(scaledPixmap, -1, -1);    //-1,-1表示热点位于图片中心
        P_imageShowForm->SetPenType(PenType::ERAS);
        P_imageShowForm->SetPenSize(_valueSize);
        P_imageShowForm->setPropertyObj("cursor", QVariant(myCursor));
        _selectId = 1;
    }
            break;
    case 4: {
        //清除
        P_imageShowForm->ClearLabel();
    }
            break;
    case 5: {
        //应用
        MenuApply();
        on_btnSave_clicked();
    }
            break;

    default:
        break;
    }
}
//“应用”按钮处理函数，处理右键“无缺陷”以及缺陷标注
void Trainer::MenuApply()
{
    //还没有选中图片
    if (_indexImageFileName == -1) {
        return;
    }
    //更新标注界面
    labelinfo retlabelinfo = LabelRegionApply();
    //更新工程结构体
    QString dataStr = P_imageShowForm->getCurrentData(_indexImageFileName).toString().simplified();
    FileProjectManager::Instance()->StreamSetImageLabelInfo(_mapTableFile.value(dataStr.toStdString()), retlabelinfo);
    //更新过滤列表
    //FileListFilterCount(curitemidx_);
    //更新图片列表
    FileListUpdataImage();
   // P_imageShowForm->getCurrentRowCount();
    //更新统计信息
    FileListFilter(P_imageShowForm->getSelectDisplayIndex());
}
//文件过滤
void Trainer::FileListFilter(int index)
{
   // QMap<int, std::string> mapFileName = _mapFileName;
    _mapFileNameShow.clear();
    _mapTableFile.clear();
    //_flag_map.clear();
    switch (index)
    {
        //all
    case 0: {
        _mapFileNameShow = _mapFileName;
        for (size_t i = 0; i < _mapFileName.size(); i++)
        {
            _mapTableFile.insert(_mapFileName.value(i), i);
            //_flag_map.insert(_mapFileName.value(i), trained_flag::all);
        }
    }
            break;
    case 1: {
        //显示trained
        for (int i = 0; i < _mapFileName.size(); i++) {
            bool trained = FileProjectManager::Instance()->StreamGetTrainedFlag(i);
            if (trained) {
                _mapFileNameShow.insert(i, _mapFileName.value(i));
                _mapTableFile.insert(_mapFileName.value(i), i);
               // _flag_map.insert(_mapFileName.value(i), trained_flag::trained);
            }
        }
    }
            break;
    case 2: {
        //显示not trained
        for (int i = 0; i < _mapFileName.size(); i++) {
            bool trained = FileProjectManager::Instance()->StreamGetTrainedFlag(i);
            if (!trained) {
                _mapFileNameShow.insert(i, _mapFileName.value(i));
                _mapTableFile.insert(_mapFileName.value(i), i);
              //  _flag_map.insert(_mapFileName.value(i), trained_flag::not_trained);
            }
        }
        break;
    }
    case 3: {
        //显示good
        for (int i = 0; i < _mapFileName.size(); i++) {
            bool isgood = FileProjectManager::Instance()->StreamGetGoodFlag(i);
            if (isgood) {
                _mapFileNameShow.insert(i, _mapFileName.value(i));
                _mapTableFile.insert(_mapFileName.value(i), i);
             //   _flag_map.insert(_mapFileName.value(i), trained_flag::good);
            }
        }
        break;
    }
    case 4: {
        //显示bad
        for (int i = 0; i < _mapFileName.size(); i++) {
            bool isbad = FileProjectManager::Instance()->StreamGetBadFlag(i);
            if (isbad) {
                _mapFileNameShow.insert(i, _mapFileName.value(i));
                _mapTableFile.insert(_mapFileName.value(i), i);
              //  _flag_map.insert(_mapFileName.value(i), trained_flag::bad);
            }
        }
        break;
    }
    case 5: {
        //显示labeled
        for (int i = 0; i < _mapFileName.size(); i++) {
            bool islabeled = FileProjectManager::Instance()->StreamGetLabeledFlag(i);
            if (islabeled) {
                _mapFileNameShow.insert(i, _mapFileName.value(i));
                _mapTableFile.insert(_mapFileName.value(i), i);
               // _flag_map.insert(_mapFileName.value(i), trained_flag::labeled);
            }
        }
        break;
    }
    case 6: {
        //显示not labeled
        for (int i = 0; i < _mapFileName.size(); i++) {
            bool islabeled = FileProjectManager::Instance()->StreamGetLabeledFlag(i);
            if (!islabeled) {
                _mapFileNameShow.insert(i, _mapFileName.value(i));
                _mapTableFile.insert(_mapFileName.value(i), i);
              //  _flag_map.insert(_mapFileName.value(i), trained_flag::not_labeled);
            }
        }
        break;
    }
    default:
        break;
    }
    P_imageShowForm->delTreeViewData();
    P_imageShowForm->setLcdFileNumber((double)_mapTableFile.size());
    if (_mapTableFile.size() <= 0) {
        P_imageShowForm->delTreeViewData();
        return;
    }
    P_imageShowForm->setCurrentImageData(_mapTableFile, _flag_map);
    P_imageShowForm->addFileListData(_mapFileNameShow);
}
labelinfo Trainer::LabelRegionApply() {
    labelinfo retlabelinfo;
    //1.判断有无缺陷
    if (P_imageShowForm->IsNoDefect()) {
        P_imageShowForm->SetLabelType(LabelRectFlag::GREEN);
        retlabelinfo.labeled = true;
        retlabelinfo.labeltype = AINNO_LABEL_GOOD;
    }
    else {
        std::vector<cv::Vec4i> hierarchy;
        hierarchy.clear();
        retlabelinfo.labeldefects.clear();
        cv::Mat emptyimg = CVS::QPixmapToCvMat(P_imageShowForm->get_maskimage());
        //cv::imwrite("emptyimg_bgr.png", emptyimg);
        cv::cvtColor(emptyimg, emptyimg, cv::COLOR_RGB2GRAY);
        //cv::imwrite("emptyimg_gray.png", emptyimg);
        //cv::findContours(emptyimg, retlabelinfo.labeldefects, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
        //hierarchy 没有用
        FindContoursByClass(emptyimg, retlabelinfo.labeldefects, hierarchy);
        //cv::Mat drawimg = cv::Mat::zeros(emptyimg.rows, emptyimg.cols, CV_8UC1);
        //cv::drawContours(drawimg, retlabelinfo.labeldefects, -1, cv::Scalar(255, 255, 255), 2);
        //cv::imwrite("draw_contours.png", drawimg);
        //2如果有缺陷
        if (retlabelinfo.labeldefects.size() > 0)
        {
            QImage plabelimg = P_imageShowForm->get_labelimage().toImage();
            cv::Moments moment;
            cv::Mat defect;
            cv::RotatedRect minRect;
            ainno_contour_info datainfo;
            retlabelinfo.labeldefectsinfo.clear();
            for (int k = 0; k < retlabelinfo.labeldefects.size(); k++) {
                //类型
                QColor labelcolor = plabelimg.pixelColor(retlabelinfo.labeldefects[k][0].x, retlabelinfo.labeldefects[k][0].y);
                for (auto it = colormap_.begin(); it != colormap_.end(); it++) {
                    if (it->second == labelcolor) {
                        datainfo.label = it->first;
                        break;
                    }
                }

                //面积
                datainfo.area = cv::contourArea(retlabelinfo.labeldefects[k]);

                //中心
                moment = cv::moments(retlabelinfo.labeldefects[k]);
                datainfo.centerx = int(moment.m10 / moment.m00);
                datainfo.centery = int(moment.m01 / moment.m00);

                //长短边
                defect = cv::Mat(retlabelinfo.labeldefects[k]);
                minRect = cv::minAreaRect(defect);
                if (minRect.size.width < minRect.size.height) {
                    datainfo.longsize = minRect.size.height;
                    datainfo.shortsize = minRect.size.width;
                }
                else {
                    datainfo.longsize = minRect.size.width;
                    datainfo.shortsize = minRect.size.height;
                }
                retlabelinfo.labeldefectsinfo.push_back(datainfo);
            }
            P_imageShowForm->SetLabelType(LabelRectFlag::RED);
            P_imageShowForm->ResizePredictdefects(P_imageShowForm->getImageWidth(),
                P_imageShowForm->getImageHeight());
            retlabelinfo.labeled = true;
            retlabelinfo.labeltype = AINNO_LABEL_BAD;
        }
        else {
            P_imageShowForm->SetLabelType(LabelRectFlag::NONE_);
            retlabelinfo.labeled = false;
        }
    }

    return retlabelinfo;
}
void Trainer::FileListUpdataImage()
{
    QString imageabspath = FileProjectManager::Instance()->ProjectGetImageAbsPath();
    QString dataStr = P_imageShowForm->getCurrentData(_indexImageFileName).toString().simplified();
    ainno_image curimg = FileProjectManager::Instance()->StreamGetImageInfo(_mapTableFile.value(dataStr.toStdString()));

}
void Trainer::FindContoursByClass(cv::Mat maskimg, std::vector<std::vector<cv::Point>> &labeldefects, std::vector<cv::Vec4i> hierarchy)
{
    for (int i_classids = 0; i_classids < maskgraycolormap_.size(); i_classids++)
    {
        int graycolor = maskgraycolormap_[i_classids + 1].red();
        std::vector<std::vector<cv::Point>> tmpcontours;
        cv::Mat mask_img_class;
        cv::threshold(maskimg, mask_img_class, graycolor - 1, 0, cv::THRESH_TOZERO);
        cv::threshold(mask_img_class, mask_img_class, graycolor, 0, cv::THRESH_TOZERO_INV);
        cv::findContours(mask_img_class, tmpcontours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
        for (int i_tmpcontours = 0; i_tmpcontours < tmpcontours.size(); i_tmpcontours++)
        {
            labeldefects.push_back(tmpcontours[i_tmpcontours]);
        }
    }
}
// 画笔大小滑块处理函数
void Trainer::slotSetPenSize(int size)
{
    _valueSize = size;
    P_imageShowForm->SetPenSize(size);
    if (_btnDrog->getBtnChecked(0) || _btnDrog->getBtnChecked(1)) {
        QPixmap pixmap(tr("%1/icons/penCusor.png").arg(publicClass::instance()->getAppPath()));
        QPixmap scaledPixmap = pixmap.scaled(_btnDrog->getCursorSize() + 2, _btnDrog->getCursorSize() + 2, Qt::KeepAspectRatio);
        QCursor myCursor(scaledPixmap, -1, -1);    //-1,-1表示热点位于图片中心
        if (_selectId == 0) {
            P_imageShowForm->SetPenType(PenType::PEN);
        }
        else if (_selectId == 1)
        {
            P_imageShowForm->SetPenType(PenType::ERAS);
        }
        else
        {
            P_imageShowForm->SetPenType(PenType::COUNT_);
        }
        P_imageShowForm->SetPenSize(_valueSize);
        P_imageShowForm->setPropertyObj("cursor", QVariant(myCursor));
    }
}
//设置画笔颜色
void Trainer::slotSetPenColor(QColor color)
{
    P_imageShowForm->SetPenColor(color);

    //根据colormap_ 找到maskgraycolormap_
    for (int i = 0; i < colormap_.size(); i++)
    {
        if (colormap_[i] == color)
        {
            P_imageShowForm->SetMaskPenColor(maskgraycolormap_[i]);
        }
    }
    if (_selectId == 0) {
        P_imageShowForm->SetPenType(PenType::PEN);
    }
    else if (_selectId == 1)
    {
        P_imageShowForm->SetPenType(PenType::ERAS);
    }
    else
    {
        P_imageShowForm->SetPenType(PenType::COUNT_);
    }

}
void Trainer::slotOpenImage(QVariant data, int row)
{
    if (row == -1) {
        return;
    }
    //QString dataStr = data.toString().simplified();

    _indexImageFileName = row; //_mapTableFile.value(dataStr.toStdString()); 
    QString dataStr = P_imageShowForm->getCurrentData(_indexImageFileName).toString().simplified();
    LabelRegionUpdata(_mapTableFile.value(dataStr.toStdString()));
    P_imageShowForm->setCurrentIndexNum(_indexImageFileName);
    _currentListFileName = dataStr;

}
// 右键“删除”按钮处理函数
void Trainer::OnSignalDelImage(bool ismain)
{
    int index = _indexImageFileName;
    if (!ismain) {

        QStringList listRows = P_imageShowForm->getAllSelectedRows();
        QVariantList dataStr = P_imageShowForm->getCurrentData(listRows);
        if (!dataStr.isEmpty()) {
            for (size_t i = 0; i < dataStr.size(); i++)
            {
                FileProjectManager::Instance()->ProjectDeleteImage(_mapTableFile.value(dataStr.at(i)
                    .toString().toStdString()));
                _mapImageNamePath.clear();
                _mapImageStruct.clear();
                _mapFileName.clear();
                _mapFileNameShow.clear();
                _mapTableFile.clear();
                _flag_map.clear();
                //ui.widget_imageList->delTreeViewData();
               
            }
            /* if (!ui.image->Open(_mapImageNamePath.value(QString(_mapFileName.value(0).c_str())), _mapImageStruct.value(QString(_mapFileName.value(0).c_str())))) {
                    return;
                }*/
            if (_mapTableFile.size() > 0) {

                _indexImageFileName = 0;
                LabelRegionUpdata(0);
            }
        }
        updateFileListAddFileName();
    }
    else
    {
        if (_indexImageFileName != -1) {

            QString dataStr = P_imageShowForm->getCurrentData(_indexImageFileName).toString().simplified();
            FileProjectManager::Instance()->ProjectDeleteImage(_mapTableFile.value(dataStr.toStdString()));
        }
        _mapImageNamePath.clear();
        _mapImageStruct.clear();
        _mapFileName.clear();
        _mapFileNameShow.clear();
        _mapTableFile.clear();
        _flag_map.clear();
        P_imageShowForm->delTreeViewData();
        FileListAddFileName();
    }
    //保存到数据文件
    FileProjectManager::Instance()->ProjectSave();
    //如果没有图片，则清除标注区域
    if (_mapFileName.isEmpty()) {
        P_imageShowForm->LabelRegionClear();
    }
    QString dataStr = P_imageShowForm->getCurrentData(index).toString().simplified();
    index = _mapTableFile.value(dataStr.toStdString());
    //删除后更新统计信息
   // FileListFilter(P_imageShowForm->getSelectDisplayIndex());
    // P_imageShowForm->setCurrentIndexNum(index);
    LabelRegionUpdata(index);
}
void Trainer::updateFileListAddFileName()
{
    slotFileloadMode(_fileLoadType);
    int startidx = _mapImageStruct.size();
    //int w = ui.listWidget->width() - 20;
    int totalimagenum = FileProjectManager::Instance()->StreamGetImageNum();//获取图片总数
    //设置进度条值
    P_imageShowForm->setprogressBarMaxValue(totalimagenum - startidx);
    //设置当前进度值
    P_imageShowForm->setprogressBarValue(0);
    QTime time;
    time.start();
    for (int i = startidx; i < totalimagenum; i++)
    {
        ainno_image curimg = FileProjectManager::Instance()->StreamGetImageInfo(i);
        if (curimg.labeled) {
            if (curimg.labeltype == 0) {
                _flag_map.insert(curimg.filename, trained_flag::good);
            }
            else if (curimg.labeltype == 1)
            {
                _flag_map.insert(curimg.filename, trained_flag::bad);
            }
        }
        else
        {
            _flag_map.insert(curimg.filename, trained_flag::all);
        }
        QString imageabspath = FileProjectManager::Instance()->ProjectGetImageAbsPath();
        _mapImageNamePath.insert(QString(curimg.filename.c_str()), imageabspath);
        _mapImageStruct.insert(QString(curimg.filename.c_str()), curimg);
        // _listFileName.append(curimg.filename);
        _mapFileName.insert(i, curimg.filename);//全部文件名与索引
        _mapTableFile.insert(curimg.filename, i);//文件名与索引映射
    }
    _mapFileNameShow = _mapFileName;
    //qDebug() << time.elapsed() / 1000.0 << "s";


    //更新列表统计
    FileListFilter(P_imageShowForm->getSelectDisplayIndex());
    // P_imageShowForm->addTreeViewData(_mapFileName);
     //如果之前没有图片，默认点击第一个图片
    /* if (startidx == 0 && _mapImageStruct.size() > 0) {
         if (!ui.image->Open(_mapImageNamePath.value(QString(_mapFileName.value(startidx).c_str())), _mapImageStruct.value(QString(_mapFileName.value(startidx).c_str())))) {
             return;
         }
         _indexImageFileName = startidx;
         //LabelRegionUpdata(startidx);
     }*/
    QObject::connect(P_imageShowForm->getbaseListWidget()/*getWidget_ImageForm()*/, SIGNAL(signalIndexValue(QVariant, int)), this, SLOT(slotOpenImage(QVariant, int)));
}
// 右键“菜单”勾选按钮处理函数
void Trainer::OnSignalMenuOperate(bool is, int op, void* imagelabel)
{
    //文件加载
    ImageLabel* item = Q_NULLPTR;
    if (imagelabel) {
        item = (ImageLabel*)imagelabel;
    }
    switch (op)//当前类型
    {
    case  MenuOperateT::O_NOLABEL_T:
        //设置无缺陷
        if (is) {
            //主界面
            MenuApply();
        }
        else {
            //列表 (列表也包括主界面更新主界面)

            QStringList listRows = P_imageShowForm->getAllSelectedRows();
            for (int i = 0; i < listRows.size(); i++) {
                ListMenuApply(listRows.at(i).toInt(), NULL);
            }
            //更新统计信息
            FileListFilter(P_imageShowForm->getSelectDisplayIndex());

        }
        break;
    case MenuOperateT::O_CLOSE_T: //关闭工具条
    {
        _btnDrog->setVisibleForm(false);

    }
    break;
    case MenuOperateT::O_OPEN_T: //打开工具条
    {

        QDesktopWidget *deskdop = QApplication::desktop();
        _btnDrog->move((deskdop->width() - _btnDrog->width() * 11), (deskdop->height() - _btnDrog->height() - 50));
        _btnDrog->setVisibleForm(true);
    }
    break;
    case MenuOperateT::O_AUTO_T:   //自动
    {
        ImageMenu imagemenu;
        if (is) {
            //主界面只改一个
                //列表改菜单值
            //ImageLabel* imagelabel = (ImageLabel*)curitem_;
            //imagemenu = ui.image->get_imagemenu();
            //ui.image->UpdateMenuTrainCheck((MenuOperate)op, imagemenu);
            //更新结构体
            QString dataStr = P_imageShowForm->getCurrentData(_indexImageFileName).toString().simplified();
            FileProjectManager::Instance()->StreamSetImageTrainpolicy(_mapTableFile.value(dataStr.toStdString()), ainno_train_policy::AINNO_TRAIN_AUTO);
        }
        else
        {

            QStringList listRows = P_imageShowForm->getAllSelectedRows();
            for (int i = 0; i < listRows.size(); i++) {

                FileProjectManager::Instance()->StreamSetImageTrainpolicy(listRows.at(i).toInt(), ainno_train_policy::AINNO_TRAIN_AUTO);
            }
            //主界面也更新                
        }
        P_imageShowForm->UpdateMenuTrainCheck((MenuOperate)op, imagemenu);
    }
    break;
    case MenuOperateT::O_NEVER_T:  //从不
    {
        ImageMenu imagemenu;
        if (is) {
            //主界面只改一个
                //列表改菜单值
            //ImageLabel* imagelabel = (ImageLabel*)curitem_;
            //imagemenu = ui.image->get_imagemenu();
            //ui.image->UpdateMenuTrainCheck((MenuOperate)op, imagemenu);
            //更新结构体
            QString dataStr = P_imageShowForm->getCurrentData(_indexImageFileName).toString().simplified();
            FileProjectManager::Instance()->StreamSetImageTrainpolicy(_mapTableFile.value(dataStr.toStdString()), ainno_train_policy::AINNO_TRAIN_NEVER);
        }
        else
        {

            QStringList listRows = P_imageShowForm->getAllSelectedRows();
            for (int i = 0; i < listRows.size(); i++) {

                FileProjectManager::Instance()->StreamSetImageTrainpolicy(listRows.at(i).toInt(), ainno_train_policy::AINNO_TRAIN_NEVER);
            }
            //主界面也更新                
        }
        P_imageShowForm->UpdateMenuTrainCheck((MenuOperate)op, imagemenu);
    }
    break;
    case MenuOperateT::O_ALWAYS_T: //总是
    {

        ImageMenu imagemenu;
        if (is) {
            //主界面只改一个
                //列表改菜单值
            //ImageLabel* imagelabel = (ImageLabel*)curitem_;
            //imagemenu = ui.image->get_imagemenu();
            //ui.image->UpdateMenuTrainCheck((MenuOperate)op, imagemenu);
            //更新结构体
            QString dataStr = P_imageShowForm->getCurrentData(_indexImageFileName).toString().simplified();
            FileProjectManager::Instance()->StreamSetImageTrainpolicy(_mapTableFile.value(dataStr.toStdString()), ainno_train_policy::AINNO_TRAIN_ALWAYS);
        }
        else
        {

            QStringList listRows = P_imageShowForm->getAllSelectedRows();
            for (int i = 0; i < listRows.size(); i++) {

                FileProjectManager::Instance()->StreamSetImageTrainpolicy(listRows.at(i).toInt(), ainno_train_policy::AINNO_TRAIN_ALWAYS);
            }
            //主界面也更新                
        }
        P_imageShowForm->UpdateMenuTrainCheck((MenuOperate)op, imagemenu);
    }
    break;
    case MenuOperateT::O_TEST_T: //测试
        on_menutest_clicked(is);
        break;
    }

}
//“应用”按钮处理函数，批量处理右键“无缺陷”以及缺陷标注
void Trainer::ListMenuApply(int index, ImageLabel* imagelabel)
{
    labelinfo retlabelinfo;
    retlabelinfo.labeled = true;
    retlabelinfo.labeltype = AINNO_LABEL_GOOD;
    //更新工程结构体
    FileProjectManager::Instance()->StreamSetImageLabelInfo(index, retlabelinfo);
    //更新图片列表
    _mapImageNamePath.clear();
    _mapImageStruct.clear();
    _mapFileName.clear();
    _mapFileNameShow.clear();
    _mapTableFile.clear();
    _flag_map.clear();
    FileListAddFileName();
}
void Trainer::slotActionSysSetting()
{
    //系统设置界面
    _sysSettingform->show();
}
void Trainer::on_btnThreshold_clicked()
{
    if (_mapImageStruct.size() <= 0) {
        CiWaMsgDialog::MessageSignal("提示:", "工程未打开");
        actionChart->setChecked(false);
        return;
    }
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    int threshold = int(P_imageShowForm->geteditThresholdValue() * 255);
    QTime time;
    time.start();
    FileProjectManager::Instance()->LoadPredictResult(threshold);
    //FileListUpdatePredictdefects();
    actionChart->setChecked(true);
    P_imageShowForm->setFormVisible(3, true);//曲线图
    P_imageShowForm->statisticalAreaInit();
    qDebug() << time.elapsed() / 1000 << "曲线图加载时间";
    emit signalRefrush();
    QApplication::restoreOverrideCursor();

}
void Trainer::slotUpdataForm()
{
    if (_indexImageFileName >= 0) {
        LabelRegionUpdata(_indexImageFileName);
    }
   // emit sysStatusFrame::instance()->signalStatusStr(sysStatusFrame::Loading, QString(""));
}
     
//训练进度条结束 (slot)
void Trainer::OnReadTrainCsvFinished()
{
    P_imageShowForm->setFormVisible(3, true);
    slotOpenImage(P_imageShowForm->getCurrentData(_indexImageFileName), _indexImageFileName);
    P_imageShowForm->setprogressBarValue(P_imageShowForm->getedit_epochText().toInt());
    bistrainopen_ = false;
    P_imageShowForm->setbtnText("btnTrain", "训练");
    UpdataTrainFlag(true);
    //_timer->stop();
    P_imageShowForm->setTimerStatus(false);
    P_imageShowForm->btnGroupEnabled(0, true);
    update();
}
//测试进度条结束(slot)
void Trainer::OnReadTestCsvFinished()
{
    int progressval = P_imageShowForm->getprogressBarValue();
    P_imageShowForm->setprogressBarValue(_mapFileNameShow.size());
    // ui.progressBar->setValue(imagelist_.size());
    bistestopen_ = false;
    P_imageShowForm->setbtnText("btnTest", "测试");
    P_imageShowForm->btnGroupEnabled(1, true);
    if ((progressval == _mapFileName.size())
        || (progressval == testindexes_.size())) {
        FileProjectManager::Instance()->LoadPredictImage(testindexes_);
        int threshold = int(P_imageShowForm->geteditThresholdValue() * 255);
        FileProjectManager::Instance()->LoadPredictResult(threshold);
        LabelRegionUpdata(_indexImageFileName);
    }
}
//训练
void Trainer::on_btnTrain_clicked()
{
    if (_mapImageStruct.size() <= 0) {
        CiWaMsgDialog::MessageSignal("提示:", "工程未打开");
        actionChart->setChecked(false);
        return;
    }
    actionChart->setChecked(true);
    P_imageShowForm->setFormVisible(3, true);
    slotOpenImage(P_imageShowForm->getCurrentData(_indexImageFileName), _indexImageFileName);

    if (!bistrainopen_)
    {
        P_imageShowForm->btnGroupEnabled(0, false);
        bool bkeeptraindata = bool(P_imageShowForm->getcom_keeptraindataCurrentIndex());
        float trainratio = P_imageShowForm->getedit_train_ratioText().toFloat();
        if (FileProjectManager::Instance()->PrepareTrainData(trainratio, bkeeptraindata)) {
            UpdataTrainFlag(false);
            alg::train();
            // _timer->start(TIME_OUT);
            P_imageShowForm->setTimerStatus(true);
            //停止
            bistrainopen_ = true;
            P_imageShowForm->setbtnText("btnTrain", "停止");
            //初始化并启动进度条线程
            //必须放在alg::train()之后，因为alg::train里面回置标志位，在下面启动的线程中查询
            ProcessInit(FileProjectManager::Instance()->ProjectGetTrainLogAbsPath(),
                P_imageShowForm->getedit_epochText().toInt(), 2);
            update();
        }
    }
    else
    {
        alg::stop();
    }
}
//测试
void Trainer::on_btnTest_clicked()
{
    testindexes_.clear();
    if (_mapImageStruct.size() <= 0) {
        CiWaMsgDialog::MessageSignal("提示:", "工程未打开");
        actionChart->setChecked(false);
        return;
    }
    actionChart->setChecked(true);
    if (!bistestopen_) {
        P_imageShowForm->btnGroupEnabled(1, false);
        if (FileProjectManager::Instance()->PrepareTestData(testindexes_)) {
            bistestopen_ = true;
            P_imageShowForm->setbtnText("btnTest", "停止");
            alg::test();
            //初始化并启动进度条线程
            ProcessInit(FileProjectManager::Instance()->ProjectGetTestLogAbsPath(),
                _mapImageStruct.size(), 1);
        }
		else {
			actionChart->setChecked(false);
		}
    }
    else {
        alg::stop();
    }
}
//导出
void Trainer::on_btnConvert_clicked()
{
    QString modelabspath = QFileDialog::getExistingDirectory(
        nullptr, QString("Open"), "", QFileDialog::ShowDirsOnly);

    //点击取消
    if (modelabspath.isEmpty()) {
        return;
    }
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if (FileProjectManager::Instance()->PrepareConvertData()) {
        int convertmode;
        if (export_mode_ == "onnx")
        {
            convertmode = 0;
        }
        else
        {
            convertmode = 1;
        }
        alg::convert(convertmode);
        while (alg::getstatus()) {
            _sleep(1);
        }

        //bool flag = serialize_encrypt(export_mode_);
        //if (true == flag)
        //{
        //    _errcode = "model convert successed, model type: " + export_mode_;
        //}
        //else
        //{
        //    _errcode = "model convert failed, model type: " + export_mode_;
        //}

        // ConvertPostprocess(modelabspath);
        std::cout << "------------------------" << _errcode << "------------------------" << std::endl;
        CiWaMsgDialog::MessageSignal("INFO:ALG", QString::fromLocal8Bit(_errcode.c_str()));
    }
    QApplication::restoreOverrideCursor();
}
void Trainer::UpdataTrainFlag(bool trained)
{
    FileProjectManager::Instance()->UpdateTrainFlag(trained);
    QString dataStr = P_imageShowForm->getCurrentData(_indexImageFileName).toString().simplified();
    P_imageShowForm->LabelRegionUpdateTrainFlag(_mapTableFile.value(dataStr.toStdString()));
    FileListFilter(P_imageShowForm->getSelectDisplayIndex());
}
//进度条对象释放
void Trainer::ProcessRelease()
{
    if (workerthread_ != NULL) {
        workerthread_->destroyed();
        workerthread_ = NULL;
    }
    if (trainprogress_ != NULL) {
        QObject::disconnect(this, &Trainer::DoWorkSignal, trainprogress_, &TrainProgress::DoWork);
        QObject::disconnect(trainprogress_, &TrainProgress::ResultReady, this, &Trainer::OnSignalResultReady);	//接受进度
        QObject::disconnect(trainprogress_, &TrainProgress::ReadFinish, this, &Trainer::OnReadTestCsvFinished);//进度条结束
        QObject::disconnect(trainprogress_, &TrainProgress::ReadFinish, this, &Trainer::OnReadTrainCsvFinished);	//进度条结束
        delete trainprogress_;
        trainprogress_ = NULL;
    }
}

//初始化并启动进度条线程 1:test ,2:train
void Trainer::ProcessInit(QString& csv, int len, int processtype)
{
    ProcessRelease();
    P_imageShowForm->setprogressBarMaxValue(len);
    QThread* workerthread = new QThread();
    TrainProgress *trainprogress = new TrainProgress();
    workerthread_ = workerthread;
    trainprogress_ = trainprogress;

    trainprogress->moveToThread(workerthread);
    QObject::connect(this, &Trainer::DoWorkSignal, trainprogress, &TrainProgress::DoWork);
    QObject::connect(trainprogress, &TrainProgress::ResultReady, this, &Trainer::OnSignalResultReady);	//接受进度
    if (processtype == 1) {
        QObject::connect(trainprogress, &TrainProgress::ReadFinish, this, &Trainer::OnReadTestCsvFinished);//进度条结束
    }
    else {
        QObject::connect(trainprogress, &TrainProgress::ReadFinish, this, &Trainer::OnReadTrainCsvFinished);//进度条结束
    }
    workerthread->start();
    emit DoWorkSignal(csv);

}
//接受进度条信号
void Trainer::OnSignalResultReady(int n)
{
    if (n < 0) {
        P_imageShowForm->setprogressBarValue(P_imageShowForm->getprogressBarValue() + 1);
    }
    else {
        P_imageShowForm->setprogressBarValue(n);
    }
}
//测试
void Trainer::on_menutest_clicked(bool ismain)
{
    testindexes_.clear();
    if (ismain) {
        QString dataStr = P_imageShowForm->getCurrentData(_indexImageFileName).toString().simplified();
        testindexes_.push_back(_mapTableFile.value(dataStr.toStdString()));
    }
    else
    {

        QStringList listRows = P_imageShowForm->getAllSelectedRows();
        for (int i = 0; i < listRows.size(); i++) {

            testindexes_.push_back(listRows.at(i).toInt());
        }
    }

    if (_mapImageStruct.size() <= 0) {
        CiWaMsgDialog::MessageSignal("提示:", "工程未打开");
        actionChart->setChecked(false);
        return;
    }
    actionChart->setChecked(true);
    if (!bistestopen_) {
        P_imageShowForm->btnGroupEnabled(1, false);
        if (FileProjectManager::Instance()->PrepareTestData(testindexes_)) {
            bistestopen_ = true;
            P_imageShowForm->setbtnText("btnTest", "停止");
            alg::test();
            //初始化并启动进度条线程
            ProcessInit(FileProjectManager::Instance()->ProjectGetTestLogAbsPath(),
                _mapImageStruct.size(), 1);
        }
    }
    else {
        alg::stop();
    }
}
//曲线区
void Trainer::slotActionChartShow()
{
    if (_mapImageStruct.size() <= 0) {
        CiWaMsgDialog::MessageSignal("提示:", "工程未打开");
        actionChart->setChecked(false);
        return;
    }
    if (P_imageShowForm->frameIsVisible(3)) {
        P_imageShowForm->setFormVisible(3, false);
        emit signalRefrush();
        return;
    }
    emit signalRefrush();
    P_imageShowForm->setFormVisible(3, true);
}
//工具条
void Trainer::slotActionToolShow()
{
    if (_mapImageStruct.size() <= 0) {
        CiWaMsgDialog::MessageSignal("提示:", "工程未打开");
        return;
    }
    if (_btnDrog->isVisible()) {
        _btnDrog->setVisibleForm(false);
        return;
    }
    _btnDrog->setVisibleForm(true);
}
bool Trainer::serialize_encrypt(std::string model_type)
{
    QString model_dir = QString("%1/models").arg(FileProjectManager::Instance()->ProjectGetTrainAbsPath());
    QString onnx_model = QString("%1/F1_best_model.onnx").arg(model_dir);
    QString tensorrt_model = QString("%1/F1_best_model.tensorrt").arg(model_dir);
    QString libtorch_model = QString("%1/F1_best_model.libtorch").arg(model_dir);
    QString xml_path = QString("%1/serialize.xml").arg("Model");

    if (model_type == "onnx")
    {
        if (!(QFile(onnx_model).exists()))
        {
            _errcode = "onnx model path not exists: " + onnx_model.toStdString();
            CiWaMsgDialog::MessageSignal("ERROR:PATH", QString::fromLocal8Bit(_errcode.c_str()));
            return false;
        }

        //序列化
        Sserializeparam sparam;
        sparam.onnxmodel = onnx_model.toStdString();
        sparam.trtengine = tensorrt_model.toStdString();
        sparam.batchSize = 1;
        sparam.dlaCore = -1;
        sparam.int8 = false;
        sparam.fp16 = true;
        sparam.workspaceSize = (size_t)2 << 30;
        SRunInfo serializeInfo = { "serialize", (void*)&sparam };

        typedef void *HANDLE;
        typedef struct
        {
            std::vector<cv::Mat> result_imgs;
            std::vector<bool> isok;
            std::vector<float> score_per_class;
            void *private_data;
        }SResult;

        typedef HANDLE(_cdecl *LPINIT)(std::string algName, std::string configFileName, int gpu);
        typedef void*(_cdecl *LPRUN)(HANDLE handle, SRunInfo *info);
        typedef void(_cdecl *LPUNINIT)(HANDLE handle);
        typedef std::string(_cdecl *LPGETLASTERR)(HANDLE handle);

        LPINIT 			m_lpInit;
        LPRUN		    m_lpRun;
        LPUNINIT		m_lpUnInit;
        LPGETLASTERR	m_lpGetLastErr;

        HINSTANCE hmod = LoadLibrary(_TEXT("manuvision.dll"));
        if (NULL == hmod)
        {
            std::cout << "ERROR: Load manuvision.dll fail" << std::endl;
        }

        m_lpInit = (LPINIT)GetProcAddress(hmod, "Init");
        m_lpRun = (LPRUN)GetProcAddress(hmod, "Run");
        m_lpUnInit = (LPUNINIT)GetProcAddress(hmod, "UnInit");
        m_lpGetLastErr = (LPGETLASTERR)GetProcAddress(hmod, "GetLastErr");

        if (NULL == m_lpInit || NULL == m_lpRun || NULL == m_lpUnInit) {
            std::cout << "ERROR: 初始化算法失败, GetProcAddress失败！" << std::endl;
            return false;
        }

        HANDLE handle = m_lpInit("Serialize", xml_path.toStdString(), 0);
        if (NULL == handle)
        {
            std::cout << "ERROR: m_lpInit fail, handle==NULL" << std::endl;
            return false;
        }
        std::string errcode = m_lpGetLastErr(handle);
        if (errcode != "")
        {
            std::cout << "ERROR: m_lpInit fail, " << errcode << std::endl;
            return false;
        }

        bool to_trt_flag = m_lpRun(handle, &serializeInfo);
        errcode = m_lpGetLastErr(handle);
        if (errcode != "")
        {
            std::cout << "ERROR: m_lpRun fail, " << errcode << std::endl;
            return false;
        }

        m_lpUnInit(handle);

        //加密
        std::string encrypt_path = model_dir.toStdString() + "/F1_best_model.encrypt.tensorrt";
        if (!(QFile(QString::fromStdString(sparam.trtengine)).exists()))
        {
            errcode = "tensorrt model path not exists: " + sparam.trtengine;
            CiWaMsgDialog::MessageSignal("ERROR:PATH", QString::fromLocal8Bit(errcode.c_str()));
            return false;
        }
        VimEncrypt2FileRc4(const_cast<char*>(sparam.trtengine.c_str()), "xxx", const_cast<char*>(encrypt_path.c_str()), "0123456789ainnovation");
    }
    else if (model_type == "tensorrt")
    {
        if (!(QFile(tensorrt_model).exists()))
        {
            _errcode = "tensorrt model path not exists: " + tensorrt_model.toStdString();
            CiWaMsgDialog::MessageSignal("ERROR:PATH", QString::fromLocal8Bit(_errcode.c_str()));
            return false;
        }

        //加密
        std::string encrypt_path = model_dir.toStdString() + "/F1_best_model.encrypt.tensorrt";
        VimEncrypt2FileRc4(const_cast<char*>(tensorrt_model.toStdString().c_str()), "xxx", const_cast<char*>(encrypt_path.c_str()), "0123456789ainnovation");
    }
    else if (model_type == "libtorch")
    {
        if (!(QFile(libtorch_model).exists()))
        {
            _errcode = "libtorch model path not exists: " + libtorch_model.toStdString();
            CiWaMsgDialog::MessageSignal("ERROR:PATH", QString::fromLocal8Bit(_errcode.c_str()));
            return false;
        }

        //加密
        std::string encrypt_path = model_dir.toStdString() + "/F1_best_model.encrypt.libtorch";
        VimEncrypt2FileRc4(const_cast<char*>(libtorch_model.toStdString().c_str()), "xxx", const_cast<char*>(encrypt_path.c_str()), "0123456789ainnovation");
    }
    else
    {
        _errcode = "unsupport model type[" + model_type + "], support onnx / tensorrt / libtorch.";
        CiWaMsgDialog::MessageSignal("ERROR:PATH", QString::fromLocal8Bit(_errcode.c_str()));
        return false;
    }

    return true;
}
void Trainer::ConvertPostprocess(QString modelabspath) {
    QString modelname;
    if (export_mode_ == "onnx") {
        modelname = "F1_best_model.encrypt.tensorrt";
    }
    else {
        modelname = "F1_best_model.encrypt.libtorch";
    }
    QString srcmodelpath = QString("%1/models/%2")
        .arg(FileProjectManager::Instance()->ProjectGetTrainAbsPath()).arg(modelname);
    QString dstmodelpath = QString("%1/%2").arg(modelabspath).arg(modelname);
    if (QFile::exists(dstmodelpath)) {
        QFile::remove(dstmodelpath);
    }
    if (!QFile::copy(srcmodelpath, dstmodelpath)) {
        CiWaMsgDialog::MessageSignal("工程", "模型拷贝失败！");
        return;
    }

    //TODO:此部分独立为一个函数；上边不进行模型拷贝；配置文件和模型均使用工程目录下的
    QString xmlabspath = QString("%1/Normalstation.xml").arg(modelabspath);
    if (QFile::exists(xmlabspath)) {
        boost::property_tree::xml_writer_settings<std::string> settings =
            boost::property_tree::xml_writer_make_settings<std::string>('\t', 1);
        boost::property_tree::ptree ptSer;
        boost::property_tree::xml_parser::read_xml(xmlabspath.toStdString(),
            ptSer, boost::property_tree::xml_parser::trim_whitespace);
        auto model = ptSer.get_child_optional("cfg.model");
        if (model) {
            model->put_value<std::string>(std::string(dstmodelpath.toLocal8Bit()));
        }
        auto scores = ptSer.get_child_optional("cfg.scores");
        if (scores) {
            QString score = QString::number(0.5, 'f', 2);
            for (int i = 1; i < colormap_.size() + 1; i++) {
                score += ",";
                score += QString::number((P_imageShowForm->getSthreshold())[i].global_ng_thr, 'f', 2);
            }
            scores->put_value<std::string>(std::string(score.toLocal8Bit()));
        }
        auto mode = ptSer.get_child_optional("cfg.mode");
        if (mode) {
            std::string strmode = (P_imageShowForm->getTestconfig())["TEST"]["IN_SIZE"]["MODE"].as<std::string>();
            if (strmode == "crop") {
                mode->put_value<std::string>(std::to_string(1));
            }
            else if (strmode == "resize") {
                mode->put_value<std::string>(std::to_string(2));
            }
        }
        boost::property_tree::xml_parser::write_xml(xmlabspath.toStdString(),
            ptSer, std::locale(), settings);
    }
}
/*文件列表操作*/
// 过滤下拉菜单处理函数
void Trainer::OnFilterDisplay(int index)
{
    FileListFilter(index);
}
//键盘事件
void Trainer::keyPressEvent(QKeyEvent * ev)
{
    /*if (ev->key() == Qt::Key_Control) {
        ispressctrl_ = true;
        return;
    }*/
   if (ev->key() == Qt::Key_Shift) {
        //ispressshift_ = true;
        P_imageShowForm->setShiftStatus(true);
        return;
    }
    if ((ev->modifiers() == Qt::ControlModifier) && (ev->key() == Qt::Key_Space)){
         QMessageBox::aboutQt(NULL, "aboutQt");
        _isCtrl = !_isCtrl;
        emit signalLabelStatus(1, _isCtrl);
    }
    
}

void Trainer::keyReleaseEvent(QKeyEvent * ev)
   {
    /*if (ev->key() == Qt::Key_Control) {
        ispressctrl_ = false;
        //on_btnHide_clicked();
        return;
    }*/
    if (ev->key() == Qt::Key_Shift) {
        //ispressshift_ = false;
        P_imageShowForm->setShiftStatus(false);
        return;
    }
    if (ev->key() == Qt::Key_Down) {

        if (_indexImageFileName + 1 >= P_imageShowForm->getCurrentRowCount()/*_mapFileNameShow.size()*/) {
            return;
        }
        _indexImageFileName = _indexImageFileName + 1;
        slotOpenImage("", _indexImageFileName);
        return;

    }
    if (ev->key() == Qt::Key_Up) {

        if (_indexImageFileName - 1 < 0) {
            return;
        }
        _indexImageFileName = _indexImageFileName - 1;
        slotOpenImage(P_imageShowForm->getCurrentData(_indexImageFileName), _indexImageFileName);
        return;

    }
    if (ev->key() == Qt::Key_Left) {

        emit signalPreviousPage();
        return;
    }
    if (ev->key() == Qt::Key_Right) {
        emit signalNextPage();
        return;
    }
    if (ev->key() == Qt::Key_Space)
    {
        if (ev->modifiers() == Qt::ControlModifier) {
            _isCtrl = !_isCtrl;
            emit signalLabelStatus(1, _isCtrl);
            return;
        }
        _isSpace = !_isSpace;
        emit signalLabelStatus(0, _isSpace);
        return;
    }
    /*if ((ev->modifiers() == Qt::ControlModifier) && (ev->key() == Qt::Key_Space)) {
        QMessageBox::aboutQt(NULL, "aboutQt");
        _isCtrl = !_isCtrl;
        emit signalLabelStatus(1, _isCtrl);
    }*/
    P_imageShowForm->setFocusPolicy(Qt::StrongFocus);
}
void Trainer::openUrl(QString url)
{

}
void Trainer::LabelRegionUpdata(int idx)
{
    QString imageabspath = FileProjectManager::Instance()->ProjectGetImageAbsPath();
    ainno_image curimg = FileProjectManager::Instance()->StreamGetImageInfo(idx);
    P_imageShowForm->ClearMenu();
    P_imageShowForm->setLineSizeWidth(_penSizeValue);
    P_imageShowForm->isFill(_isFillChecked);
    P_imageShowForm->set_keepaspectratio(_isKeepaspectratio);

    P_imageShowForm->Open(imageabspath, curimg);
    P_imageShowForm->ResizeWidetSize(P_imageShowForm->getScrolWidth() - 2, P_imageShowForm->getScrolHeight() - 2);
    //ui.image->DrawPredict();
    P_imageShowForm->ResizePredictdefects(P_imageShowForm->getScrolWidth() - 2, P_imageShowForm->getScrolHeight() - 2);

}
// 空格按键处理函数，切换预测结果显示/隐藏,
void Trainer::on_btnHideP_clicked()
{
    P_imageShowForm->ShowPredict();
}
/*标注操作*/
// 空格按键处理函数，切换标注显示/隐藏,
void Trainer::on_btnHide_clicked()
{
    P_imageShowForm->ShowLabel();
}
void Trainer::slotLabelSatus(int id, bool checked)
{
    switch (id)
    {
    case 0: {
        on_btnHide_clicked();
    }
            break;
    case 1: {
        on_btnHideP_clicked();
    }
            break;
    default:
        break;
    }
}
//导出训练报告
void Trainer::slotActionTrainerReport()
{
    if (_mapImageStruct.size() <= 0)
    {
        CiWaMsgDialog::MessageSignal("提示:", "工程未打开");
        return;
    }
    QList<QByteArray> listByteData =P_imageShowForm->getGrabPixMap();
    publicClass::instance()->saveHtmlFile(listByteData, FileProjectManager::Instance()->ProjectGetImageAbsPath());
    /* proNameListForm * form = new proNameListForm;
     form->show();*/

}