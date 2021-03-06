#include "tableWidgetParent.h"
#include "ui_tableWidgetParent.h"

tableWidgetParent::tableWidgetParent(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::tableWidgetParent)
{

    ui->setupUi(this);
	connect(ui->tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(slotTip(int, int)));



}

tableWidgetParent::~tableWidgetParent()
{
    delete ui;
}
void tableWidgetParent::setRowColumn(const int row, const int column)
{
    _Row = row;
    _Column = column;
}
void tableWidgetParent::init(const QStringList & headerList)
{

    ui->tableWidget->setColumnCount(_Column); //设置列数
    ui->tableWidget->setRowCount(_Row);
    initHeaderList(headerList);
    //    setFirstLineCenter();
    ui->tableWidget->setAlternatingRowColors(true);//隔行换色
    ui->tableWidget->setStyleSheet("alternate-background-color:rgb(44,44,44);"
                                   "background:rgb(37,37,37);"
                                   "color:rgb(135,135,135);"
                                   "gridline-color:red;"
                                   "font-size: 14px;"
								   "border: 0px solid;"
                                   );

    setStyleSheet("QTableWidget::item:selected { background-color: rgb(69,69,69) ;"
                  "font-size: 14px;"
                  "color: rgb(245, 245, 245) "
                  "};");//选中时背景色
    ui->tableWidget->setSelectionBehavior( QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->verticalHeader()->setVisible(false); //隐藏行号
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    ui->tableWidget->setShowGrid(false); //设置不显示格子线
    ui->tableWidget->horizontalHeader()->setStretchLastSection(false);//充满表格宽度 rgb(45,45,45)
    ui->tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{border:1px; "
                                                       "background:rgb(45,45,45);"
                                                       "gridline-color:rgb(45,45,45);font-size: 14px; padding-left: 2px;}");//;font:13pt '宋体'
   // ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  //  ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //ui->tableWidget->horizontalHeader()->setSectionsClickable(false); //设置表头不可点击
	ui->tableWidget->horizontalHeader()->setMinimumHeight(40);//行高度

    ui->tableWidget->setWordWrap(true);
    ui->tableWidget->setFocusPolicy(Qt::NoFocus);

	int rowheight = _Row;
	while (rowheight--)
	{
		ui->tableWidget->setRowHeight(rowheight, 40);

	}

	

	//ui->tableWidget->setColumnWidth(0, 60);
	//ui->tableWidget->setColumnWidth(1, 120);
	//ui->tableWidget->setColumnWidth(2, 300);
	//ui->tableWidget->setColumnWidth(3, 170);
	//ui->tableWidget->setColumnWidth(4, 150);
	//ui->tableWidget->setColumnWidth(5, 150);
	//ui->tableWidget->setColumnWidth(6, 150);
	//ui->tableWidget->setColumnWidth(7, 150);
	//ui->tableWidget->setColumnWidth(8, 120);
}
void tableWidgetParent::initHeaderList(const QStringList &list)
{
    ui->tableWidget->setHorizontalHeaderLabels(list);
    ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft|Qt::AlignVCenter);
}
void tableWidgetParent::addItemIcon(int row ,int column,const QString &iconName)
{
    QTableWidgetItem *item1 = new QTableWidgetItem;
    QIcon icon;
    icon.addFile(tr("%1").arg(iconName));
    icon.pixmap(120,120);
    item1->setIcon(icon);
    ui->tableWidget->setItem(row,column,item1);


}
void tableWidgetParent::setItem(int row, int column, QTableWidgetItem *item)
{
    ui->tableWidget->setItem(row,column,item);
	

}
void tableWidgetParent::setDataItem(int row, int column,const QVariant &dataStr)
{

    QTableWidgetItem *item1 = new QTableWidgetItem(tr(" %1").arg(dataStr.toString()));
    ui->tableWidget->setItem(row,column,item1);
}
void tableWidgetParent::setHeaderItemIcon( int column, QString &icon)
{
    ui->tableWidget->horizontalHeaderItem(column)->setIcon(QIcon(tr("%1").arg(icon)));
}
void tableWidgetParent::initHeaderStyle(const QString &iconName)
{
    if(_Column>=1){
        for(int i=1;i<_Column;++i){
            ui->tableWidget->horizontalHeaderItem(i)->setIcon(QIcon(tr("%1").arg(iconName)));

        }
    }
    else {
        return;
    }

}
int tableWidgetParent::getRowCount() const
{
    return _Row;
}
int tableWidgetParent::getColumnCount() const
{
    return  _Column;
}
void tableWidgetParent::firstLineCenter(bool isOk)
{
    _isCenter = isOk;
}
bool tableWidgetParent::isFirstLineCenter() const
{
    return _isCenter;
}
void tableWidgetParent::setFirstLineCenter()
{
    if(_isCenter){
        ui->tableWidget->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignCenter);
    }
}

void tableWidgetParent::setHeaderColumnPosition(int column, Qt::AlignmentFlag flag)
{
    ui->tableWidget->horizontalHeaderItem(column)->setTextAlignment(flag);
}
void tableWidgetParent::setItemPosition(int row,int column, Qt::AlignmentFlag flag)
{
    ui->tableWidget->item(row,column)->setTextAlignment(flag);

}
QModelIndex tableWidgetParent::getIndex(int x, int y)
{
	return ui->tableWidget->indexAt(QPoint(x, y));
}
QString tableWidgetParent::getTableData(int row, int column)
{
	if ((row < _Row) && (column < _Column))
	{
		return ui->tableWidget->item(row, column)->text();
	}
	else
		return QString();
}
void tableWidgetParent::clearTableData()
{
	ui->tableWidget->clear();
}
void* tableWidgetParent::getTablePoint()
{
	return ui->tableWidget;
}
void tableWidgetParent::addTtemButton(int row, int column, QToolButton *button)
{
    ui->tableWidget->setCellWidget(row,column,button);
}
void tableWidgetParent::slotTip(int a, int b)
{
	//QString text= ui->tableWidget->item(a, b)->text();
	//ui->tableWidget->item(a, b)->setToolTip(text);
}
