#include "stripQualityView.h"

stripQualityView::stripQualityView(QWidget *parent)
    : QTableView(parent)
{
    ui.setupUi(this);
    createRightMenu();  //创建一个右键菜单
    initTableView();

}

stripQualityView::~stripQualityView()
{
}
void stripQualityView::initTableView()
 {
    this->setMouseTracking(true);
    connect(this, SIGNAL(entered(QModelIndex)),this, SLOT(showToolTip(QModelIndex)));
    this->setMinimumWidth(1000);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    QStandardItemModel* item_model = new QStandardItemModel(13, 10);
    this->setModel(item_model);
    //设置表格的单元为只读属性，即不能编辑        
    this->setEditTriggers(QAbstractItemView::AllEditTriggers);
   // this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //如果你用在QTableView中使用右键菜单，需启用该属性        
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotRightMenu(QPoint)));
    //connect(this, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(slotDoubleClicked(const QModelIndex &)));
    this->horizontalHeader()->hide();                       // 隐藏水平表头
    this->verticalHeader()->hide();                         // 隐藏垂直表头
    //this->horizontalHeader()->setStretchLastSection(true);  // 伸缩最后一列
    this->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    /* QStringList column, row;
     column << "Column 1" << "Column 2" << "Column 3" << "Column 4";
     row << "row 1" << "row 2" << "row 3" << "row 4";
     item_model->setHorizontalHeaderLabels(column);                // 设置水平表头标签
     item_model->setVerticalHeaderLabels(row);                     // 设置垂直表头标签
     */

     // 添加item到model
    for (int i = 0; i < this->model()->rowCount(); ++i) {
        for (int j = 0; j < this->model()->columnCount(); ++j)
            item_model->setItem(i, j, new QStandardItem());
    }
    _listEdit << tr("1:8");
    QList<QVariantList> listAllItem;
    QVariantList list1;
    QVariantList list2;
    QVariantList list3;
    QVariantList list4;
    QVariantList list5;
    QVariantList list6;
    QVariantList list7;
    QVariantList list8;
    QVariantList list9;
    QVariantList list10;
    QVariantList list11;
    QVariantList list12;
    QVariantList list13;
    QVariantList list14;
    QVariantList list15;
    QVariantList list16;
    QVariantList list17;
    QVariantList list18;
    QVariantList list19;
    QVariantList list20;
    QVariantList list21;
    QVariantList list22;
    QVariantList list23;
    QVariantList list24;
    QVariantList list25;
    QVariantList list26;
    QVariantList list27;
    QVariantList list28;
    QVariantList list29;
    item_model->item(1, 0)->setText("工作令");
    item_model->item(1, 3)->setText("产品牌号");
    item_model->item(1, 7)->setText("卷号");
    item_model->item(3, 6)->setText("厚度/mm");
    item_model->item(3, 8)->setText("宽幅/mm");
    item_model->item(5, 6)->setText("厚度/mm");
    item_model->item(5, 8)->setText("宽幅/mm");
    item_model->item(6, 6)->setText("铺纱宽度");
    item_model->item(6, 8)->setText("速度/m/min");
    item_model->item(1, 0)->setFlags(Qt::ItemIsEnabled);
    item_model->item(1, 3)->setFlags(Qt::ItemIsEnabled);
    item_model->item(1, 7)->setFlags(Qt::ItemIsEnabled);
    item_model->item(1, 8)->setFlags(Qt::ItemIsEnabled);
    item_model->item(3, 6)->setFlags(Qt::ItemIsEnabled);
    item_model->item(3, 8)->setFlags(Qt::ItemIsEnabled);
    item_model->item(5, 6)->setFlags(Qt::ItemIsEnabled);
    item_model->item(5, 8)->setFlags(Qt::ItemIsEnabled);
    item_model->item(6, 6)->setFlags(Qt::ItemIsEnabled);
    item_model->item(6, 8)->setFlags(Qt::ItemIsEnabled);
    //item_model->item(1, 8)->setText(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmsszzz"));
    list1 << 0 << 0 << 1 << 10<<"订单信息";
    list2 << 1 << 1 << 1 << 2;
    list3 << 1 << 3 << 1 << 2<<"产品牌号";
    list4 << 1 << 8 << 1 << 2<< QDateTime::currentDateTime().toString("V_hhmmsszzz");
    list5 << 2 << 0 << 1 << 10 << "生产技术要求";
    list6 << 3 << 0 << 1 << 2 << "玻纤含量/%";
    list7 << 3 << 3 << 1 << 2 << "面密度/gsm";
    list8 << 4 << 0 << 1 << 10 << "首检记录";
    list9 << 1 << 5 << 1 << 2;
    list10 << 5 << 0 << 1 << 2 << "玻纤含量/%";
    list11 << 5 << 3 << 1 << 2 << "面密度/gsm";
    list12 << 6 << 0 << 1 << 2 << "沙筘";
    list13 << 6 << 3 << 1 << 2 << "纱线根数";
    list14 << 7 << 0 << 1 << 10 << "原材料信息";
    list15 << 8 << 0 << 1 << 4 << "类别";
    list16 << 8 << 4 << 1 << 3 << "牌号";
    list17 << 8 << 7 << 1 << 3 << "批次号*";
    list18 << 9 << 0 << 1 << 4 <<"玻璃纤维" ;
    list19 << 9 << 4 << 1 << 3 ;
    list20 << 9 << 7 << 1 << 3 ;
    list21 << 10 << 0 << 1 << 4 << "树脂";
    list22 << 10 << 4 << 1 << 3;
    list23 << 10 << 7 << 1 << 3;
    list24 << 11 << 0 << 1 << 4 << "相容剂";
    list25 << 11 << 4 << 1 << 3;
    list26 << 11 << 7 << 1 << 3;
    list27 << 12 << 0 << 1 << 4 << "产成品";
    list28 << 12 << 4 << 1 << 3;
    list29 << 12 << 7 << 1 << 3;
    listAllItem << list1 << list2<<list3<<list4<<list5<<list6
        <<list7<<list8<<list9<<list10<<list11<<list12<<list13
        <<list14<<list15<<list16<<list17<<list18<<list19<<list20
        <<list21<<list22<< list23 << list24 << list25 << list26 
        << list27 << list28 <<list29;
    editTable(listAllItem,qobject_cast<QStandardItemModel*>(this->model()));
    QStandardItemModel *model = static_cast<QStandardItemModel*>(this->model());
    for (int i = 0; i < model->rowCount(); i++) {
        for(int j = 0; j < model->columnCount(); j++) {
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
            model->item(i, j)->setTextAlignment(Qt::AlignCenter);
            model->item(i, j)->setFont(font);
            /*
            QPalette palette = QToolTip::palette();
            palette.setColor(QPalette::Inactive, QPalette::ToolTipBase, Qt::white);   //设置ToolTip背景色
            palette.setColor(QPalette::Inactive, QPalette::ToolTipText, QColor(102, 102, 102, 255)); 	//设置ToolTip字体色
            QToolTip::setPalette(palette);
            QFont font("Segoe UI", -1, 50);
            font.setPixelSize(12);
            QToolTip::setFont(font);  //设置ToolTip字体
            */
            
            //model->item(i, j)->setToolTip(model->item(i, j)->text());
        }
    }
    this->setShowGrid(true);                               // 隐藏网格线
    this->setFocusPolicy(Qt::WheelFocus);                      // 去除当前Cell周边虚线框
    this->setAlternatingRowColors(true);                    // 开启隔行异色
    _volumeNumStr = item_model->item(1, 8)->text().simplified();
 }
 //初始化数据模型
 QStandardItemModel *stripQualityView::initModel()
 {
    
     QStandardItemModel* item_model = new QStandardItemModel(13, 10);

      // 添加item到model
     for (int i = 0; i < item_model->rowCount(); ++i) {
         for (int j = 0; j < item_model->columnCount(); ++j)
             item_model->setItem(i, j, new QStandardItem());
     }
     QList<QVariantList> listAllItem;
     QVariantList list1;
     QVariantList list2;
     QVariantList list3;
     QVariantList list4;
     QVariantList list5;
     QVariantList list6;
     QVariantList list7;
     QVariantList list8;
     QVariantList list9;
     QVariantList list10;
     QVariantList list11;
     QVariantList list12;
     QVariantList list13;
     QVariantList list14;
     QVariantList list15;
     QVariantList list16;
     QVariantList list17;
     QVariantList list18;
     QVariantList list19;
     QVariantList list20;
     QVariantList list21;
     QVariantList list22;
     QVariantList list23;
     QVariantList list24;
     QVariantList list25;
     QVariantList list26;
     QVariantList list27;
     QVariantList list28;
     QVariantList list29;
     item_model->item(1, 0)->setText("工作令");
     item_model->item(1, 3)->setText("产品牌号");
     item_model->item(1, 7)->setText("卷号");
     item_model->item(3, 6)->setText("厚度/mm");
     item_model->item(3, 8)->setText("宽幅/mm");
     item_model->item(5, 6)->setText("厚度/mm");
     item_model->item(5, 8)->setText("宽幅/mm");
     item_model->item(6, 6)->setText("铺纱宽度");
     item_model->item(6, 8)->setText("速度/m/min");
     item_model->item(1, 0)->setFlags(Qt::ItemIsEnabled);
     item_model->item(1, 3)->setFlags(Qt::ItemIsEnabled);
     item_model->item(1, 7)->setFlags(Qt::ItemIsEnabled);
     item_model->item(1, 8)->setFlags(Qt::ItemIsEnabled);
     item_model->item(3, 6)->setFlags(Qt::ItemIsEnabled);
     item_model->item(3, 8)->setFlags(Qt::ItemIsEnabled);
     item_model->item(5, 6)->setFlags(Qt::ItemIsEnabled);
     item_model->item(5, 8)->setFlags(Qt::ItemIsEnabled);
     item_model->item(6, 6)->setFlags(Qt::ItemIsEnabled);
     item_model->item(6, 8)->setFlags(Qt::ItemIsEnabled);
     //item_model->item(1, 8)->setText(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmsszzz"));
     list1 << 0 << 0 << 1 << 10 << "订单信息";
     list2 << 1 << 1 << 1 << 2;
     list3 << 1 << 3 << 1 << 2 << "产品牌号";
     list4 << 1 << 8 << 1 << 2 << QDateTime::currentDateTime().toString("V_hhmmsszzz");
     list5 << 2 << 0 << 1 << 10 << "生产技术要求";
     list6 << 3 << 0 << 1 << 2 << "玻纤含量/%";
     list7 << 3 << 3 << 1 << 2 << "面密度/gsm";
     list8 << 4 << 0 << 1 << 10 << "首检记录";
     list9 << 1 << 5 << 1 << 2;
     list10 << 5 << 0 << 1 << 2 << "玻纤含量/%";
     list11 << 5 << 3 << 1 << 2 << "面密度/gsm";
     list12 << 6 << 0 << 1 << 2 << "沙筘";
     list13 << 6 << 3 << 1 << 2 << "纱线根数";
     list14 << 7 << 0 << 1 << 10 << "原材料信息";
     list15 << 8 << 0 << 1 << 4 << "类别";
     list16 << 8 << 4 << 1 << 3 << "牌号";
     list17 << 8 << 7 << 1 << 3 << "批次号*";
     list18 << 9 << 0 << 1 << 4 << "玻璃纤维";
     list19 << 9 << 4 << 1 << 3;
     list20 << 9 << 7 << 1 << 3;
     list21 << 10 << 0 << 1 << 4 << "树脂";
     list22 << 10 << 4 << 1 << 3;
     list23 << 10 << 7 << 1 << 3;
     list24 << 11 << 0 << 1 << 4 << "相容剂";
     list25 << 11 << 4 << 1 << 3;
     list26 << 11 << 7 << 1 << 3;
     list27 << 12 << 0 << 1 << 4 << "产成品";
     list28 << 12 << 4 << 1 << 3;
     list29 << 12 << 7 << 1 << 3;
     listAllItem << list1 << list2 << list3 << list4 << list5 << list6
         << list7 << list8 << list9 << list10 << list11 << list12 << list13
         << list14 << list15 << list16 << list17 << list18 << list19 << list20
         << list21 << list22 << list23 << list24 << list25 << list26
         << list27 << list28 << list29;

     editTable(listAllItem,item_model);
     for (int i = 0; i < item_model->rowCount(); i++) {
         for (int j = 0; j < item_model->columnCount(); j++) {
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
             item_model->item(i, j)->setTextAlignment(Qt::AlignCenter);
             item_model->item(i, j)->setFont(font);
         }
     }
     return item_model;
 }
 void stripQualityView::editTable(const QList<QVariantList> &list,QStandardItemModel* model)
 {
    // QStandardItemModel *model = static_cast<QStandardItemModel*>(this->model());
     for (int i = 0; i < list.size(); i++) {
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
         this->setSpan(list.at(i).at(0).toInt(), list.at(i).at(1).toInt(), list.at(i).at(2).toInt(), list.at(i).at(3).toInt());
        // model->item(list.at(i).at(0).toInt(), list.at(i).at(1).toInt())->setForeground(QBrush(Qt::green));
         model->item(list.at(i).at(0).toInt(), list.at(i).at(1).toInt())->setTextAlignment(Qt::AlignCenter);
         model->item(list.at(i).at(0).toInt(), list.at(i).at(1).toInt())->setFont(/*QFont("Times", 10, QFont::Black)*/font);
         if (list.at(i).size() > 4) {
             model->item(list.at(i).at(0).toInt(), list.at(i).at(1).toInt())->setText(list.at(i).at(4).toString());
             model->item(list.at(i).at(0).toInt(), list.at(i).at(1).toInt())->setFlags(Qt::ItemIsEnabled);
         }
         else
         {
            // model->item(list.at(i).at(0).toInt(), list.at(i).at(1).toInt())->setForeground(QBrush(Qt::green));
             //model->item(list.at(i).at(0).toInt(), list.at(i).at(1).toInt())->setText(tr("xx"));
         }
         
         
     }
     

 }
 void stripQualityView::slotDoubleClicked(const QModelIndex &index)
 {
     QStandardItemModel *model = static_cast<QStandardItemModel*>(this->model());
     if (model->item(index.row(), index.column())->text().isEmpty()||
         _listEdit.contains(tr("%1:%2").arg(index.row()).arg(index.column()))) {
         tableViewEditItemForm  * form = new tableViewEditItemForm;
         _listEdit.append(tr("%1:%2").arg(index.row()).arg(index.column()));
         form->setTitle(model->item(index.row(), index.column()-1)->text().isEmpty()?
             model->item(index.row(), index.column() - 2)->text():
             model->item(index.row(), index.column() - 1)->text(), index.row(), index.column());
         form->show();
         QObject::connect(form, SIGNAL(signalItemData(int, int, QString)), this, SLOT(slotEditItemData(int, int, QString)));
     }
 }
 //修改卷号
 void stripQualityView::slotEditItemData(int row, int column, QString text)
 {

     if (text.simplified().isEmpty()) {
         return;
     }

     QStandardItemModel *model = qobject_cast<QStandardItemModel*>(this->model());
     QString table = "quality_record_table";
     //QString tableVolume = "volume_num_table";
     QSqlQuery * ret;

     //////////////////////////////////////////////////////
     QSqlQuery * ret_volume;//卷号
     QList<QVariantList> rowData;//查询卷号id映射表
     QMap<QString, QString> where_volume_num;//索引值   
     where_volume_num["volume_number"] = /*model->item(1, 8)->text().simplified()*/text.simplified();
     QStringList ListKey;//查询卷号id映射表
     ListKey.append("volume_number");
     ListKey.append("record_ID");
     databaseSqlInset::get_db()->findData(table, where_volume_num, ListKey, rowData, &ret_volume);
     qDebug() << rowData;
     /////////////////////////////////////////////////////
     //查询当前命名是否已存在数据库中且不是当前id的数据
     if (rowData.size() == 1&&rowData.at(0).at(1).toString()==_currentKey) {
        // qDebug() << tr("卷号已存在，请重新命名");
         QMessageBox::information(NULL, tr("提示"), tr("卷号已存在,请重新命名"),
             QMessageBox::Yes);
         return;
     }
     else if (rowData.size() >= 1 && rowData.at(0).at(1).toString() != _currentKey) {
         //qDebug() << tr("卷号已存在，请重新命名");
         QMessageBox::information(NULL, tr("提示"), tr("卷号已存在,请重新命名"),
             QMessageBox::Yes);
         return;
     }
     else if(rowData.isEmpty())
     {
         std::map<QString, QString> map_data; 
         std::map<QString, QString> map_where;
         map_data["volume_number"] = text.simplified();
         map_where["volume_number"] = _volumeNumStr;
         databaseSqlInset::get_db()->Updata(table, map_where, map_data,  &ret);
         emit signalRefreshVolumeNum(true,_currentIndex);
     }
     //std::list<std::list<QString>>* rowList;
    /* if (row == 1 && column == 8) {
         if (databaseSqlInset::get_db()->isExists(table, "volume_number", text.simplified(), &ret) > 0) {
             QMessageBox::information(NULL, tr("提示"), tr("卷号已存在,请重新命名"),
                 QMessageBox::Yes );
             return;
         }
         
     }*/
     

     //QStandardItemModel *model = qobject_cast<QStandardItemModel*>(this->model());

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
 void stripQualityView::showToolTip(const QModelIndex &index)
 {
     if (!index.isValid()) {
         qDebug() << "Invalid index";
         return;
     }
     QToolTip::showText(QCursor::pos(), index.data().toString());
 }
 void stripQualityView::setItemData(int row, int column, const QString &text, QStandardItemModel* model)
 {
     //QStandardItemModel *model = qobject_cast<QStandardItemModel*>(modelPrent);
     if (model->item(row, column) == NULL) {
         return;
     }
     model->item(row, column)->setText(text);
 }
 //设置当前record_ID
 void stripQualityView::setCurrentRecord_ID(const QString &currentKey,const QString &volumeNumStr,const int &currentIndex)
 {
     _currentKey = currentKey;//recordID
     _volumeNumStr = volumeNumStr;//卷号
     _currentIndex = currentIndex;//当前索引
     //update quality_record_table set volume_number = 'v_333' where volume_number = 'V_120317927'
 }
 //更新清空table
 void stripQualityView::tableViewClear()
 {
  
     QStandardItemModel *model = static_cast<QStandardItemModel*>(this->model());
     model->item(1, 1)->clearData();
     model->item(1, 5)->clearData();
     model->item(1, 8)->clearData();
     model->item(3, 2)->clearData();
     model->item(3, 5)->clearData();
     model->item(3, 7)->clearData();
     model->item(3, 9)->clearData();
     model->item(5, 2)->clearData();
     model->item(5, 5)->clearData();
     model->item(5, 7)->clearData();
     model->item(5, 9)->clearData();
     model->item(6, 2)->clearData();
     model->item(6, 5)->clearData();
     model->item(6, 7)->clearData();
     model->item(6, 9)->clearData();
     model->item(9, 4)->clearData(); 
     model->item(9, 7)->clearData();
     model->item(10, 4)->clearData();
     model->item(10, 7)->clearData();
     model->item(11, 4)->clearData();
     model->item(11, 7)->clearData();
     model->item(12, 4)->clearData();
     model->item(12, 7)->clearData();

     model->item(1, 8)->setFlags(Qt::ItemIsEnabled);
     model->item(1, 8)->setTextAlignment(Qt::AlignCenter);
     model->item(1, 1)->setTextAlignment(Qt::AlignCenter);
     model->item(1, 5)->setTextAlignment(Qt::AlignCenter);
     model->item(1, 8)->setTextAlignment(Qt::AlignCenter);
     model->item(3, 2)->setTextAlignment(Qt::AlignCenter); 
     model->item(3, 5)->setTextAlignment(Qt::AlignCenter);
     model->item(3, 7)->setTextAlignment(Qt::AlignCenter);
     model->item(3, 9)->setTextAlignment(Qt::AlignCenter);
     model->item(5, 2)->setTextAlignment(Qt::AlignCenter);
     model->item(5, 5)->setTextAlignment(Qt::AlignCenter);
     model->item(5, 7)->setTextAlignment(Qt::AlignCenter);
     model->item(5, 9)->setTextAlignment(Qt::AlignCenter);
     model->item(6, 2)->setTextAlignment(Qt::AlignCenter);
     model->item(6, 5)->setTextAlignment(Qt::AlignCenter);
     model->item(6, 7)->setTextAlignment(Qt::AlignCenter);
     model->item(6, 9)->setTextAlignment(Qt::AlignCenter);
     model->item(9, 4)->setTextAlignment(Qt::AlignCenter);
     model->item(9, 7)->setTextAlignment(Qt::AlignCenter);
     model->item(10, 4)->setTextAlignment(Qt::AlignCenter);
     model->item(10, 7)->setTextAlignment(Qt::AlignCenter);
     model->item(11, 4)->setTextAlignment(Qt::AlignCenter);
     model->item(11, 7)->setTextAlignment(Qt::AlignCenter);
     model->item(12, 4)->setTextAlignment(Qt::AlignCenter);
     model->item(12, 7)->setTextAlignment(Qt::AlignCenter);
     
 }
 //创建右键菜单
 void stripQualityView::createRightMenu()
 {
     _rightMenu = new QMenu;
     _cutAction = new QAction("剪切", this);
     _copyAction = new QAction("复制", this);
     _pasteAction = new QAction("粘贴", this);
     _changeAction = new QAction("修改卷号", this);  //修改卷号
     _deleteAction = new QAction("删除卷号", this);

     //_rightMenu->addAction(_cutAction);
     //_rightMenu->addAction(_copyAction);
     //_rightMenu->addAction(_pasteAction);
     _rightMenu->addAction(_changeAction);
     _rightMenu->addAction(_deleteAction);

     QObject::connect(_changeAction, SIGNAL(triggered(bool)), this, SLOT(slotChangeVolumeNum(bool)));
     QObject::connect(_deleteAction, SIGNAL(triggered(bool)), this, SLOT(slotdeleteVolumeNum(bool)));
    
 }
 //右键菜单函数
 void stripQualityView::slotRightMenu(QPoint pos)
 {
   
     //获得鼠标点击的x，y坐标点
     int x = pos.x();
     int y = pos.y();
     QModelIndex index =this->indexAt(QPoint(x, y));
     if (index.row() == 1 && index.column() == 8) {
        
         _modelIndex = index;
         _rightMenu->exec(QCursor::pos());
     }
 }
 //修改卷号
 void stripQualityView::slotChangeVolumeNum(bool checked)
 {
     QStandardItemModel *model = qobject_cast<QStandardItemModel*>(this->model());
     if (model->item(_modelIndex.row(), _modelIndex.column())->text().isEmpty() ||
         _listEdit.contains(tr("%1:%2").arg(_modelIndex.row()).arg(_modelIndex.column()))) {
         tableViewEditItemForm  * form = new tableViewEditItemForm;
         _listEdit.append(tr("%1:%2").arg(_modelIndex.row()).arg(_modelIndex.column()));
         form->setTitle(model->item(_modelIndex.row(), _modelIndex.column() - 1)->text().isEmpty() ?
             model->item(_modelIndex.row(), _modelIndex.column() - 2)->text() :
             model->item(_modelIndex.row(), _modelIndex.column() - 1)->text(), _modelIndex.row(), _modelIndex.column());
         form->show();
         QObject::connect(form, SIGNAL(signalItemData(int, int, QString)), this, SLOT(slotEditItemData(int, int, QString)));
     }
 }
 //删除卷号
 void stripQualityView::slotdeleteVolumeNum(bool checked)
 {
     QStandardItemModel *model = qobject_cast<QStandardItemModel*>(this->model());
     QString table = "quality_record_table";
     QString recordID;
     //////////////////////////////////////////////////////
     //根据卷号先查询id
     QSqlQuery * ret_volume;//卷号
     QList<QVariantList> rowData;//查询卷号id映射表
     QMap<QString, QString> where_volume_num;//索引值   
     where_volume_num["volume_number"] = model->item(_modelIndex.row(), _modelIndex.column())->text().simplified();
     QStringList ListKey;//查询卷号id映射表

     ListKey.append("volume_number");
     ListKey.append("record_ID");
     databaseSqlInset::get_db()->findData(table, where_volume_num, ListKey, rowData, &ret_volume);
     if (rowData.size() > 0) {
         qDebug() << rowData << rowData.at(0).at(1);//查询id;
         recordID = rowData.at(0).at(1).toString().simplified();
     }

     QSqlQuery *ret;
     QString key = "volume_number";
     QString value = model->item(_modelIndex.row(), _modelIndex.column())->text().simplified();
     databaseSqlInset::get_db()->delRecord(table, key, value, &ret);
     ret = Q_NULLPTR;
     databaseSqlInset::get_db()->delRecord("raw_material_table", "record_ID", recordID, &ret);
     ret = Q_NULLPTR;
     databaseSqlInset::get_db()->delRecord("test_record", "record_ID", recordID, &ret);

     emit signalRefreshVolumeNum(false, _currentIndex);


     
     
 }