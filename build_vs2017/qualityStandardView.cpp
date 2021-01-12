#include "qualityStandardView.h"

qualityStandardView::qualityStandardView(QWidget *parent)
    : QTableView(parent)
{
    ui.setupUi(this);
    _filePath = publicClass::instance()->getAppPath()+"/setting/setting.json";
    initTableView();
}

qualityStandardView::~qualityStandardView()
{
}
void qualityStandardView::initTableView()
{
    //this->setStyleSheet("QTableView{selection-background-color:transparent;}");

    QMap<QString, QVariant> map = publicClass::instance()->readJson(_filePath);
    this->setMinimumWidth(750);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    // this->setSelectionBehavior(QAbstractItemView::SelectRows);
    QStandardItemModel* item_model = new QStandardItemModel(8, 5);
    this->setModel(item_model);
    
    //设置表格的单元为只读属性，即不能编辑        
    this->setEditTriggers(QAbstractItemView::AllEditTriggers);
    //如果你用在QTableView中使用右键菜单，需启用该属性        
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    //this->verticalHeader()->setDefaultSectionSize(30);
    //this->horizontalHeader()->setDefaultSectionSize(300);
    
    connect(this, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(slotDoubleClicked(const QModelIndex &)));
   // this->horizontalHeader()->hide();                       // 隐藏水平表头
    this->verticalHeader()->hide();                         // 隐藏垂直表头
    this->horizontalHeader()->setObjectName("hHeader");
    this->verticalHeader()->setObjectName("vHeader");
    this->horizontalHeader()->setHighlightSections(true);
    this->horizontalHeader()->setStretchLastSection(true);  // 伸缩最后一列
   // this->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   // this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->horizontalHeader()->setStyleSheet("QHeaderView::section{background:rgb(0, 85, 127);color: white;font: bold 10pt;}"); //设置表头背景色
    this->setColumnWidth(0, 180);
    this->setColumnWidth(1, 300);
    this->setColumnWidth(2, 300);
    this->setColumnWidth(3, 300);
    this->setColumnWidth(4, 300);
    for (int i = 0; i < item_model->rowCount(); i++) {
        this->setRowHeight(i, 60);
    }
     QStringList column, row;
     column << "缺陷" << "判定标准" << "判定结果" << "状态"<<"报警";
     //row << "row 1" << "row 2" << "row 3" << "row 4";
     item_model->setHorizontalHeaderLabels(column);                // 设置水平表头标签
    // item_model->setVerticalHeaderLabels(row);                     // 设置垂直表头标签
     

     // 添加item到model
    for (int i = 0; i < this->model()->rowCount(); ++i) {
        for (int j = 0; j < this->model()->columnCount(); ++j)
            item_model->setItem(i, j, new QStandardItem());
    }
    item_model->item(0, 0)->setText("干纱");
    item_model->item(1, 0)->setText("开裂");
    item_model->item(2, 0)->setText("黄斑");
    item_model->item(3, 0)->setText("虚化");
    item_model->item(4, 0)->setText("污物");
    item_model->item(5, 0)->setText("玻纤分布不均");
    item_model->item(6, 0)->setText("玻纤弯折");
    //item_model->item(0, 1)->setText(map.value("gansha").toString());//干纱
    QList<QVariantList> listAllItem;
    QVariantList list1;
    list1 << 0 << 0 << 1 << 7 << "在线检测记录";
    listAllItem << list1;
    for (int i = 1; i < item_model->rowCount(); i++) {
        QVariantList list;
        if (i == 1) {
            list << i << 5 << 1 << 2 << "描述*";
        }
        list << i << 5 << 1 << 2;
        listAllItem.append(list);
    }
   // editTable(listAllItem);

    qualityStandardItem *form = new qualityStandardItem;
    qualityStandardItem *form_gansha = new qualityStandardItem;

    qualityStandardItem *form_huangban = new qualityStandardItem;

    form->setFixedSize(QSize(300, 60));
    form_gansha->setFixedSize(QSize(300, 60));
    QString title_gansha = "干纱:宽"; QString range_gansha = "mm";
    form_gansha->setTitleRange(0, title_gansha, range_gansha);
    QString title2_gansha = "长"; QString range2_gansha = "cm";
    form_gansha->setTitleRange(1, title2_gansha, range2_gansha);
    form_gansha->setSpinBoxValue(map.value("gansha_width").toString(), map.value("gansha_height").toString());
    this->setIndexWidget(item_model->index(0, 1), form_gansha);

    QString title = "缝隙:宽"; QString range = "mm";
    form->setTitleRange(0, title, range);
    QString title2 = "长"; QString range2 = "cm";
    form->setTitleRange(1, title2, range2);
    form->setSpinBoxValue(map.value("kailie_width").toString(), map.value("kailie_height").toString());
    this->setIndexWidget(item_model->index(1, 1), form);
    //qualityStandardComboBox *combox = new qualityStandardComboBox;
    yellowDirtItem * dirtItem = new yellowDirtItem;

   // QStringList listComItem;
    //listComItem << "轻微黄斑（浅色）" << "严重黄斑（深色）";
   // for (int i = 0; i < listComItem.size(); i++) {
       // combox->addItemRow(listComItem.at(i));
    //}
    QColor col;
    if (map.value("huangban_color").toString().split(',').size() >= 3) {
        qreal r = map.value("huangban_color").toString().split(',').at(0).toInt();
        qreal g = map.value("huangban_color").toString().split(',').at(1).toInt();
        qreal b = map.value("huangban_color").toString().split(',').at(2).toInt();
        col = QColor(r, g, b);
    }
    dirtItem->setLabelColor(col);
    this->setIndexWidget(item_model->index(2, 1), /*dirtItem*/NULL);
    QList<QStringList> list;
    QStringList listChild1;
    listChild1 << "虚化宽度" << "mm"<<map.value("xuhua_width").toString();//虚化
    QStringList listChild2;
    listChild2 << "面积" << " mm²"<<map.value("wuwu_area").toString();//污物
    QStringList listChild3;
    listChild3 << "间隙" << "mm" <<map.value("bujun_width").toString();//玻纤分布不均
    QStringList listChild4;
    listChild4 << "弯曲度" << "mm"<< map.value("wanzhe_curvature").toString();//玻纤弯折
    list << listChild1 << listChild2 << listChild3 << listChild4;
    for (int i = 0; i < list.size(); i++) {
        qualityStandardItem *form = new qualityStandardItem;
        form->setLabelFixSize(0, QSize(100, 60));
        form->setVisibleFrame(1, false);
        form->setTitleRange(0, list.at(i).at(0), list.at(i).at(1));
        form->setSpinBoxValue(list.at(i).at(2), "");
        this->setIndexWidget(item_model->index(i+3, 1), form);
    }
    QStandardItemModel *model = static_cast<QStandardItemModel*>(this->model());
    for (int i = 0; i < model->rowCount(); i++) {
        for (int j = 0; j < model->columnCount(); j++) {
            QFont font;
            font.setFamily("Times");//字体
            //font.setPixelSize(25);//文字像素大小
            font.setPointSize(10);//文字大小
           // font.setUnderline(true);//下划线
            //font.setStrikeOut(true);//中划线
            //font.setOverline(true);//上划线
            //font.setItalic(true);//斜体
            font.setBold(true);//粗体
            // font.setStyle(QFont::StyleOblique);
            // font.setCapitalization(QFont::Capitalize);//首字母大写
           // model->item(i, j)->setForeground(QBrush(Qt::green));
            model->item(i, j)->setTextAlignment(Qt::AlignCenter);
            model->item(i, j)->setFont(font);
            model->item(i, 0)->setFlags(Qt::ItemIsEnabled);
        }
    }
    this->setShowGrid(true);                               // 隐藏网格线
    this->setFocusPolicy(Qt::StrongFocus);                      // 去除当前Cell周边虚线框
    this->setAlternatingRowColors(true);                    // 开启隔行异色
}
//获取json文件table数据
QMap<QString, QMap<QString, QVariant>> qualityStandardView::getTableData()
{
    QStandardItemModel *model = static_cast<QStandardItemModel*>(this->model());
    QMap<QString, QMap<QString, QVariant>> mapSettingData;
    QMap<QString, QVariant> list1;
    QMap<QString, QVariant> list2;
    QMap<QString, QVariant> list3;
    QMap<QString, QVariant> list4;
    QMap<QString, QVariant> list5;
    QMap<QString, QVariant> list6;
    QMap<QString, QVariant> list7;
    QMap<QString, QVariant> list8;
    QMap<QString, QVariant> list9;
    QMap<QString, QVariant> list10;
    QMap<QString, QVariant> list11;
    list1.insert("updatetime", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    mapSettingData.insert("updatetime", list1);
    //干纱 宽度
    list2.insert("gansha_width", static_cast<qualityStandardItem*>(this->indexWidget(model->index(0, 1)))->getSpinBoxValue(0));
    mapSettingData.insert("gansha_width", list2);
    //干纱 高度
    list10.insert("gansha_height", static_cast<qualityStandardItem*>(this->indexWidget(model->index(0, 1)))->getSpinBoxValue(1));
    mapSettingData.insert("gansha_height", list10);

    //开裂宽度
    list3.insert("kailie_width", static_cast<qualityStandardItem*>(this->indexWidget(model->index(1, 1)))->getSpinBoxValue(0));
    //开裂高度
    list11.insert("kailie_height", static_cast<qualityStandardItem*>(this->indexWidget(model->index(1, 1)))->getSpinBoxValue(1));
    mapSettingData.insert("kailie_width", list3);
    mapSettingData.insert("kailie_height", list11);
    //黄斑
   // list4.insert("axisNumX", "0");
    /*qreal r = static_cast<yellowDirtItem*>(this->indexWidget(model->index(2, 1)))->getLabelColor().red();
    qreal g = static_cast<yellowDirtItem*>(this->indexWidget(model->index(2, 1)))->getLabelColor().green();
    qreal b = static_cast<yellowDirtItem*>(this->indexWidget(model->index(2, 1)))->getLabelColor().blue();
    QString rbgStr = tr("%1,%2,%3").arg(r).arg(g).arg(b);
    */
    list4.insert("huangban_color", /*rbgStr*/model->item(2,1)->text());
    mapSettingData.insert("huangban_color", list4);
    //虚化宽度
    list5.insert("xuhua_width", static_cast<qualityStandardItem*>(this->indexWidget(model->index(3, 1)))->getSpinBoxValue(0));
    mapSettingData.insert("xuhua_width", list5);
    //污物面积
    list6.insert("wuwu_area", static_cast<qualityStandardItem*>(this->indexWidget(model->index(4, 1)))->getSpinBoxValue(0));
    mapSettingData.insert("wuwu_area", list6);
    //玻纤分布不均
    list7.insert("bujun_width", static_cast<qualityStandardItem*>(this->indexWidget(model->index(5, 1)))->getSpinBoxValue(0));
    mapSettingData.insert("bujun_width", list7);
    //玻纤弯折
    list8.insert("wanzhe_curvature", static_cast<qualityStandardItem*>(this->indexWidget(model->index(6, 1)))->getSpinBoxValue(0));
    mapSettingData.insert("wanzhe_curvature", list8);

    return mapSettingData;
}
void qualityStandardView::setTableData(QMap<QString, QVariant> map)
{
    QStandardItemModel *model = static_cast<QStandardItemModel*>(this->model());
    QColor col;
    if (map.value("huangban_color").toString().split(',').size() >= 3) {
        qreal r = map.value("huangban_color").toString().split(',').at(0).toInt();
        qreal g = map.value("huangban_color").toString().split(',').at(1).toInt();
        qreal b = map.value("huangban_color").toString().split(',').at(2).toInt();
        col = QColor(r, g, b);
    }
    qobject_cast<qualityStandardItem*>(this->indexWidget(model->index(0, 1)))
        ->setSpinBoxValue(map.value("gansha_width").toString(), map.value("gansha_height").toString());
    //model->item(0, 1)->setText(map.value("gansha").toString());//干纱
   /* qobject_cast<yellowDirtItem*>(this->indexWidget(model->index(2, 1)))->setLabelColor(col);*/
    model->item(2, 1)->setText(map.value("huangban_color").toString());

    qobject_cast<qualityStandardItem*>(this->indexWidget(model->index(1, 1)))
        ->setSpinBoxValue(map.value("kailie_width").toString(), map.value("kailie_height").toString());
    qobject_cast<qualityStandardItem*>(this->indexWidget(model->index(3, 1)))
        ->setSpinBoxValue(map.value("xuhua_width").toString(), "");
    qobject_cast<qualityStandardItem*>(this->indexWidget(model->index(4, 1)))
        ->setSpinBoxValue(map.value("wuwu_area").toString(), "");
    qobject_cast<qualityStandardItem*>(this->indexWidget(model->index(5, 1)))
        ->setSpinBoxValue(map.value("bujun_width").toString(), "");
    qobject_cast<qualityStandardItem*>(this->indexWidget(model->index(6, 1)))
        ->setSpinBoxValue(map.value("wanzhe_curvature").toString(), "");  
}
void qualityStandardView::slotDoubleClicked(const QModelIndex & index)
{
    if (index.row() == 0 && index.column() == 1) {
        tableViewEditItemForm  * form = new tableViewEditItemForm;
        QStandardItemModel *model = static_cast<QStandardItemModel*>(this->model());
        form->setTitle("干纱", index.row(), index.column());
        form->show();
        //QObject::connect(form, SIGNAL(signalItemData(int, int, QString)), this, SLOT(slotEditItemData(int, int, QString)));
    }
}
void qualityStandardView::slotEditItemData(int row, int column, QString text)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(this->model());

    QFont font;
    font.setFamily("Times");//字体
 //font.setPixelSize(25);//文字像素大小
    font.setPointSize(10);//文字大小
    font.setUnderline(true);//下划线
  //font.setStrikeOut(true);//中划线
  //font.setOverline(true);//上划线
  //font.setItalic(true);//斜体
    font.setBold(true);//粗体
  // font.setStyle(QFont::StyleOblique);
  // font.setCapitalization(QFont::Capitalize);//首字母大写
   // model->item(row, column)->setForeground(QBrush(Qt::green));
    model->item(row, column)->setTextAlignment(Qt::AlignCenter);
    model->item(row, column)->setFont(/*QFont("Times", 10, QFont::Black)*/font);
    model->item(row, column)->setText(text);

}
void qualityStandardView::mousePressEvent(QMouseEvent *event)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(this->model());
    if (event->button() == Qt::LeftButton)
    {
        setCurrentIndex(QModelIndex());
        QTableView::mousePressEvent(event);
        QModelIndex index = currentIndex();
        if (index.row() < 0 && index.column() < 0)
            return;
       // emit leftClicked(index);
        //  qDebug()<<"左击一下";
    }
}