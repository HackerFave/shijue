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
    //list << tr("文件列表")/*<< tr("value")*/;
    //model->setHorizontalHeaderLabels(list);//列表头部


	this->setEditTriggers(QAbstractItemView::NoEditTriggers);
	//this->setSelectionMode(SelectionMode::NoSelection);
	this->setFocusPolicy(Qt::NoFocus);
    QStandardItemModel* model = new QStandardItemModel(this);
    //表头
    model->setHorizontalHeaderLabels(QStringList() << QStringLiteral("项目") << QStringLiteral("描述"));
    model->horizontalHeaderItem(0);
	this->header()->setVisible(false);
    //model->setColumnCount(list.size());//列数
    this->header()->setDefaultSectionSize(100);
    this->header()->setDefaultAlignment(Qt::AlignCenter);//居中
   // this->header()->setVisible(false);
    //关联项目属性改变的信号和槽
    connect(model, &QStandardItemModel::itemChanged, this, &TreeViewBaseModel::SlotTreeItemChanged);
    //connect(model,SIGNAL(itemChanged(QStandardItem*)),this,SLOT(treeItemChanged(QStandardItem*)));




    this->setModel(model);

    this->setColumnWidth(0, 200);//列宽
    //this->setEditTriggers(QTreeView::NoEditTriggers);//不可编辑
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
/// \brief 根据子节点的改变，更改父节点的选择情况
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
            //itemTable =new QStandardItem(icon_map_["table"], QStringLiteral("数据表%1").arg(it.key()));
            model_data->setItem(model_data->indexFromItem(itemProject).row(), 1, new QStandardItem(QStringLiteral("方案名称")));
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
/// \brief 根据子节点的改变，更改父节点的选择情况
/// \param item
///
void TreeViewBaseModel::set_sql_all_table(QMap<QString,QStringList> mapprodata)
{
	QStandardItemModel *model_data = qobject_cast<QStandardItemModel*>(this->model());
	QStandardItem* itemTable = Q_NULLPTR;//表格
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
					
					/*QStandardItem* item = new QStandardItem("列名");
					item->setBackground(QBrush(QColor(35, 35, 35)));
					itemTable->appendRow(item);
					itemTable->setChild(model_data->indexFromItem(item).row(), 1, new QStandardItem("字段名"));*/
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
        //如果条目是存在复选框的，那么就进行下面的操作
        Qt::CheckState state = item->checkState(); //获取当前的选择状态
        if (item->isTristate())
        {
            //如果条目是三态的，说明可以对子目录进行全选和全不选的设置
            if (state != Qt::PartiallyChecked)
            {
                //当前是选中状态，需要对其子项目进行全选
                SlotTreeItemCheckAllChild(item, state == Qt::Checked ? true : false);
            }
        }
        else
        {
            //说明是两态的，两态会对父级的三态有影响
            //判断兄弟节点的情况
            SlotTreeItemCheckChildChanged(item);
        }
    }
}
///
/// \brief 递归设置所有的子项目为全选或全不选状态
/// \param item 当前项目
/// \param check true时为全选，false时全不选
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
/// \brief 测量兄弟节点的情况，如果都选中返回Qt::Checked，都不选中Qt::Unchecked,不完全选中返回Qt::PartiallyChecked
/// \param item
/// \return 如果都选中返回Qt::Checked，都不选中Qt::Unchecked,不完全选中返回Qt::PartiallyChecked
///
Qt::CheckState TreeViewBaseModel::checkSibling(QStandardItem * item)
{
    //先通过父节点获取兄弟节点
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
//获取选中数据
QMap<QString, QStringList> TreeViewBaseModel::getselecttabledata()
{
    table_map_.clear();
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(this->model());
    for (int i = 0; i < model->rowCount(); i++) {
        for (int j = 0; j < model->columnCount(); j++) {
            QStandardItem * item = model->item(i, j);
            if (item != NULL) {
                GetItem(item,1);//获取节点数据
            }

        }
    }
    return table_map_;
}
//选中关联的数据
void TreeViewBaseModel::setCheckedRelated()
{
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(this->model());
    for (int i = 0; i < model->rowCount(); i++) {
        for (int j = 0; j < model->columnCount(); j++) {
            QStandardItem * item = model->item(i, j);
            if (item != NULL) {
                GetItem(item, 2);//获取节点数据
            }
        }
    }
}
//全选中item
void TreeViewBaseModel::setCheckedAll(bool checked)
{
    itemchecked = checked;
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(this->model());
    for (int i = 0; i < model->rowCount(); i++) {
        for (int j = 0; j < model->columnCount(); j++) {
            QStandardItem * item = model->item(i, j);
            if (item != NULL) {
                GetItem(item,0);//获取节点数据
            }          
        }
    }
}
//flag 0(选中) 1(获取数据)
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
            for (int i = 0; i < item->rowCount(); i++)//行
            {
                if (item->child(i)->checkState() != Qt::Unchecked) {//子节点未选中不再循环
                    for (int j = 0; j < item->columnCount(); j++)//列
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
            for (int i = 0; i < item->rowCount(); i++)//行
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