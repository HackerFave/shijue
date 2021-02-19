#include "treeViewBaseWidget.h"
#include <qdebug.h>
#include <qpushbutton.h>
treeViewBaseWidget::treeViewBaseWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    setContextMenuPolicy(Qt::CustomContextMenu);
    this->setFixedWidth(346);
    QStandardItemModel *model = new QStandardItemModel(this);
    QStringList list;
    list << tr("�ļ��б�")/*<< tr("value")*/;
    model->setHorizontalHeaderLabels(list);//�б�ͷ��
    model->horizontalHeaderItem(0);

    model->setColumnCount(list.size());//����
    ui.treeView->header()->setDefaultSectionSize(100);
    ui.treeView->header()->setDefaultAlignment(Qt::AlignCenter);//����
    ui.treeView->header()->setVisible(false);
    ui.treeView->setModel(model);
    ui.treeView->setColumnWidth(0, 100);//�п�
    ui.treeView->setEditTriggers(QTreeView::NoEditTriggers);//���ɱ༭
    ui.treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui.treeView->setStyleSheet(tr("QTreeView::branch:open:has-children:!has-siblings,"
        "QTreeView::branch:open:has-children:has-siblings {image: url(%1/icons/openT.png);}"
        "QTreeView::branch:has-children:!has-siblings:closed,"
        "QTreeView::branch:closed:has-children:has-siblings {image: url(%1/icons/closeT.png);}"
        "QTreeView::item:hover {background-color:black;}"
        "QTreeView::item:selected {background-color:#1E90FF;border:1px solid #08347F;}"
        "QToolTip{color: black}")
    .arg(publicClass::instance()->getAppPath()));
    ui.treeView->setItemsExpandable(false);
   /* QToolTip{
        //����������ʽ
        font - family: "Microsoft YaHei";//��������
        font - size: 25px;//�����С������
        font - style: italic;//����б����ʽ��mormal��б��
        font - weight:bold;//����Ӵ���ʽ��mormal���Ӵ�
        color: #bdc8e2//������ɫ*/
    //ui.treeView->setAttribute(Qt::WA_MacShowFocusRect, 0);
    //--

    /*̷��*/

    //TC
   
   /* QHeaderView *hv = ui.treeView->header();
    hv->setSortIndicatorShown(true); //��ʾ��������
    hv->setSectionsClickable(false);     //���ñ�����Ա�����
    connect(hv, SIGNAL(sectionClicked(int)), this, SLOT(slotSectionClicked(int)));*/
   // connect(ui.treeView, SIGNAL(doubleClicked(const QModelIndex)), this, SLOT(OnlineTreeViewDoubleClick(const QModelIndex)));
    connect(ui.treeView, SIGNAL(clicked(const QModelIndex)), this, SLOT(OnlineTreeViewClick(const QModelIndex)));

    ui.treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui.treeView, &QTreeView::customContextMenuRequested, this, &treeViewBaseWidget::slotTreeMenu);
}

treeViewBaseWidget::~treeViewBaseWidget()
{
}
void treeViewBaseWidget::slotTreeViewHeaderClicked(const QModelIndex &index)
{
      // qDebug()<<index.row()<<index.column();
}
InfoMapData* treeViewBaseWidget::addLevel(const QString &displayText0,
    const QString &displayText1,
    const QString &toolTip,
    InfoMapData *parent,
    const QString &key,
    const QVariant &var
)
{
    InfoMapData*data = new InfoMapData;
    data->key = key;
    data->displayText0 = displayText0;
    data->value = var;
    data->displayText1 = displayText1;
    if (toolTip.isEmpty()) {
            //   data->toolTip = data->text;
    }
    else {
        data->toolTip = toolTip;
    }
    if (parent) {
        data->level = 1;
        parent->child.append(data);
    }
    else {
        data->level = 0;
    }
    return data;
}
static QVariant getVar(InfoMapData*data)
{
    QVariant v;
    v.setValue(data);
    return v;
}


void treeViewBaseWidget::slot_ImageFile_status_all1_data(const QString &ip, const ImageFileData *info)
{
    _mutex.lock();
    initTreeView();
    _mutex.unlock();
}
/** doubleתstring����С��λ
 * @brief MainWindow::getDecimalPlaces
 * @param d
 * @param countBit ������λС����Ĭ��3λ
 * @return
 */
QString treeViewBaseWidget::getDecimalPlaces(double d, int countBit)
{
    return QString::number(d, 'f', countBit);
}
void treeViewBaseWidget::initTreeView()
{
    QList<InfoMapData*>InfoMapDataList;
    QFileInfoList fileInfoList;// = GetFileList("D:/workT/manuvision-platform/build/bottlenew");

    //�����ļ��м��ļ�
    QString filter;

    QStringList listFilePath;

    foreach(QFileInfo fileinfo, fileInfoList)
    {
        //_filter = _fileinfo.completeSuffix();
        filter = fileinfo.suffix();    //��׺��
        if ((filter != "jpg") && (filter != "jpeg") && (filter != "png"))
        {
            continue;
        }
        //folder_list.append(fileinfo.absoluteFilePath());
        if (!listFilePath.contains(fileinfo.absolutePath())) {
            listFilePath.append(fileinfo.absolutePath());
        }
    }

    for (int i = 0; i < listFilePath.size(); i++)
    {
        int first = listFilePath.at(i).lastIndexOf("/");
        QString title = listFilePath.at(i).right(listFilePath.at(i).length() - first - 1); //���ұ߽�ȡ
        InfoMapDataList.append(addLevel(title));

        QDir dir(listFilePath.at(i));
        QStringList nameFilters;
        nameFilters << "*.png";
        QStringList files = dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Name);
        for (size_t num = 0; num < files.size(); num++)
        {
            addLevel(files.at(num), "1",QString(listFilePath.at(i)+"/"+files.at(num)), InfoMapDataList.last());
        }
    }
    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui.treeView->model());

    if (model->rowCount() == 0) {
        for (int i = 0; i < InfoMapDataList.size(); ++i)
        {
            auto var = InfoMapDataList.at(i);
            QStandardItem *item = new QStandardItem(var->displayText0);
            for (int j = 0; j < var->child.size(); ++j)
            {
                auto childVar = var->child.at(j);
                QStandardItem *childItem0 = new QStandardItem(childVar->displayText0);
                childItem0->setToolTip(childVar->toolTip);
                QStandardItem *childItem1 = new QStandardItem(childVar->displayText1);
                childItem1->setToolTip(childVar->displayText1);
                QList<QStandardItem*>childItemList;
                childItemList << childItem0 << childItem1;
                item->appendRow(childItemList);
            }
            model->appendRow(item);
            QModelIndex index = ui.treeView->model()->index(i, 0);
            ui.treeView->setExpanded(index, _isExpanded);
        }
    }
    else {
        for (int i = 0; i < InfoMapDataList.size(); ++i) {
            QModelIndex index = model->index(i, 0);
            auto var = InfoMapDataList.at(i);
            for (int j = 0; j < var->child.size(); ++j) {
                auto childVar = var->child.at(j);
                QModelIndex childIndex = index.child(j, 1);
                QStandardItem *childItem1 = model->itemFromIndex(childIndex);
                if (!childItem1) {
                    //                    SCWarning<<"error: QStandardItem is empty."<<i<<j<<childIndex;
                    continue;
                }
                childItem1->setText(childVar->displayText1);
                childItem1->setToolTip(childVar->toolTip);
            }
        }
    }

}
void treeViewBaseWidget::addTreeViewNoneHead(QList<std::string> list)
{
    QList<InfoMapData*>InfoMapDataList;
    //1 

    
    for (size_t i = 0; i < list.size(); i++)
    {
        addLevel(QString(list.at(i).c_str()), QString::number(i), "", InfoMapDataList.last());
        InfoMapDataList.append(addLevel(QString(list.at(i).c_str())));
    }
}
/*
*/
void treeViewBaseWidget::addTreeView(QMap<int, std::string> map, const int indexStart, const int indexEnd, bool isOldFlag)
{
    QList<InfoMapData*>InfoMapDataList;
    //1 
    QTime time1;
    time1.start();
    InfoMapDataList.append(addLevel(tr("�ļ��б�")));
    //InfoMapDataList.append(addLevel(tr("ceshi")));
    if (isOldFlag) {
     QMap<int, std::string>::iterator it;
        for (it= map.begin();it!=map.end();it++)
        {
        addLevel(QString::fromStdString(it.value()),"", "", InfoMapDataList.last());
        }
    }
    else
    {
        for (int i = indexStart; i < indexEnd; i++) {
            addLevel(QString::fromStdString(map.value(i)), "", "", InfoMapDataList.last());
        }
    }
   
    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui.treeView->model());

    if (model->rowCount() == 0) {
        for (int i = 0; i < InfoMapDataList.size(); ++i)
        {
            auto var = InfoMapDataList.at(i);
            QStandardItem *item = new QStandardItem(var->displayText0);
                for (int j = 0; j < var->child.size(); ++j)
                {
                    auto childVar = var->child.at(j);
                     QStandardItem *childItem0 = new QStandardItem;
                     if (!publicClass::instance()->readIniFile("switchBtnFileLoadMode").toBool()) {
                         childItem0->setText("");
                         childItem0->setSizeHint(QSize(this->width()-20, this->width() - 120));
                     }
                     else
                     {
                         childItem0->setText(childVar->displayText0);
                     }
                    childItem0->setToolTip(childVar->displayText0);//�ֶ���
                    //QStandardItem *childItem1 = new QStandardItem("childVar->displayText1");
                    //childItem1->setToolTip("childVar->displayText1");
                    //QImage image;
                   // QPixmap pix(tr("%1/icons/add.png").arg(publicClass::instance()->getAppPath()));
                    if (_flagMap.value(childVar->displayText0.toStdString()) == 0) {
                        childItem0->setIcon(QIcon(tr("%1/icons/emity.png").arg(publicClass::instance()->getAppPath())));
                    }
                    else if(_flagMap.value(childVar->displayText0.toStdString()) == 3)
                    {
                        childItem0->setIcon(QIcon(tr("%1/icons/good.png").arg(publicClass::instance()->getAppPath())));
                    }
                    else if (_flagMap.value(childVar->displayText0.toStdString()) == 4)
                    {
                        childItem0->setIcon(QIcon(tr("%1/icons/bad.png").arg(publicClass::instance()->getAppPath())));
                    }
                   
                   // QPixmap scaledPixmap = pix.scaled(500, 500, Qt::KeepAspectRatio);
                   // childItem0->setIcon(QIcon(pix));
                    QList<QStandardItem*>childItemList;
                    childItemList << childItem0;//<< childItem1;
                    item->appendRow(childItemList);
                }
                model->appendRow(item);
                QModelIndex index = ui.treeView->model()->index(i, 0);
                ui.treeView->setExpanded(index, _isExpanded);
         //qDebug() << "ͼƬ��ʽ����ʱ��1" << time1.elapsed() / 1000;
           // }
            if(!publicClass::instance()->readIniFile("switchBtnFileLoadMode").toBool())
            {
                QTime time;
                time.start();
                for (int j = 0; j < var->child.size(); ++j) {
                    auto childVar = var->child.at(j);
                    QModelIndex indexP = model->index(0, 0);
                    QModelIndex childIndex = indexP.child(j, 0);
                    ImageLabel *label = new ImageLabel(false,0);
                   // imageForm *label = new imageForm(false, 0);
                    //QGridLayout *layout = new QGridLayout;
                    label->set_ismainshow(false);
                    //_VImageLabel.push_back(label);
                    _mapLabel.insert(label, childIndex);
                    label->UpdateColorMap(_colorMap);
                    label->UpdateMaskGrayColorMap(_maskgraycolormap);
                    QObject::connect(label, SIGNAL(signalSendPressPos(const QPoint&)),
                        this, SLOT(slotTreeMenu(const QPoint&)));
                   // QObject::connect(label, SIGNAL(signalLeftBtnClicked(const QPoint&)),
                     //   this, SLOT(slotLeftBtnClicked(const QPoint&)));
                    connect(label, SIGNAL(signalSendId(bool, void *)), this, SLOT(OnSignalSendId(bool, void *)));
                   // connect(label, SIGNAL(signalDelImage(bool)), this, SLOT(OnSignalDelImage(bool)));
                    //connect(label, SIGNAL(signalInitMenu(bool, void*, int)), this, SLOT(OnSignalInitMenu(bool, void*, int)));
                    //connect(label, SIGNAL(signalMenuOperate(bool, int, void*)), this, SLOT(OnSignalMenuOperate(bool, int, void*)));                  
                    QString dataStr = childVar->displayText0;/* getCurrentData(j).toString().simplified();*/
                    
                    LabelRegionUpdata(_mapImageData.value(dataStr.toStdString()), label);
                    ui.treeView->setIndexWidget(childIndex, label);
                }
               // qDebug() << "ͼƬ��ʽ����ʱ��" << time.elapsed() / 1000;
            }
        }
    }
    
}
void treeViewBaseWidget::addTreeView(QList<std::string> list)
{
    
    
    QList<InfoMapData*>InfoMapDataList;
    //1 

    InfoMapDataList.append(addLevel(tr("�ļ��б�")));
    for (size_t i = 0; i < list.size(); i++)
    {
        addLevel(QString(list.at(i).c_str()), QString::number(i), "", InfoMapDataList.last());
    }
    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui.treeView->model());

    if (model->rowCount() == 0) {
        for (int i = 0; i < InfoMapDataList.size(); ++i)
        {
            auto var = InfoMapDataList.at(i);
            QStandardItem *item = new QStandardItem(var->displayText0);
            for (int j = 0; j < var->child.size(); ++j)
            {
                auto childVar = var->child.at(j);
                QStandardItem *childItem0 = new QStandardItem(childVar->displayText0);
                childItem0->setToolTip(childVar->displayText0);//�ֶ���
                QStandardItem *childItem1 = new QStandardItem(childVar->displayText1);
                childItem1->setToolTip(childVar->displayText1);
                QList<QStandardItem*>childItemList;
                childItemList << childItem0 << childItem1;
                item->appendRow(childItemList);
            }
            model->appendRow(item);
            QModelIndex index = ui.treeView->model()->index(i, 0);
            ui.treeView->setExpanded(index, _isExpanded);
        }
    }
    else {
        for (int i = 0; i < InfoMapDataList.size(); ++i) {
            QModelIndex index = model->index(i, 0);
            auto var = InfoMapDataList.at(i);
            for (int j = 0; j < var->child.size(); ++j) {
                auto childVar = var->child.at(j);
                QModelIndex childIndex = index.child(j, 1);
                QStandardItem *childItem1 = model->itemFromIndex(childIndex);
                if (!childItem1) {
                    //                    SCWarning<<"error: QStandardItem is empty."<<i<<j<<childIndex;
                    continue;
                }
                childItem1->setText(childVar->displayText0);
                childItem1->setToolTip(childVar->toolTip);
            }
        }
    }
}
void treeViewBaseWidget::rbkWarningCode(const QVariantList & list, QList<InfoMapData*>&infoMapDataList)
{
    for (int i = 0; i < list.size(); ++i) {
        QVariantMap map = list.at(i).toMap();
        for (int j = 0; j < map.keys().size(); ++j) {

            QString  sKey = map.keys().at(j);
            int key = sKey.toInt();
            qint64 time64 = map.value(sKey).toLongLong();
            QString rbkTime = QString::number(time64);
            QString rbkStr = QString::number(key);
            addLevel(QString("[%1][%2]").arg(rbkTime).arg(key), rbkStr, rbkStr, infoMapDataList.last());
        }
    }
}
void treeViewBaseWidget::slotSectionClicked(int column)
{
    //    SCDebug<<column;
    if (0 == column) {
        slotExpandedClicked(_isPressedSection);//�Ŵ�
        _isPressedSection = !_isPressedSection;//
    }
}
//�ݹ�չ��
void treeViewBaseWidget::recursiveCollapseExpand(const QModelIndex index, bool isExpanded)
{
    int rowCount = ui.treeView->model()->rowCount(index);

    for (int childNo = 0; childNo < rowCount; ++childNo)
    {
        QModelIndex childIndex = index.child(childNo, 0);
        if (ui.treeView->isExpanded(childIndex))
        {
            ui.treeView->setExpanded(childIndex, isExpanded);
            int childCount = ui.treeView->model()->rowCount(childIndex);
            if (childCount > 0) {
                recursiveCollapseExpand(childIndex, isExpanded);
            }
        }
    }
}
void treeViewBaseWidget::slotExpandedClicked(bool checked)
{
    if (ui.treeView->model()) {
        for (int i = 0; i < ui.treeView->model()->rowCount(); ++i) {
            QModelIndex index = ui.treeView->model()->index(i, 0);
            ui.treeView->setExpanded(index, !checked);
        }
    }
}
void treeViewBaseWidget::OnlineTreeViewDoubleClick(const QModelIndex & index)
{

  // qDebug() << index.data() << "";
    /* QAbstractItemModel* m=(QAbstractItemModel*)index.model();
     for(int columnIndex = 0; columnIndex < m->columnCount(); columnIndex++)
     {
         QModelIndex x=m->index(index.row(),columnIndex);

         QString s= x.data().toString();
         //QMessageBox::about(0,s,s);

     }*/
   // emit signalIndexValue(index.data(), index.row());
}
void treeViewBaseWidget::OnlineTreeViewClick(const QModelIndex & index)
{
    if (index.data().toString() == tr("�ļ��б�")) {
        return;
    }
    emit signalIndexValue(index.data(), index.row());
    _indexSelect = index.row();
}
void treeViewBaseWidget::openImageFileDir(QString & filePath)
{
    
}
//�ݹ�����ļ���
QFileInfoList treeViewBaseWidget::GetFileList(QString path)
{
    QDir dir(path);
    QString filter;
    //�г�dir(path)Ŀ¼�ļ��������ļ���Ŀ¼��Ϣ���洢��file_list����
    QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);

    //�г�dir(path)Ŀ¼���������ļ���
    QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    //�������ļ���folder_list�ݹ�����������ݴ���file_list����
    for (int i = 0; i != folder_list.size(); i++)
    {
        QString name = folder_list.at(i).absoluteFilePath();
        QFileInfoList child_file_list = GetFileList(name);
        file_list.append(child_file_list);
    }
    //�����ļ��м��ļ�
    foreach(QFileInfo fileinfo, file_list)
    {
        //_filter = _fileinfo.completeSuffix();
        filter = fileinfo.suffix();    //��׺��
        if ((filter != "jpg") && (filter != "jpeg") && (filter != "png"))
        {
            continue;
        }
    }

    return file_list;
}
QModelIndex treeViewBaseWidget::getIndex(int index)
{
    return QModelIndex();
}
void treeViewBaseWidget::delTreeViewData()
{
    for (int i = 0; i < ui.treeView->model()->rowCount(); i++) {
        ui.treeView->model()->removeRow(i);
    }
    if (_VImageLabel.size() > 0) {
        for (auto iter = _VImageLabel.begin(); iter != _VImageLabel.end(); iter++)
        {
            (*iter)->deleteLater();
        }
        _VImageLabel.clear();
    }
    if (_mapLabel.size() > 0) {
        for (auto iter = _mapLabel.begin(); iter != _mapLabel.end(); iter++)
        {
            iter.key()->deleteLater();
        }
        _mapLabel.clear();
    }
    
      
}
void treeViewBaseWidget::setCurrentIndexNum(int index)
{
  
   // QModelIndex rootIndex = ui.treeView->rootIndex();
    QStandardItemModel *model = static_cast<QStandardItemModel*>(ui.treeView->model());
    QModelIndex indexP = model->index(0, 0);
    QModelIndex childIndex = indexP.child(index, 0);
   // emit ui.treeView->clicked(selIndex);
    ui.treeView->setCurrentIndex(childIndex);

   
  //  QModelIndex item = model->index(index, 0);
    //model->setData(item, QColor(240, 240, 240), Qt::BackgroundRole);//���ñ���ɫ
   // ui.treeView->update();
}
QVariant treeViewBaseWidget::getCurrentData(int index)
{
    QStandardItemModel *model = static_cast<QStandardItemModel*>(ui.treeView->model());
    QStandardItem *item = model->item(0);
    if (item->child(index) == NULL) {
        return QString();
    }
    QStandardItem * childitem = item->child(index);
    QString data =  childitem->toolTip();
    return data;
}
int treeViewBaseWidget::getCurrentRowCount()
{
    QStandardItemModel *model = static_cast<QStandardItemModel*>(ui.treeView->model());
    if (model->item(0) == NULL) {
        return 0;
    }
    QStandardItem *item = model->item(0);
    return item->rowCount();
}
QVariantList treeViewBaseWidget::getCurrentData(QStringList list)
{
    QVariantList listData;
    QStandardItemModel *model = static_cast<QStandardItemModel*>(ui.treeView->model());
    QStandardItem *item = model->item(0);
    for (int i = 0; i < list.size(); i++) {
        QStandardItem * childitem = item->child(list.at(i).toInt());
        QString data = childitem->toolTip();
        listData.push_back(data);
    }   
    return listData;
}
void treeViewBaseWidget::GetNode()
{
    QStandardItemModel *model = static_cast<QStandardItemModel*>(ui.treeView->model());
    for (int i = 0; i < model->rowCount(); i++)
    {
        QStandardItem *item = model->item(i);
        GetItem(model->item(i));
    }
}
void treeViewBaseWidget::GetItem(QStandardItem *item)
{
    Q_ASSERT(item);
    if (item->hasChildren())
    {
        for (int i = 0; i < item->rowCount(); i++)
        {
            QStandardItem * childitem = item->child(i);
            GetItem(childitem);
        }
    }
}
QStringList treeViewBaseWidget::getAllSelectedRow()
{
    /*if (item->child(index) == NULL) {
        return QString();
    }
    QStandardItem * childitem = item->child(index);
    QString data = childitem->toolTip();*/
    QStringList listRows;
    QModelIndexList indexList =  ui.treeView->selectionModel()->selectedRows();
    for (size_t i = 0; i < indexList.size(); i++)
    {
        qDebug() << indexList.at(i).row() << indexList.at(i).column() << indexList.at(i).data().toString();
        listRows.append(QString::number(indexList.at(i).row()));

    }
    return listRows;
  
}
void treeViewBaseWidget::getRowCounts()
{
    QStandardItemModel *model = static_cast<QStandardItemModel*>(ui.treeView->model());
    for (int i = 0; i < model->rowCount(); i++)
    {
        QStandardItem *item = model->item(i);
        qDebug() <<item->rowCount() ;
    }
}
void treeViewBaseWidget::slotLeftBtnClicked(const QPoint &pos)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui.treeView->model());
    
    QModelIndex curIndex = ui.treeView->indexAt(pos);      //��ǰ�����Ԫ�ص�index
    QModelIndex index = curIndex.sibling(curIndex.row(), 0); //���еĵ�1��Ԫ�ص�index
    OnlineTreeViewClick(index);
}
void treeViewBaseWidget::slotTreeMenu(const QPoint &pos)
{
    
    QModelIndex curIndex = ui.treeView->indexAt(pos);      //��ǰ�����Ԫ�ص�index
    QModelIndex index = curIndex.sibling(curIndex.row(), 0); //���еĵ�1��Ԫ�ص�index
    //emit signalIndexValue(curIndex.data(), curIndex.row());
    if (index.isValid())
    {
        if (index.parent() != ui.treeView->rootIndex())     //����һ���ڵ㣬��Ϊֻ�Զ����ڵ��������꼶�ƶ�
        {
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
        }
    }
    
    setContextMenuPolicy(Qt::ActionsContextMenu);
}
void treeViewBaseWidget::triget_menu_delete()//ɾ��
{
    emit signalDelImage(false);
}
void treeViewBaseWidget::triget_menu_nolabel()//��ȱ��
{
    emit signalMenuOperate(false, (int)MenuOperateT::O_NOLABEL_T,NULL);
}
void treeViewBaseWidget::triget_menu_close_auto()//�رչ�����
{
    emit signalMenuOperate(false, (int)MenuOperateT::O_CLOSE_T, NULL);
}
void treeViewBaseWidget::triget_menu_open_auto()//�򿪹�����
{
    emit signalMenuOperate(false, (int)MenuOperateT::O_OPEN_T, NULL);
}
void treeViewBaseWidget::triget_menu_test()//����
{
    emit signalMenuOperate(false, (int)MenuOperateT::O_TEST_T, NULL);
}
void treeViewBaseWidget::triget_menu_train_always()//����
{
    //�����ź�
    emit signalMenuOperate(false, (int)MenuOperateT::O_ALWAYS_T, NULL);
}
void treeViewBaseWidget::triget_menu_train_auto()//�Զ�
{
    emit signalMenuOperate(false, (int)MenuOperateT::O_AUTO_T, NULL);
}
void treeViewBaseWidget::triget_menu_train_never()//�Ӳ�
{
    emit signalMenuOperate(false, (int)MenuOperateT::O_NEVER_T, NULL);
}
int treeViewBaseWidget::getListIndex()
{
    return _indexSelect;
}
void treeViewBaseWidget::setCurrentImageData(QMap <std::string, int> imageData, QMap<std::string, trained_flag> flag_map)//��ǰӳ���ͼƬ����
{
    _mapImageData = imageData;
    _flagMap = flag_map;
}
void treeViewBaseWidget::LabelRegionUpdata(int idx , ImageLabel *label)
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
    label->ResizeWidetSize(this->width() - 20, this->width() - 130);
    label->ResizePredictdefects(this->width() - 20, this->width() - 130);
}
//��������ļ��б��пؼ�����¼�
void treeViewBaseWidget::OnSignalSendId(bool is, void* item)
{
    ImageLabel* label = (ImageLabel*)item;
   // imageForm* Imageform = (imageForm*)form;
    QModelIndex index = _mapLabel.value(label);
    OnlineTreeViewClick(index);
}
// �Ҽ���ɾ������ť������
void treeViewBaseWidget::OnSignalDelImage(bool ismain)
{
    
       
}
// �Ҽ����˵�����ť������
void treeViewBaseWidget::OnSignalInitMenu(bool is, void* item, int size) {
   /* ImageLabel* temp = (ImageLabel*)item;
    temp->setLineSizeWidth(_penSizeValue);
    temp->isFill(_isFillChecked);
    temp->DrawLabel();
    temp->DrawPredict();
    temp->ShowInfo();
    temp->SetMenu(true);
    temp->set_keepaspectratio(_isKeepaspectratio);
    temp->ResizeWidetSize(size, size);
    temp->ResizePredictdefects(size, size);
    */
}
// �Ҽ����˵�����ѡ��ť������
void treeViewBaseWidget::OnSignalMenuOperate(bool is, int op, void* imagelabel)
{
    //�ļ�����
    
}
void treeViewBaseWidget::setColorMap(std::map<int, QColor> colorMap, std::map<int, QColor> maskgraycolormap)
{
    _colorMap = colorMap;
    _maskgraycolormap = maskgraycolormap;
}