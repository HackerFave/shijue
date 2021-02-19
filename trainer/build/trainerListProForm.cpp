#include "trainerListProForm.h"

trainerListProForm::trainerListProForm(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    ui.widget_imageList->setVisible(false);
    //connect(ui.widget_imageList, SIGNAL(signalDelImage(bool)), this, SLOT(OnSignalDelImage(bool)));
    //connect(ui.widget_imageList, SIGNAL(signalMenuOperate(bool, int, void*)), this, SLOT(OnSignalMenuOperate(bool, int, void*)));
    //connect(ui.widget_imageList, SIGNAL(signalIndexValue(QVariant, int)), this, SLOT(slotOpenImage(QVariant, int)));
    ui.selectDisplay_comBox->setFocusPolicy(Qt::NoFocus);
    connect(ui.selectDisplay_comBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnFilterDisplay(int)));
    connect(ui.toolButton_previousPage, SIGNAL(clicked()), this, SIGNAL(signalPreviousPage()));
    connect(ui.toolButton_nextPage, SIGNAL(clicked()), this, SIGNAL(signalNextPage()));
    //隐藏按钮
    ui.frame_switch->setVisible(false);
    QMap<int,QComboBox*> map;
    map.insert(0, ui.selectDisplay_comBox);
    initComboBoxStyle(map);
}

trainerListProForm::~trainerListProForm()
{

}
void trainerListProForm::OnFilterDisplay(int index)//筛选combox
{
    emit signalFilterDisplay(index);
}
void trainerListProForm::OnSignalDelImage(bool checked)//删除图片
{
    emit signalDelImage(checked);
}
void trainerListProForm::OnSignalMenuOperate(bool checked, int id, void* item)
{
    emit signalMenuOperate(checked,id, item);
}
void trainerListProForm::setLcdFileNumber(int value)
{
    ui.lcdFileNumber->display(value);
}
int trainerListProForm::getSelectDisplayIndex()
{
    return  ui.selectDisplay_comBox->currentIndex();
}
void trainerListProForm::addTreeViewData(QMap<int, std::string> map, const int indexStart, const int indexEnd, bool isOldFlag)
{
    //ui.widget_imageList->addTreeView(map, indexStart,indexEnd,isOldFlag);
    ui.listWidget->addListWidget(map, indexStart, indexEnd, isOldFlag);
}
void trainerListProForm::delTreeViewData()
{
    //ui.widget_imageList->delTreeViewData();
    ui.listWidget->clearListWidget();
}
void trainerListProForm::setWidget_imageListVisible(bool checked)
{
    //ui.widget_imageList->setVisible(checked);
    ui.listWidget->setVisible(checked);
}
QString trainerListProForm::getWidget_imageListCurrentData(int index)
{
   //return ui.widget_imageList->getCurrentData(index).toString().simplified();
   return ui.listWidget->getCurrentData(index).toString().simplified();
}
QStringList trainerListProForm::getAllSelectedRows()
{
    //return ui.widget_imageList->getAllSelectedRow();
    return ui.listWidget->getAllSelectedRow();
}
void trainerListProForm::setWidget_ImageListFixedSize(QSize size)
{
   // ui.widget_imageList->setFixedSize(size);   
    ui.listWidget->setFixedSize(size);
}
QComboBox * trainerListProForm::getComboBox()
{
    return ui.selectDisplay_comBox;
}
QVariantList trainerListProForm::getWidget_imageListCurrentData(QStringList list)
{
   //return ui.widget_imageList->getCurrentData(list); 
   return ui.listWidget->getCurrentData(list);
}
void trainerListProForm::setCurrentIndexNum(int index)
{
   // ui.widget_imageList->setCurrentIndexNum(index);
    ui.listWidget->setCurrentIndexNum(index);
}

treeViewBaseWidget * trainerListProForm::getWidget_ImageForm()
{
    return ui.widget_imageList;
}
baseListWidget * trainerListProForm::getbaseListWidget()
{
    return ui.listWidget;
}
void trainerListProForm::FileListClear()
{
    ui.listWidget->clearListWidget();
    ui.lcdFileNumber->display(0);
}
void trainerListProForm::setPageText(const QString &text)
{
    ui.label_pageNum->setText(text);
}
int trainerListProForm::getListIndex()
{
    //return ui.widget_imageList->getListIndex();
    return ui.listWidget->getListIndex();
}
int trainerListProForm::getCurrentRowCount()
{  
    //return ui.widget_imageList->getCurrentRowCount();
    return ui.listWidget->getCurrentRowCount();
}
void trainerListProForm::setCurrentImageData(QMap <std::string, int> imageData, QMap<std::string, trained_flag> flag_map)//当前映射表图片数据
{
    //ui.widget_imageList->setCurrentImageData(imageData,flag_map);
    ui.listWidget->setCurrentImageData(imageData, flag_map);
}
void trainerListProForm::setColorMap(std::map<int, QColor> colorMap, std::map<int, QColor> maskgraycolormap)
{
   // ui.widget_imageList->setColorMap(colorMap, maskgraycolormap);
    ui.listWidget->setColorMap(colorMap, maskgraycolormap);
}
void trainerListProForm::initComboBoxStyle(QMap<int, QComboBox *> comBoxMap)
{
    for (size_t j = 0; j < comBoxMap.size(); j++)
    {
        QStandardItemModel *pItemModel = qobject_cast<QStandardItemModel*>(comBoxMap.value(j)->model());
        //修改某项文本
        for (int i = 0; i < comBoxMap.value(j)->count(); i++)
        {
            pItemModel->item(i)->setForeground(QColor(255, 255, 255));            //修改某项文本颜色
            pItemModel->item(i)->setBackground(QColor(90, 90, 90));    //修改某项背景颜色    
            pItemModel->item(i)->setSizeHint(QSize(180, 30));
            pItemModel->item(i)->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);    //修改某项文本对齐方式
        }
    }

}