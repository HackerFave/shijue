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
    list << tr("文件列表")/*<< tr("value")*/;
    model->setHorizontalHeaderLabels(list);//列表头部
    model->horizontalHeaderItem(0);

    model->setColumnCount(list.size());//列数
    ui.treeView->header()->setDefaultSectionSize(100);
    ui.treeView->header()->setDefaultAlignment(Qt::AlignCenter);//居中
    ui.treeView->header()->setVisible(false);
    ui.treeView->setModel(model);
    ui.treeView->setColumnWidth(0, 100);//列宽
    ui.treeView->setEditTriggers(QTreeView::NoEditTriggers);//不可编辑
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
        //设置字体样式
        font - family: "Microsoft YaHei";//字体类型
        font - size: 25px;//字体大小，像素
        font - style: italic;//字体斜体样式，mormal不斜体
        font - weight:bold;//字体加粗样式，mormal不加粗
        color: #bdc8e2//字体颜色*/
    //ui.treeView->setAttribute(Qt::WA_MacShowFocusRect, 0);
    //--

    /*谭创*/

    //TC
   
   /* QHeaderView *hv = ui.treeView->header();
    hv->setSortIndicatorShown(true); //显示排序三角
    hv->setSectionsClickable(false);     //设置标题可以被单击
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
/** double转string保留小数位
 * @brief MainWindow::getDecimalPlaces
 * @param d
 * @param countBit 保留几位小数，默认3位
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

    //遍历文件夹及文件
    QString filter;

    QStringList listFilePath;

    foreach(QFileInfo fileinfo, fileInfoList)
    {
        //_filter = _fileinfo.completeSuffix();
        filter = fileinfo.suffix();    //后缀名
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
        QString title = listFilePath.at(i).right(listFilePath.at(i).length() - first - 1); //从右边截取
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
    InfoMapDataList.append(addLevel(tr("文件列表")));
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
                    childItem0->setToolTip(childVar->displayText0);//字段名
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
         //qDebug() << "图片形式加载时间1" << time1.elapsed() / 1000;
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
               // qDebug() << "图片形式加载时间" << time.elapsed() / 1000;
            }
        }
    }
    
}
void treeViewBaseWidget::addTreeView(QList<std::string> list)
{
    
    
    QList<InfoMapData*>InfoMapDataList;
    //1 

    InfoMapDataList.append(addLevel(tr("文件列表")));
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
                childItem0->setToolTip(childVar->displayText0);//字段名
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
        slotExpandedClicked(_isPressedSection);//放大
        _isPressedSection = !_isPressedSection;//
    }
}
//递归展开
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
    if (index.data().toString() == tr("文件列表")) {
        return;
    }
    emit signalIndexValue(index.data(), index.row());
    _indexSelect = index.row();
}
void treeViewBaseWidget::openImageFileDir(QString & filePath)
{
    
}
//递归遍历文件夹
QFileInfoList treeViewBaseWidget::GetFileList(QString path)
{
    QDir dir(path);
    QString filter;
    //列出dir(path)目录文件下所有文件和目录信息，存储到file_list容器
    QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);

    //列出dir(path)目录下所有子文件夹
    QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    //进行子文件夹folder_list递归遍历，将内容存入file_list容器
    for (int i = 0; i != folder_list.size(); i++)
    {
        QString name = folder_list.at(i).absoluteFilePath();
        QFileInfoList child_file_list = GetFileList(name);
        file_list.append(child_file_list);
    }
    //遍历文件夹及文件
    foreach(QFileInfo fileinfo, file_list)
    {
        //_filter = _fileinfo.completeSuffix();
        filter = fileinfo.suffix();    //后缀名
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
    //model->setData(item, QColor(240, 240, 240), Qt::BackgroundRole);//设置背景色
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
    
    QModelIndex curIndex = ui.treeView->indexAt(pos);      //当前点击的元素的index
    QModelIndex index = curIndex.sibling(curIndex.row(), 0); //该行的第1列元素的index
    OnlineTreeViewClick(index);
}
void treeViewBaseWidget::slotTreeMenu(const QPoint &pos)
{
    
    QModelIndex curIndex = ui.treeView->indexAt(pos);      //当前点击的元素的index
    QModelIndex index = curIndex.sibling(curIndex.row(), 0); //该行的第1列元素的index
    //emit signalIndexValue(curIndex.data(), curIndex.row());
    if (index.isValid())
    {
        if (index.parent() != ui.treeView->rootIndex())     //不是一级节点，因为只对二级节点往其他年级移动
        {
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
        }
    }
    
    setContextMenuPolicy(Qt::ActionsContextMenu);
}
void treeViewBaseWidget::triget_menu_delete()//删除
{
    emit signalDelImage(false);
}
void treeViewBaseWidget::triget_menu_nolabel()//无缺陷
{
    emit signalMenuOperate(false, (int)MenuOperateT::O_NOLABEL_T,NULL);
}
void treeViewBaseWidget::triget_menu_close_auto()//关闭工具条
{
    emit signalMenuOperate(false, (int)MenuOperateT::O_CLOSE_T, NULL);
}
void treeViewBaseWidget::triget_menu_open_auto()//打开工具条
{
    emit signalMenuOperate(false, (int)MenuOperateT::O_OPEN_T, NULL);
}
void treeViewBaseWidget::triget_menu_test()//测试
{
    emit signalMenuOperate(false, (int)MenuOperateT::O_TEST_T, NULL);
}
void treeViewBaseWidget::triget_menu_train_always()//总是
{
    //发送信号
    emit signalMenuOperate(false, (int)MenuOperateT::O_ALWAYS_T, NULL);
}
void treeViewBaseWidget::triget_menu_train_auto()//自动
{
    emit signalMenuOperate(false, (int)MenuOperateT::O_AUTO_T, NULL);
}
void treeViewBaseWidget::triget_menu_train_never()//从不
{
    emit signalMenuOperate(false, (int)MenuOperateT::O_NEVER_T, NULL);
}
int treeViewBaseWidget::getListIndex()
{
    return _indexSelect;
}
void treeViewBaseWidget::setCurrentImageData(QMap <std::string, int> imageData, QMap<std::string, trained_flag> flag_map)//当前映射表图片数据
{
    _mapImageData = imageData;
    _flagMap = flag_map;
}
void treeViewBaseWidget::LabelRegionUpdata(int idx , ImageLabel *label)
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
    label->ResizeWidetSize(this->width() - 20, this->width() - 130);
    label->ResizePredictdefects(this->width() - 20, this->width() - 130);
}
//接收左侧文件列表中控件点击事件
void treeViewBaseWidget::OnSignalSendId(bool is, void* item)
{
    ImageLabel* label = (ImageLabel*)item;
   // imageForm* Imageform = (imageForm*)form;
    QModelIndex index = _mapLabel.value(label);
    OnlineTreeViewClick(index);
}
// 右键“删除”按钮处理函数
void treeViewBaseWidget::OnSignalDelImage(bool ismain)
{
    
       
}
// 右键“菜单”按钮处理函数
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
// 右键“菜单”勾选按钮处理函数
void treeViewBaseWidget::OnSignalMenuOperate(bool is, int op, void* imagelabel)
{
    //文件加载
    
}
void treeViewBaseWidget::setColorMap(std::map<int, QColor> colorMap, std::map<int, QColor> maskgraycolormap)
{
    _colorMap = colorMap;
    _maskgraycolormap = maskgraycolormap;
}