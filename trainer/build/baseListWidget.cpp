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
    //�͸ò��Զ�Ӧ���źţ�showListWidgetMenuSlotΪ�Լ�����Ĳۺ���.
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
    _isKeepaspectratio = publicClass::instance()->readIniFile("switchBtnInproportion").toBool();//�Ƿ񱣳�ԭ����
    _penSizeValue = publicClass::instance()->readIniFile("spinBoxPenSize").toInt();
    export_mode_ = std::string(publicClass::instance()->readIniFile("fileExportMode").toString().toLocal8Bit());//�ļ�����ģʽ
    _fileLoadType = publicClass::instance()->readIniFile("switchBtnFileLoadMode").toBool();;//�ļ����ط�ʽ
    */
    QString imageabspath = FileProjectManager::Instance()->ProjectGetImageAbsPath();
    ainno_image curimg = FileProjectManager::Instance()->StreamGetImageInfo(idx);
    label->ClearMenu();
    label->setLineSizeWidth(publicClass::instance()->readIniFile("spinBoxPenSize").toInt());//���ʿ��
    label->isFill(publicClass::instance()->readIniFile("switchBtnFill").toBool());//�Ƿ����
    label->set_keepaspectratio(publicClass::instance()->readIniFile("switchBtnInproportion").toBool());//�Ƿ񱣳�ԭ����

    label->Open(imageabspath, curimg);
    label->ResizeWidetSize(this->width() - 20, this->width() - 20);
    label->ResizePredictdefects(this->width() - 20, this->width() - 20);
}
//��ǰ��ʾ��ͼƬ����
void baseListWidget::setCurrentImageData(QMap <std::string, int> imageData, QMap<std::string, trained_flag> flag_map)//��ǰӳ���ͼƬ����
{
    _mapImageData = imageData;
    _flagMap = flag_map;
}
void baseListWidget::triget_menu_delete()//ɾ��
{
    emit signalDelImage(false);
}
void baseListWidget::triget_menu_nolabel()//��ȱ��
{
    emit signalMenuOperate(false, (int)MenuOperateT::O_NOLABEL_T, NULL);
}
void baseListWidget::triget_menu_close_auto()//�رչ�����
{
    emit signalMenuOperate(false, (int)MenuOperateT::O_CLOSE_T, NULL);
}
void baseListWidget::triget_menu_open_auto()//�򿪹�����
{
    emit signalMenuOperate(false, (int)MenuOperateT::O_OPEN_T, NULL);
}
void baseListWidget::triget_menu_test()//����
{
    emit signalMenuOperate(false, (int)MenuOperateT::O_TEST_T, NULL);
}
void baseListWidget::triget_menu_train_always()//����
{
    //�����ź�
    emit signalMenuOperate(false, (int)MenuOperateT::O_ALWAYS_T, NULL);
}
void baseListWidget::triget_menu_train_auto()//�Զ�
{
    emit signalMenuOperate(false, (int)MenuOperateT::O_AUTO_T, NULL);
}
void baseListWidget::triget_menu_train_never()//�Ӳ�
{
    emit signalMenuOperate(false, (int)MenuOperateT::O_NEVER_T, NULL);
}
//��������˵�
void baseListWidget::slotTreeMenu(const QPoint &pos)
{

    QListWidgetItem* curItem = this->itemAt(pos);
    if (curItem == NULL)
        return;
    if (this->selectedItems().size() == 1) {
        slotListWidgetClick(this->indexFromItem(curItem));
    } 
    QMenu menu;
    QAction* actionDel = menu.addAction(tr("&ɾ��"));    //���˵�
    QAction* actionTrain = menu.addAction(tr("&ѵ��"));    //���˵�
    QAction* actionDefect = menu.addAction(tr("&��ȱ��"));    //���˵�
    QAction* actionCloseTool = menu.addAction(tr("&�رչ�����"));    //���˵�
    QAction* actionOpenTool = menu.addAction(tr("&�򿪹�����"));    //���˵�
    QAction* actionTest = menu.addAction(tr("&����"));    //���˵�
    QMenu* subMenu = new QMenu(&menu);  //�Ӳ˵�
    QAction* chil1 = subMenu->addAction(tr("&����"));
    QAction* chil2 = subMenu->addAction(tr("&�Զ�"));
    QAction* chil3 = subMenu->addAction(tr("&�Ӳ�"));
    actionTrain->setMenu(subMenu);
    connect(actionDel, SIGNAL(triggered()), this, SLOT(triget_menu_delete()));//ɾ��
    connect(actionDefect, SIGNAL(triggered()), this, SLOT(triget_menu_nolabel()));//��ȱ��
    connect(actionCloseTool, SIGNAL(triggered()), this, SLOT(triget_menu_close_auto()));//�رչ�����
    connect(actionOpenTool, SIGNAL(triggered()), this, SLOT(triget_menu_open_auto()));//�򿪹�����
    connect(actionTest, SIGNAL(triggered()), this, SLOT(triget_menu_test()));//����

    connect(chil1, SIGNAL(triggered()), this, SLOT(triget_menu_train_always()));//����
    connect(chil2, SIGNAL(triggered()), this, SLOT(triget_menu_train_auto()));//�Զ�
    connect(chil3, SIGNAL(triggered()), this, SLOT(triget_menu_train_never()));//�Ӳ�
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
//��������ļ��б��пؼ�����¼�
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