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
    QStandardItemModel* item_model = new QStandardItemModel(map.size(), 1);//��/��
    this->setModel(item_model);
    //���ñ��ĵ�ԪΪֻ�����ԣ������ܱ༭        
    //this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setEditTriggers(QAbstractItemView::AllEditTriggers);
    //���������QTableView��ʹ���Ҽ��˵��������ø�����        
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setWordWrap(true);//���ֻ���
   // connect(this, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(slotDoubleClicked(const QModelIndex &)));
    this->horizontalHeader()->hide();                       // ����ˮƽ��ͷ
    this->verticalHeader()->hide();                         // ���ش�ֱ��ͷ
    //ui.tableView->horizontalHeader()->setStretchLastSection(true);  // �������һ��
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
     item_model->setHorizontalHeaderLabels(column);                // ����ˮƽ��ͷ��ǩ
     item_model->setVerticalHeaderLabels(row);                     // ���ô�ֱ��ͷ��ǩ
     */

     // ���item��model
    for (int i = 0; i < this->model()->rowCount(); ++i) {
        for (int j = 0; j < this->model()->columnCount(); ++j)
            item_model->setItem(i, j, new QStandardItem());
    }

    QStandardItemModel *model = static_cast<QStandardItemModel*>(this->model());
    for (int i = 0; i < model->rowCount(); i++) {
        for (int j = 0; j < model->columnCount(); j++) {
            QFont font;
            font.setFamily("Times");//����
            //font.setPixelSize(25);//�������ش�С
            font.setPointSize(10);//���ִ�С
           // font.setUnderline(true);//�»���
            //font.setStrikeOut(true);//�л���
            //font.setOverline(true);//�ϻ���
            //font.setItalic(true);//б��
            font.setBold(true);//����
            // font.setStyle(QFont::StyleOblique);
            // font.setCapitalization(QFont::Capitalize);//����ĸ��д
           // model->item(i, j)->setTextAlignment(Qt::AlignCenter);
            //model->item(i, j)->setFont(font);
            proListItem * itemForm = new proListItem(this,QString::number(i));
            itemForm->setProName(listPro.at(i));
            this->setIndexWidget(model->index(i, 0), itemForm);
            connect(itemForm, SIGNAL(signalStatus(bool,QString,QString)), this, SLOT(slotStatus(bool,QString,QString)));
            connect(itemForm, SIGNAL(signalSelect(bool,QString,QString)), this, SLOT(slotSelect(bool,QString,QString)));
        }
    }
  
    this->setShowGrid(true);                               // ����������
    this->setFocusPolicy(Qt::StrongFocus);                      // ȥ����ǰCell�ܱ����߿�
    this->setAlternatingRowColors(true);                    // ����������ɫ
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
