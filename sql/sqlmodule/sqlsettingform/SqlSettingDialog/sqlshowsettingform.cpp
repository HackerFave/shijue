#include "sqlshowsettingform.h"
#include "../../sqlmethod/DatabaseInterface/mrun_sqloperation_define.h"
#include "mrun_messagebox.h"
#include "sqldatashowdialog.h"

QMap<QString, QString> SqlShowSettingForm::fromstdmap(std::map<std::string, std::string> map)
{
    std::map<std::string, std::string>::iterator it;
    QMap<QString, QString> map_q;
    for (it = map.begin(); it != map.end(); it++) {
        it->first;
        map_q[QString::fromLocal8Bit((it->first.data()))] = QString::fromLocal8Bit((it->second.data()));
    }
    return map_q;
}
QStringList SqlShowSettingForm::fromstdlist(std::list<std::string> list)
{
    QStringList list_q;
    std::list<std::string>::iterator it;
    for (it = list.begin(); it != list.end(); it++) {

        list_q.push_back(QString::fromLocal8Bit((*it).data()));
    }
    return list_q;
}

SqlShowSettingForm::SqlShowSettingForm(QWidget *parent)
    : QWidget(parent)
    ,ui(new Ui::SqlShowSettingForm)
{
    ui->setupUi(this);
	
    treeviewmodel_ = new TreeViewBaseModel;//左侧数据库结构展示模型
	//treeviewmodel_->setFixedWidth(436);

    ui->gridLayout_addtreeview->addWidget(treeviewmodel_);
    std::list<std::string> list;
    MRUN_Sql_GetProSqlList(list);
    AddItemsCombox(list);
    //initsql();//初始化数据库
    //工程切换槽函数
    connect(ui->comboBox_pro, SIGNAL(currentTextChanged(const QString &)), this, SLOT(SlotProChange(const QString &)));
    //是否启动关联槽函数
    connect(ui->checkBox_table_contact, SIGNAL(stateChanged(int)), this, SLOT(SlotCheckedContact(int)));
    cleartab();//清除原始tab
    //获取当前工程名
    SlotProChange(ui->comboBox_pro->currentText());
    //隐藏控件
    ui->checkBox_table_contact->setVisible(false);
}

SqlShowSettingForm::~SqlShowSettingForm()
{
}
//清空tab
void SqlShowSettingForm::cleartab()
{
    int curTabCount = ui->tabWidget->count();
    // 4、删除tab页
    while (0 != curTabCount)
    {
        ui->tabWidget->removeTab(0);
        curTabCount = ui->tabWidget->count();
    }
}
//初始化数据库
void SqlShowSettingForm::initsql()
{
    if (MRUN_Sql_InitSqlConnect(ui->comboBox_pro->currentText().toStdString() + "read")) {

    }

}
void SqlShowSettingForm::AddItemsCombox(const std::list<std::string> list)
{
    ui->comboBox_pro->clear();
    std::list<std::string>::const_iterator it;
    for (it = list.cbegin(); it != list.cend(); it++) {
        ui->comboBox_pro->addItem(QString::fromLocal8Bit((*it).data()));
    }
	initComboxColor(ui->comboBox_pro);
}
//合并两条qstringlist
QStringList SqlShowSettingForm::mergelist(QList<QStringList> list)
{
    QStringList mergelist;
    if (list.size() > 1) {
        QStringList list0 = list.at(0);
        QStringList list1 = list.at(1);
        for (int i = 0; i < list0.size(); i++)
        {
            QString str = QString("%1|%2").arg(list0.at(i).simplified()).arg(list1.at(i).simplified());
            mergelist.append(str);
        }
    }
    return mergelist;
}
//工程切换
void SqlShowSettingForm::SlotProChange(const QString &proname)
{
    initsql();
    cleartab();
    ui->checkBox_table_contact->setChecked(false);//初始化不选中关联复选框
    QMap <QString, QMap<QString, QStringList>> alldatamap;//整个数据库结构
    QMap<QString, QStringList> tablemap;//数据库表结构+字段
    QSqlQuery *ret = Q_NULLPTR;
    std::string table = "public_manage_table";//公共表table，存放所有的当前工程相关表
    //QMap<QString, QString> where;//匹配条件
    std::map<std::string, std::string> where;
    //QList<QMap<QString, QString>> data;//查询到的数据(表名)
    std::list<std::map<std::string, std::string>> data;
    //QStringList keyList;
    std::list<std::string> keyList;
    keyList.push_back("tablename");
    where["proname"] = proname.simplified().toStdString();//匹配关于该项目的所有table
    if (!MRUN_Sql_FinDataMapSql(table, where, keyList, data)) {//查询数据库
        setmessage(HINT_FLAG::MRUN_ERROR, "获取数据失败");
    }
    //遍历该项目的所有表
    std::list<std::map<std::string, std::string>>::iterator it;
    for (it = data.begin(); it != data.end(); it++)
        /*for (int i = 0; i < data.size(); i++)*/ {
        //QMap<QString, QString> map = *it;
        std::map<std::string, std::string> map = *it;
        // QMap<QString, QString>::iterator it_child;
        std::map<std::string, std::string>::iterator it_child;
        for (it_child = map.begin(); it_child != map.end(); it_child++) {
            //获取所有表的字段名
            std::list<std::string> listfiled;

            MRUN_Sql_FindTableIdNameSql(it_child->second, "mrun", listfiled);
            //获取所有表的字段描述
            std::list<std::string>  listfiledoper;

            MRUN_Sql_FindFieldOperatorSql(it_child->second, "mrun", listfiledoper);
            QList<QStringList> list;
            list << fromstdlist(listfiled) << fromstdlist(listfiledoper);
            mergelist_table_filed_ = mergelist(list);//合并字段与字段描述list
            //数据库表名+字段
            tablemap.insert(QString::fromLocal8Bit((it_child->second.data())), mergelist_table_filed_);
        }
    }
    tablemap_ = tablemap;
    alldatamap.insert(proname, tablemap);//工程名+表结构
    treeviewmodel_->delTreeViewData();//清空左侧数据库展示界面
    treeviewmodel_->set_sql_all_table(/*alldatamap*/tablemap);//加载数据库结构展示
    SlotCheckedContact(0);//不启动关联
}
//关联的槽函数（关联id卷号+卷号数据+表名）
void SqlShowSettingForm::SlotRelated(const QString &strrelated)
{
    if (!strrelated.isEmpty()) {
        //关联id
        if (strrelated.split("|").at(0).simplified() != relatedid_) {
            return;
        }
        else
        {
            for (int i = 0; i < listtablerelatedname_.size(); i++)
            {
                std::string table;
                //如果关联表名list不等于最高级别表名
                if (listtablerelatedname_.at(i) != relatedlevelfirsttable_) {
                    qobject_cast<SqlDataShowDialog*>(tabformmap_.value(listtablerelatedname_.at(i)))->cleartabledata();
                    table = listtablerelatedname_.at(i).toStdString();//表名
                }
                QSqlQuery *ret = Q_NULLPTR;

                //QMap<QString, QString> where;
                std::map<std::string, std::string> where;
                //QList<QMap<QString, QString>> data;
                std::list<std::map<std::string, std::string>> data;
                //QStringList keyList;
                std::list<std::string> keyList;
                std::list<std::string>  filedlist;//字段
                std::list<std::string>  filedoperlist;//字段说明
                std::map<std::string, std::string> mapwhere;
                //list 字段
                QStringList list = selecttablemap_.value(listtablerelatedname_.at(i));
                for (int i = 0; i < list.size(); i++)
                {
                    QString strlist = list.at(i);
                    if (strlist.split("|").at(0).toInt() == 0) {
                        filedlist.push_back(strlist.split("|").at(1).toStdString());//字段
                    }
                    else if (strlist.split("|").at(0).toInt() == 1)
                    {
                        filedoperlist.push_back(strlist.split("|").at(1).toStdString());//字段描述
                    }
                }
                keyList = filedlist;//字段
                //strrelated 卷号+表名
                where[strrelated.split("|").at(0).simplified().toStdString()] = strrelated.split("|").at(1).simplified().toStdString();//匹配关于该字段的所有table数据
                strrelatedrecordid_ = strrelated.split("|").at(1).simplified();
                if (!table.empty()) {
                    //查询该表记录数条数
                    int sizecount = MRUN_Sql_GetCountSizeSql(table, where);
                    qobject_cast<SqlDataShowDialog*>(tabformmap_.value(QString::fromLocal8Bit(table.data())))
                        ->setlabelinfo(SQL_INFO::Record, tr("当前表数据库记录总数:  %1").arg(sizecount));
                    int count;
                    if (sizecount%DATA_COUNT_PAGE == 0) {
                        count = sizecount / DATA_COUNT_PAGE;
                    }
                    else
                    {
                        count = sizecount / DATA_COUNT_PAGE + 1;
                    }
                    qobject_cast<SqlDataShowDialog*>(tabformmap_.value(QString::fromLocal8Bit(table.data())))
                        ->setmaxvalue(count);
                    qobject_cast<SqlDataShowDialog*>(tabformmap_.value(QString::fromLocal8Bit(table.data())))
                        ->setlabelinfo(SQL_INFO::Page, tr("当前页:  %1/%2").arg(1).arg(count));

                    //QString strwhere = relatedid_ + "|" + strrelated.split("|").at(1).simplified();
                    //listwhere.push_back(string((const char*)strwhere.toLocal8Bit()));
                    mapwhere[string((const char*)relatedid_.toLocal8Bit())] = string((const char*)(strrelated.split("|").at(1).simplified()).toLocal8Bit());
                    if (!MRUN_Sql_FindNoIdSql(table, 0, DATA_COUNT_PAGE, keyList, mapwhere, data))
                    {
                        setmessage(HINT_FLAG::MRUN_ERROR, "获取数据失败");
                    }
                }
                std::list<std::map<std::string, std::string>>::iterator it;
                for (it = data.begin(); it != data.end(); it++) {
                    qobject_cast<SqlDataShowDialog*>(tabformmap_.value(QString::fromStdString(table)))->setsqldatatomodel(fromstdmap(*it), fromstdlist(filedlist));
                    ui->tabWidget->setCurrentWidget(tabformmap_.value(QString::fromStdString(table)));
                    // AddTabWidget(it_tab.key(), it_tab.value());

                }


            }
        }
    }
    return;

}
//启动(关闭)关联复选框
void SqlShowSettingForm::SlotCheckedContact(int checked)
{
    cleartab();
    QMap<QString, QStringList>::iterator it_tab;
    if (!checked) {
        treeviewmodel_->setCheckedAll(true);//全选展示
        QMap<QString, QStringList> selecttablemap;//选中的展示数据
        selecttablemap = treeviewmodel_->getselecttabledata();//获取选中的数据（表名+字段list）
        for (it_tab = selecttablemap.begin(); it_tab != selecttablemap.end(); it_tab++) {
            AddTabWidget(it_tab.key(), it_tab.value());
        }
    }
    else //启动选中关联的表
    {
        treeviewmodel_->setCheckedAll(false);
        cleartab();

        QSqlQuery *ret = Q_NULLPTR;
        std::string table = "public_manage_table";//公共表table
        //QMap<QString, QString> where;
        std::map<std::string, std::string> where;
        //QList<QMap<QString, QString>> data;
        std::list<std::map<std::string, std::string>> data;
        // QStringList keyList;
        std::list<std::string> keyList;
        //关联表名<<是否关联<<关联id<<关联等级
        //keyList << "tablename"<<"related"<<"relatedid"<<"relatedlevel";
        keyList.push_back("tablename");
        keyList.push_back("related");
        keyList.push_back("relatedid");
        keyList.push_back("relatedlevel");
        where["proname"] = ui->comboBox_pro->currentText().simplified().toStdString();//匹配关于该项目的所有table
        if (!MRUN_Sql_FinDataMapSql(table, where, keyList, data)) {
            setmessage(HINT_FLAG::MRUN_ERROR, "获取数据表名失败");
        }
        //遍历该项目的所有表 获取有关联的表
        QStringList listtablerelatedname;//表list
        QString relatedlevelfirsttable;//关联级别最高的表
        QString relatedid;//关联级别最高的表字段id
        //for (int i = 0; i < data.size(); i++) 
        std::list<std::map<std::string, std::string>>::iterator it;
        for (it = data.begin(); it != data.end(); it++)
        {
            std::map<std::string, std::string> map = *it;
            if (QString::fromStdString((map["related"])).toInt() == 1) { //1关联 0不关联              
                QString strlist = QString::fromStdString((map["tablename"])).simplified() + "|" +
                    QString::fromStdString((map["relatedid"])).simplified()
                    + "|" + QString::fromStdString((map["relatedlevel"])).simplified();
                related_table_list_.append(strlist);//关联表名+关联字段+关联级别
                listtablerelatedname.append(QString::fromStdString((map["tablename"])).simplified());//所有关联表名
                relatedid = QString::fromStdString((map["relatedid"])).simplified();
                relatedid_ = relatedid;//所有关联id
            }
            if (QString::fromStdString((map["relatedlevel"])).toInt() == 1) {//关联等级1级最高
                relatedlevelfirsttable = QString::fromStdString((map["tablename"])).simplified();
                relatedlevelfirsttable_ = relatedlevelfirsttable;//关联级别最高表名
            }
        }
        listtablerelatedname_ = listtablerelatedname;
        //创建关联表名
        treeviewmodel_->setrelatedtablelist(listtablerelatedname);
        //选中关联数据
        treeviewmodel_->setCheckedRelated();

        QMap<QString, QStringList>::iterator it_tab;
        QMap<QString, QStringList> selecttablemap;//选中的展示数据
        selecttablemap = treeviewmodel_->getselecttabledata();//关联表（表名+字段list）
        selecttablemap_ = selecttablemap; //关联表（表名 + 字段list）
        for (it_tab = selecttablemap.begin(); it_tab != selecttablemap.end(); it_tab++) {
            AddTabWidget(it_tab.key(), it_tab.value());
        }
        //设置当前选中界面
        ui->tabWidget->setCurrentWidget(tabformmap_.value(relatedlevelfirsttable));
    }
}
//点击数据展示
void SqlShowSettingForm::on_toolButton_showdata_clicked()
{
    cleartab();
    QMap<QString, QStringList>::iterator it_tab;
    QMap<QString, QStringList> selecttablemap;//选中的展示数据(表名+字段)
    selecttablemap = treeviewmodel_->getselecttabledata();
    for (it_tab = selecttablemap.begin(); it_tab != selecttablemap.end(); it_tab++) {
        AddTabWidget(it_tab.key(), it_tab.value());
    }

}
//head(表名) list (字段名+字段说明)
void SqlShowSettingForm::AddTabWidget(QString head, QStringList list)
{
    std::list<std::string> filedlist;//字段
    std::list<std::string> filedoperlist;//字段说明
    for (int i = 0; i < list.size(); i++)
    {
        QString strlist = list.at(i);
        if (strlist.split("|").at(0).toInt() == 0) {
            filedlist.push_back(strlist.split("|").at(1).toStdString());
        }
        else if (strlist.split("|").at(0).toInt() == 1)
        {
            filedoperlist.push_back(string((const char *)strlist.split("|").at(1).toLocal8Bit()));
        }
    }
    SqlDataShowDialog * form = new SqlDataShowDialog;//右侧数据显示界面
    //数据库表格之间关联槽
    QObject::connect(form, SIGNAL(signalChangeValue(int)), this, SLOT(SlotChangePageValue(int)));
    form->setObjectName(head);
    ui->tabWidget->addTab(form, head);
    //初始化每一个数据库表格展示界面
    form->initshowform(fromstdlist(filedoperlist), head);
    //tabformmap_存放每一个数据库展示界面指针
    tabformmap_.insert(form->objectName(), form);
    //QList<QMap<QString, QString>> row;
    std::list<std::map<std::string, std::string>> row;
    QSqlQuery * ret = Q_NULLPTR;

    //QMap<QString, QString> where;
std:map < std::string, std::string > where;

    std::map<std::string, std::string>  mapwhere;
    //查询该表记录数条数
    int sizecount = MRUN_Sql_GetCountSizeSql(head.toStdString(), where);
    qobject_cast<SqlDataShowDialog*>(tabformmap_.value(head))
        ->setlabelinfo(SQL_INFO::Record, tr("当前表数据库记录总数:  %1").arg(sizecount));
    int count;
    if (sizecount%DATA_COUNT_PAGE == 0) {
        count = sizecount / DATA_COUNT_PAGE;
    }
    else
    {
        count = sizecount / DATA_COUNT_PAGE + 1;
    }
    form->setmaxvalue(count);
    qobject_cast<SqlDataShowDialog*>(tabformmap_.value(head))
        ->setlabelinfo(SQL_INFO::Page, tr("当前页:  %1/%2").arg(1).arg(count));
    if (sizecount > DATA_COUNT_PAGE) {//记录总数大于该页总数

        MRUN_Sql_FindNoIdSql(
            head.toStdString(),
            (qobject_cast<SqlDataShowDialog*>(tabformmap_.value(head))->getspinboxvalue() - 1)*DATA_COUNT_PAGE,
            DATA_COUNT_PAGE,
            filedlist,
            mapwhere,
            row);
        std::list<std::map<std::string, std::string>>::iterator it;
        for (it = row.begin(); it != row.end(); it++) {
            form->setsqldatatomodel(fromstdmap(*it), fromstdlist(filedlist));
        }
    }
    else//记录总数小于该页总数
    {
        MRUN_Sql_FindNoIdSql(head.toStdString(),
            (qobject_cast<SqlDataShowDialog*>(tabformmap_.value(head))->getspinboxvalue() - 1)*DATA_COUNT_PAGE,
            DATA_COUNT_PAGE
            , filedlist, mapwhere, row);
        std::list<std::map<std::string, std::string>>::iterator it;
		for (int i = 0; i < DATA_COUNT_PAGE - sizecount; i++) {
			std::map<std::string, std::string> mapchild;
			mapchild["1"] = "";
			row.push_back(mapchild);
		}

        for (it = row.begin(); it != row.end(); it++) {
            form->setsqldatatomodel(fromstdmap(*it), fromstdlist(filedlist));
        }
		
    }

    if (ui->checkBox_table_contact->isChecked()) {//当前启动关联才响应槽函数
        QObject::connect(form, SIGNAL(signalItemText(const QString &)), this, SLOT(SlotRelated(const QString &)));
    }
}
void SqlShowSettingForm::SlotChangePageValue(int value)
{
    QMap<QString, QStringList> map = treeviewmodel_->getselecttabledata();//当前勾选数据
    QString head = ui->tabWidget->currentWidget()->objectName();//表名
    QStringList list = map.value(head);//当前表
    std::list<std::string> filedlist;//字段
    std::list<std::string> filedoperlist;//字段说明
    for (int i = 0; i < list.size(); i++)
    {
        QString strlist = list.at(i);
        if (strlist.split("|").at(0).toInt() == 0) {
            filedlist.push_back(strlist.split("|").at(1).toStdString());
        }
        else if (strlist.split("|").at(0).toInt() == 1)
        {
            filedoperlist.push_back(strlist.split("|").at(1).toStdString());
        }
    }
    qobject_cast<SqlDataShowDialog*>(tabformmap_.value(head))->cleartabledata();//情况该表数据
    std::list<std::map<std::string, std::string>> row;//查询返回数据
    QSqlQuery * ret = Q_NULLPTR;

    std::map<std::string, std::string> where;//查询记录数匹配
    std::map<std::string, std::string> mapwhere;//查询数据匹配
    //&&head!= relatedlevelfirsttable_


    //选中关联
    if (ui->checkBox_table_contact->isChecked()) {

        where[string((const char*)relatedid_.toLocal8Bit())] = string((const char*)strrelatedrecordid_.toLocal8Bit());
        if (head == relatedlevelfirsttable_) {//当前表名不等于主表
            //listwhere.clear();
            where.clear();
        }
        //查询该表记录数条数
        int sizecount = MRUN_Sql_GetCountSizeSql(head.toStdString(), where);
        int count;
        if (sizecount%DATA_COUNT_PAGE == 0) {
            count = sizecount / DATA_COUNT_PAGE;
        }
        else
        {
            count = sizecount / DATA_COUNT_PAGE + 1;
        }
        qobject_cast<SqlDataShowDialog*>(tabformmap_.value(head))
            ->setlabelinfo(SQL_INFO::Page, tr("当前页:  %1/%2").arg(value).arg(count));
        qobject_cast<SqlDataShowDialog*>(tabformmap_.value(head))->setmaxvalue(count);


        QString strwhere = relatedid_ + "|" + strrelatedrecordid_;
        mapwhere[string((const char*)strwhere.toLocal8Bit())] = string((const char*)strrelatedrecordid_.toLocal8Bit());
        if (head == relatedlevelfirsttable_) {//当前表名不等于主表
            mapwhere.clear();
            //where.clear();
        }
        if (sizecount > DATA_COUNT_PAGE) {

            MRUN_Sql_FindNoIdSql(
                head.toStdString(),
                (qobject_cast<SqlDataShowDialog*>(tabformmap_.value(head))->getspinboxvalue() - 1)*DATA_COUNT_PAGE,
                DATA_COUNT_PAGE,
                filedlist,
                mapwhere,
                row);
            std::list<std::map<std::string, std::string>>::iterator it;
            for (it = row.begin(); it != row.end(); it++) {
                qobject_cast<SqlDataShowDialog*>(tabformmap_.value(head))->setsqldatatomodel(fromstdmap(*it), fromstdlist(filedlist));
            }
        }
        else
        {
            MRUN_Sql_FindNoIdSql(head.toStdString(),
                (qobject_cast<SqlDataShowDialog*>(tabformmap_.value(head))->getspinboxvalue() - 1)*DATA_COUNT_PAGE,
                DATA_COUNT_PAGE
                , filedlist, mapwhere, row);
            std::list<std::map<std::string, std::string>>::iterator it;
            for (it = row.begin(); it != row.end(); it++) {
                qobject_cast<SqlDataShowDialog*>(tabformmap_.value(head))->setsqldatatomodel(fromstdmap(*it), fromstdlist(filedlist));
            }
        }
    }
    else
    {
        //查询该表记录数条数
        int sizecount = MRUN_Sql_GetCountSizeSql(head.toStdString(), where);
        int count;
        if (sizecount%DATA_COUNT_PAGE == 0) {
            count = sizecount / DATA_COUNT_PAGE;
        }
        else
        {
            count = sizecount / DATA_COUNT_PAGE + 1;
        }
        qobject_cast<SqlDataShowDialog*>(tabformmap_.value(head))
            ->setlabelinfo(SQL_INFO::Page, tr("当前页:  %1/%2").arg(value).arg(count));
        qobject_cast<SqlDataShowDialog*>(tabformmap_.value(head))->setmaxvalue(count);


        QString strwhere = "";
        //mapwhere[string((const char*)strwhere.toLocal8Bit())] = string((const char*)strrelatedrecordid_.toLocal8Bit());
        mapwhere.clear();
        if (sizecount > DATA_COUNT_PAGE) {

            MRUN_Sql_FindNoIdSql(
                head.toStdString(),
                (qobject_cast<SqlDataShowDialog*>(tabformmap_.value(head))->getspinboxvalue() - 1)*DATA_COUNT_PAGE,
                DATA_COUNT_PAGE,
                filedlist,
                mapwhere,
                row);
            std::list<std::map<std::string, std::string>>::iterator it;
            for (it = row.begin(); it != row.end(); it++) {
                qobject_cast<SqlDataShowDialog*>(tabformmap_.value(head))->setsqldatatomodel(fromstdmap(*it), fromstdlist(filedlist));
            }
        }
        else
        {
            MRUN_Sql_FindNoIdSql(head.toStdString(),
                (qobject_cast<SqlDataShowDialog*>(tabformmap_.value(head))->getspinboxvalue() - 1)*DATA_COUNT_PAGE,
                DATA_COUNT_PAGE
                , filedlist, mapwhere, row);
            std::list<std::map<std::string, std::string>>::iterator it;
            for (it = row.begin(); it != row.end(); it++) {
                qobject_cast<SqlDataShowDialog*>(tabformmap_.value(head))->setsqldatatomodel(fromstdmap(*it), fromstdlist(filedlist));
            }
        }
    }
}
//下拉框样式
void SqlShowSettingForm::initComboxColor(QComboBox * comBox)
{
	QStandardItemModel *pItemModel = qobject_cast<QStandardItemModel*>(comBox->model());
	//修改某项文本
	for (int i = 0; i < comBox->count(); i++)
	{
		pItemModel->item(i)->setForeground(QColor(255, 255, 255));					//修改某项文本颜色
		pItemModel->item(i)->setBackground(QColor(29, 30, 31));						//修改某项背景颜色    
		pItemModel->item(i)->setSizeHint(QSize(180, 40));
		pItemModel->item(i)->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);    //修改某项文本对齐方式
	}
}
void SqlShowSettingForm::on_toolButton_left_clicked(bool checked)//左侧收缩按钮
{
	ui->groupBox->setVisible(false);
	ui->toolButton_right->setEnabled(true);
	ui->toolButton_left->setVisible(false);
	ui->line_2->setStyleSheet("background-color:transparent");
	ui->line_3->setStyleSheet("background-color:transparent");
}
void SqlShowSettingForm::on_toolButton_right_clicked(bool checked)//左侧收缩按钮
{
	//if (checked) {
		ui->groupBox->setVisible(true);
		ui->toolButton_right->setEnabled(false);
		ui->toolButton_left->setVisible(true);
		ui->line_2->setStyleSheet("background-color:#000000");
		ui->line_3->setStyleSheet("background-color:#000000");
	//}
}