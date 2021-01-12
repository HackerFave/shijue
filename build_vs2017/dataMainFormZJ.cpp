#include "dataMainFormZJ.h"
#include <QThreadPool>
#include "postprocess.h"
#include <windows.h>
#include <math.h>  
using namespace std;
dataMainFormZJ::dataMainFormZJ(QWidget *parent, QRect rect)
    : QWidget(parent)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    ui.setupUi(this);
    this->setFocusPolicy(Qt::StrongFocus);
    p_sqlOperation = new sqlOperation(0);
    p_sqlOperationJson = new sqlOperation(0);
    p_sqlOperationTestRecord = new sqlOperation(0);
    p_sqlOperationReportImg = new sqlOperation(0);
    p_sqlOperationOrder = new sqlOperation(0);
    p_sqlOperationLastRoll = new sqlOperation(0);
    p_sqlOperationEditOrder = new sqlOperation(0);
    p_sqlOperationTem = new sqlOperation(0);
    if (!p_sqlOperation->Init("127.0.0.1", "root", "123456", "zhongji","type1"))
    {
        QMessageBox::warning(nullptr, tr("错误"), tr("初始化数据库失败"), QMessageBox::Apply);
    } 
    if (!p_sqlOperationJson->Init("127.0.0.1", "root", "123456", "zhongji", "type2"))
    {
        QMessageBox::warning(nullptr, tr("错误"), tr("初始化数据库失败"), QMessageBox::Apply);
    }
    if (!p_sqlOperationTestRecord->Init("127.0.0.1", "root", "123456", "zhongji", "type3"))
    {
        QMessageBox::warning(nullptr, tr("错误"), tr("初始化数据库失败"), QMessageBox::Apply);
    }
    if (!p_sqlOperationReportImg->Init("127.0.0.1", "root", "123456", "zhongji", "type4"))
    {
        QMessageBox::warning(nullptr, tr("错误"), tr("初始化数据库失败"), QMessageBox::Apply);
    }
    if (!p_sqlOperationOrder->Init("127.0.0.1", "root", "123456", "zhongji", "type5"))
    {
        QMessageBox::warning(nullptr, tr("错误"), tr("初始化数据库失败"), QMessageBox::Ok);
    }
    if (!p_sqlOperationLastRoll->Init("127.0.0.1", "root", "123456", "zhongji", "type_LastRoll"))
    {
        QMessageBox::warning(nullptr, tr("错误"), tr("初始化数据库失败"), QMessageBox::Ok);
    }
    if (!p_sqlOperationEditOrder->Init("127.0.0.1", "root", "123456", "zhongji", "type_EditOrder"))
    {
        QMessageBox::warning(nullptr, tr("错误"), tr("初始化数据库失败"), QMessageBox::Ok);
    }
    if (!p_sqlOperationTem->Init("127.0.0.1", "root", "123456", "zhongji", "type_Tem"))
    {
        QMessageBox::warning(nullptr, tr("错误"), tr("初始化数据库失败"), QMessageBox::Ok);
    }
    setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    
    //重置大小
    this->resize(rect.width(), rect.height());
    qApp->processEvents();
    initTableView();//初始化生产工艺记录表
    setTableViewEditFlags();//生产工艺记录表
    QMap<QString, QStringList> map;
    QStringList list;
    list << "1区" << "2区" << "3区" << "4区" << "5区" << "6区" << "7区" << "8区" << "9区";
    map.insert("分区", list);
    setRowData(4, map);
    QMap<QString, QStringList> map1;
    QStringList list1;
    list1 << "10区" << "11区" << "12区" << "13区" << "14区" << "15区" << "16区" << "17区" << "18区";
    map1.insert("分区", list1);
    setRowData(10, map1);
    QMap<QString, QStringList> map2;
    QStringList list2;
    list2 << "19区" << "20区" << " " << "" << "" << "" << "" << "复合1" << "复合2";
    map2.insert("分区", list2);
    setRowData(16, map2);
    //json文件列表
    ui.listWidget_settingFile->setSelectionMode(QAbstractItemView::SingleSelection);//json文件列表控件
    connect(ui.listWidget_settingFile, SIGNAL(currentRowChanged(int)), this, SLOT(OnListWidgetCurrentListChanged(int)));
    connect(ui.listWidget_settingFile, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(ItemClicked(QListWidgetItem*)));
    connect(ui.listWidget_settingFile, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(ItemDoubleClicked(QListWidgetItem*)));
    //订单信息
    connect(ui.tableView_stripQuality, SIGNAL(signalRefreshVolumeNum(bool,int)), this, SLOT(slotRefreshVolumeNum(bool,int)));
	connect(ui.tableView_testRecord, SIGNAL(signalComboxPageUpdate(const QString&, int)), this, SLOT(slotComboxPageIndex(const QString&, int)));
	connect(ui.spinBox_page, SIGNAL(valueChanged(int)), this, SLOT(slotSpinBoxUpDatePage(int)));
	connect(ui.tableView_testRecord, SIGNAL(signalOldDataClear(bool)), this, SLOT(slotOldDataClear(bool)));
	connect(ui.tableView_testRecord, SIGNAL(signalGetSpinBoxValue()), this, SLOT(slotGetSpinBoxValue()));//获取当前页数
	initVolumeNum();//读取数据，获取三天内的所有卷号
    ui.comboBox_juanhao->setCurrentIndex(0);
    connect(ui.comboBox_juanhao, SIGNAL(currentTextChanged(const QString &)), this, SLOT(slotChangeItem(const QString &)));
    ui.tableView_stripQuality->setItemData(1, 8, ui.comboBox_juanhao->currentText(),
        qobject_cast<QStandardItemModel*>(ui.tableView_stripQuality->model()));
    //slotChangeItem(ui.comboBox_juanhao->currentText());
    initTempArea();//初始化温度显示区域
    QMap<int, QComboBox*> mapBox;
    mapBox.insert(0, ui.comboBox_juanhao);
    mapBox.insert(1, ui.comboBox_value);
	mapBox.insert(2, ui.comboBox_banci);//班次
    publicClass::instance()->initComboBoxStyle(mapBox);
    connect(dataSql::getInstance(), SIGNAL(sendDataSql(const std::map<QString, QString>))
        , this, SLOT(slotAddDataSql(const std::map<QString, QString>)));
    connect(this, SIGNAL(signalThresholdJsonPath(const QString &)),
        dataSql::getInstance(), SLOT(slotThresholdJson(const QString &)));
    _timerAddRecord = new QTimer;
    connect(_timerAddRecord, SIGNAL(timeout()), this, SLOT(slotTimerAddRecord()));
    _timerAddRecord->start(3000);
    //隐藏
    ui.toolButton_saveData_test->setVisible(false);
    ui.toolButton_addRecord_test->setVisible(false);
    ui.toolButton_add->setVisible(false);
	ui.toolButton_test->setVisible(false);
	ui.toolButton_pdf->setVisible(false);
	
	ui.toolButton_previous->setVisible(false);
    ui.toolButton_next->setVisible(false);
    //ui.comboBox_page->setVisible(false);
}

dataMainFormZJ::~dataMainFormZJ()
{
    p_sqlOperation->CloseDB();
    p_sqlOperationJson->CloseDB();
    p_sqlOperationTestRecord->CloseDB();
    p_sqlOperationReportImg->CloseDB();
    p_sqlOperationOrder->CloseDB();
    if (p_sqlOperation)
    {
        delete p_sqlOperation;
    }
    if (p_sqlOperationJson)
    {
        delete p_sqlOperationJson;
    }
    if (p_sqlOperationTestRecord) {
        delete p_sqlOperationTestRecord;
    }
    if (p_sqlOperationReportImg) {
        delete p_sqlOperationReportImg;
    }
    if (p_sqlOperationOrder) {
        delete p_sqlOperationOrder;
    }
    if (p_sqlOperationLastRoll) {
        p_sqlOperationLastRoll->CloseDB();
        delete p_sqlOperationLastRoll;
    }
    if (p_sqlOperationEditOrder) {
        p_sqlOperationEditOrder->CloseDB();
        delete p_sqlOperationEditOrder;
    }
    if (_timerAddRecord) {
        _timerAddRecord->stop();
        delete _timerAddRecord;
    }
}
//初始化温度区域表格
void dataMainFormZJ::initTableView()
{
   // setFixedSize(600, 300);                                       // 设置窗体大小
    QHBoxLayout * alayout = new QHBoxLayout(this);
   // ui.tableView = new QTableView;
    //alayout->addWidget(table_view);
    ui.tableView_record->setSelectionMode(QAbstractItemView::SingleSelection);
    P_proCategoryItem = new proCategoryItem(this);
    QStandardItemModel* item_model = new QStandardItemModel(23, 10);
   ui.tableView_record->setModel(item_model);
   //设置表格的单元为只读属性，即不能编辑        
   //ui.tableView_record->setEditTriggers(QAbstractItemView::NoEditTriggers);
   ui.tableView_record->setEditTriggers(QAbstractItemView::AllEditTriggers);
   //如果你用在QTableView中使用右键菜单，需启用该属性        
   ui.tableView_record->setContextMenuPolicy(Qt::CustomContextMenu);

   //connect(ui.tableView_record, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(slotDoubleClicked(const QModelIndex &)));
   ui.tableView_record->horizontalHeader()->hide();                       // 隐藏水平表头
   ui.tableView_record->verticalHeader()->hide();                         // 隐藏垂直表头
   //ui.tableView->horizontalHeader()->setStretchLastSection(true);  // 伸缩最后一列
   ui.tableView_record->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   ui.tableView_record->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   /* QStringList column, row;
    column << "Column 1" << "Column 2" << "Column 3" << "Column 4";
    row << "row 1" << "row 2" << "row 3" << "row 4";
    item_model->setHorizontalHeaderLabels(column);                // 设置水平表头标签
    item_model->setVerticalHeaderLabels(row);                     // 设置垂直表头标签
    */

    // 添加item到model
   for (int i = 0; i < ui.tableView_record->model()->rowCount(); ++i) {
      for (int j = 0; j < ui.tableView_record->model()->columnCount(); ++j)
           item_model->setItem(i, j, new QStandardItem());
    }
       
   ui.tableView_record->setSpan(0, 0, 1, 2);		//合并第0行和第1行22个单元格到1个单元格
   QStandardItem *item = new QStandardItem;
   item->setTextAlignment(Qt::AlignCenter);
   item->setFont(QFont("Times", 10, QFont::Black));
   item->setText("生产设备");
   item_model->setItem(0, 0, item);

   ui.tableView_record->setSpan(0, 2, 1, 2);
   QString num = "7";
   item_model->setItem(0, 2, new QStandardItem(QString("7号线")));
   //<font color = green>7< / font>
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
   //item_model->item(0, 2)->setForeground(QBrush(Qt::green));
   item_model->item(0, 2)->setTextAlignment(Qt::AlignCenter);
   item_model->item(0, 2)->setFont(/*QFont("Times", 10, QFont::Black)*/font);
   ui.tableView_record->setSpan(0, 4, 1, 2);
   item_model->setItem(0, 4, new QStandardItem("生产日期"));
   item_model->item(0, 4)->setTextAlignment(Qt::AlignCenter);
   item_model->item(0, 4)->setFont(QFont("Times", 10, QFont::Black));
   ui.tableView_record->setSpan(0, 6, 1, 4);
   QDateTime current_date_time = QDateTime::currentDateTime();
   QString current_date = current_date_time.toString("yyyy-MM-dd");//hh:mm:ss.zzz ddd
   item_model->setItem(0, 6, new QStandardItem(current_date));
   item_model->item(0, 6)->setTextAlignment(Qt::AlignCenter);
   item_model->item(0, 6)->setForeground(QBrush(QColor(255, 0, 0)));
   item_model->item(0, 6)->setFlags(Qt::ItemIsEnabled);
   ui.tableView_record->setSpan(1, 0, 1, 3);
   item_model->setItem(1, 0, new QStandardItem("生产配速m/min"));
   item_model->item(1, 0)->setTextAlignment(Qt::AlignCenter);
   item_model->item(1, 0)->setFont(QFont("Times", 10, QFont::Black));

   ui.tableView_record->setSpan(1, 3, 1, 3);
   item_model->setItem(1, 3, new QStandardItem("8.5 m/min"));
   item_model->item(1, 3)->setTextAlignment(Qt::AlignCenter);
   item_model->item(1, 3)->setFont(QFont("Times", 10, QFont::Black));

   ui.tableView_record->setSpan(1, 6, 1, 4);
   item_model->setItem(1, 6, new QStandardItem("产品种类"));
   item_model->item(1, 6)->setTextAlignment(Qt::AlignCenter);
   item_model->item(1, 6)->setFont(QFont("Times", 10, QFont::Black));

   ui.tableView_record->setSpan(2, 0, 1, 2);
   item_model->setItem(2, 0, new QStandardItem("复合1压力/MPa"));
   item_model->item(2, 0)->setTextAlignment(Qt::AlignCenter);
   item_model->item(2, 0)->setFont(QFont("Times", 10, QFont::Black));

   ui.tableView_record->setSpan(2, 3, 1, 2);
   item_model->setItem(2, 3, new QStandardItem("复合2压力/MPa"));
   item_model->item(2, 3)->setTextAlignment(Qt::AlignCenter);
   item_model->item(2, 3)->setFont(QFont("Times", 10, QFont::Black));

   item_model->setItem(2, 6, new QStandardItem("白班记录员"));
   item_model->item(2, 6)->setTextAlignment(Qt::AlignCenter);
   item_model->item(2, 6)->setFont(QFont("Times", 10, QFont::Black));

   item_model->setItem(2, 7, new QStandardItem("  "));
  // item_model->item(2, 7)->setForeground(QBrush(Qt::green));
   item_model->item(2, 7)->setTextAlignment(Qt::AlignCenter);
   item_model->item(2, 7)->setFont(font);

   item_model->setItem(2, 8, new QStandardItem("夜班记录员"));
   item_model->item(2, 8)->setTextAlignment(Qt::AlignCenter);
   item_model->item(2, 8)->setFont(QFont("Times", 10, QFont::Black));


   item_model->setItem(2, 9, new QStandardItem("  "));
   //item_model->item(2, 9)->setForeground(QBrush(Qt::green));
   item_model->item(2, 9)->setTextAlignment(Qt::AlignCenter);
   item_model->item(2, 9)->setFont(font);

   ui.tableView_record->setSpan(3, 0, 1, 10);
   item_model->setItem(3, 0, new QStandardItem("加热区实际温度/℃"));
   //item_model->item(3, 0)->setForeground(QBrush(QColor(255, 0, 0)));
   item_model->item(3, 0)->setTextAlignment(Qt::AlignCenter);
   item_model->item(3, 0)->setFont(QFont("Times", 10, QFont::Black));
   ui.tableView_record->setSpan(9, 0, 1, 10);
   item_model->setItem(9, 0, new QStandardItem("加热区实际温度/℃"));
   //item_model->item(3, 0)->setForeground(QBrush(QColor(255, 0, 0)));
   item_model->item(9, 0)->setTextAlignment(Qt::AlignCenter);
   item_model->item(9, 0)->setFont(QFont("Times", 10, QFont::Black));

   ui.tableView_record->setSpan(15, 0, 1, 10);
   item_model->setItem(15, 0, new QStandardItem("加热区实际温度/℃"));
   //item_model->item(3, 0)->setForeground(QBrush(QColor(255, 0, 0)));
   item_model->item(15, 0)->setTextAlignment(Qt::AlignCenter);
   item_model->item(15, 0)->setFont(QFont("Times", 10, QFont::Black));

   ui.tableView_record->setSpan(21, 0, 2, 1);
   item_model->setItem(21, 0, new QStandardItem("备注"));
   //item_model->item(3, 0)->setForeground(QBrush(QColor(255, 0, 0)));
   item_model->item(21, 0)->setTextAlignment(Qt::AlignCenter);
   item_model->item(21, 0)->setFont(QFont("Times", 10, QFont::Black));
   ui.tableView_record->setSpan(21, 1, 2, 9);

   item_model->setItem(21, 1, new QStandardItem("     "));
  // item_model->item(15, 1)->setForeground(QBrush(Qt::green));
   item_model->item(21, 1)->setTextAlignment(Qt::AlignCenter);
   item_model->item(21, 1)->setFont(font);

   item_model->setItem(5, 0, new QStandardItem("白班1"));
   //item_model->item(15, 1)->setForeground(QBrush(Qt::green));
   item_model->item(5, 0)->setTextAlignment(Qt::AlignCenter);
   item_model->item(5, 0)->setFont(QFont("Times", 10, QFont::Black));
   item_model->setItem(6, 0, new QStandardItem("白班2"));
   //item_model->item(15, 1)->setForeground(QBrush(Qt::green));
   item_model->item(6, 0)->setTextAlignment(Qt::AlignCenter);
   item_model->item(6, 0)->setFont(QFont("Times", 10, QFont::Black));
   item_model->setItem(7, 0, new QStandardItem("夜班1"));
   //item_model->item(15, 1)->setForeground(QBrush(Qt::green));
   item_model->item(7, 0)->setTextAlignment(Qt::AlignCenter);
   item_model->item(7, 0)->setFont(QFont("Times", 10, QFont::Black));
   item_model->setItem(8, 0, new QStandardItem("夜班2"));
   //item_model->item(15, 1)->setForeground(QBrush(Qt::green));
   item_model->item(8, 0)->setTextAlignment(Qt::AlignCenter);
   item_model->item(8, 0)->setFont(QFont("Times", 10, QFont::Black));


   item_model->setItem(11, 0, new QStandardItem("白班1"));
   //item_model->item(15, 1)->setForeground(QBrush(Qt::green));
   item_model->item(11, 0)->setTextAlignment(Qt::AlignCenter);
   item_model->item(11, 0)->setFont(QFont("Times", 10, QFont::Black));
   item_model->setItem(12, 0, new QStandardItem("白班2"));
   //item_model->item(15, 1)->setForeground(QBrush(Qt::green));
   item_model->item(12, 0)->setTextAlignment(Qt::AlignCenter);
   item_model->item(12, 0)->setFont(QFont("Times", 10, QFont::Black));
   item_model->setItem(13, 0, new QStandardItem("夜班1"));
   //item_model->item(15, 1)->setForeground(QBrush(Qt::green));
   item_model->item(13, 0)->setTextAlignment(Qt::AlignCenter);
   item_model->item(13, 0)->setFont(QFont("Times", 10, QFont::Black));
   item_model->setItem(14, 0, new QStandardItem("夜班2"));
   //item_model->item(15, 1)->setForeground(QBrush(Qt::green));
   item_model->item(14, 0)->setTextAlignment(Qt::AlignCenter);
   item_model->item(14, 0)->setFont(QFont("Times", 10, QFont::Black));

   item_model->setItem(17, 0, new QStandardItem("白班1"));
   //item_model->item(15, 1)->setForeground(QBrush(Qt::green));
   item_model->item(17, 0)->setTextAlignment(Qt::AlignCenter);
   item_model->item(17, 0)->setFont(QFont("Times", 10, QFont::Black));
   item_model->setItem(18, 0, new QStandardItem("白班2"));
   //item_model->item(15, 1)->setForeground(QBrush(Qt::green));
   item_model->item(18, 0)->setTextAlignment(Qt::AlignCenter);
   item_model->item(18, 0)->setFont(QFont("Times", 10, QFont::Black));
   item_model->setItem(19, 0, new QStandardItem("夜班1"));
   //item_model->item(15, 1)->setForeground(QBrush(Qt::green));
   item_model->item(19, 0)->setTextAlignment(Qt::AlignCenter);
   item_model->item(19, 0)->setFont(QFont("Times", 10, QFont::Black));
   item_model->setItem(20, 0, new QStandardItem("夜班2"));
   //item_model->item(15, 1)->setForeground(QBrush(Qt::green));
   item_model->item(20, 0)->setTextAlignment(Qt::AlignCenter);
   item_model->item(20, 0)->setFont(QFont("Times", 10, QFont::Black));

  // ui.tableView->setSpan(1, 0, 1, 3);
  // ui.tableView->setSpan(1, 3, 1, 3);
  // ui.tableView->setSpan(1, 6, 1, 4);
   
   ui.tableView_record->setIndexWidget(ui.tableView_record->model()->index(1, 6), P_proCategoryItem);    //向表格单元添加一个控件
   proCategoryItem *form = static_cast<proCategoryItem*>(ui.tableView_record->indexWidget(ui.tableView_record->model()->index(1, 6)));
    ui.tableView_record->setShowGrid(true);                               // 隐藏网格线
    ui.tableView_record->setFocusPolicy(Qt::StrongFocus);                      // 去除当前Cell周边虚线框
    ui.tableView_record->setAlternatingRowColors(true);                    // 开启隔行异色
}
void dataMainFormZJ::slotDoubleClicked(const QModelIndex & index)
{
    if (index.row() == 0 && index.column() == 2) {
        tableViewEditItemForm  * form = new tableViewEditItemForm;
        QStandardItemModel *model = static_cast<QStandardItemModel*>(ui.tableView_record->model());
        form->setTitle("生产设备",index.row(),index.column());
        form->show();
        QObject::connect(form, SIGNAL(signalItemData(int,int,QString)), this, SLOT(slotEditItemData(int,int,QString)));
    }
    else if (index.row() == 2 && index.column() == 7) {
        tableViewEditItemForm  * form = new tableViewEditItemForm;
        QStandardItemModel *model = static_cast<QStandardItemModel*>(ui.tableView_record->model());
        form->setTitle("白班记录员", index.row(), index.column());
        form->show();
        QObject::connect(form, SIGNAL(signalItemData(int, int, QString)), this, SLOT(slotEditItemData(int, int, QString)));
    }
    else if (index.row() == 2 && index.column() == 9) {
        tableViewEditItemForm  * form = new tableViewEditItemForm;
        QStandardItemModel *model = static_cast<QStandardItemModel*>(ui.tableView_record->model());
        form->setTitle("夜班记录员", index.row(), index.column());
        form->show();
        QObject::connect(form, SIGNAL(signalItemData(int, int, QString)), this, SLOT(slotEditItemData(int, int, QString)));
    }
    else if (index.row() == 15 && index.column() == 1) {
        tableViewEditItemForm  * form = new tableViewEditItemForm;
        QStandardItemModel *model = static_cast<QStandardItemModel*>(ui.tableView_record->model());
        form->setTitle("备注", index.row(), index.column());
        form->show();
        QObject::connect(form, SIGNAL(signalItemData(int, int, QString)), this, SLOT(slotEditItemData(int, int, QString)));
    }
 
    qDebug() << index.data().toString()<<index.row()<<index.column();
}
//温度区域获取编辑数据并填充
void dataMainFormZJ::slotEditItemData(int row, int column, QString text)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui.tableView_record->model());

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
//温度区域表头数据加载
void dataMainFormZJ::setRowData(int row, const QMap<QString, QStringList> & maplist)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui.tableView_record->model());
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

    QStringList list;

    QMap<QString, QStringList>::const_iterator it;
    for (it = maplist.begin(); it != maplist.end(); it++) {
       // model->item(row, 0)->setForeground(QBrush(Qt::black));
        model->item(row, 0)->setTextAlignment(Qt::AlignCenter);
        model->item(row, 0)->setFont(/*QFont("Times", 10, QFont::Black)*/font);
        model->item(row, 0)->setText(it.key());
        list = it.value();
    }
    for (int i = 0; i < list.size(); i++) {
       // model->item(row, i + 1)->setForeground(QBrush(Qt::black));
        model->item(row, i + 1)->setTextAlignment(Qt::AlignCenter);
        model->item(row, i + 1)->setFont(/*QFont("Times", 10, QFont::Black)*/font);
        model->item(row, i + 1)->setText(list.at(i));
    }   
}
//导出Excel 温度区域
void dataMainFormZJ::on_toolButton_export_clicked()
{
    //QStandardItemModel *model = static_cast<QStandardItemModel*>(ui.tableView->model());
    //tableViewEditItemForm  * form = new tableViewEditItemForm;
  //  QPixmap pix = P_proCategoryItem->grab();
    
    QString fileDir = tr("%1/report").arg(publicClass::instance()->getAppPath());
    QDir dir(fileDir);
    if (!dir.exists()) {
        dir.mkdir(fileDir);
    }
    //QPixmap pix = ui.tableView_record->grab();
    //pix.save(tr("%1/report/%2.png").arg(publicClass::instance()->getAppPath()).arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")));
    //pix.save(tr("D:/workT/manuvision-platform/build/%1.png").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd")));
    QString title = /*QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")*/tr("带材线生产工艺记录表");
   // QString fileName = QFileDialog::getSaveFileName(this, tr("保存"), "", tr("Excel 文件(*.xls *.xlsx)"));
    QStandardItemModel * model = qobject_cast<QStandardItemModel*>(ui.tableView_record->model());
    QStringList list = P_proCategoryItem->getProType();
    QString proType = "";
    for (int i = 0; i < list.size(); i++)
    {
        proType += list.at(i) + ",";
    }
    proType.chop(2);
    model->item(1, 6)->setText(tr("产品种类: %1").arg(proType));
    QString fileName = tr("%1/report/%2.xls").arg(publicClass::instance()->getAppPath()).arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
    if (fileName != "") {
        QAxObject *excel = new QAxObject;
        if (excel->setControl("Excel.Application")) {
            excel->dynamicCall("SetVisible (bool Visible)", "false");//不显示窗体
            excel->setProperty("DisplayAlerts", false);//不显示任何警告信息。如果为true那么在关闭是会出现类似“文件已修改，是否保存”的提示
            QAxObject *workbooks = excel->querySubObject("WorkBooks");//获取工作簿集合
            workbooks->dynamicCall("Add");//新建一个工作簿
            QAxObject *workbook = excel->querySubObject("ActiveWorkBook");//获取当前工作簿
            QAxObject *worksheet = workbook->querySubObject("Worksheets(int)", 1);
            int i, j;
            int colcount = ui.tableView_record->model()->columnCount();
            int rowcount = ui.tableView_record->model()->rowCount();

            QAxObject *cell, *col;
            //标题行
            cell = worksheet->querySubObject("Cells(int,int)", 1, 1);
            cell->dynamicCall("SetValue(const QString&)", title);
            cell->querySubObject("Font")->setProperty("Size", 18);
            //调整行高
            worksheet->querySubObject("Range(const QString&)", "1:1")->setProperty("RowHeight", 20);
          //  worksheet->querySubObject("Range(const QString&)", "1:1")->setProperty("ColumnWidth", 15);

            //合并标题行
            QString cellTitle;
            cellTitle.append("A1:J1");
           // cellTitle.append(QChar(colcount - 1 + 'A'));
            //cellTitle.append(QString::number(1));
            QAxObject *range = worksheet->querySubObject("Range(const QString&)", cellTitle);

            range->setProperty("WrapText", true);
            range->setProperty("MergeCells", true);
            range->setProperty("HorizontalAlignment", -4108);//xlCenter
            range->setProperty("VerticalAlignment", -4108);//xlCenter

            //列标题
            for (i = 0; i < colcount; i++)
            {
                QString columnName;
                columnName.append(QChar(i + 'A'));
                columnName.append(":");
                columnName.append(QChar(i + 'A'));
                col = worksheet->querySubObject("Columns(const QString&)", columnName);
                col->setProperty("ColumnWidth", ui.tableView_record->columnWidth(i) / 16);
                cell = worksheet->querySubObject("Cells(int,int)", 2, i + 1);
                //QTableView 获取表格头部文字信息
                columnName = ui.tableView_record->model()->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
                cell->dynamicCall("SetValue(const QString&)", columnName);
                cell->querySubObject("Font")->setProperty("Bold", true);
                cell->querySubObject("Interior")->setProperty("Color", QColor(191, 191, 191));
                cell->setProperty("HorizontalAlignment", -4108);//xlCenter
                cell->setProperty("VerticalAlignment", -4108);//xlCenter
            }
            //合并第二行
            QString cellTitle1;
            cellTitle1.append("A2:");
            cellTitle1.append(QChar(colcount/2 - 1 + 'A'));
            cellTitle1.append(QString::number(2));
            QAxObject *range1 = worksheet->querySubObject("Range(const QString&)", cellTitle1);
            range1->dynamicCall("SetValue(const QString&)", "QCCC/G06-005-003");
            range1->setProperty("WrapText", true);
            range1->setProperty("MergeCells", true);
            range1->setProperty("HorizontalAlignment", -4108);//xlCenter
            range1->setProperty("VerticalAlignment", -4108);//xlCenter

             //合并第二行
            QString cellTitle2;
            cellTitle2.append("F2:");
            cellTitle2.append(QChar(colcount / 2 - 1 + 'F'));
            cellTitle2.append(QString::number(2));
            QAxObject *range2 = worksheet->querySubObject("Range(const QString&)", cellTitle2);
            range2->dynamicCall("SetValue(const QString&)", "QCCC/DC.02A");
            range2->setProperty("WrapText", true);
            range2->setProperty("MergeCells", true);
            range2->setProperty("HorizontalAlignment", -4108);//xlCenter
            range2->setProperty("VerticalAlignment", -4108);//xlCenter
            //数据区

            QString row3;
            row3.append("A3:");
            row3.append(QChar(1 + 'A'));//列数
            row3.append(QString::number(3));//行
            QAxObject *rangeRow3 = worksheet->querySubObject("Range(const QString&)", row3);
            rangeRow3->setProperty("WrapText", true);
            rangeRow3->setProperty("MergeCells", true);
            rangeRow3->setProperty("HorizontalAlignment", -4108);//xlCenter
            rangeRow3->setProperty("VerticalAlignment", -4108);//xlCenter
            QString row4;
            row4.append("C3:");
            row4.append(QChar(1 + 'C'));//列数
            row4.append(QString::number(3));//行
            QAxObject *rangeRow4 = worksheet->querySubObject("Range(const QString&)", row4);
            rangeRow4->setProperty("WrapText", true);
            rangeRow4->setProperty("MergeCells", true);
            rangeRow4->setProperty("HorizontalAlignment", -4108);//xlCenter
            rangeRow4->setProperty("VerticalAlignment", -4108);//xlCenter
            QString row5;
            row5.append("E3:");
            row5.append(QChar(1 + 'E'));//列数
            row5.append(QString::number(3));//行
            QAxObject *rangeRow5 = worksheet->querySubObject("Range(const QString&)", row5);
            rangeRow5->setProperty("WrapText", true);
            rangeRow5->setProperty("MergeCells", true);
            rangeRow5->setProperty("HorizontalAlignment", -4108);//xlCenter
            rangeRow5->setProperty("VerticalAlignment", -4108);//xlCenter
            QString row6;
            row6.append("G3:");
            row6.append(QChar(3 + 'G'));//列数
            row6.append(QString::number(3));//行
            QAxObject *rangeRow6 = worksheet->querySubObject("Range(const QString&)", row6);
            rangeRow6->setProperty("WrapText", true);
            rangeRow6->setProperty("MergeCells", true);
            rangeRow6->setProperty("HorizontalAlignment", -4108);//xlCenter
            rangeRow6->setProperty("VerticalAlignment", -4108);//xlCenter
            QString row7;
            row7.append("A4:");
            row7.append(QChar(2 + 'A'));//列数
            row7.append(QString::number(4));//行
            QAxObject *rangeRow7 = worksheet->querySubObject("Range(const QString&)", row7);
            rangeRow7->setProperty("WrapText", true);
            rangeRow7->setProperty("MergeCells", true);
            rangeRow7->setProperty("HorizontalAlignment", -4108);//xlCenter
            rangeRow7->setProperty("VerticalAlignment", -4108);//xlCenter
            QString row8;
            row8.append("D4:");
            row8.append(QChar(2 + 'D'));//列数
            row8.append(QString::number(4));//行
            QAxObject *rangeRow8 = worksheet->querySubObject("Range(const QString&)", row8);
            rangeRow8->setProperty("WrapText", true);
            rangeRow8->setProperty("MergeCells", true);
            rangeRow8->setProperty("HorizontalAlignment", -4108);//xlCenter
            rangeRow8->setProperty("VerticalAlignment", -4108);//xlCenter
            QString row9;
            row9.append("G4:");
            row9.append(QChar(3 + 'G'));//列数
            row9.append(QString::number(4));//行
            QAxObject *rangeRow9 = worksheet->querySubObject("Range(const QString&)", row9);
            rangeRow9->setProperty("WrapText", true);
            rangeRow9->setProperty("MergeCells", true);
            rangeRow9->setProperty("HorizontalAlignment", -4108);//xlCenter
            rangeRow9->setProperty("VerticalAlignment", -4108);//xlCenter
            QString row10;
            row10.append("A5:");
            row10.append(QChar(1 + 'A'));//列数
            row10.append(QString::number(5));//行
            QAxObject *rangeRow10 = worksheet->querySubObject("Range(const QString&)", row10);
            rangeRow10->setProperty("WrapText", true);
            rangeRow10->setProperty("MergeCells", true);
            rangeRow10->setProperty("HorizontalAlignment", -4108);//xlCenter
            rangeRow10->setProperty("VerticalAlignment", -4108);//xlCenter
            QString row11;
            row11.append("D5:");
            row11.append(QChar(1 + 'D'));//列数
            row11.append(QString::number(5));//行
            QAxObject *rangeRow11 = worksheet->querySubObject("Range(const QString&)", row11);
            rangeRow11->setProperty("WrapText", true);
            rangeRow11->setProperty("MergeCells", true);
            rangeRow11->setProperty("HorizontalAlignment", -4108);//xlCenter
            rangeRow11->setProperty("VerticalAlignment", -4108);//xlCenter

            /*QString cellTitle_a12_j12;
            cellTitle.append("A12:J12");
            QAxObject *rangeRow_a12_j12 = worksheet->querySubObject("Range(const QString&)", cellTitle_a12_j12);
            rangeRow_a12_j12->setProperty("WrapText", true);
            rangeRow_a12_j12->setProperty("MergeCells", true);
            rangeRow_a12_j12->setProperty("HorizontalAlignment", -4108);//xlCenter
            rangeRow_a12_j12->setProperty("VerticalAlignment", -4108);//xlCenter
            */
            QString row12;
            row12.append("A24:");
            row12.append(QChar(0 + 'A'));//列数
            row12.append(QString::number(25));//行
            QAxObject *rangeRow12 = worksheet->querySubObject("Range(const QString&)", row12);
            rangeRow12->setProperty("WrapText", true);
            rangeRow12->setProperty("MergeCells", true);
            rangeRow12->setProperty("HorizontalAlignment", -4108);//xlCenter
            rangeRow12->setProperty("VerticalAlignment", -4108);//xlCenter
            QString row13;
            row13.append("B24:");
            row13.append(QChar(colcount - 2 + 'B'));//列数
            row13.append(QString::number(25));//行
            QAxObject *rangeRow13 = worksheet->querySubObject("Range(const QString&)", row13);
            rangeRow13->setProperty("WrapText", true);
            rangeRow13->setProperty("MergeCells", true);
            rangeRow13->setProperty("HorizontalAlignment", -4108);//xlCenter
            rangeRow13->setProperty("VerticalAlignment", -4108);//xlCenter
            //QTableView 获取表格数据部分
            for (i = 0; i < rowcount; i++) //行数
            {
                if (i == 5||i==11||i==17) {
                    QString cell;
                    cell.append(tr("A%1:").arg(i+1));
                    cell.append(QChar(colcount - 1 + 'A'));
                    cell.append(QString::number(i+1));
                    QAxObject *range = worksheet->querySubObject("Range(const QString&)", cell);
                    range->setProperty("WrapText", true);
                    range->setProperty("MergeCells", true);
                    range->setProperty("HorizontalAlignment", -4108);//xlCenter
                    range->setProperty("VerticalAlignment", -4108);//xlCenter
                }
                for (j = 0; j < colcount; j++)   //列数
                {
                    QModelIndex index = ui.tableView_record->model()->index(i, j);
                    QString strdata = ui.tableView_record->model()->data(index).toString();
                    QAxObject *cell = worksheet->querySubObject("Cells(int,int)", i + 3, j + 1);
                    cell->dynamicCall("SetValue(const QString&)", strdata);
                   // QString picDir = QString("D:\\workT\\manuvision-platform\\build\\2020-06-30.png");
                    //cell->dynamicCall("AddPicture( QString&, bool, bool, double, double, double, double", picDir, true, true, 20, 12, pix.width() / 2, pix.height() / 2);
                    cell->setProperty("WrapText", true);
                    cell->setProperty("MergeCells", true);
                    cell->setProperty("HorizontalAlignment", -4108);//xlCenter
                    cell->setProperty("VerticalAlignment", -4108);//xlCenter
                }
            }
            //画框线
            QString lrange;
            lrange.append("A2:");
            lrange.append(colcount - 1 + 'A');
            lrange.append(QString::number(rowcount + 2));
            range = worksheet->querySubObject("Range(const QString&)", lrange);
            range->querySubObject("Borders")->setProperty("LineStyle", QString::number(1));
            range->querySubObject("Borders")->setProperty("Color", QColor(0, 0, 0));

            //调整数据区行高
            QString rowsName;
            rowsName.append("2:");
            rowsName.append(QString::number(rowcount + 2));
            range = worksheet->querySubObject("Range(const QString&)", rowsName);
            range->setProperty("RowHeight", 20);

            workbook->dynamicCall("SaveAs(const QString&)", QDir::toNativeSeparators(fileName));//保存至fileName
            workbook->dynamicCall("Close()");//关闭工作簿
            excel->dynamicCall("Quit()");//关闭excel
            delete excel;
            excel = nullptr;
            if (QMessageBox::question(nullptr, tr("完成"), tr("文件已经导出，是否现在打开？"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
            {
                QDesktopServices::openUrl(QUrl("file:///" + QDir::toNativeSeparators(fileName)));
            }
        }
        else
        {
            QMessageBox::warning(nullptr, tr("错误"), tr("未能创建 Excel 对象，请安装 Microsoft Excel。"), QMessageBox::Apply);
        }
    }
}

void dataMainFormZJ::on_toolButton_test_clicked_1()
{

   // QString filename = QFileDialog::getSaveFileName(this,
     //   tr("保存管柱序列"), "", tr("*.xls")); //选择路径
    QString filename = tr("%1/report/%2.xls").arg(publicClass::instance()->getAppPath())
        .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
    if (filename.isEmpty())
    {
        QMessageBox::critical(0, tr("错误"), tr("要保存的文件名为空！"));
        return;
    }
    QPixmap pix = ui.tableView_record->grab();
    //QPixmap scaledPixmap = pix.scaled(500, 300, Qt::KeepAspectRatio);
    QString filePix = tr("%1/report/%2.png").arg(publicClass::instance()->getAppPath())
        .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
    pix.save(filePix);

    //建立Excel对象
    QAxObject *excel = new QAxObject("Excel.Application");
    excel->dynamicCall("SetVisible(bool)", false); //如果为了看自己的程序到底怎样工作，可以设置为true
    excel->setProperty("Visible", false);
    QAxObject *workbooks = excel->querySubObject("WorkBooks");
    workbooks->dynamicCall("Add");

    QAxObject *workbook = excel->querySubObject("ActiveWorkBook");//获取当前工作簿
    QAxObject *worksheets = workbook->querySubObject("Sheets");//获取工作表集合
   // QAxObject *worksheet = workbook->querySubObject("Worksheets(int)", 1);
    QAxObject *worksheet = worksheets->querySubObject("Item(int)", 1);//获取工作表集合的工作表1，即sheet1

    QImage image(filePix);
    // image = image.scaled(image.width()/2, image.height()/2);
   // QClipboard* clip = QApplication::clipboard();
   // clip->setImage(image);

   // QAxObject * cell = worksheet->querySubObject("Cells(int,int)", 3, 1);
   // int first = filePix.lastIndexOf("/"); //从后面查找"/"位置
   // QString pngName = filePix.right(filePix.length() - first - 1); //从右边截取
   // QString picDir = filePix;// QString("D:\\workT\\manuvision-platform\\build\\x64\\Release\\report\\20200708_101428.png");
   // QString needPixPath = filePix.replace("\"", "\\");
   // QString picDir = QString(picDir);
    QAxObject * shapes = worksheet->querySubObject("Shapes");
    worksheet->dynamicCall("Paste()");
    shapes->dynamicCall("AddPicture( QString&, bool, bool, double, double, double, double", image, true, true, 10, 12, image.width()/2, image.height()/2);
    workbook->dynamicCall("SaveAs(const QString&)", QDir::toNativeSeparators(filename));//保存至filepath，注意一定要用QDir::toNativeSeparators将路径中的"/"转换为"\"，不然一定保存不了。
    workbook->dynamicCall("Close()");//关闭工作簿
    excel->dynamicCall("Quit()");//关闭excel
    delete excel;
}
//测试导出文件
typedef QSharedPointer<QAxObject> QAxObjectPtr;
void dataMainFormZJ::on_toolButton_test_clicked()
{
    QString filename = tr("%1/report/%2.xls").arg(publicClass::instance()->getAppPath())
        .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
    if (filename.isEmpty())
    {
        QMessageBox::critical(0, tr("错误"), tr("要保存的文件名为空！"));
        return;
    }
    QPixmap pix = ui.tableView_record->grab();
    //按比例缩放：
    //QPixmap scaledPixmap = pix.scaled(500,300, Qt::KeepAspectRatio);

    //不按照比例缩放：
   // QPixmap scaledPixmap = pix.scaled(800,00);
    QString filePix = tr("%1/report/%2.png").arg(publicClass::instance()->getAppPath())
        .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
    pix.save(filePix);
    QAxObject excel("Excel.Application", 0);
    excel.setProperty("Visible", false);

    QAxObjectPtr workbooks(excel.querySubObject("Workbooks"));
    QAxObjectPtr workbook(workbooks->querySubObject("Add()"));
    QAxObjectPtr sheets(workbook->querySubObject("Worksheets"));
    sheets->dynamicCall("Add()");
    QAxObjectPtr sheet(sheets->querySubObject("Item( int )", 1));
   // QAxObjectPtr sheet(sheets->querySubObject("Cells(int,int)", 10, 18));
    // sheet->setProperty("Name","Nova Planilha");

   // QAxObjectPtr range(sheet->querySubObject("Cells(int,int)", 1, 1));
    //range->setProperty("Value", QVariant(1234));

    QImage image(filePix);
   // QImage igScaled = image.scaled(316, 236, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
   // image = image.scaled(image.width()/2, image.height()/2);
    QClipboard* clip = QApplication::clipboard();
    clip->setImage(image);
    QAxObjectPtr shapes(sheet->querySubObject("Shapes"));
    sheet->dynamicCall("Paste()");
    //int shapeCount = shapes->property("Count").toInt();
    //QAxObjectPtr shape(shapes->querySubObject(QString::fromLatin1("Item(%1)").arg(shapeCount).toLatin1()));

    shapes->dynamicCall("AddPicture( QString&, bool, bool, double, double, double, double", image,true,true,10,10,300,100);

    excel.setProperty("DisplayAlerts", false);
    workbook->dynamicCall("SaveAs(QString&)", QDir::toNativeSeparators(filename));
    // //workbook->dynamicCall("Close()");
    // //excel.dynamicCall("Quit()");
    // workbook->dynamicCall("Close (Boolean)", true);

  //  workbook->dynamicCall("SaveAs(const QString&)", QDir::toNativeSeparators(filename));//保存至filepath，注意一定要用QDir::toNativeSeparators将路径中的"/"转换为"\"，不然一定保存不了。
    workbook->dynamicCall("Close()");//关闭工作簿
    excel.dynamicCall("Quit()");
       if (QMessageBox::question(nullptr, tr("完成"), tr("文件已经导出，是否现在打开？"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
        {
          QDesktopServices::openUrl(QUrl("file:///" + QDir::toNativeSeparators(filename)));
        }
        else
        {
        QMessageBox::warning(nullptr, tr("错误"), tr("未能创建 Excel 对象，请安装 Microsoft Excel。"), QMessageBox::Apply);
        }
}
//在线监测记录加一行
void dataMainFormZJ::on_toolButtonAdd_clicked()
{
	//ui.comboBox_page->setEnabled(false);
	//disconnect(ui.comboBox_page, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(slotComboxUpDatePage(const QString &)));
    _addRecordIndexjuanhao = ui.comboBox_juanhao->currentText().simplified();
    addTestReord * form = new addTestReord;
    form->show();
    connect(form, SIGNAL(signalRowData(QMap<QString, QString>,bool)), 
        this, SLOT(slotAddRowData(QMap<QString, QString>,bool)));
    ui.tableView_testRecord->stopUpdate();
    
}
//导出pdf
void dataMainFormZJ::on_toolButton_pdf_clicked()
{
    int index = _index;
    switch (index)
    {
    case 0: { // pdf添加图片
        QPrinter printer_pixmap(QPrinter::HighResolution);
        printer_pixmap.setPageSize(QPrinter::A4);  //设置纸张大小为A4
        printer_pixmap.setOutputFormat(QPrinter::PdfFormat);  //设置输出格式为pdf
        printer_pixmap.setOutputFileName("D:\\workT\\manuvision-platform\\build\\test_pixmap.pdf");   //设置输出路径
        QPixmap pix = ui.tableView_record->grab();//获取界面的图片
        pix.save(tr("D:/workT/manuvision-platform/build/%1.png").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd")));

        // QPixmap pixmap = QPixmap::grabWidget(main_widget, main_widget->rect());  //获取界面的图片

        QPainter painter_pixmap;
        painter_pixmap.begin(&printer_pixmap);
        QRect rect = painter_pixmap.viewport();
        int multiple = rect.width() / pix.width();
        painter_pixmap.scale(multiple, multiple); //将图像(所有要画的东西)在pdf上放大multiple-1倍
        painter_pixmap.drawPixmap(0, 0, pix);  //画图
        painter_pixmap.end();
    }
            break;
    case 1: { //文本
        QPrinter printer_text;
        printer_text.setOutputFormat(QPrinter::PdfFormat);
        printer_text.setOutputFileName("D:\\workT\\manuvision-platform\\build\\test_text.pdf");
        QPainter painter_text;
        painter_text.begin(&printer_text);

        QDateTime current_date_time = QDateTime::currentDateTime();
        QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss ddd");

        QPoint point(50, 10);
        for (int i = 1; i <= 5; i++)
        {
            QString message = QString("%1          %2          %3").arg(QString::number(i)).arg(current_date).arg(tr("北京"));
            int y = point.y();
            point.setY(y + 20);
            painter_text.drawText(point, message);
            message.clear();
        }
        //printer_text.newPage();  //生成新的一页，可继续插入
        painter_text.end();
    }
            break;
    case 2: { //html
        //Html生成PDF  

        QFile file("D:\\workT\\manuvision-platform\\build\\a.txt");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        QTextStream in(&file);
        QString html = in.readAll();
        file.close();
        QPrinter printerHtml;
        printerHtml.setPageSize(QPrinter::A4);
        printerHtml.setOutputFormat(QPrinter::PdfFormat);
        printerHtml.setOutputFileName("D:\\workT\\manuvision-platform\\build\\test_html.pdf");
        QTextDocument textDocument;
        textDocument.setHtml(html);
        textDocument.print(&printerHtml);
        textDocument.end();
    }
            break;
    default:
        break;
    }
    
    _index++;   
}
//到处excel
void dataMainFormZJ::on_toolButton_export_stripQuality_clicked()
{
    //cv::Mat defectmap = DefectMap(database_record, );
    //bool success = DefectMap(database_record, height, width, std::string savename="D:/202007311631_V1111.jpg");
    //QStandardItemModel *model = static_cast<QStandardItemModel*>(ui.tableView->model());
  //tableViewEditItemForm  * form = new tableViewEditItemForm;
   // QPixmap pix = P_proCategoryItem->grab();
    //pix.save(tr("D:/workT/manuvision-platform/build/%1.png").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd")));
    QString title = /*QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")*/tr("带材质量记录表");
    QString fileName = QFileDialog::getSaveFileName(this, tr("保存"), "", tr("Excel 文件(*.xls *.xlsx)"));
    if (fileName != "") {
        QAxObject* excel = new QAxObject;
        if (excel->setControl("Excel.Application")) {
            excel->dynamicCall("SetVisible (bool Visible)", "false");//不显示窗体
            excel->setProperty("DisplayAlerts", false);//不显示任何警告信息。如果为true那么在关闭是会出现类似“文件已修改，是否保存”的提示
            QAxObject *workbooks = excel->querySubObject("WorkBooks");//获取工作簿集合
            workbooks->dynamicCall("Add");//新建一个工作簿
            QAxObject *workbook = excel->querySubObject("ActiveWorkBook");//获取当前工作簿
            QAxObject *worksheet = workbook->querySubObject("Worksheets(int)", 1);
            int i, j;
           // QStandardItemModel *model_stripQuality = ui.tableView_stripQuality->initModel();

            int colcount_stripQuality = ui.tableView_stripQuality->model()/*model_stripQuality*/->columnCount();//订单信息
            int rowcount_stripQuality = ui.tableView_stripQuality->model()/*model_stripQuality*/->rowCount();

            int colcount_testRecord = ui.tableView_testRecord->model()->columnCount();//在线检测记录
           // int rowcount_testRecord = ui.tableView_testRecord->model()->rowCount();
            int rowcount_testRecord = ui.tableView_testRecord->getTestRecordCurrentRowData().size()+2;
            QList<QVariantList> testRecordData = ui.tableView_testRecord->getTestRecordCurrentRowData();
            int colcount_remark = ui.tableView_remark->model()->columnCount();//备注信息
            int rowcount_remark = ui.tableView_remark->model()->rowCount();
            int colcount_Inspection = ui.tableView_qualityInspectionRecord->model()->columnCount();//质检记录
            int rowcount_Inspection = ui.tableView_qualityInspectionRecord->model()->rowCount();
            QAxObject *cell, *col;
            //标题行
            cell = worksheet->querySubObject("Cells(int,int)", 1, 1);
            cell->dynamicCall("SetValue(const QString&)", title);
            cell->querySubObject("Font")->setProperty("Size", 18);
            //调整行高
            worksheet->querySubObject("Range(const QString&)", "1:1")->setProperty("RowHeight", 20);
            //  worksheet->querySubObject("Range(const QString&)", "1:1")->setProperty("ColumnWidth", 15);

              //合并标题行
            QString cellTitle;
            cellTitle.append("A1:J1");
            // cellTitle.append(QChar(colcount - 1 + 'A'));
             //cellTitle.append(QString::number(1));
            QAxObject *range = worksheet->querySubObject("Range(const QString&)", cellTitle);

            range->setProperty("WrapText", true);
            range->setProperty("MergeCells", true);
            range->setProperty("HorizontalAlignment", -4108);//xlCenter
            range->setProperty("VerticalAlignment", -4108);//xlCenter

            //列标题（订单信息）
            for (i = 0; i < colcount_stripQuality; i++)
            {
                QString columnName;
                columnName.append(QChar(i + 'A'));
                columnName.append(":");
                columnName.append(QChar(i + 'A'));
                col = worksheet->querySubObject("Columns(const QString&)", columnName);
                col->setProperty("ColumnWidth", ui.tableView_stripQuality->columnWidth(i) / 8);
                cell = worksheet->querySubObject("Cells(int,int)",  2, i + 1);
                //QTableView 获取表格头部文字信息
                columnName = ui.tableView_stripQuality->model()->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
                cell->dynamicCall("SetValue(const QString&)", columnName);
                cell->querySubObject("Font")->setProperty("Bold", true);
                cell->querySubObject("Interior")->setProperty("Color", QColor(191, 191, 191));
                cell->setProperty("HorizontalAlignment", -4108);//xlCenter
                cell->setProperty("VerticalAlignment", -4108);//xlCenter
            }
            //列标题（在线检测记录）
            for (i = 0; i < colcount_testRecord; i++)
            {
                QString columnName;
                columnName.append(QChar(i + 'A'));
                columnName.append(":");
                columnName.append(QChar(i + 'A'));
                col = worksheet->querySubObject("Columns(const QString&)", columnName);
                col->setProperty("ColumnWidth", ui.tableView_stripQuality->columnWidth(i) / 8);
                cell = worksheet->querySubObject("Cells(int,int)", 2+rowcount_stripQuality, i + 1);
                //QTableView 获取表格头部文字信息
                columnName = ui.tableView_testRecord->model()->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
                cell->dynamicCall("SetValue(const QString&)", columnName);
                cell->querySubObject("Font")->setProperty("Bold", true);
                cell->querySubObject("Interior")->setProperty("Color", QColor(191, 191, 191));
                cell->setProperty("HorizontalAlignment", -4108);//xlCenter
                cell->setProperty("VerticalAlignment", -4108);//xlCenter
            }
            //列标题（备注）
            for (i = 0; i < colcount_remark; i++)
            {
                QString columnName;
                columnName.append(QChar(i + 'A'));
                columnName.append(":");
                columnName.append(QChar(i + 'A'));
                col = worksheet->querySubObject("Columns(const QString&)", columnName);
                col->setProperty("ColumnWidth", ui.tableView_stripQuality->columnWidth(i) / 8);
                cell = worksheet->querySubObject("Cells(int,int)", 2 + rowcount_stripQuality+rowcount_testRecord, i + 1);
                //QTableView 获取表格头部文字信息
                columnName = ui.tableView_remark->model()->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
                cell->dynamicCall("SetValue(const QString&)", columnName);
                cell->querySubObject("Font")->setProperty("Bold", true);
                cell->querySubObject("Interior")->setProperty("Color", QColor(191, 191, 191));
                cell->setProperty("HorizontalAlignment", -4108);//xlCenter
                cell->setProperty("VerticalAlignment", -4108);//xlCenter
            }
            //列标题（备注）
            for (i = 0; i < colcount_Inspection; i++)
            {
                QString columnName;
                columnName.append(QChar(i + 'A'));
                columnName.append(":");
                columnName.append(QChar(i + 'A'));
                col = worksheet->querySubObject("Columns(const QString&)", columnName);
                col->setProperty("ColumnWidth", ui.tableView_stripQuality->columnWidth(i) / 8);
                cell = worksheet->querySubObject("Cells(int,int)", 2 + rowcount_stripQuality + rowcount_testRecord+ rowcount_remark+2, i + 1);
                //QTableView 获取表格头部文字信息
                columnName = ui.tableView_qualityInspectionRecord->model()->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
                cell->dynamicCall("SetValue(const QString&)", columnName);
                cell->querySubObject("Font")->setProperty("Bold", true);
                cell->querySubObject("Interior")->setProperty("Color", QColor(191, 191, 191));
                cell->setProperty("HorizontalAlignment", -4108);//xlCenter
                cell->setProperty("VerticalAlignment", -4108);//xlCenter
            }
            for (i = 0; i < colcount_Inspection; i++)
            {
                QString columnName;
                columnName.append(QChar(i + 'A'));
                columnName.append(":");
                columnName.append(QChar(i + 'A'));
                col = worksheet->querySubObject("Columns(const QString&)", columnName);
                col->setProperty("ColumnWidth", ui.tableView_stripQuality->columnWidth(i) / 8);
                cell = worksheet->querySubObject("Cells(int,int)", 2 + rowcount_stripQuality + rowcount_testRecord + rowcount_remark + 2+9, i + 1);
                //QTableView 获取表格头部文字信息
                columnName = ui.tableView_qualityInspectionRecord->model()->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
                cell->dynamicCall("SetValue(const QString&)", columnName);
                cell->querySubObject("Font")->setProperty("Bold", true);
                cell->querySubObject("Interior")->setProperty("Color", QColor(191, 191, 191));
                cell->setProperty("HorizontalAlignment", -4108);//xlCenter
                cell->setProperty("VerticalAlignment", -4108);//xlCenter
            }
            //合并第二行
            QString cellTitle1;
            cellTitle1.append("A2:J2");
           // cellTitle1.append(QChar(colcount_stripQuality / 2 - 1 + 'A'));
            //cellTitle1.append(QString::number(2));
            QAxObject *range1 = worksheet->querySubObject("Range(const QString&)", cellTitle1);
           // range1->dynamicCall("SetValue(const QString&)", "QCCC/G06-005-003");
            range1->setProperty("WrapText", true);
            range1->setProperty("MergeCells", true);
            range1->setProperty("HorizontalAlignment", -4108);//xlCenter
            range1->setProperty("VerticalAlignment", -4108);//xlCenter

            QString row3;
            row3.append("B3:C3");
           // row3.append(QChar(1 + 'A'));//列数
           // row3.append(QString::number(3));//行
            QAxObject *rangeRow3 = worksheet->querySubObject("Range(const QString&)", row3);
            rangeRow3->setProperty("WrapText", true);
            rangeRow3->setProperty("MergeCells", true);
            rangeRow3->setProperty("HorizontalAlignment", -4108);//xlCenter
            rangeRow3->setProperty("VerticalAlignment", -4108);//xlCenter
            QString row4;
            row4.append("D3:E3");
           // row4.append(QChar(1 + 'C'));//列数
            //row4.append(QString::number(3));//行
            QAxObject *rangeRow4 = worksheet->querySubObject("Range(const QString&)", row4);
            rangeRow4->setProperty("WrapText", true);
            rangeRow4->setProperty("MergeCells", true);
            rangeRow4->setProperty("HorizontalAlignment", -4108);//xlCenter
            rangeRow4->setProperty("VerticalAlignment", -4108);//xlCenter
            QString row5;
            row5.append("F3:G3");
           // row5.append(QChar(1 + 'E'));//列数
            //row5.append(QString::number(3));//行
            QAxObject *rangeRow5 = worksheet->querySubObject("Range(const QString&)", row5);
            rangeRow5->setProperty("WrapText", true);
            rangeRow5->setProperty("MergeCells", true);
            rangeRow5->setProperty("HorizontalAlignment", -4108);//xlCenter
            rangeRow5->setProperty("VerticalAlignment", -4108);//xlCenter
            QString row6;
            row6.append("I3:J3");
           // row6.append(QChar(3 + 'G'));//列数
            //row6.append(QString::number(3));//行
            QAxObject *rangeRow6 = worksheet->querySubObject("Range(const QString&)", row6);
            rangeRow6->setProperty("WrapText", true);
            rangeRow6->setProperty("MergeCells", true);
            rangeRow6->setProperty("HorizontalAlignment", -4108);//xlCenter
            rangeRow6->setProperty("VerticalAlignment", -4108);//xlCenter
            QString row7;
            row7.append("A4:J4");
           // row7.append(QChar(2 + 'A'));//列数
            //row7.append(QString::number(4));//行
            QAxObject *rangeRow7 = worksheet->querySubObject("Range(const QString&)", row7);
            rangeRow7->setProperty("WrapText", true);
            rangeRow7->setProperty("MergeCells", true);
            rangeRow7->setProperty("HorizontalAlignment", -4108);//xlCenter
            rangeRow7->setProperty("VerticalAlignment", -4108);//xlCenter
            QString row8;
            row8.append("A5:B5");
            //row8.append(QChar(2 + 'D'));//列数
            //row8.append(QString::number(4));//行
            QAxObject *rangeRow8 = worksheet->querySubObject("Range(const QString&)", row8);
            rangeRow8->setProperty("WrapText", true);
            rangeRow8->setProperty("MergeCells", true);
            rangeRow8->setProperty("HorizontalAlignment", -4108);//xlCenter
            rangeRow8->setProperty("VerticalAlignment", -4108);//xlCenter
            QString row9;
            row9.append("D5:E5");
            //row9.append(QChar(3 + 'G'));//列数
            //row9.append(QString::number(4));//行
            QAxObject *rangeRow9 = worksheet->querySubObject("Range(const QString&)", row9);
            rangeRow9->setProperty("WrapText", true);
            rangeRow9->setProperty("MergeCells", true);
            rangeRow9->setProperty("HorizontalAlignment", -4108);//xlCenter
            rangeRow9->setProperty("VerticalAlignment", -4108);//xlCenter
            QString row10;
            row10.append("A6:J6");
            //row10.append(QChar(1 + 'A'));//列数
            //row10.append(QString::number(5));//行
            QAxObject *rangeRow10 = worksheet->querySubObject("Range(const QString&)", row10);
            rangeRow10->setProperty("WrapText", true);
            rangeRow10->setProperty("MergeCells", true);
            rangeRow10->setProperty("HorizontalAlignment", -4108);//xlCenter
            rangeRow10->setProperty("VerticalAlignment", -4108);//xlCenter
            QString row11;
            row11.append("A7:B7");
           // row11.append(QChar(1 + 'D'));//列数
            //row11.append(QString::number(5));//行
            QAxObject *rangeRow11 = worksheet->querySubObject("Range(const QString&)", row11);
            rangeRow11->setProperty("WrapText", true);
            rangeRow11->setProperty("MergeCells", true);
            rangeRow11->setProperty("HorizontalAlignment", -4108);//xlCenter
            rangeRow11->setProperty("VerticalAlignment", -4108);//xlCenter
            QString row12;
            row12.append("D7:E7");
            //row12.append(QChar(0 + 'A'));//列数
            //row12.append(QString::number(19));//行
            QAxObject *rangeRow12 = worksheet->querySubObject("Range(const QString&)", row12);
            rangeRow12->setProperty("WrapText", true);
            rangeRow12->setProperty("MergeCells", true);
            rangeRow12->setProperty("HorizontalAlignment", -4108);//xlCenter
            rangeRow12->setProperty("VerticalAlignment", -4108);//xlCenter
            QString row13;
            row13.append("A8:B8");
           // row13.append(QChar(colcount_stripQuality - 2 + 'B'));//列数
            //row13.append(QString::number(19));//行
            QAxObject *rangeRow13 = worksheet->querySubObject("Range(const QString&)", row13);
            rangeRow13->setProperty("WrapText", true);
            rangeRow13->setProperty("MergeCells", true);
            rangeRow13->setProperty("HorizontalAlignment", -4108);//xlCenter
            rangeRow13->setProperty("VerticalAlignment", -4108);//xlCenter
            QString row14;
            row14.append("D8:E8");
            // row13.append(QChar(colcount_stripQuality - 2 + 'B'));//列数
             //row13.append(QString::number(19));//行
            QAxObject *rangeRow14 = worksheet->querySubObject("Range(const QString&)", row14);
            rangeRow14->setProperty("WrapText", true);
            rangeRow14->setProperty("MergeCells", true);
            rangeRow14->setProperty("HorizontalAlignment", -4108);//xlCenter
            rangeRow14->setProperty("VerticalAlignment", -4108);//xlCenter

            QString row15;
            row15.append("A9:J9");
            // row13.append(QChar(colcount_stripQuality - 2 + 'B'));//列数
             //row13.append(QString::number(19));//行
            QAxObject *rangeRow15 = worksheet->querySubObject("Range(const QString&)", row15);
            rangeRow15->setProperty("WrapText", true);
            rangeRow15->setProperty("MergeCells", true);
            rangeRow15->setProperty("HorizontalAlignment", -4108);//xlCenter
            rangeRow15->setProperty("VerticalAlignment", -4108);//xlCenter
            for (int i = 10; i < rowcount_stripQuality+2; i++) {
                QString rowCell1;
                rowCell1.append(tr("A%1:D%2").arg(i).arg(i));
                initStripQualityExcelItem(worksheet->querySubObject("Range(const QString&)", rowCell1));
                QString rowCell2;
                rowCell2.append(tr("E%1:G%2").arg(i).arg(i));
                initStripQualityExcelItem(worksheet->querySubObject("Range(const QString&)", rowCell2));
                QString rowCell3;
                rowCell3.append(tr("H%1:J%2").arg(i).arg(i));
                initStripQualityExcelItem(worksheet->querySubObject("Range(const QString&)", rowCell3));
            }
            QString row16;
            row16.append("A15:J15");//在线检测记录
            initStripQualityExcelItem(worksheet->querySubObject("Range(const QString&)", row16));
            for (int i = 16; i < rowcount_stripQuality+rowcount_testRecord + 2; i++) {
               /* QString rowCell1;
                rowCell1.append(tr("A%1:B%2").arg(i).arg(i));
                initStripQualityExcelItem(worksheet->querySubObject("Range(const QString&)", rowCell1));
                QString rowCell2;
                rowCell2.append(tr("E%1:F%2").arg(i).arg(i));
                initStripQualityExcelItem(worksheet->querySubObject("Range(const QString&)", rowCell2));*/
                QString rowCell3;
                rowCell3.append(tr("F%1:I%2").arg(i).arg(i));//描述
                initStripQualityExcelItem(worksheet->querySubObject("Range(const QString&)", rowCell3));
            }
            //QTableView 获取表格数据部分
            for (i = 0; i < rowcount_stripQuality; i++) //行数
            {
              
                for (j = 0; j < colcount_stripQuality; j++)   //列数
                {
                    QModelIndex index = ui.tableView_stripQuality->model()/*model_stripQuality*/->index(i, j);
                    QString strdata = ui.tableView_stripQuality->model()/*model_stripQuality*/->data(index).toString();
                    QAxObject *cell = worksheet->querySubObject("Cells(int,int)", i + 2, j + 1);
                    cell->dynamicCall("SetValue(const QString&)", strdata);
                    // QString picDir = QString("D:\\workT\\manuvision-platform\\build\\2020-06-30.png");
                     //cell->dynamicCall("AddPicture( QString&, bool, bool, double, double, double, double", picDir, true, true, 20, 12, pix.width() / 2, pix.height() / 2);
                    cell->setProperty("WrapText", true);
                    cell->setProperty("MergeCells", true);
                    cell->setProperty("HorizontalAlignment", -4108);//xlCenter
                    cell->setProperty("VerticalAlignment", -4108);//xlCenter
                }
            }
            //QTableView 获取表格数据部分
            for (i = 0; i < rowcount_testRecord; i++) //行数
            {
                for (j = 0; j < colcount_testRecord; j++)   //列数
                {
                    QString strdata;
                    if (i <=1) {
                        QModelIndex index = ui.tableView_testRecord->model()->index(i, j);
                        strdata = ui.tableView_testRecord->model()->data(index).toString();
						if (j == 6) {
							strdata = "处置";
						}
                    }
                    else
                    {
                        strdata = testRecordData.at(i-2).at(j + 1).toString();
						if (j == 6) {
							strdata = testRecordData.at(i - 2).at(13).toString();
							if (strdata.simplified() == "1") {
								strdata = "让步";
							}
							else
							{
								strdata = "剪切让步";
							}
						}
                    }
					if (j == 6) {
						QAxObject *cell = worksheet->querySubObject("Cells(int,int)", i + 15, j + 4);
						cell->dynamicCall("SetValue(const QString&)", strdata);
						// QString picDir = QString("D:\\workT\\manuvision-platform\\build\\2020-06-30.png");
						 //cell->dynamicCall("AddPicture( QString&, bool, bool, double, double, double, double", picDir, true, true, 20, 12, pix.width() / 2, pix.height() / 2);
						cell->setProperty("WrapText", true);
						cell->setProperty("MergeCells", true);
						cell->setProperty("HorizontalAlignment", -4108);//xlCenter
						cell->setProperty("VerticalAlignment", -4108);//xlCenter
					}
					else
					{
						QAxObject *cell = worksheet->querySubObject("Cells(int,int)", i + 15, j + 1);
						cell->dynamicCall("SetValue(const QString&)", strdata);
						// QString picDir = QString("D:\\workT\\manuvision-platform\\build\\2020-06-30.png");
						 //cell->dynamicCall("AddPicture( QString&, bool, bool, double, double, double, double", picDir, true, true, 20, 12, pix.width() / 2, pix.height() / 2);
						cell->setProperty("WrapText", true);
						cell->setProperty("MergeCells", true);
						cell->setProperty("HorizontalAlignment", -4108);//xlCenter
						cell->setProperty("VerticalAlignment", -4108);//xlCenter
					}
                    
				
                }
            }
      
            QString row17;
            row17.append(tr("A%1:A%2").arg(rowcount_testRecord + rowcount_stripQuality+2)
                .arg(rowcount_testRecord + rowcount_stripQuality + 3));
            initStripQualityExcelItem(worksheet->querySubObject("Range(const QString&)", row17));
            QString row18;
            row18.append(tr("B%1:J%2").arg(rowcount_testRecord + rowcount_stripQuality + 2)
                .arg(rowcount_testRecord + rowcount_stripQuality + 3));
            initStripQualityExcelItem(worksheet->querySubObject("Range(const QString&)", row18));
            QString row19;
            row19.append(tr("A%1:D%1").arg(rowcount_testRecord + rowcount_stripQuality + 4));
            initStripQualityExcelItem(worksheet->querySubObject("Range(const QString&)", row19));
           // QString row20;
            //row20.append("E23:F23");
            //initStripQualityExcelItem(worksheet->querySubObject("Range(const QString&)", row20));
            QString row21;
            row21.append(tr("F%1:J%1").arg(rowcount_testRecord + rowcount_stripQuality + 4));
            initStripQualityExcelItem(worksheet->querySubObject("Range(const QString&)", row21));
            QString row22;
            row22.append(tr("A%1:J%1").arg(rowcount_testRecord + rowcount_stripQuality+rowcount_remark+4));
            initStripQualityExcelItem(worksheet->querySubObject("Range(const QString&)", row22));
            QString row23;
            row23.append(tr("A%1:J%2").arg(rowcount_testRecord + rowcount_stripQuality + rowcount_remark + 2)
            .arg(rowcount_testRecord + rowcount_stripQuality + rowcount_remark + 3));
            initStripQualityExcelItem(worksheet->querySubObject("Range(const QString&)", row23));
            QString row24;
            row24.append(tr("A%1:J%2").arg(rowcount_testRecord + rowcount_stripQuality + rowcount_remark + 5)
                .arg(rowcount_testRecord + rowcount_stripQuality + rowcount_remark + 5));
            initStripQualityExcelItem(worksheet->querySubObject("Range(const QString&)", row24));
            QString row25;
            row25.append(tr("A%1:J%2").arg(rowcount_testRecord + rowcount_stripQuality + rowcount_remark + 14)
                .arg(rowcount_testRecord + rowcount_stripQuality + rowcount_remark + 14));
            initStripQualityExcelItem(worksheet->querySubObject("Range(const QString&)", row25));
            QString row26;
            row26.append(tr("A%1:J%2").arg(rowcount_testRecord + rowcount_stripQuality + rowcount_remark + 13)
                .arg(rowcount_testRecord + rowcount_stripQuality + rowcount_remark + 13));
            initStripQualityExcelItem(worksheet->querySubObject("Range(const QString&)", row26));
            QString row27;
            row27.append(tr("A%1:A%2").arg(rowcount_testRecord + rowcount_stripQuality + rowcount_remark + 11)
                .arg(rowcount_testRecord + rowcount_stripQuality + rowcount_remark + 12));
            initStripQualityExcelItem(worksheet->querySubObject("Range(const QString&)", row27));
            QString row28;
            row28.append(tr("B%1:J%2").arg(rowcount_testRecord + rowcount_stripQuality + rowcount_remark + 11)
                .arg(rowcount_testRecord + rowcount_stripQuality + rowcount_remark + 12));
            initStripQualityExcelItem(worksheet->querySubObject("Range(const QString&)", row28));
            QString row29;
            row29.append(tr("A%1:A%2").arg(rowcount_testRecord + rowcount_stripQuality + rowcount_remark + 20)
                .arg(rowcount_testRecord + rowcount_stripQuality + rowcount_remark + 21));
            initStripQualityExcelItem(worksheet->querySubObject("Range(const QString&)", row29));
            QString row30;
            row30.append(tr("B%1:J%2").arg(rowcount_testRecord + rowcount_stripQuality + rowcount_remark + 20)
                .arg(rowcount_testRecord + rowcount_stripQuality + rowcount_remark + 21));
            initStripQualityExcelItem(worksheet->querySubObject("Range(const QString&)", row30));
            QString row31;
            row31.append(tr("A%1:J%2").arg(rowcount_testRecord + rowcount_stripQuality + rowcount_remark + 7)
                .arg(rowcount_testRecord + rowcount_stripQuality + rowcount_remark + 7));
            initStripQualityExcelItem(worksheet->querySubObject("Range(const QString&)", row31));
            QString row32;
            row32.append(tr("A%1:J%2").arg(rowcount_testRecord + rowcount_stripQuality + rowcount_remark + 16)
                .arg(rowcount_testRecord + rowcount_stripQuality + rowcount_remark + 16));
            initStripQualityExcelItem(worksheet->querySubObject("Range(const QString&)", row32));
          
            QStringList listRow;
            listRow << "6" << "8" << "9" << "10" << "15" << "17" << "18" << "19";
            for (int i = 0; i < listRow.size(); i++) {
                QString row39;
                row39.append(tr("G%1:J%1").arg(rowcount_testRecord + rowcount_stripQuality + rowcount_remark + listRow.at(i).toInt()));
                initStripQualityExcelItem(worksheet->querySubObject("Range(const QString&)", row39));
               
            }
            //QTableView 获取表格数据部分
            for (i = 0; i < rowcount_remark; i++) //行数
            {

                for (j = 0; j < colcount_remark; j++)   //列数
                {
                    QModelIndex index = ui.tableView_remark->model()->index(i, j);
                    QString strdata = ui.tableView_remark->model()->data(index).toString();
                    QAxObject *cell = worksheet->querySubObject("Cells(int,int)", i + rowcount_stripQuality + rowcount_testRecord + 2, j + 1);
                    cell->dynamicCall("SetValue(const QString&)", strdata);
                    // QString picDir = QString("D:\\workT\\manuvision-platform\\build\\2020-06-30.png");
                     //cell->dynamicCall("AddPicture( QString&, bool, bool, double, double, double, double", picDir, true, true, 20, 12, pix.width() / 2, pix.height() / 2);
                    cell->setProperty("WrapText", true);
                    cell->setProperty("MergeCells", true);
                    cell->setProperty("HorizontalAlignment", -4108);//xlCenter
                    cell->setProperty("VerticalAlignment", -4108);//xlCenter
                }
            }
            for (i = 0; i < rowcount_Inspection; i++) //行数
            {

                for (j = 0; j < colcount_Inspection; j++)   //列数
                {
                    QModelIndex index = ui.tableView_qualityInspectionRecord->model()->index(i, j);
                    QString strdata = ui.tableView_qualityInspectionRecord->model()->data(index).toString();
                    QAxObject *cell = worksheet->querySubObject("Cells(int,int)", i + rowcount_stripQuality + rowcount_testRecord + rowcount_remark + 2 + 2, j + 1);
                    cell->dynamicCall("SetValue(const QString&)", strdata);
                    // QString picDir = QString("D:\\workT\\manuvision-platform\\build\\2020-06-30.png");
                     //cell->dynamicCall("AddPicture( QString&, bool, bool, double, double, double, double", picDir, true, true, 20, 12, pix.width() / 2, pix.height() / 2);
                    cell->setProperty("WrapText", true);
                    cell->setProperty("MergeCells", true);
                    cell->setProperty("HorizontalAlignment", -4108);//xlCenter
                    cell->setProperty("VerticalAlignment", -4108);//xlCenter
                }
            }
           //画框线
            QString lrange;
            lrange.append(tr("A2:J%1").arg(rowcount_stripQuality+rowcount_testRecord+rowcount_remark+rowcount_Inspection+2+1));
           // lrange.append(colcount_stripQuality - 1 + 'A');
            //lrange.append(QString::number(colcount_stripQuality + 2));
            range = worksheet->querySubObject("Range(const QString&)", lrange);
            range->querySubObject("Borders")->setProperty("LineStyle", QString::number(1));
            range->querySubObject("Borders")->setProperty("Color", QColor(0, 0, 0));

            //调整数据区行高
            QString rowsName;
            rowsName.append("2:");
            rowsName.append(QString::number(rowcount_stripQuality + rowcount_testRecord + rowcount_remark + rowcount_Inspection + 2 + 2));
            range = worksheet->querySubObject("Range(const QString&)", rowsName);
            range->setProperty("RowHeight", 20);

            workbook->dynamicCall("SaveAs(const QString&)", QDir::toNativeSeparators(fileName));//保存至fileName
            workbook->dynamicCall("Close()");//关闭工作簿
            excel->dynamicCall("Quit()");//关闭excel
            delete excel;
            excel = nullptr;
            if (QMessageBox::question(nullptr, tr("完成"), tr("文件已经导出，是否现在打开？"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
            {
                QDesktopServices::openUrl(QUrl("file:///" + QDir::toNativeSeparators(fileName)));
            }
        }
        else
        {
            QMessageBox::warning(nullptr, tr("错误"), tr("未能创建 Excel 对象，请安装 Microsoft Excel。"), QMessageBox::Apply);
        }
    }
}
//excel操作
void dataMainFormZJ::initStripQualityExcelItem(QAxObject *Object)
{
    Object->setProperty("WrapText", true);
    Object->setProperty("MergeCells", true);
    Object->setProperty("HorizontalAlignment", -4108);//xlCenter
    Object->setProperty("VerticalAlignment", -4108);//xlCenter
}
//保存配置文件json
void dataMainFormZJ::on_pushButton_save_setting_clicked()
{    
    QListWidgetItem *item = ui.listWidget_settingFile->currentItem();
    QMap<QString, QMap<QString, QVariant>> mapSettingData;
    QMap<QString, QVariant> listValue;
    mapSettingData = ui.tableView_qualityStandard->getTableData();
    listValue.insert("type", item->text().split('_').at(0));//
    mapSettingData.insert("type", listValue);
    if (ui.listWidget_settingFile->currentItem()== NULL) {
        QMessageBox::question(nullptr, ("提示"), ("请添加阈值文件"), QMessageBox::Yes);
        return;
    }
    QString filePath = QString("%1%2").arg(_settingFilePath).arg(item->text());
    publicClass::instance()->writeJson(filePath.simplified(),mapSettingData);
    std::map<QString, QString> data;
    data["thresholdID"] = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmsszzz"); //ID
    data["updatetime"] = mapSettingData.value("updatetime").value("updatetime").toString(); //更新时间
    //干纱宽度
    data["gansha_width"] = mapSettingData.value("gansha_width").value("gansha_width").toString();
    //干纱高度
    data["gansha_height"] = mapSettingData.value("gansha_height").value("gansha_height").toString();

    //开裂宽度
    data["kailie_width"] = mapSettingData.value("kailie_width").value("kailie_width").toString();
    //开裂高度
    data["kailie_height"] = mapSettingData.value("kailie_height").value("kailie_height").toString();
    
    data["huangban_color"] = mapSettingData.value("huangban_color").value("huangban_color").toString();//黄斑
    data["xuhua_width"] = mapSettingData.value("xuhua_width").value("xuhua_width").toString();//虚化宽度
    data["wuwu_area"] = mapSettingData.value("wuwu_area").value("wuwu_area").toString();//污物
    data["bujun_width"] = mapSettingData.value("bujun_width").value("bujun_width").toString();//玻纤分布不均
    data["wanzhe_curvature"] = mapSettingData.value("wanzhe_curvature").value("wanzhe_curvature").toString();//玻纤弯折
    data["type"] = mapSettingData.value("type").value("type").toString();//阈值类型  
    //data["model_name"] = _modelName;//模型名称
    //data["json_path"] = filePath;//
    QString table = "threshold_table";
    QSqlQuery * ret;
    std::map<QString, QString> where;
    where["json_path"] = filePath.simplified();
    if (p_sqlOperationJson->Updata(table, where, data, &ret)) {
        QMessageBox::question(nullptr, ("提示"), ("保存配置文件成功"), QMessageBox::Ok);
    }
    else
    {
        QMessageBox::question(nullptr, ("提示"), 
            tr("保存配置文件失败（%1）").arg(ret->lastError().text()), QMessageBox::Ok);
    }
    //p_sqlOperationJson->Add(table, data, &ret);

}
void dataMainFormZJ::initThresholdJsonFile()
{
    QStringList listStr;
    listStr << tr("database_%1.json").arg(_modelName) << tr("alarm_%1.json").arg(_modelName)
        << tr("export_%1.json").arg(_modelName);
    ui.listWidget_settingFile->clear();
    for (int i = 0; i < listStr.size(); i++) {
        QListWidgetItem *item = new QListWidgetItem;
        QString fileStr;        
        item->setText(listStr.at(i));
        //  item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
        ui.listWidget_settingFile->addItem(item);
        //  item->setSelected(true);
        ui.listWidget_settingFile->setCurrentItem(item);
        QMap<QString, QMap<QString, QVariant>> mapSettingData;
        QMap<QString, QVariant> listValue;
        mapSettingData = ui.tableView_qualityStandard->getTableData();
        listValue.insert("type", item->text().split('_').at(0));//
        mapSettingData.insert("type", listValue);
        /*
        if (ui.listWidget_settingFile->currentItem() == NULL) {
            QMessageBox::question(nullptr, ("提示"), ("请添加阈值文件"), QMessageBox::Yes);
            return;
        }*/
        QString filePath = QString("%1%2").arg(_settingFilePath).arg(item->text());
        publicClass::instance()->writeJson(filePath.simplified(), mapSettingData);
        std::map<QString, QString> data;
        data["thresholdID"] = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmsszzz"); //ID
        data["updatetime"] = mapSettingData.value("updatetime").value("updatetime").toString(); //更新时间
        //干纱宽度
        data["gansha_width"] = mapSettingData.value("gansha_width").value("gansha_width").toString();
        //干纱高度
        data["gansha_height"] = mapSettingData.value("gansha_height").value("gansha_height").toString();

        //开裂宽度
        data["kailie_width"] = mapSettingData.value("kailie_width").value("kailie_width").toString();
        //开裂高度
        data["kailie_height"] = mapSettingData.value("kailie_height").value("kailie_height").toString();

        data["huangban_color"] = mapSettingData.value("huangban_color").value("huangban_color").toString();//黄斑
        data["xuhua_width"] = mapSettingData.value("xuhua_width").value("xuhua_width").toString();//虚化宽度
        data["wuwu_area"] = mapSettingData.value("wuwu_area").value("wuwu_area").toString();//污物
        data["bujun_width"] = mapSettingData.value("bujun_width").value("bujun_width").toString();//玻纤分布不均
        data["wanzhe_curvature"] = mapSettingData.value("wanzhe_curvature").value("wanzhe_curvature").toString();//玻纤弯折
        data["type"] = mapSettingData.value("type").value("type").toString();//阈值类型  
        data["model_name"] = _modelName;//模型名称
        data["json_path"] = filePath.simplified();//json path
        QString table = "threshold_table";
        QSqlQuery * ret;
        if (!p_sqlOperationJson->Add(table, data, &ret)) {
            QMessageBox::question(nullptr, ("提示"),
                tr("添加配置文件失败（%1）").arg(ret->lastError().text()), QMessageBox::Ok);
        }
    }
    
}
//添加json文件类型
void dataMainFormZJ::on_toolButton_add_clicked()
{
    QListWidgetItem *item = new QListWidgetItem;
    QString fileStr;
    switch (ui.comboBox_value->currentIndex())
    {
    case 0: {
        fileStr = "database";
    }
            break;
    case 1: {
        fileStr = "alarm";
    }
            break;
    case 2: {
        fileStr = "export";
    }
            break;
    default:
        break;
    }
    item->setText(tr("%1_%2.json").arg(fileStr)
        .arg(_modelName/*QDateTime::currentDateTime().toString("yyyyMMdd_hhmmsszzz")*/));
  //  item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
    ui.listWidget_settingFile->addItem(item);
  //  item->setSelected(true);
    ui.listWidget_settingFile->setCurrentItem(item);
}
//双击json重命名
void dataMainFormZJ::ItemDoubleClicked(QListWidgetItem* item) {
    _PrevName = _settingFilePath+item->text().simplified();
    _RenameIndex = ui.listWidget_settingFile->currentRow(); 
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    _bIsRename = true;
    //QFile file(tr("%1%2").arg(_settingFilePath).arg(item->text().simplified()));
    //file.rename(text, item->text().simplified());
   // ui.plainTextEdit->setPlainText(text);
}

//点击json文件列表，读取json数据到界面
void dataMainFormZJ::ItemClicked(QListWidgetItem* item)
{
    //读取json文件
   qDebug()<< item->text();
   QString filePath = QString(_settingFilePath + item->text().simplified());
   QMap<QString, QVariant> map = publicClass::instance()->readJson(filePath);
   ui.tableView_qualityStandard->setTableData(map);//界面上配置json数据
}
//初始化json文件列表
void dataMainFormZJ::initListWidgetFilelist(const QFileInfoList &list)
{
    ui.listWidget_settingFile->clear();
    for (int i = 0; i < list.size(); i++)
    {
        list.at(i).fileName();

        QListWidgetItem *item = new QListWidgetItem;
        QString fileStr;
        item->setText(list.at(i).fileName());
        //  item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
        ui.listWidget_settingFile->addItem(item);
        //  item->setSelected(true);
        if (i == 0) {
            ui.listWidget_settingFile->setCurrentItem(item);
        }
       
    }
}
// json文件列表item重命名
void dataMainFormZJ::OnListWidgetCurrentListChanged(int index)
{
    if (!_bIsRename)
    {
        return;
    }
    else
    {
        _bIsRename = false;
        QString fileName = _settingFilePath + ui.listWidget_settingFile->item(_RenameIndex)->text().simplified();
        if (_PrevName == fileName) {
            return;
        }
        bool ok = QFile::rename(_PrevName, fileName);
        if (!ok)
        {
            QMessageBox::information(NULL, tr("Error"), tr("Rename Failed"));
            return;
        }
    }
   
}

//返回订单信息 ， 生产技术要求 ， 首检记录 数据
std::map<QString, QString> dataMainFormZJ::getTableView_stripQuality_Data()
{
    //delete from quality_record_table where volume_number is null 删除空的数据
    //delete from quality_record_table where volume_number = 111
    std::map<QString, QString> data;
    QStandardItemModel *model = static_cast<QStandardItemModel*>(ui.tableView_stripQuality->model());

	QStandardItemModel *modelRemark = static_cast<QStandardItemModel*>(ui.tableView_remark->model());
    //订单信息
	//QString startTime = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
    data["record_ID"] = _dateTimeKey;//唯一ID
    data["volume_number"] = _volumeNum;//卷号
    data["work_order"] = model->item(1, 1)->text().simplified();//工作令
    data["product_brand"] = model->item(1, 5)->text().simplified();//产品牌号
    data["glass_fiber_content_technical"] = model->item(3, 2)->text().simplified();//玻纤含量 生产技术要求
    data["areal_density_technical"] = model->item(3, 5)->text().simplified();//面密度 
    data["thickness_technical"] = model->item(3, 7)->text().simplified();//厚度 
    data["wide_format_technical"] = model->item(3, 9)->text().simplified();//宽幅

    data["glass_fiber_content_test"] = model->item(5, 2)->text().simplified();//玻纤含量 首检记录
    data["areal_density_test"] = model->item(5, 5)->text().simplified(); //面密度
    data["thickness_test"] = model->item(5,7)->text().simplified(); //厚度
    data["wide_format_test"] = model->item(5, 9)->text().simplified(); //宽幅

    data["sha_xin_test"] = model->item(6, 2)->text().simplified(); //沙筘
    data["number_of_yarns_test"] = model->item(6, 5)->text().simplified();//纱线根数
    data["spreading_width_test"] = model->item(6, 7)->text().simplified();//铺纱宽度
    data["speed_test"] = model->item(6, 9)->text().simplified();//速度
    data["date_time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd");  //创建日期 
    //data["last_roll_length"] = "";
    data["glass_fiber_grade"] = model->item(9, 4)->text().simplified();//玻璃纤维牌号
    data["glass_fiber_batch_number"] = model->item(9, 7)->text().simplified();//玻璃纤维批次号
    data["resin_grade"] = model->item(10, 4)->text().simplified();//树脂牌号
    data["resin_batch_number"] = model->item(10, 7)->text().simplified();//树脂批次号
    data["compatibilizer_grade"] = model->item(11, 4)->text().simplified();//相容剂牌号
    data["compatibilizer_batch_number"] = model->item(11, 7)->text().simplified();//相容剂批次号
    data["finished_product_grade"] = model->item(12, 4)->text().simplified();//产成品牌号
    data["finished_product_batch_number"] = model->item(12, 7)->text().simplified();//产成品批次号 
    //modelRemark->item(2, 0)->setText(QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss")); //生产时间
	data["start_time"] = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");//开始生产时间
    return data;
}
std::map<QString, QString> dataMainFormZJ::getTableView_stripQuality_Data(const QString &recordId)
{
    //delete from quality_record_table where volume_number is null 删除空的数据
    //delete from quality_record_table where volume_number = 111
    std::map<QString, QString> data;
    QStandardItemModel *model = static_cast<QStandardItemModel*>(ui.tableView_stripQuality->model());
	//备注区域
	QStandardItemModel *modelRemark = qobject_cast<QStandardItemModel*>(ui.tableView_remark->model());
    //订单信息

    //data["record_ID"] = _dateTimeKey;//唯一ID
    //data["volume_number"] = _volumeNum;//卷号
    data["work_order"] = model->item(1, 1)->text().simplified();//工作令
    data["product_brand"] = model->item(1, 5)->text().simplified();//产品牌号
    data["glass_fiber_content_technical"] = model->item(3, 2)->text().simplified();//玻纤含量 生产技术要求
    data["areal_density_technical"] = model->item(3, 5)->text().simplified();//面密度 
    data["thickness_technical"] = model->item(3, 7)->text().simplified();//厚度 
    data["wide_format_technical"] = model->item(3, 9)->text().simplified();//宽幅

    data["glass_fiber_content_test"] = model->item(5, 2)->text().simplified();//玻纤含量 首检记录
    data["areal_density_test"] = model->item(5, 5)->text().simplified(); //面密度
    data["thickness_test"] = model->item(5, 7)->text().simplified(); //厚度
    data["wide_format_test"] = model->item(5, 9)->text().simplified(); //宽幅

    data["sha_xin_test"] = model->item(6, 2)->text().simplified(); //沙筘
    data["number_of_yarns_test"] = model->item(6, 5)->text().simplified();//纱线根数
    data["spreading_width_test"] = model->item(6, 7)->text().simplified();//铺纱宽度
    data["speed_test"] = model->item(6, 9)->text().simplified();//速度
    data["date_time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd");  //创建日期 
    //data["last_roll_length"] = "";
    data["glass_fiber_grade"] = model->item(9, 4)->text().simplified();//玻璃纤维牌号
    data["glass_fiber_batch_number"] = model->item(9, 7)->text().simplified();//玻璃纤维批次号
    data["resin_grade"] = model->item(10, 4)->text().simplified();//树脂牌号
    data["resin_batch_number"] = model->item(10, 7)->text().simplified();//树脂批次号
    data["compatibilizer_grade"] = model->item(11, 4)->text().simplified();//相容剂牌号
    data["compatibilizer_batch_number"] = model->item(11, 7)->text().simplified();//相容剂批次号
    data["finished_product_grade"] = model->item(12, 4)->text().simplified();//产成品牌号
    data["finished_product_batch_number"] = model->item(12, 7)->text().simplified();//产成品批次号 
	//备注区域
	//data["start_time"] = modelRemark->item(2, 0)->text(); //生产时间
	data["remark"] = modelRemark->item(0, 1)->text().simplified();//备注
	data["operator"] = modelRemark->item(2, 5)->text(); //操作员
    return data;
}
//带材质量记录表 原材料信息
std::map<QString, QString> dataMainFormZJ::getTableView_stripQuality_raw_material_Data()
{
    std::map<QString, QString> data;
    QStandardItemModel *model = static_cast<QStandardItemModel*>(ui.tableView_stripQuality->model());
    data["glass_fiber_grade"] = model->item(9, 4)->text().simplified();//玻璃纤维牌号
    data["glass_fiber_batch_number"] = model->item(9, 7)->text().simplified();//玻璃纤维批次号
    data["resin_grade"] = model->item(10, 4)->text().simplified();//树脂牌号
    data["resin_batch_number"] = model->item(10, 7)->text().simplified();//树脂批次号
    data["compatibilizer_grade"] = model->item(11, 4)->text().simplified();//相容剂牌号
    data["compatibilizer_batch_number"] = model->item(11, 7)->text().simplified();//相容剂批次号
    data["finished_product_grade"] = model->item(12, 4)->text().simplified();//产成品牌号
    data["finished_product_batch_number"] = model->item(12, 7)->text().simplified();//产成品批次号 
    data["record_ID"] = _dateTimeKey;//唯一id
    data["raw_materialID"] = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmsszzz");//原材料信息id
    return data;
}
//在线检测记录数据
std::map<QString, QString> dataMainFormZJ::getTableView_stripQuality_test_record_Data()
{
    QStandardItemModel *model = static_cast<QStandardItemModel*>(ui.tableView_stripQuality->model());
    std::map<QString, QString> data;
    data["dataID"] = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmsszzz"); //ID
    data["meters"] = "1";//米数
    data["thickness"] = "2";//厚度
    data["defect"] = "3";//缺陷
    data["position"] = "4";//位置
    data["length"] = "5";//长度
    data["description"] = "6";//描述
    data["remarks"] = tr("前200m收卷起褶，多跑；0点边部1cm虚化，分切时注意,");//备注
    data["production_time"] = "2020.06.17  22:25——2020.06.18  02:38 ";//生产时间
    data["operator"] = "谭创";//操作员
    data["record_ID"] = _dateTimeKey;
    data["savename"] = ".png";//图片文件名  
    return data;
}
//加载订单信息数据
void dataMainFormZJ::setTableView_stripQuality_Data(const QString &recordID)
{
    if (recordID.isEmpty()) {
        return;
    }
    QString table = "quality_record_table";
    QSqlQuery * ret_volume;//卷号
    //QList<QVariantList> rowData;//查询卷号id映射表
    QList<QMap<QString, QString>> rowData;//查询卷号id映射表
    QMap<QString, QString> where_volume_num;//索引值   
    where_volume_num["record_ID"] = recordID;
    QStringList ListKey;//查询卷号id映射表   
    ListKey.push_back("record_ID");
    ListKey.push_back("volume_number");//卷号
    ListKey.push_back("work_order");//工作令
    ListKey.push_back("product_brand");//产品牌号
    ListKey.push_back("glass_fiber_content_technical");//玻纤含量 生产技术要求
    ListKey.push_back("areal_density_technical");// = model->item(3, 5)->text().simplified();//面密度 
    ListKey.push_back("thickness_technical");//厚度 
    ListKey.push_back("wide_format_technical");//宽幅

    ListKey.push_back("glass_fiber_content_test");//玻纤含量 首检记录
    ListKey.push_back("areal_density_test"); //面密度
    ListKey.push_back("thickness_test"); //厚度
    ListKey.push_back("wide_format_test"); //宽幅

    ListKey.push_back("sha_xin_test"); //沙筘
    ListKey.push_back("number_of_yarns_test");//纱线根数
    ListKey.push_back("spreading_width_test");//铺纱宽度
    ListKey.push_back("speed_test");//速度
    ListKey.push_back("date_time");  //创建日期 
  
    ListKey.push_back("last_roll_length");
    ListKey.push_back("glass_fiber_grade");//玻璃纤维牌号
    ListKey.push_back("glass_fiber_batch_number");//玻璃纤维批次号
    ListKey.push_back("resin_grade");//树脂牌号
    ListKey.push_back("resin_batch_number");//树脂批次号
    ListKey.push_back("compatibilizer_grade");//相容剂牌号
    ListKey.push_back("compatibilizer_batch_number");//相容剂批次号
    ListKey.push_back("finished_product_grade");//产成品牌号
    ListKey.push_back("finished_product_batch_number");//产成品批次号 
    ListKey.push_back("last_check_length");
    ListKey.push_back("other_lenth");
	ListKey.push_back("start_time");//开始时间 
	ListKey.push_back("end_time");//结束时间
	ListKey.push_back("remark");//备注
	ListKey.push_back("operator");//作者
    if (!/*databaseSqlInset::get_db()*/p_sqlOperationOrder->findData(table, where_volume_num, ListKey, rowData, &ret_volume)) {
        QMessageBox::warning(nullptr, tr("错误"), 
            tr("查询订单数据失败(%1)").arg(ret_volume->lastError().text()), QMessageBox::Apply);
    }
    if (rowData.size()<= 0) {
        return;
    }
    QMap<QString, QString> mapData = rowData.at(0);
    QStandardItemModel *model = static_cast<QStandardItemModel*>(ui.tableView_stripQuality->model());
	//备注区域
	QStandardItemModel *modelRemark = static_cast<QStandardItemModel*>(ui.tableView_remark->model());
   // data["record_ID"] = _dateTimeKey;//唯一ID
   // data["volume_number"] = _volumeNum;//卷号

    model->item(1, 1)->setText(mapData.value("work_order"));//工作令
    model->item(1, 5)->setText(mapData.value("product_brand"));//产品牌号
    model->item(3, 2)->setText(mapData.value("glass_fiber_content_technical"));//玻纤含量 生产技术要求
    model->item(3, 5)->setText(mapData.value("areal_density_technical"));//面密度 
    model->item(3, 7)->setText(mapData.value("thickness_technical"));//厚度 
    model->item(3, 9)->setText(mapData.value("wide_format_technical"));//宽幅

    model->item(5, 2)->setText(mapData.value("glass_fiber_content_test"));//玻纤含量 首检记录
    model->item(5, 5)->setText(mapData.value("areal_density_test")); //面密度
    model->item(5, 7)->setText(mapData.value("thickness_test")); //厚度
    model->item(5, 9)->setText(mapData.value("wide_format_test")); //宽幅

    model->item(6, 2)->setText(mapData.value("sha_xin_test")); //沙筘
    model->item(6, 5)->setText(mapData.value("number_of_yarns_test"));//纱线根数
    model->item(6, 7)->setText(mapData.value("spreading_width_test"));//铺纱宽度
    model->item(6, 9)->setText(mapData.value("speed_test"));//速度
   // data["date_time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd");  //创建日期 
  //  data["last_roll_length"] = "";
    model->item(9, 4)->setText(mapData.value("glass_fiber_grade"));//玻璃纤维牌号
    model->item(9, 7)->setText(mapData.value("glass_fiber_batch_number"));//玻璃纤维批次号
    model->item(10, 4)->setText(mapData.value("resin_grade"));//树脂牌号
    model->item(10, 7)->setText(mapData.value("resin_batch_number"));//树脂批次号
    model->item(11, 4)->setText(mapData.value("compatibilizer_grade"));//相容剂牌号
    model->item(11, 7)->setText(mapData.value("compatibilizer_batch_number"));//相容剂批次号
    model->item(12, 4)->setText(mapData.value("finished_product_grade"));//产成品牌号
    model->item(12, 7)->setText(mapData.value("finished_product_batch_number"));//产成品批次号
	//备注区域
	modelRemark->item(2, 0)->setText(
	QString("%1---%2").arg(mapData.value("start_time"))
	.arg(mapData.value("end_time"))); //生产时间
    modelRemark->item(0, 1)->setText(mapData.value("remark"));//备注
    modelRemark->item(2, 5)->setText(mapData.value("operator")); //操作员
	modelRemark->item(2, 0)->setTextAlignment(Qt::AlignCenter);
	modelRemark->item(0, 1)->setTextAlignment(Qt::AlignCenter);//备注
	modelRemark->item(2, 5)->setTextAlignment(Qt::AlignCenter); //操作员
}
//加载原材料信息
void dataMainFormZJ::setTableView_stripQuality_raw_material_Data(const QString &recordID)
{

}
//加载在线检测记录数据
void dataMainFormZJ::setTableView_stripQuality_test_record_Data(const QString &recordID)
{
    //if (!_isRefresh) {
      //  ui.tableView_testRecord->stopUpdate();
        ui.tableView_testRecord->updataItem(recordID);
    //}
    
}
//卷号与唯一id识别值映射表
/*std::map<QString, QString> dataMainFormZJ::getTableView_stripQuality_volume_id_Data()
{
    QStandardItemModel *model = static_cast<QStandardItemModel*>(ui.tableView_stripQuality->model());
    std::map<QString, QString> data;
    data["record_ID"] = _dateTimeKey;
    data["volume_number"] = model->item(1, 8)->text().simplified();
    return data;
}*/
//根据当前数据库获取卷号数量（三天内）
void dataMainFormZJ::initVolumeNum()
{
    ui.comboBox_juanhao->clear();
    _mapVolume_recordID.clear();
    QString table = "quality_record_table";
    QSqlQuery * ret;
    QList<QVariantList> row;
    QString dataKey = "date_time";
    QStringList keyList;
    keyList << "volume_number"<<"record_ID";
    if (!databaseSqlInset::get_db()->findData_Date(table, keyList, row, RECORD_DAYS, dataKey, &ret)) {
        QMessageBox::warning(nullptr, tr("错误"), tr("初始化卷号数据失败（%1）")
            .arg(ret->lastError().text()), QMessageBox::Apply);
    }
    for (int i = 0; i < row.size(); i++) {
        ui.comboBox_juanhao->addItem(row.at(i).at(0).toString());
        _mapVolume_recordID.insert(row.at(i).at(0).toString(), row.at(i).at(1).toString());
    }
	if (row.size() != 0) {
		//ui.tableView_testRecord->updataItem(row.at(row.size() - 1).at(1).toString());
	}
	
    
}
//添加卷号
void dataMainFormZJ::addComboxVolumeNumItem(const QString &text)
{
    if (text.simplified().isEmpty()) {
        return;
    }
    ui.comboBox_juanhao->addItem(text.simplified());
    ui.comboBox_juanhao->currentIndex();
}
//跟卷号同步
void dataMainFormZJ::slotChangeItem(const QString & textItem)
{
    //切换订单信息表格卷号
    ui.tableView_stripQuality->setItemData(1, 8, textItem,qobject_cast<QStandardItemModel*>(ui.tableView_stripQuality->model()));
    //映射卷号与record_ID
    ui.tableView_stripQuality->setCurrentRecord_ID(_mapVolume_recordID.value(textItem.simplified()).simplified(),
              textItem, ui.comboBox_juanhao->currentIndex()); 
    //加载订单信息
    setTableView_stripQuality_Data(_mapVolume_recordID.value(textItem.simplified()).simplified());
    //加载在线检测记录
    setTableView_stripQuality_test_record_Data(_mapVolume_recordID.value(textItem.simplified()).simplified());
    //加载质检记录
    ui.tableView_qualityInspectionRecord->setInspectionFirst(_mapVolume_recordID.value(textItem.simplified()).simplified());
    ui.tableView_qualityInspectionRecord->setInspectionEnd(_mapVolume_recordID.value(textItem.simplified()).simplified());

}
void dataMainFormZJ::updateChangeItem(const QString & text)
{
    //切换订单信息表格卷号
    ui.tableView_stripQuality->setItemData(1, 8, text, qobject_cast<QStandardItemModel*>(ui.tableView_stripQuality->model()));
    //映射卷号与record_ID
    ui.tableView_stripQuality->setCurrentRecord_ID(_mapVolume_recordID.value(text.simplified()).simplified(),
        text, ui.comboBox_juanhao->currentIndex());
    //加载订单信息
    setTableView_stripQuality_Data(_mapVolume_recordID.value(text.simplified()).simplified());
    //加载在线检测记录
    setTableView_stripQuality_test_record_Data(_mapVolume_recordID.value(text.simplified()).simplified());
}
//刷新卷号下拉菜单
void dataMainFormZJ::slotRefreshVolumeNum(bool isRefresh,int index)
{
    if (isRefresh) {
        initVolumeNum();
        QString text = ui.tableView_stripQuality->model()->index(1, 8).data().toString().simplified();
        ui.comboBox_juanhao->setCurrentIndex(index);
    }
    else
    {//删除卷号
        initVolumeNum();
        QString text = ui.tableView_stripQuality->model()->index(1, 8).data().toString().simplified();
        ui.comboBox_juanhao->setCurrentIndex(0);
    }
}
//添加新的ID，用来测试
void dataMainFormZJ::on_toolButton_addRecord_test_clicked()
{
    _dateTimeKey = QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz");//id
    _volumeNum = QDateTime::currentDateTime().toString("V_hhmmsszzz");//卷号

    //id改变先通知数据库(查询使用)
    databaseSqlInset::get_db()->setCurrentDataStatus(_dateTimeKey);

    //在线监测表格通知已更换recordID
    ui.tableView_testRecord->setCurrentRecordId(_dateTimeKey);
    //清空表格数据
    ui.tableView_stripQuality->tableViewClear();//订单信息
	ui.tableView_remark->tableViewClear();//备注区域
    //ui.tableView_remark->tableViewClear();//备注区域清除（暂时不清理）
    ui.tableView_testRecord->tableViewClear();//检测记录
   

    std::map<QString, QString> dataTableStrip2 = getTableView_stripQuality_Data();//订单信息
    if (!QString(dataTableStrip2["record_ID"]).isEmpty() && !QString(dataTableStrip2["volume_number"]).isEmpty()) { //订单信息

        QString table = "quality_record_table";
        QSqlQuery * ret;
        std::map<QString, QString> map_qua = dataTableStrip2;
        std::map<QString, QString> mapkey;
        mapkey["volume_number"] = map_qua["volume_number"];
        if (!/*databaseSqlInset::get_db()*/p_sqlOperationOrder->Add(table, dataTableStrip2, &ret)) {
            QMessageBox::warning(nullptr, tr("错误"), tr("插入订单信息失败（%1）")
                .arg(ret->lastError().text()), QMessageBox::Apply);
            return;
        }
        //卷号与id插入数据结构
        _mapVolume_recordID.insert(dataTableStrip2["volume_number"].simplified(), dataTableStrip2["record_ID"].simplified());
        addComboxVolumeNumItem(_volumeNum);//combox添加一个item，卷号信息
        int numCount = ui.comboBox_juanhao->count();
        //_isRefresh = true;
        if (!_isAddFlag) {
            ui.comboBox_juanhao->setCurrentIndex(numCount - 1);
            return;
        } 
        ui.tableView_stripQuality->setItemData(1, 8, 
            ui.comboBox_juanhao->currentText().simplified(),
            qobject_cast<QStandardItemModel*>(ui.tableView_stripQuality->model()));
        //slotChangeItem(_volumeNum.simplified());
    }
    
}
//新数据保存到数据库 （测试）
void dataMainFormZJ::on_toolButton_saveData_test_clicked()
{
    //setLastRolllength("129.024");
}
//保存订单信息等的编辑数据
void dataMainFormZJ::on_toolButton_editDataSave_clicked()
{

   // QMap<QString, QString> mapData = rowData.at(0);
    QStandardItemModel *model = static_cast<QStandardItemModel*>(ui.tableView_stripQuality->model());
    QString table = "quality_record_table";
    QSqlQuery *ret;
    std::map<QString, QString> where;
    std::map<QString, QString> data = getTableView_stripQuality_Data(_mapVolume_recordID.value(
        ui.comboBox_juanhao->currentText().simplified()));
    where["record_ID"] = _mapVolume_recordID.value(
        ui.comboBox_juanhao->currentText().simplified());//获取recordID并写入
   // data["last_roll_length"] = length;
    if (_mapVolume_recordID.value(ui.comboBox_juanhao->currentText().simplified()).isEmpty()) {
        return;
    }
    if (!/*p_sqlOperation->*/p_sqlOperationEditOrder->Updata(table, where, data, &ret)) {
        QMessageBox::warning(nullptr, tr("错误"),
            tr("更新订单信息数据失败(%1)").arg(ret->lastError().text()), QMessageBox::Ok);
    }
}
//启动在线检测记录刷新
void dataMainFormZJ::on_toolButton_startUpdate_clicked()
{

    if (!ui.tableView_testRecord->timerStatusisActive()) {
        ui.tableView_testRecord->startUpdate();
        ui.comboBox_juanhao->setCurrentIndex(ui.comboBox_juanhao->count() - 1);
        _timerAddRecord->start(3000);
        ui.toolButton_startUpdate->setText("停止刷新");
        _isAddFlag = false;
        ui.comboBox_juanhao->setCurrentIndex(ui.comboBox_juanhao->count() - 1);
		//ui.comboBox_page->setEnabled(false);
		//ui.toolButtonAdd->setEnabled(true);
		//disconnect(ui.comboBox_page, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(slotComboxUpDatePage(const QString &)));
    } 
    else
    {
        _isAddFlag = true;
        ui.tableView_testRecord->stopUpdate();
        _timerAddRecord->stop();
        ui.toolButton_startUpdate->setText("启动刷新");
		//ui.comboBox_page->setEnabled(true);
		//ui.toolButtonAdd->setEnabled(false);
		//connect(ui.comboBox_page, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(slotComboxUpDatePage(const QString &)));
    }
    //ui.comboBox_juanhao->setCurrentIndex(ui.comboBox_juanhao->count() - 1);
}
//根据卷号查询recordID
/*QList<QVariantList> dataMainFormZJ::getRecordData(const QString &volume)
{

    //映射卷号与record_ID
    QString table = "quality_record_table";
    QSqlQuery * ret_volume;//卷号
    QList<QVariantList> rowData;//查询卷号id映射表
    QMap<QString, QString> where_volume_num;//索引值   
    where_volume_num["volume_number"] = volume.simplified();
    QStringList ListKey;//查询卷号id映射表
    ListKey.append("volume_number");
    ListKey.append("record_ID");
    p_sqlOperation->findData(table, where_volume_num, ListKey, rowData, &ret_volume);
    return rowData;
}
*/
//生产工艺记录表编辑属性
void dataMainFormZJ::setTableViewEditFlags()
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui.tableView_record->model());

    for (int i = 0; i < model->rowCount(); i++) {
        for (int j = 0; j < model->columnCount(); j++)
        {
            if ((i == 0 && j == 2)||(i==0&& j==6)||(i==1&&j==3)||(i==2&&j==2)
                || (i == 2 && j == 5)|| (i == 2 && j == 7) || (i == 2 && j == 9)
                || (i == 21 && j == 1) ) {
                continue;
            }
            model->item(i, j)->setFlags(Qt::ItemIsEnabled);
        }
    }
}
//载入加热区温度85

void dataMainFormZJ::setTempData(const QMap<QString, QVariantList> &map)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui.tableView_record->model());
    QMap<QString, QVariantList> mapTemp = map;
    QMap<QString, QVariantList>::iterator it;
    QVariantList listTemp;
    int index;
    for (it = mapTemp.begin(); it != mapTemp.end(); it++) {
        if (it.key().simplified() == "0") {
           listTemp = mapTemp.value("0");
           index = 0;
        }
        else if (it.key().simplified() == "1")
        {
            listTemp = mapTemp.value("1");
            index = 1;
        }
        else if (it.key().simplified() == "2")
        {
            listTemp = mapTemp.value("2");
            index = 2;
        }
        else if (it.key().simplified() == "3")
        {
            listTemp = mapTemp.value("3");
            index = 3;
        }
        
    }
    //QVariantList listTemp = mapTemp.value(it.key());
    //int index = it.key().toInt();
    switch (index)
    {
    case 0: {
        for (int i = 0; i < listTemp.size(); i++) {
            model->item(_listTempDay1.at(i).x(), _listTempDay1.at(i).y())->setTextAlignment(Qt::AlignCenter);
            model->item(_listTempDay1.at(i).x(), _listTempDay1.at(i).y())->setForeground(QBrush(QColor(255, 0, 0)));
            model->item(_listTempDay1.at(i).x(), _listTempDay1.at(i).y())->setText(listTemp.at(i).toString());
         }
    }
            break;
    case 1: {
        for (int i = 0; i < listTemp.size(); i++) {
            model->item(_listTempDay2.at(i).x(), _listTempDay2.at(i).y())->setTextAlignment(Qt::AlignCenter);
            model->item(_listTempDay2.at(i).x(), _listTempDay2.at(i).y())->setForeground(QBrush(QColor(255, 0, 0)));
            model->item(_listTempDay2.at(i).x(), _listTempDay2.at(i).y())->setText(listTemp.at(i).toString());
        }
    }
            break;
    case 2: {
        for (int i = 0; i < listTemp.size(); i++) {
            model->item(_listTempNight1.at(i).x(), _listTempNight1.at(i).y())->setTextAlignment(Qt::AlignCenter);
            model->item(_listTempNight1.at(i).x(), _listTempNight1.at(i).y())->setForeground(QBrush(QColor(255, 0, 0)));
            model->item(_listTempNight1.at(i).x(), _listTempNight1.at(i).y())->setText(listTemp.at(i).toString());
        }
    }
            break;
    case 3: {
        for (int i = 0; i < listTemp.size(); i++) {
            model->item(_listTempNight2.at(i).x(), _listTempNight2.at(i).y())->setTextAlignment(Qt::AlignCenter);
            model->item(_listTempNight2.at(i).x(), _listTempNight2.at(i).y())->setForeground(QBrush(QColor(255, 0, 0)));
            model->item(_listTempNight2.at(i).x(), _listTempNight2.at(i).y())->setText(listTemp.at(i).toString());
        }
    }
            break;
    default:
        break;
    }
   
}
//更新加热区域
void dataMainFormZJ::initTempArea()
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui.tableView_record->model());
    for (int i = 0; i < model->rowCount(); i++) {
        for (int j = 1; j < model->columnCount(); j++) {

            if (i == 5||i==11) {
                _listTempDay1.push_back(QPoint(i, j));
            }
            else if(i == 17)
            {
                _listTempDay1.push_back(QPoint(i, 1));
                _listTempDay1.push_back(QPoint(i, 2));
                _listTempDay1.push_back(QPoint(i, 8));
                _listTempDay1.push_back(QPoint(i, 9));
            }
            else if (i == 6 || i == 12) {
                _listTempDay2.push_back(QPoint(i, j));
            }
            else if(i == 18)
            {
                _listTempDay2.push_back(QPoint(i, 1));
                _listTempDay2.push_back(QPoint(i, 2));
                _listTempDay2.push_back(QPoint(i, 8));
                _listTempDay2.push_back(QPoint(i, 9));
            }
            else if (i == 7 || i == 13) {
                _listTempNight1.push_back(QPoint(i, j));
            }
            else if (i == 19)
            {
                _listTempNight1.push_back(QPoint(i, 1));
                _listTempNight1.push_back(QPoint(i, 2));
                _listTempNight1.push_back(QPoint(i, 8));
                _listTempNight1.push_back(QPoint(i, 9));
            }
            else if (i == 8 || i == 14) {
                _listTempNight2.push_back(QPoint(i, j));
            }
            else if (i == 20)
            {
                _listTempNight2.push_back(QPoint(i, 1));
                _listTempNight2.push_back(QPoint(i, 2));
                _listTempNight2.push_back(QPoint(i, 8));
                _listTempNight2.push_back(QPoint(i, 9));
            }
        }
    }
}
//通知换卷信号
void dataMainFormZJ::setUpdataNewVolumeNum()
{
    on_toolButton_addRecord_test_clicked();
    QMap<int, QComboBox*> mapBox;
    mapBox.insert(0, ui.comboBox_juanhao);
    //mapBox.insert(1, ui.comboBox_value);
    publicClass::instance()->initComboBoxStyle(mapBox);
}
//记录该卷的总长(ciwa回调)
void dataMainFormZJ::setLastRolllength(QStringList lengthData,bool isEnd)
{
    _isEnd = isEnd;
    QString table = "quality_record_table";
    QSqlQuery *ret;
    std::map<QString, QString> where;
    std::map<QString, QString> data;
    where["record_ID"] = _dateTimeKey;//获取recordID并写入
    if (lengthData.isEmpty()) {
        return;
    }
    data["last_roll_length"] = lengthData.at(0);
    data["last_check_length"] = lengthData.at(1);
    data["other_lenth"] = lengthData.at(2);  
	data["end_time"] = lengthData.at(3);
    if (_dateTimeKey.isEmpty()) {
        return;
    }
    if (!p_sqlOperationLastRoll->Updata(table, where, data, &ret)) {
        QMessageBox::warning(nullptr, tr("错误"), 
            tr("更新卷长失败(%1)").arg(ret->lastError().text()), QMessageBox::Ok);
    }
}
//在线检测记录插入数据库
void dataMainFormZJ::slotAddDataSql(const std::map<QString, QString> data)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui.tableView_remark->model());
    QString table = "test_record";
    QSqlQuery * ret;
    std::map<QString, QString> dataAll = data;
    dataAll["record_ID"] = _dateTimeKey;
    dataAll["remarks"] = model->item(0, 1)->text(); //备注
    dataAll["production_time"] = model->item(2, 0)->text(); //生产时间
    dataAll["operator"] = model->item(2, 5)->text(); //操作员
	dataAll["outprint"] = "1";
	dataAll["concession"] = "1";
		
    _testRecordData.push_back(dataAll);
   
}
void dataMainFormZJ::slotDoWork(int type)
{
    QStandardItemModel *model =  qobject_cast<QStandardItemModel*>(ui.tableView_stripQuality->model());
    switch (type) {
    case 0: {
    }
        break;
    case 1: {
    }
        break;
    case 2: {
    }
        break;
    case 3: {
    }
        break;
    default:
        break;
    }
}
//手动添加一行数据
void dataMainFormZJ::slotAddRowData(QMap<QString, QString> data,bool flag)
{
    if (flag) {
        QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui.tableView_remark->model());
        QString table = "test_record";
        QSqlQuery *ret;
        std::map<QString, QString> dataAll;

        //dataAll["dataID"] = QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz"); //ID
        dataAll["meters"] = data.value("meters"); //米数
        dataAll["thickness"] = data.value("thickness"); //厚度
        dataAll["defect"] = data.value("defect"); //缺陷
        dataAll["position"] = data.value("position"); //位置
        dataAll["length"] = data.value("length"); //长度
        dataAll["description"] = data.value("description"); //描述
        dataAll["remarks"] = model->item(0,1)->text(); //备注
        dataAll["production_time"] = model->item(2,0)->text(); //生产时间
        dataAll["operator"] = model->item(2,5)->text(); //操作员
        dataAll["record_ID"] = _mapVolume_recordID.value(_addRecordIndexjuanhao) ;
        dataAll["savename"] = QString::fromStdString(std::to_string(-1)); //图片文件名
        dataAll["add_type"] = "1";
		dataAll["outprint"] = "1";
		dataAll["concession"] = "1";
        if (_mapVolume_recordID.value(_addRecordIndexjuanhao).isEmpty()) {
            ui.tableView_testRecord->startUpdate();
            return;
        }
        if (!p_sqlOperationTestRecord->Add(table, dataAll, &ret)) {
            QMessageBox::warning(nullptr, tr("错误"),
                tr("手动添加数据失败(%1)").arg(ret->lastError().text()), QMessageBox::Ok);
       }
       // ui.tableView_testRecord->addRowItem(data);
    }
    ui.tableView_testRecord->startUpdate();
   
}
void dataMainFormZJ::setModelName()
{
    emit signalGetModelName(true);
}
//方案名称,用于阈值配置文件的生成使用
void dataMainFormZJ::slotModeName(QString &modelName)
{
    _modelName = modelName;
    if (_modelName.isEmpty()) {
        return;
    }
    ui.listWidget_settingFile->clear();
    _settingFilePath = publicClass::instance()->getAppPath() + "/setting/json/";
    //获取json文件列表
    QFileInfoList fileInfoList;
    QSqlQuery * ret;
    QString table = "threshold_table";
    QMap<QString, QString> where;
    where["model_name"] = _modelName;
    QStringList keyList;
    keyList << "thresholdID" << "type" << "updatetime"
        << "gansha_width" << "gansha_height" << "kailie_width"
        << "kailie_height" << "huangban_color" << "xuhua_width"
        << "wuwu_area" << "bujun_width" << "wanzhe_curvature"
        << "model_name" << "json_path";
    QList<QMap<QString, QString>> rowData;
    if (!p_sqlOperationJson->findData(table, where, keyList, rowData, &ret)) {
        QMessageBox::warning(nullptr, tr("错误"),
            tr("查询json数据失败（%1）").arg(ret->lastError().text()), QMessageBox::Ok);
    }
    if (rowData.size() <= 0) {
        initThresholdJsonFile();
        rowData.clear();
        if (!p_sqlOperationJson->findData(table, where, keyList, rowData, &ret)) {
            QMessageBox::warning(nullptr, tr("错误"), 
                tr("查询json数据失败(%1)").arg(ret->lastError().text()), QMessageBox::Ok);
        } 
    }
    for (int num = 0; num < rowData.size(); num++) {
        QFileInfo info(rowData.at(num).value("json_path").simplified());
        fileInfoList.append(info);
    }
    /*if (fileInfoList.size() < 3) {
        initThresholdJsonFile();
    }*/
    //_fileInfoList = publicClass::instance()->GetFileList(_settingFilePath);
    initListWidgetFilelist(fileInfoList);//初始化json文件列表
    emit signalThresholdJsonPath(_modelName);
}
void dataMainFormZJ::showForm(QRect rect)
{
    this->resize(rect.width(),rect.height());
    this->show();
}
//获取检测记录数据
std::vector<std::map<QString, QString>> dataMainFormZJ::getRecordData()
{
    QString table = "test_record";
    QSqlQuery * ret;//卷号
    std::vector<std::map<QString, QString>> rowData;
    QMap<QString, QString> where;//索引值   
    where["record_ID"] = _mapVolume_recordID.value(ui.comboBox_juanhao->currentText().simplified());
    where["add_type"] = "0";

    QStringList ListKey;//查询卷号id映射表
    ListKey.push_back("dataID");  //0
    ListKey.push_back("meters");//米数 1
    ListKey.push_back("thickness");//厚度2  
    ListKey.push_back("position");//位置3
    ListKey.push_back("length");//长度4
    ListKey.push_back("defect");//缺陷5
    ListKey.push_back("description");//描述6

    ListKey.push_back("remarks");
    ListKey.push_back("production_time");
    ListKey.push_back("operator");
    ListKey.push_back("record_ID");
    ListKey.push_back("savename");

    ListKey.push_back("width");
    ListKey.push_back("height");
    ListKey.push_back("color");
    ListKey.push_back("area");
    ListKey.push_back("curvature");
    ListKey.push_back("add_type");
	ListKey.push_back("outprint");//打印
	ListKey.push_back("concession");//让步
    if (!p_sqlOperationReportImg->findDataRecord(table, where, ListKey, rowData, &ret)) {
        QMessageBox::warning(nullptr, tr("错误"), 
            tr("查询检测记录数据失败(%1)").arg(ret->lastError().text()), QMessageBox::Ok);
    }
    return rowData;
}
//获取卷长及其当前卷数据
QMap<QString, QString> dataMainFormZJ::getLastRollLength()
{
    QString table = "quality_record_table";
    QSqlQuery *ret;
    QMap<QString, QString> where;
    QList<QMap<QString, QString>> data;
    QStringList keyList;
    keyList <<"work_order"<<"product_brand"<<"glass_fiber_content_technical"
        <<"areal_density_technical"<<"thickness_technical"<<"wide_format_technical"
        <<"glass_fiber_content_test"<<"areal_density_test"<<"thickness_test"
        <<"wide_format_test"<<"sha_xin_test"<<"number_of_yarns_test"
        <<"spreading_width_test"<<"speed_test"<<"date_time"<<"volume_number"
        <<"last_roll_length"<<"glass_fiber_grade"<<"glass_fiber_batch_number"
        <<"resin_grade"<<"resin_batch_number"<<"compatibilizer_grade"
        <<"compatibilizer_batch_number"<<"finished_product_grade"<<"finished_product_batch_number"
        << "last_check_length"<<"other_lenth"<<"start_time"<<"end_time"<<"remark"<<"operator";

    where["record_ID"] = _mapVolume_recordID.value(ui.comboBox_juanhao->currentText().simplified());    
    if (!p_sqlOperationReportImg->findData(table,where, keyList, data, &ret)) {
        QMessageBox::warning(nullptr, tr("错误"), 
            tr("获取卷长失败(%1)").arg(ret->lastError().text()), QMessageBox::Ok);
    }
    if (data.size() > 0) {
        return data.at(0);
    } 
    return QMap<QString, QString>();
}
void dataMainFormZJ::on_toolButton_export_endReport_clicked()
{
    QStandardItemModel *itemModel = qobject_cast<QStandardItemModel*>(ui.tableView_stripQuality->model());
    if (itemModel->item(1, 8)->text().isEmpty()) {
        return;
    }
   //on_toolButton_editDataSave_clicked();
   QMap<QString,QString> map = getLastRollLength();
   if (map.value("wide_format_technical").isEmpty()) {
       QMessageBox::warning(nullptr, tr("提示"),
           tr("请填写宽幅并点击保存"), QMessageBox::Ok);
       return;
   }
   std::vector<DatabaseMap> data= getRecordData();
   if (map.isEmpty()||data.size()<=0) {
       return;
   }
    QString saveName = QString("%0/report/%1_%2")
        .arg(publicClass::instance()->getAppPath())
        .arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"))
        .arg(ui.comboBox_juanhao->currentText());
    QString png = tr("%1.png").arg(saveName);
    std::vector<DatabaseMap> export_data =  DefectMap(data, map.value("last_roll_length").toInt(),
        map.value("wide_format_technical").toInt(), png.toStdString());


    operationPDF * pdfOper = new operationPDF(0);

    QString pdfname = QString("%1.pdf").arg(saveName);
    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOrientation(QPrinter::Portrait);
    printer.setOutputFileName(pdfname);
    QTextDocument document;
   // QMap<QString, QString> map;
    QPixmap pix(png);
    
    QString html = pdfOper->saveHtmlToPDF(export_data,map, png, pix.width()/2,pix.height()/3);
    document.setHtml(html);
    document.print(&printer);
    document.end();
    if (QMessageBox::question(nullptr, tr("完成"), tr("文件已经导出，是否现在打开？"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
        QDesktopServices::openUrl(QUrl("file:///" + QDir::toNativeSeparators(pdfname)));
    }
    return;
}
void dataMainFormZJ::on_toolButton_inspection_clicked()
{
    _addRecordIndexjuanhao = ui.comboBox_juanhao->currentText().simplified();
    //首检
    ui.tableView_qualityInspectionRecord->
        getInspectionFirst(_mapVolume_recordID.value(
            /*ui.comboBox_juanhao->currentText().simplified()*/_addRecordIndexjuanhao));
    ui.tableView_qualityInspectionRecord->
        getInspectionEnd(_mapVolume_recordID.value(
            /*ui.comboBox_juanhao->currentText().simplified()*/_addRecordIndexjuanhao));

}
void dataMainFormZJ::slotTimerAddRecord()
{
    if (!_isAddFlag) {
        ui.comboBox_juanhao->setCurrentIndex(ui.comboBox_juanhao->count() - 1);
    }
    QString table = "test_record";
    QSqlQuery * ret;
    if (_testRecordData.size() > 0) {
        //获取recordID写入
        if (!p_sqlOperation->Add(table, _testRecordData, &ret)) {
            QMessageBox::warning(nullptr, tr("错误"),
                tr("插入检测记录失败(%1)").arg(ret->lastError().text()), QMessageBox::Ok);
        }
        _testRecordData.clear();
    }
}
void dataMainFormZJ::on_btnPdf_clicked()
{
    
}
void dataMainFormZJ::on_toolButton_previous_clicked()
{

}
void dataMainFormZJ::on_toolButton_next_clicked()
{

}
void dataMainFormZJ::slotComboxPageIndex(const QString &datastr,int recordNum)
{
        _isUpDateSpinBox = true;
        ui.label_recordNum->setText(QString("记录:%1").arg(recordNum));
        _pageSize = datastr.split("/").at(1).toInt();
        ui.spinBox_page->setMaximum(_pageSize);
        ui.spinBox_page->setValue(_pageSize);
        ui.label_page->setText(QString("%1 页").arg(datastr));
        _isUpDateSpinBox = false;
}
void dataMainFormZJ::slotComboxUpDatePage(const QString &currentPage)
{
	if (!_isUpDateCombox) {
		ui.tableView_testRecord->findCurPageRecord(currentPage);
	}
}
void dataMainFormZJ::slotSpinBoxUpDatePage(int page)
{
	if (!_isUpDateSpinBox) {
		ui.label_page->setText(QString("%1/%2 页").arg(page).arg(_pageSize));
		ui.tableView_testRecord->findCurPageRecord(QString("%1/%2").arg(page).arg(_pageSize));
	}
}
//温度读取，加载
void dataMainFormZJ::on_toolButton_load_clicked()
{
	QVariantList list;
	QMap<QString, QVariantList> mapTemp1;
	std::vector<float> vec;
	processinfo_s data = plcmaster::gettemperature(); //get temp
	
	for (int i = 0; i < data.area_temperature.size(); i++) {
		list.push_back(data.area_temperature[i]);
	}
	for (int i = 0; i < data.composite_temperature.size(); i++) {
		list.push_back(data.composite_temperature[i]);
	}
	mapTemp1.insert(QString::number(ui.comboBox_banci->currentIndex()), list);
	setTempData(mapTemp1);
	QStandardItemModel *model = static_cast<QStandardItemModel*>(ui.tableView_record->model());
	model->item(2, 2)->setTextAlignment(Qt::AlignCenter);
	model->item(2, 2)->setText(QString("%1").arg(data.composite_pressure[0]));
	model->item(2, 5)->setTextAlignment(Qt::AlignCenter);
	model->item(2, 5)->setText(QString("%1").arg(data.composite_pressure[1]));

}
void dataMainFormZJ::on_toolButton_savesql_clicked()
{
    QStandardItemModel *model = static_cast<QStandardItemModel*>(ui.tableView_record->model());
    std::vector<std::map<QString, QString>> allData;
    std::map<QString, QString> data;//白班1
    std::map<QString, QString> data1;//白班2
    std::map<QString, QString> data2;//夜班1
    std::map<QString, QString> data3;//夜班2

    QStringList list = P_proCategoryItem->getProType();
    QString proType = "";
    for (int i = 0; i < list.size(); i++)
    {
        proType += list.at(i) + ",";
    }
    proType.chop(2);
    model->item(1, 6)->setText(tr("产品种类: %1").arg(proType));

    data["area_1"] = model->item(_listTempDay1.at(0).x(), _listTempDay1.at(0).y())->text();
    data["area_2"] = model->item(_listTempDay1.at(1).x(), _listTempDay1.at(1).y())->text();
    data["area_3"] = model->item(_listTempDay1.at(2).x(), _listTempDay1.at(2).y())->text();
    data["area_4"] = model->item(_listTempDay1.at(3).x(), _listTempDay1.at(3).y())->text();
    data["area_5"] = model->item(_listTempDay1.at(4).x(), _listTempDay1.at(4).y())->text();
    data["area_6"] = model->item(_listTempDay1.at(5).x(), _listTempDay1.at(5).y())->text();
    data["area_7"] = model->item(_listTempDay1.at(6).x(), _listTempDay1.at(6).y())->text();
    data["area_8"] = model->item(_listTempDay1.at(7).x(), _listTempDay1.at(7).y())->text();
    data["area_9"] = model->item(_listTempDay1.at(8).x(), _listTempDay1.at(8).y())->text();
    data["area_10"] = model->item(_listTempDay1.at(9).x(), _listTempDay1.at(9).y())->text();
    data["area_11"] = model->item(_listTempDay1.at(10).x(), _listTempDay1.at(10).y())->text();
    data["area_12"] = model->item(_listTempDay1.at(11).x(), _listTempDay1.at(11).y())->text();
    data["area_13"] = model->item(_listTempDay1.at(12).x(), _listTempDay1.at(12).y())->text();
    data["area_14"] = model->item(_listTempDay1.at(13).x(), _listTempDay1.at(13).y())->text();
    data["area_15"] = model->item(_listTempDay1.at(14).x(), _listTempDay1.at(14).y())->text();
    data["area_16"] = model->item(_listTempDay1.at(15).x(), _listTempDay1.at(15).y())->text();
    data["area_17"] = model->item(_listTempDay1.at(16).x(), _listTempDay1.at(16).y())->text();
    data["area_18"] = model->item(_listTempDay1.at(17).x(), _listTempDay1.at(17).y())->text();
    data["area_19"] = model->item(_listTempDay1.at(18).x(), _listTempDay1.at(18).y())->text();
    data["area_20"] = model->item(_listTempDay1.at(19).x(), _listTempDay1.at(19).y())->text();
    data["area_complex_1"] = model->item(_listTempDay1.at(20).x(), _listTempDay1.at(20).y())->text();
    data["area_complex_2"] = model->item(_listTempDay1.at(21).x(), _listTempDay1.at(21).y())->text();
    data["product_shebei"] = model->item(0, 2)->text();
    data["product_date"] = model->item(0, 6)->text();
    data["product_vate"] = model->item(1, 3)->text();
    data["product_type"] = proType;
    data["complex_pressuer_1"] = model->item(2, 2)->text();
    data["complex_pressuer_2"] = model->item(2, 5)->text();
    data["product_name_1"] = model->item(2, 7)->text();
    data["product_name_2"] = model->item(2, 9)->text();
    data["remark"] = model->item(21, 1)->text();
    data["shift_type"] = "0";

    data1["area_1"] = model->item(_listTempDay2.at(0).x(), _listTempDay2.at(0).y())->text();
    data1["area_2"] = model->item(_listTempDay2.at(1).x(), _listTempDay2.at(1).y())->text();
    data1["area_3"] = model->item(_listTempDay2.at(2).x(), _listTempDay2.at(2).y())->text();
    data1["area_4"] = model->item(_listTempDay2.at(3).x(), _listTempDay2.at(3).y())->text();
    data1["area_5"] = model->item(_listTempDay2.at(4).x(), _listTempDay2.at(4).y())->text();
    data1["area_6"] = model->item(_listTempDay2.at(5).x(), _listTempDay2.at(5).y())->text();
    data1["area_7"] = model->item(_listTempDay2.at(6).x(), _listTempDay2.at(6).y())->text();
    data1["area_8"] = model->item(_listTempDay2.at(7).x(), _listTempDay2.at(7).y())->text();
    data1["area_9"] = model->item(_listTempDay2.at(8).x(), _listTempDay2.at(8).y())->text();
    data1["area_10"] = model->item(_listTempDay2.at(9).x(), _listTempDay2.at(9).y())->text();
    data1["area_11"] = model->item(_listTempDay2.at(10).x(), _listTempDay2.at(10).y())->text();
    data1["area_12"] = model->item(_listTempDay2.at(11).x(), _listTempDay2.at(11).y())->text();
    data1["area_13"] = model->item(_listTempDay2.at(12).x(), _listTempDay2.at(12).y())->text();
    data1["area_14"] = model->item(_listTempDay2.at(13).x(), _listTempDay2.at(13).y())->text();
    data1["area_15"] = model->item(_listTempDay2.at(14).x(), _listTempDay2.at(14).y())->text();
    data1["area_16"] = model->item(_listTempDay2.at(15).x(), _listTempDay2.at(15).y())->text();
    data1["area_17"] = model->item(_listTempDay2.at(16).x(), _listTempDay2.at(16).y())->text();
    data1["area_18"] = model->item(_listTempDay2.at(17).x(), _listTempDay2.at(17).y())->text();
    data1["area_19"] = model->item(_listTempDay2.at(18).x(), _listTempDay2.at(18).y())->text();
    data1["area_20"] = model->item(_listTempDay2.at(19).x(), _listTempDay2.at(19).y())->text();
    data1["area_complex_1"] = model->item(_listTempDay2.at(20).x(), _listTempDay2.at(20).y())->text();
    data1["area_complex_2"] = model->item(_listTempDay2.at(21).x(), _listTempDay2.at(21).y())->text();
    data1["product_shebei"] = model->item(0, 2)->text();
    data1["product_date"] = model->item(0, 6)->text();
    data1["product_vate"] = model->item(1, 3)->text();
    data1["product_type"] = proType;
    data1["complex_pressuer_1"] = model->item(2, 2)->text();
    data1["complex_pressuer_2"] = model->item(2, 5)->text();
    data1["product_name_1"] = model->item(2, 7)->text();
    data1["product_name_2"] = model->item(2, 9)->text();
    data1["remark"] = model->item(21, 1)->text();
    data1["shift_type"] = "1";

    data2["area_1"] = model->item(_listTempNight1.at(0).x(), _listTempNight1.at(0).y())->text();
    data2["area_2"] = model->item(_listTempNight1.at(1).x(), _listTempNight1.at(1).y())->text();
    data2["area_3"] = model->item(_listTempNight1.at(2).x(), _listTempNight1.at(2).y())->text();
    data2["area_4"] = model->item(_listTempNight1.at(3).x(), _listTempNight1.at(3).y())->text();
    data2["area_5"] = model->item(_listTempNight1.at(4).x(), _listTempNight1.at(4).y())->text();
    data2["area_6"] = model->item(_listTempNight1.at(5).x(), _listTempNight1.at(5).y())->text();
    data2["area_7"] = model->item(_listTempNight1.at(6).x(), _listTempNight1.at(6).y())->text();
    data2["area_8"] = model->item(_listTempNight1.at(7).x(), _listTempNight1.at(7).y())->text();
    data2["area_9"] = model->item(_listTempNight1.at(8).x(), _listTempNight1.at(8).y())->text();
    data2["area_10"] = model->item(_listTempNight1.at(9).x(), _listTempNight1.at(9).y())->text();
    data2["area_11"] = model->item(_listTempNight1.at(10).x(), _listTempNight1.at(10).y())->text();
    data2["area_12"] = model->item(_listTempNight1.at(11).x(), _listTempNight1.at(11).y())->text();
    data2["area_13"] = model->item(_listTempNight1.at(12).x(), _listTempNight1.at(12).y())->text();
    data2["area_14"] = model->item(_listTempNight1.at(13).x(), _listTempNight1.at(13).y())->text();
    data2["area_15"] = model->item(_listTempNight1.at(14).x(), _listTempNight1.at(14).y())->text();
    data2["area_16"] = model->item(_listTempNight1.at(15).x(), _listTempNight1.at(15).y())->text();
    data2["area_17"] = model->item(_listTempNight1.at(16).x(), _listTempNight1.at(16).y())->text();
    data2["area_18"] = model->item(_listTempNight1.at(17).x(), _listTempNight1.at(17).y())->text();
    data2["area_19"] = model->item(_listTempNight1.at(18).x(), _listTempNight1.at(18).y())->text();
    data2["area_20"] = model->item(_listTempNight1.at(19).x(), _listTempNight1.at(19).y())->text();
    data2["area_complex_1"] = model->item(_listTempNight1.at(20).x(), _listTempNight1.at(20).y())->text();
    data2["area_complex_2"] = model->item(_listTempNight1.at(21).x(), _listTempNight1.at(21).y())->text();
    data2["product_shebei"] = model->item(0, 2)->text();
    data2["product_date"] = model->item(0, 6)->text();
    data2["product_vate"] = model->item(1, 3)->text();
    data2["product_type"] = proType;
    data2["complex_pressuer_1"] = model->item(2, 2)->text();
    data2["complex_pressuer_2"] = model->item(2, 5)->text();
    data2["product_name_1"] = model->item(2, 7)->text();
    data2["product_name_2"] = model->item(2, 9)->text();
    data2["remark"] = model->item(21, 1)->text();
    data2["shift_type"] = "2";


    data3["area_1"] = model->item(_listTempNight1.at(0).x(), _listTempNight1.at(0).y())->text();
    data3["area_2"] = model->item(_listTempNight1.at(1).x(), _listTempNight1.at(1).y())->text();
    data3["area_3"] = model->item(_listTempNight1.at(2).x(), _listTempNight1.at(2).y())->text();
    data3["area_4"] = model->item(_listTempNight1.at(3).x(), _listTempNight1.at(3).y())->text();
    data3["area_5"] = model->item(_listTempNight1.at(4).x(), _listTempNight1.at(4).y())->text();
    data3["area_6"] = model->item(_listTempNight1.at(5).x(), _listTempNight1.at(5).y())->text();
    data3["area_7"] = model->item(_listTempNight1.at(6).x(), _listTempNight1.at(6).y())->text();
    data3["area_8"] = model->item(_listTempNight1.at(7).x(), _listTempNight1.at(7).y())->text();
    data3["area_9"] = model->item(_listTempNight1.at(8).x(), _listTempNight1.at(8).y())->text();
    data3["area_10"] = model->item(_listTempNight1.at(9).x(), _listTempNight1.at(9).y())->text();
    data3["area_11"] = model->item(_listTempNight1.at(10).x(), _listTempNight1.at(10).y())->text();
    data3["area_12"] = model->item(_listTempNight1.at(11).x(), _listTempNight1.at(11).y())->text();
    data3["area_13"] = model->item(_listTempNight1.at(12).x(), _listTempNight1.at(12).y())->text();
    data3["area_14"] = model->item(_listTempNight1.at(13).x(), _listTempNight1.at(13).y())->text();
    data3["area_15"] = model->item(_listTempNight1.at(14).x(), _listTempNight1.at(14).y())->text();
    data3["area_16"] = model->item(_listTempNight1.at(15).x(), _listTempNight1.at(15).y())->text();
    data3["area_17"] = model->item(_listTempNight1.at(16).x(), _listTempNight1.at(16).y())->text();
    data3["area_18"] = model->item(_listTempNight1.at(17).x(), _listTempNight1.at(17).y())->text();
    data3["area_19"] = model->item(_listTempNight1.at(18).x(), _listTempNight1.at(18).y())->text();
    data3["area_20"] = model->item(_listTempNight1.at(19).x(), _listTempNight1.at(19).y())->text();
    data3["area_complex_1"] = model->item(_listTempNight1.at(20).x(), _listTempNight1.at(20).y())->text();
    data3["area_complex_2"] = model->item(_listTempNight1.at(21).x(), _listTempNight1.at(21).y())->text();
    data3["product_shebei"] = model->item(0, 2)->text();
    data3["product_date"] = model->item(0, 6)->text();
    data3["product_vate"] = model->item(1, 3)->text();
    data3["product_type"] = proType;
    data3["complex_pressuer_1"] = model->item(2, 2)->text();
    data3["complex_pressuer_2"] = model->item(2, 5)->text();
    data3["product_name_1"] = model->item(2, 7)->text();
    data3["product_name_2"] = model->item(2, 9)->text();
    data3["remark"] = model->item(21, 1)->text();
    data3["shift_type"] = "3";
    QString table = "heating_zone_table";
    allData.push_back(data);
    allData.push_back(data1);
    allData.push_back(data2);
    allData.push_back(data3);
    QSqlQuery * ret;
    if (!p_sqlOperationTem->Add(table, allData, &ret)) {
        QMessageBox::question(nullptr, ("提示"),
            tr("添加配置文件失败（%1）").arg(ret->lastError().text()), QMessageBox::Ok);
    }
}
void dataMainFormZJ::slotOldDataClear(bool checked)
{
	ui.label_recordNum->setText(QString("记录:0"));
	ui.spinBox_page->setValue(1);
	ui.label_page->clear();
}
//获取当前页
void dataMainFormZJ::slotGetSpinBoxValue()
{
	ui.tableView_testRecord->setCurPage(ui.spinBox_page->value());
}