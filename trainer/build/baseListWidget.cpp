#include "baseListWidget.h"

baseListWidget::baseListWidget(QWidget *parent)
    : QListWidget(parent)
{
    ui.setupUi(this);
    initUi();

}

baseListWidget::~baseListWidget()
{

}
void baseListWidget::initUi()
{
    connect(this, SIGNAL(clicked(const QModelIndex &)), this, SLOT(slotListWidgetClick(const QModelIndex &)));
    this->setSelectionMode(QAbstractItemView::ContiguousSelection);  //
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    //和该策略对应的信号，showListWidgetMenuSlot为自己定义的槽函数.
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(slotTreeMenu(const QPoint&)));
}
void baseListWidget::addListWidget(QMap<int, std::string> map, const int indexStart, const int indexEnd, bool isOldFlag)
{
   int w = this->width() - 20;
   clearListWidget();
    for (int i = indexStart; i<indexEnd; i++)
    {
        
        //ainno_image curimg = FileProjectManager::Instance()->StreamGetImageInfo(0);
        //QString imageabspath = FileProjectManager::Instance()->ProjectGetImageAbsPath();
        QListWidgetItem* pListWidgetItem = new QListWidgetItem();     
        this->addItem(pListWidgetItem);
        //qDebug() << publicClass::instance()->readIniFile("switchBtnFileLoadMode").toBool() << "KK";
        pListWidgetItem->setToolTip(QString(map.value(i).c_str()));     
        if (_flagMap.value(map.value(i).c_str()) == 0) {
            pListWidgetItem->setIcon(QIcon(tr("%1/icons/emity.png").arg(publicClass::instance()->getAppPath())));
        }
        else if (_flagMap.value(map.value(i).c_str()) == 3)
        {
            pListWidgetItem->setIcon(QIcon(tr("%1/icons/good.png").arg(publicClass::instance()->getAppPath())));
        }
        else if (_flagMap.value(map.value(i).c_str()) == 4)
        {
            pListWidgetItem->setIcon(QIcon(tr("%1/icons/bad.png").arg(publicClass::instance()->getAppPath())));
        }
        if (publicClass::instance()->readIniFile("switchBtnFileLoadMode").toBool()) {
            pListWidgetItem->setText(QString(map.value(i).c_str()));
        }
        else
        {
            pListWidgetItem->setSizeHint(QSize(w, w-100));
            ImageLabel* tempImage = new ImageLabel(false, 0);
            _mapLabel.insert(tempImage, pListWidgetItem);
            tempImage->resize(w - 20, w / 2);
            tempImage->UpdateColorMap(_colorMap);
            tempImage->UpdateMaskGrayColorMap(_maskgraycolormap);
            tempImage->set_ismainshow(false);
            LabelRegionUpdata(_mapImageData.value(map.value(i)), tempImage);
            this->setItemWidget(pListWidgetItem, tempImage);
            connect(tempImage, SIGNAL(signalSendId(bool, void *)), this, SLOT(OnSignalSendId(bool, void *)));
        }       
       /* connect(tempImage, SIGNAL(signalSendId(bool, void *)), this, SLOT(OnSignalSendId(bool, void *)));
        connect(tempImage, SIGNAL(signalDelImage(bool)), this, SLOT(OnSignalDelImage(bool)));
        connect(tempImage, SIGNAL(signalInitMenu(bool, void*, int)), this, SLOT(OnSignalInitMenu(bool, void*, int)));
        connect(tempImage, SIGNAL(signalMenuOperate(bool, int, void*)), this, SLOT(OnSignalMenuOperate(bool, int, void*)));
        */
    }   
}
void baseListWidget::LabelRegionUpdata(int idx, ImageLabel *label)
{
    /*_isFillChecked = publicClass::instance()->readIniFile("switchBtnFill").toBool();
    _isKeepaspectratio = publicClass::instance()->readIniFile("switchBtnInproportion").toBool();//是否保持原比例
    _penSizeValue = publicClass::instance()->readIniFile("spinBoxPenSize").toInt();
    export_mode_ = std::string(publicClass::instance()->readIniFile("fileExportMode").toString().toLocal8Bit());//文件导出模式
    _fileLoadType = publicClass::instance()->readIniFile("switchBtnFileLoadMode").toBool();;//文件加载方式
    */
    QString imageabspath = FileProjectManager::Instance()->ProjectGetImageAbsPath();
    ainno_image curimg = FileProjectManager::Instance()->StreamGetImageInfo(idx);
    label->ClearMenu();
    label->setLineSizeWidth(publicClass::instance()->readIniFile("spinBoxPenSize").toInt());//画笔宽度
    label->isFill(publicClass::instance()->readIniFile("switchBtnFill").toBool());//是否填充
    label->set_keepaspectratio(publicClass::instance()->readIniFile("switchBtnInproportion").toBool());//是否保持原比例

    label->Open(imageabspath, curimg);
    label->ResizeWidetSize(this->width() - 20, this->width() - 20);
    label->ResizePredictdefects(this->width() - 20, this->width() - 20);
}
//当前显示的图片数据
void baseListWidget::setCurrentImageData(QMap <std::string, int> imageData, QMap<std::string, trained_flag> flag_map)//当前映射表图片数据
{
    _mapImageData = imageData;
    _flagMap = flag_map;
}
void baseListWidget::triget_menu_delete()//删除
{
    emit signalDelImage(false);
}
void baseListWidget::triget_menu_nolabel()//无缺陷
{
    emit signalMenuOperate(false, (int)MenuOperateT::O_NOLABEL_T, NULL);
}
void baseListWidget::triget_menu_close_auto()//关闭工具条
{
    emit signalMenuOperate(false, (int)MenuOperateT::O_CLOSE_T, NULL);
}
void baseListWidget::triget_menu_open_auto()//打开工具条
{
    emit signalMenuOperate(false, (int)MenuOperateT::O_OPEN_T, NULL);
}
void baseListWidget::triget_menu_test()//测试
{
    emit signalMenuOperate(false, (int)MenuOperateT::O_TEST_T, NULL);
}
void baseListWidget::triget_menu_train_always()//总是
{
    //发送信号
    emit signalMenuOperate(false, (int)MenuOperateT::O_ALWAYS_T, NULL);
}
void baseListWidget::triget_menu_train_auto()//自动
{
    emit signalMenuOperate(false, (int)MenuOperateT::O_AUTO_T, NULL);
}
void baseListWidget::triget_menu_train_never()//从不
{
    emit signalMenuOperate(false, (int)MenuOperateT::O_NEVER_T, NULL);
}
//左键弹出菜单
void baseListWidget::slotTreeMenu(const QPoint &pos)
{

    QListWidgetItem* curItem = this->itemAt(pos);
    if (curItem == NULL)
        return;
    if (this->selectedItems().size() == 1) {
        slotListWidgetClick(this->indexFromItem(curItem));
    } 
    QMenu menu;
    QAction* actionDel = menu.addAction(tr("&删除"));    //父菜单
    QAction* actionTrain = menu.addAction(tr("&训练"));    //父菜单
    QAction* actionDefect = menu.addAction(tr("&无缺陷"));    //父菜单
    QAction* actionCloseTool = menu.addAction(tr("&关闭工具条"));    //父菜单
    QAction* actionOpenTool = menu.addAction(tr("&打开工具条"));    //父菜单
    QAction* actionTest = menu.addAction(tr("&测试"));    //父菜单
    QMenu* subMenu = new QMenu(&menu);  //子菜单
    QAction* chil1 = subMenu->addAction(tr("&总是"));
    QAction* chil2 = subMenu->addAction(tr("&自动"));
    QAction* chil3 = subMenu->addAction(tr("&从不"));
    actionTrain->setMenu(subMenu);
    connect(actionDel, SIGNAL(triggered()), this, SLOT(triget_menu_delete()));//删除
    connect(actionDefect, SIGNAL(triggered()), this, SLOT(triget_menu_nolabel()));//无缺陷
    connect(actionCloseTool, SIGNAL(triggered()), this, SLOT(triget_menu_close_auto()));//关闭工具条
    connect(actionOpenTool, SIGNAL(triggered()), this, SLOT(triget_menu_open_auto()));//打开工具条
    connect(actionTest, SIGNAL(triggered()), this, SLOT(triget_menu_test()));//测试

    connect(chil1, SIGNAL(triggered()), this, SLOT(triget_menu_train_always()));//总是
    connect(chil2, SIGNAL(triggered()), this, SLOT(triget_menu_train_auto()));//自动
    connect(chil3, SIGNAL(triggered()), this, SLOT(triget_menu_train_never()));//从不
    menu.exec(QCursor::pos());
    //setContextMenuPolicy(Qt::ActionsContextMenu);
}
void baseListWidget::slotListWidgetClick(const QModelIndex &index)
{
    if (_isPressShift) {
        _endRow = index.row();

        for (int i = _startRow; i < _endRow; i++) {
            this->item(i)->setSelected(true);
        }
        return;
    }
   emit signalIndexValue(index.data(), index.row());
  // item->setSelected(true);
   //this->setCurrentIndex(index);
   _indexSelect = index.row();
}
void baseListWidget::clearListWidget()
{
    this->clear();
    for (int i = 0; i < this->count(); i++) {
        this->removeItemWidget(this->item(0));
    }
    _mapLabel.clear();
   /* if (_mapLabel.size() > 0) {
        for (auto iter = _mapLabel.begin(); iter != _mapLabel.end(); iter++)
        {
            iter.key()->deleteLater();
        }
        _mapLabel.clear();
    }*/
}
//接收左侧文件列表中控件点击事件
void baseListWidget::OnSignalSendId(bool is, void* item)
{
    ImageLabel* label = (ImageLabel*)item;
    QListWidgetItem *item1 = _mapLabel.value(label);
    QModelIndex index = this->indexFromItem(item1);
    slotListWidgetClick(index);
}
void baseListWidget::setColorMap(std::map<int, QColor> colorMap, std::map<int, QColor> maskgraycolormap)
{
    _colorMap = colorMap;
    _maskgraycolormap = maskgraycolormap;
}
QVariant baseListWidget::getCurrentData(int index)
{
    if (this->item(index) == NULL) {
        return QString();
   }
    QListWidgetItem *item = this->item(index);
    return item->toolTip();
}
QVariantList baseListWidget::getCurrentData(QStringList list)
{
    QVariantList listData;
    for (int i = 0; i < list.size(); i++) {
        QListWidgetItem * item = this->item(list.at(i).toInt());
        QString data = item->toolTip().simplified();
        listData.push_back(data);
    }
    return listData;
}
QStringList baseListWidget::getAllSelectedRow()
{
    QStringList listRows;
    QList<QListWidgetItem*> list = this->selectedItems();
    for (int i = 0; i < list.size(); i++)
    {
        listRows.append(QString::number(this->indexFromItem(list.at(i)).row()));

    }
    return listRows;

}
void baseListWidget::setCurrentIndexNum(int index)
{
    this->setCurrentItem(this->item(index));
}
int baseListWidget::getCurrentRowCount()
{
   return this->count();
}
int baseListWidget::getListIndex()
{
    return _indexSelect;
}