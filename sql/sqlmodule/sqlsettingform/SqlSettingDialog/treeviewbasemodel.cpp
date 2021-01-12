#include "treeviewbasemodel.h"
#include "ui_treeviewbasemodel.h"
#include <qdebug.h>
#pragma execution_character_set("utf-8")
TreeViewBaseModel::TreeViewBaseModel(QDialog *parent)
    : QTreeView(parent)
    , ui(new Ui::TreeViewBaseModel)
{
    ui->setupUi(this);
    init_icon_map();
    InitTreeView();
	
}

TreeViewBaseModel::~TreeViewBaseModel()
{
}
void TreeViewBaseModel::InitTreeView()
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    //this->setFixedWidth(346);
    //QStandardItemModel *model = new QStandardItemModel(this);
   // QStringList list;
    //list << tr("�ļ��б�")/*<< tr("value")*/;
    //model->setHorizontalHeaderLabels(list);//�б�ͷ��


	this->setEditTriggers(QAbstractItemView::NoEditTriggers);
	//this->setSelectionMode(SelectionMode::NoSelection);
	this->setFocusPolicy(Qt::NoFocus);
    QStandardItemModel* model = new QStandardItemModel(this);
    //��ͷ
    model->setHorizontalHeaderLabels(QStringList() << QStringLiteral("��Ŀ") << QStringLiteral("����"));
    model->horizontalHeaderItem(0);
	this->header()->setVisible(false);
    //model->setColumnCount(list.size());//����
    this->header()->setDefaultSectionSize(100);
    this->header()->setDefaultAlignment(Qt::AlignCenter);//����
   // this->header()->setVisible(false);
    //������Ŀ���Ըı���źźͲ�
    connect(model, &QStandardItemModel::itemChanged, this, &TreeViewBaseModel::SlotTreeItemChanged);
    //connect(model,SIGNAL(itemChanged(QStandardItem*)),this,SLOT(treeItemChanged(QStandardItem*)));




    this->setModel(model);

    this->setColumnWidth(0, 200);//�п�
    //this->setEditTriggers(QTreeView::NoEditTriggers);//���ɱ༭
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    /*this->setStyleSheet(tr("QTreeView::branch:open:has-children:!has-siblings,"
        "QTreeView::branch:open:has-children:has-siblings {image: url(%1/icons/openT.png);}"
        "QTreeView::branch:has-children:!has-siblings:closed,"
        "QTreeView::branch:closed:has-children:has-siblings {image: url(%1/icons/closeT.png);}"
        "QTreeView::item:hover {background-color:black;}"
        "QTreeView::item:selected {background-color:#1E90FF;border:1px solid #08347F;}"
        "QToolTip{color: black}")
        .arg(qApp->applicationDirPath()));*/
        // this->setItemsExpandable(false);
}
void TreeViewBaseModel::init_icon_map()
{

    icon_map_["project"] = QIcon(tr("%1/icons/pro.png").arg(qApp->applicationDirPath()));
    icon_map_["table"] = QIcon(tr("%1/icons/table.png").arg(qApp->applicationDirPath()));
    icon_map_["describe"] = QIcon(tr("%1/icons/oper.png").arg(qApp->applicationDirPath()));
    // icon_map_["treeItem_group"] = QIcon((":/treeItemIcon/res_treeItemIcon/group.png"));
    // icon_map_["treeItem_channel"] = QIcon((":/treeItemIcon/res_treeItemIcon/channel.png"));
}
///
/// \brief �����ӽڵ�ĸı䣬���ĸ��ڵ��ѡ�����
/// \param item
///
void TreeViewBaseModel::set_sql_all_table(QMap<QString, QMap<QString, /*QMap<QString, QString>*/QStringList>> mapprodata)
{
    QStandardItemModel *model_data = qobject_cast<QStandardItemModel*>(this->model());
    QStandardItem* itemProject = Q_NULLPTR;
    QStandardItem* itemTable = Q_NULLPTR;
    QMap<QString, QMap<QString, /*QMap<QString, QString>*/QStringList>>::iterator it_prent;
    QMap<QString, /*QMap<QString, QString>*/QStringList>::iterator it;
    //QMap<QString, QString>::iterator it_child; QStringList 

    for (it_prent = mapprodata.begin(); it_prent != mapprodata.end(); it_prent++)
    {
        itemProject = new QStandardItem(icon_map_[QStringLiteral("project")], QStringLiteral("%1").arg(it_prent.key()));
		//itemProject->setEditable(false);
        model_data->appendRow(itemProject);
        for (it = it_prent.value().begin(); it != it_prent.value().end(); it++) {
            //itemTable =new QStandardItem(icon_map_["table"], QStringLiteral("���ݱ�%1").arg(it.key()));
            model_data->setItem(model_data->indexFromItem(itemProject).row(), 1, new QStandardItem(QStringLiteral("��������")));
            itemTable = new QStandardItem(icon_map_["table"], QStringLiteral("%1").arg(it.key()));
            itemTable->setCheckable(true);
            itemTable->setTristate(true);
			//itemTable->setEditable(false);
            QStandardItem* itemFolderDes = new QStandardItem(icon_map_["describe"], QStringLiteral("%1").arg(it.key()));
            itemProject->appendRow(itemTable);
            itemProject->setChild(itemTable->index().row(), 1, itemFolderDes);
            //for (it_child = it.value().begin(); it_child != it.value().end(); it_child++) {
            for(int i=0;i<it.value().size();i++){
                QStandardItem* item = new QStandardItem(icon_map_[QStringLiteral("describe")], QStringLiteral("%1").arg(it.value().at(i).split("|").at(0)));
                item->setCheckable(true);
				//item->setEditable(false);
                itemTable->appendRow(item);
                itemTable->setChild(model_data->indexFromItem(item).row(), 1, new QStandardItem(QStringLiteral("%1").arg(it.value().at(i).split("|").at(1))));
            }
        }
    }
    this->expandAll();
}
/// \brief �����ӽڵ�ĸı䣬���ĸ��ڵ��ѡ�����
/// \param item
///
void TreeViewBaseModel::set_sql_all_table(QMap<QString,QStringList> mapprodata)
{
	QStandardItemModel *model_data = qobject_cast<QStandardItemModel*>(this->model());
	QStandardItem* itemTable = Q_NULLPTR;//���
	QMap<QString, QStringList>::iterator it;
		for (it = mapprodata.begin(); it != mapprodata.end(); it++) {
			itemTable = new QStandardItem(icon_map_["table"], QStringLiteral("%1").arg(it.key()));
			itemTable->setCheckable(true);
			itemTable->setTristate(true);
			//QStandardItem *itemp = new QStandardItem;
			//model_data->setItem(model_data->indexFromItem(itemTable).row(), 1, itemp);
			itemTable->setBackground(QBrush(QColor(48, 48, 48)));
			itemTable->setSelectable(false);
			itemTable->setToolTip(it.key());
			QList<QStandardItem*> listitem;
			listitem.push_back(itemTable);
			QStandardItem *item1 = new QStandardItem;
			item1->setBackground(QBrush(QColor(48, 48, 48)));
			item1->setSelectable(false);
			listitem.push_back(item1);
			model_data->appendRow(listitem);
			for (int i = 0; i < it.value().size()+1; i++) {
				if (i == 0) {
					
					/*QStandardItem* item = new QStandardItem("����");
					item->setBackground(QBrush(QColor(35, 35, 35)));
					itemTable->appendRow(item);
					itemTable->setChild(model_data->indexFromItem(item).row(), 1, new QStandardItem("�ֶ���"));*/
					continue;
				}
				QStandardItem* item = new QStandardItem(icon_map_[QStringLiteral("describe")], QStringLiteral("%1").arg(it.value().at(i-1).split("|").at(0)));
				item->setCheckable(true);
				item->setBackground(QBrush(QColor(35, 35, 35)));
				item->setSelectable(false);
				item->setToolTip(it.value().at(i - 1).split("|").at(0));
				itemTable->appendRow(item);
				QStandardItem *itemchild = new QStandardItem(QStringLiteral("%1").arg(it.value().at(i - 1).split("|").at(1)));
				itemchild->setToolTip(it.value().at(i - 1).split("|").at(1));
				itemTable->setChild(model_data->indexFromItem(item).row(), 1, itemchild);
			}
		}
	this->expandAll();
}
void TreeViewBaseModel::SlotTreeItemCheckChildChanged(QStandardItem * item)
{
    if (nullptr == item)
        return;
    Qt::CheckState siblingState = checkSibling(item);
    QStandardItem * parentItem = item->parent();
    if (nullptr == parentItem)
        return;
    if (Qt::PartiallyChecked == siblingState)
    {
        if (parentItem->isCheckable() && parentItem->isTristate())
            parentItem->setCheckState(Qt::PartiallyChecked);
    }
    else if (Qt::Checked == siblingState)
    {
        if (parentItem->isCheckable())
            parentItem->setCheckState(Qt::Checked);
    }
    else
    {
        if (parentItem->isCheckable())
            parentItem->setCheckState(Qt::Unchecked);
    }
    SlotTreeItemCheckChildChanged(parentItem);
}

void TreeViewBaseModel::SlotTreeItemChanged(QStandardItem * item)
{
    if (item == nullptr)
        return;
    if (item->isCheckable())
    {
        //�����Ŀ�Ǵ��ڸ�ѡ��ģ���ô�ͽ�������Ĳ���
        Qt::CheckState state = item->checkState(); //��ȡ��ǰ��ѡ��״̬
        if (item->isTristate())
        {
            //�����Ŀ����̬�ģ�˵�����Զ���Ŀ¼����ȫѡ��ȫ��ѡ������
            if (state != Qt::PartiallyChecked)
            {
                //��ǰ��ѡ��״̬����Ҫ��������Ŀ����ȫѡ
                SlotTreeItemCheckAllChild(item, state == Qt::Checked ? true : false);
            }
        }
        else
        {
            //˵������̬�ģ���̬��Ը�������̬��Ӱ��
            //�ж��ֵܽڵ�����
            SlotTreeItemCheckChildChanged(item);
        }
    }
}
///
/// \brief �ݹ��������е�����ĿΪȫѡ��ȫ��ѡ״̬
/// \param item ��ǰ��Ŀ
/// \param check trueʱΪȫѡ��falseʱȫ��ѡ
///
void TreeViewBaseModel::SlotTreeItemCheckAllChild(QStandardItem * item, bool check)
{
    if (item == nullptr)
        return;
    int rowCount = item->rowCount();
    for (int i = 0; i < rowCount; ++i)
    {
        QStandardItem* childItems = item->child(i);
        SlotTreeItemCheckAllChildRecursion(childItems, check);
    }
    if (item->isCheckable())
        item->setCheckState(check ? Qt::Checked : Qt::Unchecked);
}
void TreeViewBaseModel::SlotTreeItemCheckAllChildRecursion(QStandardItem * item, bool check)
{
    if (item == nullptr)
        return;
    int rowCount = item->rowCount();
    for (int i = 0; i < rowCount; ++i)
    {
        QStandardItem* childItems = item->child(i);
        SlotTreeItemCheckAllChildRecursion(childItems, check);
    }
    if (item->isCheckable())
        item->setCheckState(check ? Qt::Checked : Qt::Unchecked);
}
///
/// \brief �����ֵܽڵ������������ѡ�з���Qt::Checked������ѡ��Qt::Unchecked,����ȫѡ�з���Qt::PartiallyChecked
/// \param item
/// \return �����ѡ�з���Qt::Checked������ѡ��Qt::Unchecked,����ȫѡ�з���Qt::PartiallyChecked
///
Qt::CheckState TreeViewBaseModel::checkSibling(QStandardItem * item)
{
    //��ͨ�����ڵ��ȡ�ֵܽڵ�
    QStandardItem * parent = item->parent();
    if (nullptr == parent)
        return item->checkState();
    int brotherCount = parent->rowCount();
    int checkedCount(0), unCheckedCount(0);
    Qt::CheckState state;
    for (int i = 0; i < brotherCount; ++i)
    {
        QStandardItem* siblingItem = parent->child(i);
        state = siblingItem->checkState();
        if (Qt::PartiallyChecked == state)
            return Qt::PartiallyChecked;
        else if (Qt::Unchecked == state)
            ++unCheckedCount;
        else
            ++checkedCount;
        if (checkedCount > 0 && unCheckedCount > 0)
            return Qt::PartiallyChecked;
    }
    if (unCheckedCount > 0)
        return Qt::Unchecked;
    return Qt::Checked;
}
void TreeViewBaseModel::delTreeViewData()
{
	
	int count = this->model()->rowCount();
    for (int i = 0; i < count; i++) {
        this->model()->removeRow(0);
    }
}
//��ȡѡ������
QMap<QString, QStringList> TreeViewBaseModel::getselecttabledata()
{
    table_map_.clear();
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(this->model());
    for (int i = 0; i < model->rowCount(); i++) {
        for (int j = 0; j < model->columnCount(); j++) {
            QStandardItem * item = model->item(i, j);
            if (item != NULL) {
                GetItem(item,1);//��ȡ�ڵ�����
            }

        }
    }
    return table_map_;
}
//ѡ�й���������
void TreeViewBaseModel::setCheckedRelated()
{
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(this->model());
    for (int i = 0; i < model->rowCount(); i++) {
        for (int j = 0; j < model->columnCount(); j++) {
            QStandardItem * item = model->item(i, j);
            if (item != NULL) {
                GetItem(item, 2);//��ȡ�ڵ�����
            }
        }
    }
}
//ȫѡ��item
void TreeViewBaseModel::setCheckedAll(bool checked)
{
    itemchecked = checked;
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(this->model());
    for (int i = 0; i < model->rowCount(); i++) {
        for (int j = 0; j < model->columnCount(); j++) {
            QStandardItem * item = model->item(i, j);
            if (item != NULL) {
                GetItem(item,0);//��ȡ�ڵ�����
            }          
        }
    }
}
//flag 0(ѡ��) 1(��ȡ����)
void TreeViewBaseModel::GetItem(QStandardItem *item, int flag)
{
    Q_ASSERT(item);
    switch (flag)
    {
    case 0: {
        if (item->hasChildren())
        {
           for (int i = 0; i < item->rowCount(); i++)
            {
                QStandardItem * childitem = item->child(i);
                childitem->setCheckState(itemchecked ==true ? Qt::Checked : Qt::Unchecked);
                GetItem(childitem,0);
            }
        }
    }
            break;
    case 1: {
        if (item->hasChildren())
        {
            QStringList list;
            for (int i = 0; i < item->rowCount(); i++)//��
            {
                if (item->child(i)->checkState() != Qt::Unchecked) {//�ӽڵ�δѡ�в���ѭ��
                    for (int j = 0; j < item->columnCount(); j++)//��
                    {
                        QStandardItem * childitem = item->child(i, j);
                        //if (childitem->checkState() == Qt::Checked) {
                        QString strList;
                          if (childitem->index().column() == 0) {
                              strList += QString("%1|%2").arg(childitem->index().column()).arg(childitem->text().simplified());
                             }
                          else if(childitem->index().column() == 1)
                          {
                              strList += QString("%1|%2").arg(childitem->index().column()).arg(childitem->text().simplified());
                          }
                            list.append(strList);
                            GetItem(childitem, 1);
                        //}
                       
                    }
                }
            }
            if (item->checkState() != Qt::Unchecked) {
                table_map_.insert(item->text().simplified(), list);
            }          
        }
    }
            break;
    case 2: {
        if (item->hasChildren())
        {
            QStringList list;
            for (int i = 0; i < item->rowCount(); i++)//��
            {
                QStandardItem * childitem = item->child(i);
                if (related_table_name_.contains(childitem->text().simplified())) {
                    childitem->setCheckState(Qt::Checked);
                }
                GetItem(childitem, 1);
               
            }
        }
    }
            break;
    default:
        break;
    }
    
    
}
void TreeViewBaseModel::setrelatedtablelist(const QStringList &list)
{
    related_table_name_ = list;
}
/*void TreeViewBaseModel::GetNode()
{
    for (int i = 0; i < model->rowCount(); i++)
    {
        QStandardItem *item = model->item(i);

        qDebug() << "item = " << item->text();
        GetItem(model->item(i));
    }
}*/