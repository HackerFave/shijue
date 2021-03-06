﻿#include "testRecordView.h"
#include <QMessageBox>
#include <QScrollBar>
testRecordView::testRecordView(QWidget *parent)
    : QTableView(parent)
{
    ui.setupUi(this);
   // initTableView();
	
	//QScrollBar *barV = new QScrollBar; //this->verticalScrollBar();
	QScrollBar *barH = this->horizontalScrollBar();
	/*barV->setStyleSheet("QScrollBar:vertical{"        //垂直滑块整体
		"background:#FFFFFF;"  //背景色
		"padding-top:20px;"    //上预留位置（放置向上箭头）
		"padding-bottom:20px;" //下预留位置（放置向下箭头）
		"padding-left:3px;"    //左预留位置（美观）
		"padding-right:3px;"   //右预留位置（美观）
		"border-left:1px solid #d7d7d7;}"//左分割线
		"QScrollBar::handle:vertical{"//滑块样式
		"background:#dbdbdb;"  //滑块颜色
		"border-radius:6px;"   //边角圆润
		"min-height:80px;}"    //滑块最小高度
		"QScrollBar::handle:vertical:hover{"//鼠标触及滑块样式
		"background:#d0d0d0;}" //滑块颜色
		"QScrollBar::add-line:vertical{"//向下箭头样式
		"background:url(:/images/resource/images/checkout/down.png) center no-repeat;}"
		"QScrollBar::sub-line:vertical{"//向上箭头样式
		"background:url(:/images/resource/images/checkout/up.png) center no-repeat;}");
		*/
	barH->setStyleSheet(
		 
		"QScrollBar::horizontal{\n"
		/*"width:28px;\n"*/
		"height:8px;"
		"background-color:#ffaaff;\n"
		"}\n"
		"QScrollBar::handle:horizontal{\n"
		/*"background:rgba(80,80,80,0.6);\n"*/
		"background:#303030;\n"
		"border-radius: 2px;\n"
		"border: none;\n"
		"}\n"
		"QScrollBar::add-page:horizontal{\n"

		/*"background:#000000;\n"*/
		"}\n"
		"QScrollBar::sub-page:horizontal{\n"
		/*"background:#000000;\n"*/
		"}\n"
    );
}

testRecordView::~testRecordView()
{
    
}
void testRecordView::iniTableView()
{

	
}
void testRecordView::initTableView(const QStringList &list,QString head)
{
	
    this->scrollToBottom();
    this->setMouseTracking(true);
    this->setMinimumWidth(750);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
   // this->setSelectionBehavior(QAbstractItemView::SelectRows);
    QStandardItemModel* item_model = new QStandardItemModel(2, list.size());
    this->setModel(item_model);
	
    //设置表格的单元为只读属性，即不能编辑        
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
   // this->setEditTriggers(QAbstractItemView::AllEditTriggers);//可编辑
    //如果你用在QTableView中使用右键菜单，需启用该属性        
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->verticalHeader()->setDefaultSectionSize(40);
    this->setWordWrap(true);//文字换行
    //connect(this, SIGNAL(entered(QModelIndex)), this, SLOT(showToolTip(QModelIndex)));
    connect(this, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(slotDoubleClicked(const QModelIndex &)));
    this->horizontalHeader()->hide();                       // 隐藏水平表头
    this->verticalHeader()->hide();                         // 隐藏垂直表头
    //ui.tableView->horizontalHeader()->setStretchLastSection(true);  // 伸缩最后一列
   // this->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);//只有设置fixed才能手动设置宽度
    /* QStringList column, row;
     column << "Column 1" << "Column 2" << "Column 3" << "Column 4";
     row << "row 1" << "row 2" << "row 3" << "row 4";
     item_model->setHorizontalHeaderLabels(column);                // 设置水平表头标签
     item_model->setVerticalHeaderLabels(row);                     // 设置垂直表头标签
     */

     // 添加item到model
    for (int i = 0; i < this->model()->rowCount(); ++i) {
        for (int j = 0; j < this->model()->columnCount(); ++j) {
            item_model->setItem(i, j, new QStandardItem());
            this->setColumnWidth(j, 200);
        }
          
    }
    for (int i = 0; i < list.size(); i++) {
       item_model->item(1, i)->setText(list.at(i));//表头
	   item_model->item(1, i)->setIcon(QIcon(":/resources/icons/line.png"));
		//item_model->item(0, i)->setText(list.at(i));//表头
    }
    QList<QVariantList> listAllItem;
    QVariantList list1;
    list1 << 0 << 0 << 1 << list.size() << head;
    listAllItem << list1;
   
    editTable(listAllItem);//
	//this->setRowHidden(0, true);//隐藏第一行
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
            model->item(i, j)->setFlags(Qt::ItemIsEnabled);
            //model->item(1, j)->setBackground(QColor(0, 85, 127));
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
                model->item(1, j)->setBackground(QColor(48, 48, 48));
            }
           this->setColumnWidth(j, 300);
        }	
        this->setRowHeight(i, 42);
        
    }
    this->setShowGrid(false);                               // 隐藏网格线
    this->setFocusPolicy(Qt::StrongFocus);                      // 去除当前Cell周边虚线框
    
	this->setAlternatingRowColors(true);                    // 开启隔行异色
	this->setRowHeight(0, 0);
	this->setFocusPolicy(Qt::NoFocus);
	this->setSelectionBehavior(QAbstractItemView::SelectRows);
}
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
        model->item(list.at(i).at(0).toInt(), list.at(i).at(3).toInt()-1)->setTextAlignment(Qt::AlignCenter);
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
void testRecordView::addRowItem(const QMap<QString, QString> &data,const QStringList &keyList)
{
    QStandardItemModel *model = static_cast<QStandardItemModel*>(this->model());
    QList<QStandardItem *> listItem;
    for (int i = 0; i < keyList.size(); i++) {
        QStandardItem *item = new QStandardItem(data.value(keyList.at(i)));
        item->setTextAlignment(Qt::AlignCenter);
        item->setToolTip(keyList.at(i));
        listItem.append(item);
		
    }
    model->appendRow(listItem);

	for (int i = 0; i < model->rowCount(); i++) {
		this->setRowHeight(i, 40);
	}
	this->setRowHeight(0, 0);
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
    model->appendRow(listItem);
    QList<QVariantList> listAllItem;
    QVariantList list;
    list << model->rowCount()-1 << 5 << 1 << 2;
    listAllItem.append(list);

    model->item(model->rowCount() - 1, 0)->setFlags(Qt::ItemIsEnabled);
    //model->item(i, 1)->setFlags(Qt::ItemIsEnabled);
    model->item(model->rowCount() - 1, 2)->setFlags(Qt::ItemIsEnabled);
    model->item(model->rowCount() - 1, 3)->setFlags(Qt::ItemIsEnabled);
    model->item(model->rowCount() - 1, 4)->setFlags(Qt::ItemIsEnabled);
    this->setRowHeight(model->rowCount() - 1, 40);
    editTable(listAllItem);
}
void testRecordView::slotDoubleClicked(const QModelIndex &index)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(this->model());
    QStandardItem * currentItem = model->itemFromIndex(index);
    emit signalItemText(currentItem->toolTip().simplified()+"|"+currentItem->text().simplified()
    +"|"+model->item(0,0)->text().simplified());
}
void testRecordView::slotEditItemData(int row, int column, QString text)
{ 
   
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
	//int modelCount = model->rowCount();
	for (int row = 2; row < modelCount; row++) {
		for (int column = 0; column < model->columnCount(); column++) {
			model->item(row, column)->setText(_currentRowData.at(row - 2 + (curPage - 1) * 11).at(column + 1).toString());
			model->item(row, column)->setTextAlignment(Qt::AlignCenter);
		}
	}
}
void testRecordView::setCurrentRecordId(const QString &record)
{
   // _recordID = record;
   // startUpdate();
    
}
void testRecordView::setrelatedid(const QString & relatedid)
{
    relatedid_ = relatedid;
}