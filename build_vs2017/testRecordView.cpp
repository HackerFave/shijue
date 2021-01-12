#include "testRecordView.h"
#include <QMessageBox>
#include "opercheckboxitem.h"
testRecordView::testRecordView(QWidget *parent)
    : QTableView(parent)
{
    ui.setupUi(this);
    p_sqlOperation = new sqlOperation(0);
    if (!p_sqlOperation->Init("127.0.0.1", "root", "123456", "zhongji", "testRecord"))
    {
        QMessageBox::warning(nullptr, tr("错误"), tr("初始化数据库失败"), QMessageBox::Apply);
    }
    initTableView();
    _timer = new QTimer;
    connect(_timer, SIGNAL(timeout()),this, SLOT(slotUpdataItem()));
    _timer->start(3000);
}

testRecordView::~testRecordView()
{
    if (p_sqlOperation)
    {
        p_sqlOperation->CloseDB();
        delete p_sqlOperation;
    }
}
void testRecordView::initTableView()
{
    this->scrollToBottom();
    this->setMouseTracking(true);
    this->setMinimumWidth(750);//表格宽度
    this->setSelectionMode(QAbstractItemView::SingleSelection);
   // this->setSelectionBehavior(QAbstractItemView::SelectRows);
    QStandardItemModel* item_model = new QStandardItemModel(2, 8);//行 列
    this->setModel(item_model);
    //设置表格的单元为只读属性，即不能编辑        
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
   // this->setEditTriggers(QAbstractItemView::AllEditTriggers);//可编辑
    //如果你用在QTableView中使用右键菜单，需启用该属性        
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->verticalHeader()->setDefaultSectionSize(15);
    this->setWordWrap(true);//文字换行
    connect(this, SIGNAL(entered(QModelIndex)), this, SLOT(showToolTip(QModelIndex)));
    connect(this, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(slotDoubleClicked(const QModelIndex &)));
    this->horizontalHeader()->hide();                       // 隐藏水平表头
    this->verticalHeader()->hide();                         // 隐藏垂直表头
    //ui.tableView->horizontalHeader()->setStretchLastSection(true);  // 伸缩最后一列
   // this->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

     // 添加item到model
    for (int i = 0; i < this->model()->rowCount(); ++i) {
        for (int j = 0; j < this->model()->columnCount(); ++j)
            item_model->setItem(i, j, new QStandardItem());
    }
    item_model->item(1, 0)->setText("米数");
    item_model->item(1, 1)->setText("厚度*");
    item_model->item(1, 2)->setText("位置/mm");
    item_model->item(1, 3)->setText("长度/mm");
    item_model->item(1, 4)->setText("缺陷");
	item_model->item(1, 5)->setText("描述*");
    QList<QVariantList> listAllItem;
    QVariantList list1;
    list1 << 0 << 0 << 1 << 8<< "在线检测记录";//合并第一行单元格，加入表头（第0行，第0列，合并1行，8列）
    listAllItem << list1;
    for (int i = 1; i < item_model->rowCount(); i++) {
        QVariantList list;
        if (i == 1) {
            list << i << 6 << 1 << 2 << "操作";
			
        }
        list << i << 6 << 1 << 2;
        listAllItem.append(list);
		
    }
    editTable(listAllItem);

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
           // font.setBold(true);//粗体
            // font.setStyle(QFont::StyleOblique);
            // font.setCapitalization(QFont::Capitalize);//首字母大写
            model->item(i, j)->setTextAlignment(Qt::AlignCenter);
            model->item(i, j)->setFont(font);
           
            if (i >= 0&&i<=1) {
                QFont font;
                font.setFamily("黑体");//字体
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
                model->item(i, j)->setFlags(Qt::ItemIsEnabled);
                model->item(1, j)->setBackground(QColor(0, 85, 127));
            }
            if (i >1) {
                model->item(i, 0)->setFlags(Qt::ItemIsEnabled);
                //model->item(i, 1)->setFlags(Qt::ItemIsEnabled);
                model->item(i, 2)->setFlags(Qt::ItemIsEnabled);
                model->item(i, 3)->setFlags(Qt::ItemIsEnabled);
                model->item(i, 4)->setFlags(Qt::ItemIsEnabled);
                //model->item(i, 5)->setFlags(Qt::ItemIsEnabled);

                
           }
        }
        this->setRowHeight(i, 42);
    }
    this->setShowGrid(true);                               // 隐藏网格线
    this->setFocusPolicy(Qt::StrongFocus);                      // 去除当前Cell周边虚线框
    this->setAlternatingRowColors(true);                    // 开启隔行异色
    
}
//合并单元格
void testRecordView::editTable(const QList<QVariantList> &list)
{
    QStandardItemModel *model = static_cast<QStandardItemModel*>(this->model());
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
        model->item(list.at(i).at(0).toInt(), 1)->setTextAlignment(Qt::AlignCenter);
        model->item(list.at(i).at(0).toInt(), list.at(i).at(1).toInt())->setFont(/*QFont("Times", 10, QFont::Black)*/font);
        if (list.at(i).size() > 4) {
            model->item(list.at(i).at(0).toInt(), list.at(i).at(1).toInt())->setText(list.at(i).at(4).toString());
        }
        else
        {
           // model->item(list.at(i).at(0).toInt(), list.at(i).at(1).toInt())->setForeground(QBrush(Qt::green));
            model->item(list.at(i).at(0).toInt(), list.at(i).at(1).toInt())->setText(tr(" "));
        }
    }
}
void testRecordView::addRowItem(const QMap<QString, QString> &data)
{
    QStandardItemModel *model = static_cast<QStandardItemModel*>(this->model());
    QList<QStandardItem *> listItem;
    listItem.append(new QStandardItem(data.value("meters")));//米数 0
    listItem.append(new QStandardItem(data.value("thickness")));//厚度 1
    listItem.append(new QStandardItem(data.value("position")));//位置 2
    listItem.append(new QStandardItem(data.value("length")));//长度 3
    listItem.append(new QStandardItem(data.value("defect")));//缺陷 4
    listItem.append(new QStandardItem(data.value("description")));//描述 5
    listItem.append(new QStandardItem()); //6操作
	listItem.append(new QStandardItem()); //7操作
	
    model->appendRow(listItem);
    QList<QVariantList> listAllItem;
    QVariantList list;
    list << model->rowCount() - 1 << 6 << 1 << 2;
    listAllItem.append(list);

    model->item(model->rowCount() - 1, 0)->setFlags(Qt::ItemIsEnabled);
    //model->item(i, 1)->setFlags(Qt::ItemIsEnabled);
    model->item(model->rowCount() - 1, 2)->setFlags(Qt::ItemIsEnabled);
    model->item(model->rowCount() - 1, 3)->setFlags(Qt::ItemIsEnabled);
    model->item(model->rowCount() - 1, 4)->setFlags(Qt::ItemIsEnabled);
	/*operCheckBoxItem *formCheckeBox = new operCheckBoxItem;
	formCheckeBox->setObjectName(QString::number(model->rowCount() - 1));
	this->setIndexWidget(model->index(model->rowCount() - 1, 6), formCheckeBox);
	*/
	//qobject_cast<qualityStandardItem*>(this->indexWidget(model->index(1, 1)))
    this->setRowHeight(model->rowCount() - 1, 40);
    editTable(listAllItem);
}
void testRecordView::addRowItem()
{
    QStandardItemModel *model = static_cast<QStandardItemModel*>(this->model());
	if (model->rowCount() > 13) {
		return;
	}
    QList<QStandardItem *> listItem;
    listItem.append(new QStandardItem());
    listItem.append(new QStandardItem());
    listItem.append(new QStandardItem());
    listItem.append(new QStandardItem());
    listItem.append(new QStandardItem());
    listItem.append(new QStandardItem());
    listItem.append(new QStandardItem());
	listItem.append(new QStandardItem());
    model->appendRow(listItem);
    QList<QVariantList> listAllItem;
    QVariantList list;
    list << model->rowCount()-1 << 6 << 1 << 2;
    listAllItem.append(list);

    model->item(model->rowCount() - 1, 0)->setFlags(Qt::ItemIsEnabled);
    //model->item(i, 1)->setFlags(Qt::ItemIsEnabled);
    model->item(model->rowCount() - 1, 2)->setFlags(Qt::ItemIsEnabled);
    model->item(model->rowCount() - 1, 3)->setFlags(Qt::ItemIsEnabled);
    model->item(model->rowCount() - 1, 4)->setFlags(Qt::ItemIsEnabled);
	
	/*operCheckBoxItem *formCheckeBox = new operCheckBoxItem;
	formCheckeBox->setObjectName(QString::number(model->rowCount() - 1));
	this->setIndexWidget(model->index(model->rowCount() - 1, 6), formCheckeBox);
	*/
    this->setRowHeight(model->rowCount() - 1, 40);
    editTable(listAllItem);
}
void testRecordView::slotDoubleClicked(const QModelIndex &index)
{
    if (index.row() >1 && index.column() == 1) {
        stopUpdate();
        tableViewEditItemForm  * form = new tableViewEditItemForm;
        QStandardItemModel *model = static_cast<QStandardItemModel*>(this->model());
        form->setTitle("厚度", index.row(), index.column());
        form->show();
        QObject::connect(form, SIGNAL(signalItemData(int, int, QString)), this, SLOT(slotEditItemData(int, int, QString)));
    }
    else if(index.row() > 1 && index.column() == 5)
    {
        stopUpdate();
        tableViewEditItemForm  * form = new tableViewEditItemForm;
        QStandardItemModel *model = static_cast<QStandardItemModel*>(this->model());
        form->setTitle("描述", index.row(), index.column());
        form->show();
        QObject::connect(form, SIGNAL(signalItemData(int, int, QString)), this, SLOT(slotEditItemData(int, int, QString)));
    }
}
//编辑数据
void testRecordView::slotEditItemData(int row, int column, QString text)
{ 
    if (text.isEmpty()) {
        startUpdate();
        return;
    }
	emit signalGetSpinBoxValue();//获取当前页数
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
    //model->item(row, column)->setForeground(QBrush(Qt::green));
    model->item(row, column)->setTextAlignment(Qt::AlignCenter);
    model->item(row, column)->setFont(/*QFont("Times", 10, QFont::Black)*/font);
    model->item(row, column)->setText(text);

    QString dataID;
    if (_currentRowData.at(row + (_curPage - 1) * 11 - 2).size() > 10) {
          if (_currentRowData.at(row + (_curPage - 1) * 11 - 2).at(0).toString().isEmpty())
             {
              startUpdate();
              return;
             } 
          dataID = _currentRowData.at(row + (_curPage - 1) * 11 - 2).at(0).toString().simplified();
    }
     QString table = "test_record";
     QSqlQuery *ret;
     std::map<QString, QString> where;  
     std::map<QString, QString> data;
     where["dataID"] = dataID;
     if(column == 1) {
         data["thickness"] = text;

		 QVariantList list = _currentRowData[row + (_curPage - 1) * 11 - 2];
		 QVariant var = list[2];		 
		 list.replace(2, text);
		 _currentRowData.replace(row + (_curPage - 1) * 11 - 2, list);
     }
     else if(column == 5)
     {
         data["description"] = text;
		 QVariantList list = _currentRowData[row + (_curPage - 1) * 11 - 2];
		 QVariant var = list[6];
		 list.replace(6, text);
		 _currentRowData.replace(row + (_curPage - 1) * 11 - 2, list);
     }
     /*databaseSqlInset::get_db()*/p_sqlOperation->Updata(table, where, data, &ret);
	
     //开始更新
     startUpdate();
	 //findCurrentData();
}
//处置
void testRecordView::slotConcessionCheckedStatus(bool checked)
{
	
	emit signalGetSpinBoxValue();//获取当前页数
	QStandardItemModel *model = qobject_cast<QStandardItemModel*>(this->model());
	QString str = qobject_cast<operCheckBoxItem*>(sender())->objectName();
	QString dataID;
	if (_currentRowData.at(str.toInt() + (_curPage - 1) * 11 - 2).size() > 10) {
		if (_currentRowData.at(str.toInt() + (_curPage - 1) * 11 - 2).at(0).toString().isEmpty())
		{
			startUpdate();
			return;
		}
		dataID = _currentRowData.at(str.toInt() + (_curPage - 1) * 11 - 2).at(0).toString().simplified();
	}
	QString table = "test_record";
	QSqlQuery *ret;
	std::map<QString, QString> where;
	std::map<QString, QString> data;
	where["dataID"] = dataID;
	int value = checked;
	data["concession"] = QString::number(value);
	/*databaseSqlInset::get_db()*/p_sqlOperation->Updata(table, where, data, &ret);
	//更新数据结构
	QVariantList list = _currentRowData[str.toInt() + (_curPage - 1) * 11 - 2];
	QVariant var = list[13];
	list.replace(13, value);
	_currentRowData.replace(str.toInt() + (_curPage - 1) * 11 - 2, list);
	//开始更新
	startUpdate();
	//findCurrentData();
}
//打印
void testRecordView::slotOutprintCheckedStatus(bool checked)
{
	emit signalGetSpinBoxValue();//获取当前页数
	QStandardItemModel *model = qobject_cast<QStandardItemModel*>(this->model());
	QString str = qobject_cast<operCheckBoxItem*>(sender())->objectName();
	QString dataID;
	if (_currentRowData.at(str.toInt()+(_curPage-1)*11 - 2).size() > 10) {
		if (_currentRowData.at(str.toInt() + (_curPage - 1) * 11- 2).at(0).toString().isEmpty())
		{
			startUpdate();
			return;
		}
		dataID = _currentRowData.at(str.toInt() + (_curPage - 1) * 11 - 2).at(0).toString().simplified();
	}
	QString table = "test_record";
	QSqlQuery *ret;
	std::map<QString, QString> where;
	std::map<QString, QString> data;
	where["dataID"] = dataID;
	int value = checked;
	data["outprint"] = QString::number(value);
	/*databaseSqlInset::get_db()*/p_sqlOperation->Updata(table, where, data, &ret);
	//更新数据结构
	QVariantList list = _currentRowData[str.toInt() + (_curPage - 1) * 11 - 2];
	QVariant var = list[12];
	list.replace(12, value);
	_currentRowData.replace(str.toInt() + (_curPage - 1) * 11 - 2, list);
	//开始更新
	startUpdate();
	//findCurrentData();
}
void testRecordView::tableViewClear()
{
    _isClear = true;
    int numRows = this->model()->rowCount();
    //qDebug() << numRows;
    for (int i = 0; i < numRows; i++)
    {
        this->model()->removeRow(2);
    }
    _isClear = false;
}
bool testRecordView::getClearStatus()
{
    return _isClear;
}
void testRecordView::setToolTipModel()
{
    QStandardItemModel * model = qobject_cast<QStandardItemModel*>(this->model());

    
}
void testRecordView::showToolTip(const QModelIndex &index)
{
    if (!index.isValid()) {
        qDebug() << "Invalid index";
        return;
    }
    QToolTip::showText(QCursor::pos(), index.data().toString());
}
void testRecordView::setTestRecordItem(std::map<std::string, std::string> map)
{

}
//更新列表
void testRecordView::updataItem(const QString &recordID)
{
    if (recordID.isEmpty()) {
        return;
    }
    QString table = "test_record";
    QSqlQuery * ret;//卷号
    QList<QVariantList> rowData;//查询卷号id映射表
    QMap<QString, QString> where;//索引值   
    where["record_ID"] = recordID;

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
	ListKey.push_back("outprint");
	ListKey.push_back("concession");
    if (!/*databaseSqlInset::get_db()*/p_sqlOperation->findData(table, where, ListKey, rowData, &ret)) {
        QMessageBox::warning(nullptr, tr("错误"),
            tr("查询数据库失败(%1)").arg(ret->lastError().text()), QMessageBox::Ok);
    }
    QStandardItemModel *model = static_cast<QStandardItemModel*>(this->model());
    int size = rowData.size();
	_currentRowData = rowData;
	tableViewClear();
	if (size <= 0) {
		emit signalOldDataClear(true);
		return;
	}
	int modelCount;
	int page = size / 11;//页数
	int rem = size % 11;//余数

	if (rem > 0||page==0) {
		page = page + 1;
	}
	emit signalComboxPageUpdate(QString("%1/%2").arg(page).arg(page),size);
	if (rem > 0) {
		for (int i = 0; i < rem; i++) {
			addRowItem();
		}
	}
	if (rem == 0) {
		for (int i = 0; i < 11; i++) {
			addRowItem();
		}
	}
	this->scrollToBottom();	
	modelCount = model->rowCount();
	for (int row = 2; row < modelCount; row++) {
		for (int column = 0; column < model->columnCount(); column++) {
			if (column == 6) {
				operCheckBoxItem *formCheckeBox = new operCheckBoxItem;
				formCheckeBox->setObjectName(QString::number(row));
				formCheckeBox->set_checkBox_outprint_checked(rowData.at(row - 2 + (page - 1) * 11).at(12).toBool());
				formCheckeBox->set_checkBox_concession_checked(rowData.at(row - 2 + (page - 1) * 11).at(13).toBool());
				connect(formCheckeBox, SIGNAL(signalConcessionCheckedStatus(bool)), this, SLOT(slotConcessionCheckedStatus(bool)));
				connect(formCheckeBox, SIGNAL(signalOutprintCheckedStatus(bool)), this, SLOT(slotOutprintCheckedStatus(bool)));
				this->setIndexWidget(model->index(row, 6), formCheckeBox);
			}
			model->item(row, column)->setText(rowData.at(row - 2 + (page - 1) * 11).at(column + 1).toString());
			model->item(row, column)->setTextAlignment(Qt::AlignCenter);
		}
	}
	
}
//每两秒钟查询一次数据库
void testRecordView::slotUpdataItem()
{
    if (_recordID.isEmpty()) {
        return;
    }
    QString table = "test_record";
    QSqlQuery * ret;//卷号
    QList<QVariantList> rowData;//查询卷号id映射表
    QMap<QString, QString> where;//索引值   
    where["record_ID"] = _recordID;

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
	ListKey.push_back("outprint");
	ListKey.push_back("concession");
    p_sqlOperation->findData(table, where, ListKey, rowData, &ret);

    QStandardItemModel *model = static_cast<QStandardItemModel*>(this->model());
    int size = rowData.size();
	_currentRowData = rowData;
	tableViewClear();
	if (size <= 0) {
		emit signalOldDataClear(true);
		return;
	} 
	int modelCount;
	int page = size / 11;//页数
	int rem = size % 11;//余数
	if (rem > 0 || page == 0) {
		page = page + 1;
	}
	emit signalComboxPageUpdate(QString("%1/%2").arg(page).arg(page),size);
	if (rem > 0) {
		for (int i = 0; i < rem; i++) {
			addRowItem();
		}
	}
	if (rem == 0) {
		for (int i = 0; i < 11; i++) {
			addRowItem();
		}
	}
	this->scrollToBottom();
	 modelCount = model->rowCount();
	for (int row = 2; row < modelCount; row++) {
		for (int column = 0; column < model->columnCount(); column++) {
			if (column == 6) {
				operCheckBoxItem *formCheckeBox = new operCheckBoxItem;
				formCheckeBox->setObjectName(QString::number(row));
				formCheckeBox->set_checkBox_outprint_checked(rowData.at(row - 2 + (page - 1) * 11).at(12).toBool());
				formCheckeBox->set_checkBox_concession_checked(rowData.at(row - 2 + (page - 1) * 11).at(13).toBool());
				connect(formCheckeBox, SIGNAL(signalConcessionCheckedStatus(bool)), this, SLOT(slotConcessionCheckedStatus(bool)));
				connect(formCheckeBox, SIGNAL(signalOutprintCheckedStatus(bool)), this, SLOT(slotOutprintCheckedStatus(bool)));
				this->setIndexWidget(model->index(row, 6), formCheckeBox);
			}
			model->item(row, column)->setText(rowData.at(row - 2 + (page - 1) * 11).at(column + 1).toString());
			model->item(row, column)->setTextAlignment(Qt::AlignCenter);
		}
	}
}
//当前页的检测记录
void testRecordView::findCurPageRecord(const QString &curStr)
{
	tableViewClear();
	QStandardItemModel *model = static_cast<QStandardItemModel*>(this->model());
	int size =_currentRowData.size() ;
	if (size <= 0||curStr.isEmpty()) {
		return;
	}
	int modelCount;
	int curPage = curStr.split("/").at(0).toInt();//当前页
	int page = curStr.split("/").at(1).toInt();//总页数
	if (curPage == page) {
		for (int i = 0; i < size-(curPage-1)*11 ; i++) {
			addRowItem();
		}
		modelCount = size - (curPage-1) * 11 + 2;
	}
	else
	{
		for (int i = 0; i <11; i++) {
			addRowItem();
		}
		modelCount = 11 + 2;
	}
	this->scrollToBottom();
	for (int row = 2; row < modelCount; row++) {
		for (int column = 0; column < model->columnCount(); column++) {
			if (column == 6) {
				operCheckBoxItem *formCheckeBox = new operCheckBoxItem;
				formCheckeBox->setObjectName(QString::number(row));
				formCheckeBox->set_checkBox_concession_checked(_currentRowData.at(row - 2 + (curPage - 1) * 11).at(13).toBool());
				formCheckeBox->set_checkBox_outprint_checked(_currentRowData.at(row - 2 + (curPage - 1) * 11).at(12).toBool());
				connect(formCheckeBox, SIGNAL(signalConcessionCheckedStatus(bool)), this, SLOT(slotConcessionCheckedStatus(bool)));
				connect(formCheckeBox, SIGNAL(signalOutprintCheckedStatus(bool)), this, SLOT(slotOutprintCheckedStatus(bool)));
				this->setIndexWidget(model->index(row, 6), formCheckeBox);
			}
			model->item(row, column)->setText(_currentRowData.at(row - 2 + (curPage - 1) * 11).at(column + 1).toString());
			model->item(row, column)->setTextAlignment(Qt::AlignCenter);
		}
	}
	//findCurrentData();
}
void testRecordView::setCurrentRecordId(const QString &record)
{
    _recordID = record;
    startUpdate();
    
}
void testRecordView::stopUpdate()
{
    if (_timer->isActive()) {
        _timer->stop();
    }    
}
void testRecordView::startUpdate()
{
    if (!_timer->isActive()) {
        _timer->start(3000);
    }
}
bool testRecordView::timerStatusisActive()
{
    return _timer->isActive();
}
QList<QVariantList> testRecordView::getTestRecordCurrentRowData() 
{
	QList<QVariantList> rowdata;
	for (int i = 0; i < _currentRowData.size(); i++) {
		if (_currentRowData.at(i).at(12).toInt() == 1) {
			rowdata.push_back(_currentRowData.at(i));
		}
	}
	return rowdata;
}
//查询当前检测记录
void testRecordView::findCurrentData()
{
	QString table = "test_record";
	QSqlQuery * ret;//卷号
	QList<QVariantList> rowData;//查询卷号id映射表
	QMap<QString, QString> where;//索引值   
	where["record_ID"] = _recordID;

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
	ListKey.push_back("outprint");
	ListKey.push_back("concession");
	p_sqlOperation->findData(table, where, ListKey, rowData, &ret);
	_currentRowData = rowData;
}