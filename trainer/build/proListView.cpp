#include "proListView.h"
#include <qdebug.h>
proListView::proListView(QWidget *parent)
    : QTableView(parent)
{
    ui.setupUi(this);
    //QStringList list = getListFile();
    //initTableView(list);
}

proListView::~proListView()
{
}
void proListView::initTableView(const QMap<QString, QVariant> &map)
{
    _proMap = map;
   QStringList listPro;
   QMap<QString, QVariant>::const_iterator it;
   for (it = map.constBegin(); it != map.constEnd(); ++it) {
       listPro.push_back(it.key());
   }
    //this->setMinimumWidth(750);
    this->setSelectionMode(QAbstractItemView::NoSelection);
    // this->setSelectionBehavior(QAbstractItemView::SelectRows);
    QStandardItemModel* item_model = new QStandardItemModel(map.size(), 1);//行/列
    this->setModel(item_model);
    //设置表格的单元为只读属性，即不能编辑        
    //this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setEditTriggers(QAbstractItemView::AllEditTriggers);
    //如果你用在QTableView中使用右键菜单，需启用该属性        
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setWordWrap(true);//文字换行
   // connect(this, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(slotDoubleClicked(const QModelIndex &)));
    this->horizontalHeader()->hide();                       // 隐藏水平表头
    this->verticalHeader()->hide();                         // 隐藏垂直表头
    //ui.tableView->horizontalHeader()->setStretchLastSection(true);  // 伸缩最后一列
    //this->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->verticalHeader()->setDefaultSectionSize(80);
    /*for (int i = 0; i < this->model()->rowCount(); i++) {
        this->setRowHeight(i, 40);
    }
   */
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
           // model->item(i, j)->setTextAlignment(Qt::AlignCenter);
            //model->item(i, j)->setFont(font);
            proListItem * itemForm = new proListItem(this,QString::number(i));
            itemForm->setProName(listPro.at(i));
            this->setIndexWidget(model->index(i, 0), itemForm);
            connect(itemForm, SIGNAL(signalStatus(bool,QString,QString)), this, SLOT(slotStatus(bool,QString,QString)));
            connect(itemForm, SIGNAL(signalSelect(bool,QString,QString)), this, SLOT(slotSelect(bool,QString,QString)));
        }
    }
  
    this->setShowGrid(true);                               // 隐藏网格线
    this->setFocusPolicy(Qt::StrongFocus);                      // 去除当前Cell周边虚线框
    this->setAlternatingRowColors(true);                    // 开启隔行异色
}
void proListView::setListFile(const QStringList &list)
{
    for (int i = 0; i < list.size(); i++)
    {
        
    }
}
void proListView::addItem(const QString & str)
{

}
QStringList proListView::getListFile()
{
    QStringList list;
    list << "111" << "222" << "333";
    return list;
}
void proListView::tableViewClear()
{
    _isClear = true;
    int numRows = this->model()->rowCount();
    //qDebug() << numRows;
    for (int i = 0; i < numRows; i++)
    {
        this->model()->removeRow(0);
    }
    _isClear = false;
}
void proListView::slotStatus(bool checked ,QString id,QString proName)
{
    qDebug() << id;
    QStandardItemModel *model = static_cast<QStandardItemModel*>(this->model());
    proListItem * itemForm = qobject_cast<proListItem*>(this->indexWidget(model->index(0, 1)));

    //if (checked) {
        emit signalFindPro(_proMap.value(proName).toString());
    //}
}

void proListView::slotSelect(bool checked, QString id,QString proName)
{
    qDebug() << id;
    //if (checked) {
        emit signalProName(_proMap.value(proName).toString());
   // }
}
