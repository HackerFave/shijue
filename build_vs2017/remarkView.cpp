#include "remarkView.h"

remarkView::remarkView(QWidget *parent)
    : QTableView(parent)
{
    ui.setupUi(this);
    initTableView();
}

remarkView::~remarkView()
{
}
void remarkView::initTableView()
{
    //this->setMinimumWidth(750);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    // this->setSelectionBehavior(QAbstractItemView::SelectRows);
    QStandardItemModel* item_model = new QStandardItemModel(3, 6);
    this->setModel(item_model);
    //设置表格的单元为只读属性，即不能编辑        
    //this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setEditTriggers(QAbstractItemView::AllEditTriggers);
    //如果你用在QTableView中使用右键菜单，需启用该属性        
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setWordWrap(true);//文字换行
    connect(this, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(slotDoubleClicked(const QModelIndex &)));
    this->horizontalHeader()->hide();                       // 隐藏水平表头
    this->verticalHeader()->hide();                         // 隐藏垂直表头
    //ui.tableView->horizontalHeader()->setStretchLastSection(true);  // 伸缩最后一列
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
    item_model->item(2, 4)->setText("操作员");
    QList<QVariantList> listAllItem;
    QVariantList list1;
    QVariantList list2;
    QVariantList list3;
    list1 << 0 << 0 << 2 << 1 << "备注";
    list2 << 0 << 1 << 2 << 5 ;
    list3 << 2 << 0 << 1 << 4 <<"生产时间";
    listAllItem << list1<<list2<<list3;
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
            font.setBold(true);//粗体
            // font.setStyle(QFont::StyleOblique);
            // font.setCapitalization(QFont::Capitalize);//首字母大写
            model->item(i, j)->setTextAlignment(Qt::AlignCenter);
            model->item(i, j)->setFont(font);
        }
    }
    item_model->item(0, 0)->setFlags(Qt::ItemIsEnabled);
    item_model->item(2, 4)->setFlags(Qt::ItemIsEnabled);
    item_model->item(2, 0)->setFlags(Qt::ItemIsEnabled);

    this->setShowGrid(true);                               // 隐藏网格线
    this->setFocusPolicy(Qt::StrongFocus);                      // 去除当前Cell周边虚线框
    this->setAlternatingRowColors(true);                    // 开启隔行异色
}
void remarkView::editTable(const QList<QVariantList> &list)
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
        model->item(list.at(i).at(0).toInt(), list.at(i).at(1).toInt())->setFont(/*QFont("Times", 10, QFont::Black)*/font);
        if (list.at(i).size() > 4) {
            model->item(list.at(i).at(0).toInt(), list.at(i).at(1).toInt())->setText(list.at(i).at(4).toString());
        }
        else
        {
           // model->item(list.at(i).at(0).toInt(), list.at(i).at(1).toInt())->setForeground(QBrush(Qt::green));
            model->item(list.at(i).at(0).toInt(), list.at(i).at(1).toInt())->setText(tr(""));
        }
    }
}
void remarkView::slotDoubleClicked(const QModelIndex &index)
{
    if (index.row() == 0 && index.column() == 1) {
        tableViewEditItemForm  * form = new tableViewEditItemForm;
        QStandardItemModel *model = static_cast<QStandardItemModel*>(this->model());
        form->setTitle("备注", index.row(), index.column());
        form->show();
        QObject::connect(form, SIGNAL(signalItemData(int, int, QString)), this, SLOT(slotEditItemData(int, int, QString)));
    }
    else if (index.row() == 2 && index.column() == 5)
    {
        tableViewEditItemForm  * form = new tableViewEditItemForm;
        QStandardItemModel *model = static_cast<QStandardItemModel*>(this->model());
        form->setTitle("操作员", index.row(), index.column());
        form->show();
        QObject::connect(form, SIGNAL(signalItemData(int, int, QString)), this, SLOT(slotEditItemData(int, int, QString)));
    }
}
void remarkView::slotEditItemData(int row, int column, QString text)
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
  //  model->item(row, column)->setForeground(QBrush(Qt::green));
    model->item(row, column)->setTextAlignment(Qt::AlignCenter);
    model->item(row, column)->setFont(/*QFont("Times", 10, QFont::Black)*/font);

    model->item(row, column)->setText(text);

}
void remarkView::tableViewClear()
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(this->model());
    model->item(0, 1)->clearData();
    model->item(2, 5)->clearData();
    model->item(2, 0)->setText("生产时间");
}