#include "tableViewQualityInspectionRecord.h"
#include <QMessageBox>
tableViewQualityInspectionRecord::tableViewQualityInspectionRecord(QWidget *parent)
    : QTableView(parent)
{
    ui.setupUi(this);
    p_sqlOperation = new sqlOperation(0);
    if (!p_sqlOperation->Init("127.0.0.1", "root", "123456", "zhongji", "typeInspection"))
    {
        QMessageBox::warning(nullptr, tr("错误"), tr("初始化数据库失败"), QMessageBox::Apply);
    }
    initTableView();
}

tableViewQualityInspectionRecord::~tableViewQualityInspectionRecord()
{
    if (p_sqlOperation) {
        p_sqlOperation->CloseDB();
        delete p_sqlOperation;
    }
}
void tableViewQualityInspectionRecord::initTableView()
{
    this->setMouseTracking(true);
    this->setMinimumWidth(750);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    // this->setSelectionBehavior(QAbstractItemView::SelectRows);
    QStandardItemModel* item_model = new QStandardItemModel(18, 6);
    this->setModel(item_model);
    //设置表格的单元为只读属性，即不能编辑        
    //this->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    this->setEditTriggers(QAbstractItemView::AllEditTriggers);//可编辑
    //如果你用在QTableView中使用右键菜单，需启用该属性        
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->verticalHeader()->setDefaultSectionSize(15);
    this->setWordWrap(true);//文字换行
    connect(this, SIGNAL(entered(QModelIndex)), this, SLOT(showToolTip(QModelIndex)));
    //connect(this, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(slotDoubleClicked(const QModelIndex &)));
    this->horizontalHeader()->hide();                       // 隐藏水平表头
    this->verticalHeader()->hide();                         // 隐藏垂直表头
    this->horizontalHeader()->setStretchLastSection(true);  // 伸缩最后一列
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
    item_model->item(2, 0)->setText("玻纤含量/%");
    item_model->item(2, 2)->setText("面克重/gsm");
    item_model->item(2, 4)->setText("厚度/mm");
    item_model->item(4, 0)->setText("位置/mm");
    item_model->item(5, 0)->setText("拉伸强度/MPa");
    item_model->item(6, 0)->setText("拉伸模量/GPa");

    item_model->item(11, 0)->setText("玻纤含量/%");
    item_model->item(11, 2)->setText("面克重/gsm");
    item_model->item(11, 4)->setText("厚度/mm");
    item_model->item(13, 0)->setText("位置/mm");
    item_model->item(14, 0)->setText("拉伸强度/MPa");
    item_model->item(15, 0)->setText("拉伸模量/GPa");
    QList<QVariantList> listAllItem;
    QVariantList list1, list2, list3, list4, list5, list6, list7, list8,list9,list10;
    list1 << 0 << 0 << 1 << 6 << "质检记录（首检）";
    list2 << 1 << 0 << 1 << 6 << "基础参数";
    list3 << 3<< 0 << 1 << 6 << "性能参数";

    list4 << 7 << 0 << 2 << 1 << "备注";
    list5 << 7 << 1 << 2 << 6;

    list6 << 9 << 0 << 1 << 6 << "质检记录（尾检）";
    list7 << 10 << 0 << 1 << 6 << "基础参数";
    list8 << 12 << 0 << 1 << 6 << "性能参数";

    list9 << 16 << 0 << 2 << 1 << "备注";
    list10 << 16 << 1 << 2 << 6;

    listAllItem << list1<<list2<<list3<<list4<<list5<<list6<<list7<<list8<<list9<<list10;
    for (int i = 1; i < item_model->rowCount(); i++) {
        QVariantList list;
        if (i == 1) {
            list << i << 5 << 1 << 2 << "描述*";
        }
        list << i << 5 << 1 << 2;
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
           
            if (i >= 0 && i <= 1) {
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
                model->item(0, j)->setBackground(QColor(0, 85, 127));
               
            }
            if (i == 3||i==9||i==10||i==12) {
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
                model->item(i, 0)->setTextAlignment(Qt::AlignCenter);
                model->item(i, 0)->setFont(font);
                model->item(i, 0)->setFlags(Qt::ItemIsEnabled);
                model->item(9, 0)->setBackground(QColor(0, 85, 127));
            }
            if (i > 1) {
                model->item(i, 0)->setFlags(Qt::ItemIsEnabled);
                //model->item(i, 1)->setFlags(Qt::ItemIsEnabled);
                if (i == 2 || i == 11) {
                    model->item(i, 2)->setFlags(Qt::ItemIsEnabled);
                    // model->item(i, 3)->setFlags(Qt::ItemIsEnabled);
                    model->item(i, 4)->setFlags(Qt::ItemIsEnabled);
                    //model->item(i, 5)->setFlags(Qt::ItemIsEnabled);
                }
            }
            if (i == 7 || i == 16) {
                model->item(i, 1)->setTextAlignment(Qt::AlignCenter);
            }
            model->item(i, j)->setTextAlignment(Qt::AlignCenter);
        }
        this->setRowHeight(i, 42);
    }
    this->setShowGrid(true);                               // 隐藏网格线
    this->setFocusPolicy(Qt::StrongFocus);                      // 去除当前Cell周边虚线框
    this->setAlternatingRowColors(true);                    // 开启隔行异色

}
void tableViewQualityInspectionRecord::editTable(const QList<QVariantList> &list)
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
void tableViewQualityInspectionRecord::getInspectionFirst(const QString &recordID)
{
    QStandardItemModel *model = static_cast<QStandardItemModel*>(this->model());
    QSqlQuery * ret;
    std::map<QString, QString> data;
    QString table = "inspection_table";
    data["boxianhanliang"] = model->item(2,1)->text();//玻纤含量
    data["mianke"] = model->item(2, 3)->text();//面克重
    data["houdu"] = model->item(2, 5)->text();//厚度
    data["weizhi_1"] = model->item(4, 1)->text();
    data["weizhi_2"] = model->item(4, 2)->text();
    data["weizhi_3"] = model->item(4, 3)->text();
    data["weizhi_4"] = model->item(4, 4)->text();
    data["weizhi_5"] = model->item(4, 5)->text();
    data["lashenqiangdu_1"] = model->item(5, 1)->text();
    data["lashenqiangdu_2"] = model->item(5, 2)->text();
    data["lashenqiangdu_3"] = model->item(5, 3)->text();
    data["lashenqiangdu_4"] = model->item(5, 4)->text();
    data["lashenqiangdu_5"] = model->item(5, 5)->text();
    data["lashenmoliang_1"] = model->item(6, 1)->text();
    data["lashenmoliang_2"] = model->item(6, 2)->text();
    data["lashenmoliang_3"] = model->item(6, 3)->text();
    data["lashenmoliang_4"] = model->item(6, 4)->text();
    data["lashenmoliang_5"] = model->item(6, 5)->text();
    data["remark"] = model->item(7, 1)->text();//备注
    data["inspection_type"] = "0";//首检
    data["record_ID"] = recordID;//备注

    QSqlQuery * retFind;
    QMap<QString, QString> where;
    std::map<QString, QString> whereUpdate;
    QStringList keyList;
    QList<QMap<QString, QString>> row;
    where["record_ID"] = recordID;
    where["inspection_type"] = "0";
    whereUpdate["record_ID"] = recordID;
    whereUpdate["inspection_type"] = "0";
    keyList << "record_ID" << "inspection_type";
    if (p_sqlOperation->findData(table,where,keyList,row, &retFind)) {

        if (row.size() >= 1) {
            if (!p_sqlOperation->Updata(table, whereUpdate, data, &ret)) {
                QMessageBox::warning(nullptr, tr("错误"),
                    tr("质检记录更新失败（%1）").arg(ret->lastError().text()), QMessageBox::Ok);
            }
        }
        else
        {
            if (!p_sqlOperation->Add(table, data, &ret)) {
                QMessageBox::warning(nullptr, tr("错误"),
                    tr("质检记录添加失败（%1）").arg(ret->lastError().text()), QMessageBox::Ok);
            }
        }
        
    }
}
void tableViewQualityInspectionRecord::getInspectionEnd(const QString &recordID)
{
    QStandardItemModel *model = static_cast<QStandardItemModel*>(this->model());
    QSqlQuery * ret;
    std::map<QString, QString> data;
    QString table = "inspection_table";
    data["boxianhanliang"] = model->item(11, 1)->text();//玻纤含量
    data["mianke"] = model->item(11, 3)->text();//面克重
    data["houdu"] = model->item(11, 5)->text();//厚度
    data["weizhi_1"] = model->item(13, 1)->text();
    data["weizhi_2"] = model->item(13, 2)->text();
    data["weizhi_3"] = model->item(13, 3)->text();
    data["weizhi_4"] = model->item(13, 4)->text();
    data["weizhi_5"] = model->item(13, 5)->text();
    data["lashenqiangdu_1"] = model->item(14, 1)->text();
    data["lashenqiangdu_2"] = model->item(14, 2)->text();
    data["lashenqiangdu_3"] = model->item(14, 3)->text();
    data["lashenqiangdu_4"] = model->item(14, 4)->text();
    data["lashenqiangdu_5"] = model->item(14, 5)->text();
    data["lashenmoliang_1"] = model->item(15, 1)->text();
    data["lashenmoliang_2"] = model->item(15, 2)->text();
    data["lashenmoliang_3"] = model->item(15, 3)->text();
    data["lashenmoliang_4"] = model->item(15, 4)->text();
    data["lashenmoliang_5"] = model->item(15, 5)->text();
    data["remark"] = model->item(16, 1)->text();//备注
    data["inspection_type"] = "1";//尾检
    data["record_ID"] = recordID;//备注

    QSqlQuery * retFind;
    QMap<QString, QString> where;
    std::map<QString, QString> whereUpdate;
    QStringList keyList;
    QList<QMap<QString, QString>> row;
    where["record_ID"] = recordID;
    where["inspection_type"] = "1";
    whereUpdate["record_ID"] = recordID;
    whereUpdate["inspection_type"] = "1";
    keyList << "record_ID" << "inspection_type";
    if (p_sqlOperation->findData(table, where, keyList, row, &retFind)) {

        if (row.size() >= 1) {
            if (!p_sqlOperation->Updata(table, whereUpdate, data, &ret)) {
                QMessageBox::warning(nullptr, tr("错误"),
                    tr("质检记录更新失败（%1）").arg(ret->lastError().text()), QMessageBox::Ok);
            }
        }
        else
        {
            if (!p_sqlOperation->Add(table, data, &ret)) {
                QMessageBox::warning(nullptr, tr("错误"),
                    tr("质检记录添加失败（%1）").arg(ret->lastError().text()), QMessageBox::Ok);
            }
        }

    }

}
void tableViewQualityInspectionRecord::setInspectionFirst(const QString &recordID)
{
    QStandardItemModel *model = static_cast<QStandardItemModel*>(this->model());
    model->item(2, 1)->clearData();//玻纤含量
    model->item(2, 3)->clearData();//面克重
    model->item(2, 5)->clearData();//厚度
    model->item(4, 1)->clearData();
    model->item(4, 2)->clearData();
    model->item(4, 3)->clearData();
    model->item(4, 4)->clearData();
    model->item(4, 5)->clearData();
    model->item(5, 1)->clearData();
    model->item(5, 2)->clearData();
    model->item(5, 3)->clearData();
    model->item(5, 4)->clearData();
    model->item(5, 5)->clearData();
    model->item(6, 1)->clearData();
    model->item(6, 2)->clearData();
    model->item(6, 3)->clearData();
    model->item(6, 4)->clearData();
    model->item(6, 5)->clearData();
    model->item(7, 1)->clearData();//备注
    for (int n = 0; n < model->rowCount(); n++) {
        for (int m = 0; m < model->columnCount(); m++)
        {
            model->item(n, m)->setTextAlignment(Qt::AlignCenter);
        }
    }
    QSqlQuery * ret;
    QString table = "inspection_table";
    QMap<QString, QString> where;
    QStringList keyList;
    QList<QMap<QString, QString>> row;
    QMap <QString, QString> data;
    where["record_ID"] = recordID;
    where["inspection_type"] = "0";
    keyList << "boxianhanliang" << "mianke" << "houdu"
        << "weizhi_1" << "weizhi_2" << "weizhi_3" << "weizhi_4" << "weizhi_5"
        << "lashenqiangdu_1" << "lashenqiangdu_2" << "lashenqiangdu_3" << "lashenqiangdu_4" << "lashenqiangdu_5"
        << "lashenmoliang_1" << "lashenmoliang_2" << "lashenmoliang_3" << "lashenmoliang_4" << "lashenmoliang_5"
        << "remark";
    if (p_sqlOperation->findData(table, where, keyList, row, &ret)) {
        if (row.size() > 0) {
            data = row.at(0);
            model->item(2, 1)->setText(data["boxianhanliang"]);//玻纤含量
            model->item(2, 3)->setText(data["mianke"]);//面克重
            model->item(2, 5)->setText(data["houdu"]);//厚度
            model->item(4, 1)->setText(data["weizhi_1"]);
            model->item(4, 2)->setText(data["weizhi_2"]);
            model->item(4, 3)->setText(data["weizhi_3"]);
            model->item(4, 4)->setText(data["weizhi_4"]);
            model->item(4, 5)->setText(data["weizhi_5"]);
            model->item(5, 1)->setText(data["lashenqiangdu_1"]);
            model->item(5, 2)->setText(data["lashenqiangdu_2"]);
            model->item(5, 3)->setText(data["lashenqiangdu_3"]);
            model->item(5, 4)->setText(data["lashenqiangdu_4"]);
            model->item(5, 5)->setText(data["lashenqiangdu_5"]);
            model->item(6, 1)->setText(data["lashenmoliang_1"]);
            model->item(6, 2)->setText(data["lashenmoliang_2"]);
            model->item(6, 3)->setText(data["lashenmoliang_3"]);
            model->item(6, 4)->setText(data["lashenmoliang_4"]);
            model->item(6, 5)->setText(data["lashenmoliang_5"]);
            model->item(7, 1)->setText(data["remark"]);//备注
        }
        return;
    }
    else
    {
        QMessageBox::warning(nullptr, tr("错误"),
            tr("首检记录查询失败（%1）").arg(ret->lastError().text()), QMessageBox::Ok);
    }

}
void tableViewQualityInspectionRecord::setInspectionEnd(const QString &recordID)
{
    QStandardItemModel *model = static_cast<QStandardItemModel*>(this->model());
    model->item(11, 1)->clearData();//玻纤含量
    model->item(11, 3)->clearData();//面克重
    model->item(11, 5)->clearData();//厚度
    model->item(13, 1)->clearData();
    model->item(13, 2)->clearData();
    model->item(13, 3)->clearData();
    model->item(13, 4)->clearData();
    model->item(13, 5)->clearData();
    model->item(14, 1)->clearData();
    model->item(14, 2)->clearData();
    model->item(14, 3)->clearData();
    model->item(14, 4)->clearData();
    model->item(14, 5)->clearData();
    model->item(15, 1)->clearData();
    model->item(15, 2)->clearData();
    model->item(15, 3)->clearData();
    model->item(15, 4)->clearData();
    model->item(15, 5)->clearData();
    model->item(16, 1)->clearData();

    for (int n = 0; n < model->rowCount(); n++) {
        for (int m = 0; m < model->columnCount(); m++)
        {
            model->item(n, m)->setTextAlignment(Qt::AlignCenter);
        }
    }
    QSqlQuery * ret;
    QString table = "inspection_table";
    QMap<QString, QString> where;
    QStringList keyList;
    QList<QMap<QString, QString>> row;
    QMap <QString, QString> data;
    where["record_ID"] = recordID;
    where["inspection_type"] = "1";
    keyList << "boxianhanliang" << "mianke" << "houdu"
        << "weizhi_1" << "weizhi_2" << "weizhi_3" << "weizhi_4" << "weizhi_5"
        << "lashenqiangdu_1" << "lashenqiangdu_2" << "lashenqiangdu_3" << "lashenqiangdu_4" << "lashenqiangdu_5"
        << "lashenmoliang_1" << "lashenmoliang_2" << "lashenmoliang_3" << "lashenmoliang_4" << "lashenmoliang_5"
        << "remark";
    if (p_sqlOperation->findData(table, where, keyList, row, &ret)) {
        if (row.size() > 0) {
            data = row.at(0);
            model->item(11, 1)->setText(data["boxianhanliang"]);//玻纤含量
            model->item(11, 3)->setText(data["mianke"]);//面克重
            model->item(11, 5)->setText(data["houdu"]);//厚度
            model->item(13, 1)->setText(data["weizhi_1"]);
            model->item(13, 2)->setText(data["weizhi_2"]);
            model->item(13, 3)->setText(data["weizhi_3"]);
            model->item(13, 4)->setText(data["weizhi_4"]);
            model->item(13, 5)->setText(data["weizhi_5"]);
            model->item(14, 1)->setText(data["lashenqiangdu_1"]);
            model->item(14, 2)->setText(data["lashenqiangdu_2"]);
            model->item(14, 3)->setText(data["lashenqiangdu_3"]);
            model->item(14, 4)->setText(data["lashenqiangdu_4"]);
            model->item(14, 5)->setText(data["lashenqiangdu_5"]);
            model->item(15, 1)->setText(data["lashenmoliang_1"]);
            model->item(15, 2)->setText(data["lashenmoliang_2"]);
            model->item(15, 3)->setText(data["lashenmoliang_3"]);
            model->item(15, 4)->setText(data["lashenmoliang_4"]);
            model->item(15, 5)->setText(data["lashenmoliang_5"]);
            model->item(16, 1)->setText(data["remark"]);//备注
        }
        return;
    }
    else
    {
        QMessageBox::warning(nullptr, tr("错误"),
            tr("尾检记录查询失败（%1）").arg(ret->lastError().text()), QMessageBox::Ok);
    }
}
void tableViewQualityInspectionRecord::tableViewClear()
{
    QStandardItemModel *model = static_cast<QStandardItemModel*>(this->model());
    model->item(2, 1)->clearData();//玻纤含量
    model->item(2, 3)->clearData();//面克重
    model->item(2, 5)->clearData();//厚度
    model->item(4, 1)->clearData();
    model->item(4, 2)->clearData();
    model->item(4, 3)->clearData();
    model->item(4, 4)->clearData();
    model->item(4, 5)->clearData();
    model->item(5, 1)->clearData();
    model->item(5, 2)->clearData();
    model->item(5, 3)->clearData();
    model->item(5, 4)->clearData();
    model->item(5, 5)->clearData();
    model->item(6, 1)->clearData();
    model->item(6, 2)->clearData();
    model->item(6, 3)->clearData();
    model->item(6, 4)->clearData();
    model->item(6, 5)->clearData();
    model->item(7, 1)->clearData();//备注
    model->item(11, 1)->clearData();//玻纤含量
    model->item(11, 3)->clearData();//面克重
    model->item(11, 5)->clearData();//厚度
    model->item(13, 1)->clearData();
    model->item(13, 2)->clearData();
    model->item(13, 3)->clearData();
    model->item(13, 4)->clearData();
    model->item(13, 5)->clearData();
    model->item(14, 1)->clearData();
    model->item(14, 2)->clearData();
    model->item(14, 3)->clearData();
    model->item(14, 4)->clearData();
    model->item(14, 5)->clearData();
    model->item(15, 1)->clearData();
    model->item(15, 2)->clearData();
    model->item(15, 3)->clearData();
    model->item(15, 4)->clearData();
    model->item(15, 5)->clearData();
    model->item(16, 1)->clearData();
}