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
	
    treeviewmodel_ = new TreeViewBaseModel;//������ݿ�ṹչʾģ��
	//treeviewmodel_->setFixedWidth(436);

    ui->gridLayout_addtreeview->addWidget(treeviewmodel_);
    std::list<std::string> list;
    MRUN_Sql_GetProSqlList(list);
    AddItemsCombox(list);
    //initsql();//��ʼ�����ݿ�
    //�����л��ۺ���
    connect(ui->comboBox_pro, SIGNAL(currentTextChanged(const QString &)), this, SLOT(SlotProChange(const QString &)));
    //�Ƿ����������ۺ���
    connect(ui->checkBox_table_contact, SIGNAL(stateChanged(int)), this, SLOT(SlotCheckedContact(int)));
    cleartab();//���ԭʼtab
    //��ȡ��ǰ������
    SlotProChange(ui->comboBox_pro->currentText());
    //���ؿؼ�
    ui->checkBox_table_contact->setVisible(false);
}

SqlShowSettingForm::~SqlShowSettingForm()
{
}
//���tab
void SqlShowSettingForm::cleartab()
{
    int curTabCount = ui->tabWidget->count();
    // 4��ɾ��tabҳ
    while (0 != curTabCount)
    {
        ui->tabWidget->removeTab(0);
        curTabCount = ui->tabWidget->count();
    }
}
//��ʼ�����ݿ�
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
//�ϲ�����qstringlist
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
//�����л�
void SqlShowSettingForm::SlotProChange(const QString &proname)
{
    initsql();
    cleartab();
    ui->checkBox_table_contact->setChecked(false);//��ʼ����ѡ�й�����ѡ��
    QMap <QString, QMap<QString, QStringList>> alldatamap;//�������ݿ�ṹ
    QMap<QString, QStringList> tablemap;//���ݿ��ṹ+�ֶ�
    QSqlQuery *ret = Q_NULLPTR;
    std::string table = "public_manage_table";//������table��������еĵ�ǰ������ر�
    //QMap<QString, QString> where;//ƥ������
    std::map<std::string, std::string> where;
    //QList<QMap<QString, QString>> data;//��ѯ��������(����)
    std::list<std::map<std::string, std::string>> data;
    //QStringList keyList;
    std::list<std::string> keyList;
    keyList.push_back("tablename");
    where["proname"] = proname.simplified().toStdString();//ƥ����ڸ���Ŀ������table
    if (!MRUN_Sql_FinDataMapSql(table, where, keyList, data)) {//��ѯ���ݿ�
        setmessage(HINT_FLAG::MRUN_ERROR, "��ȡ����ʧ��");
    }
    //��������Ŀ�����б�
    std::list<std::map<std::string, std::string>>::iterator it;
    for (it = data.begin(); it != data.end(); it++)
        /*for (int i = 0; i < data.size(); i++)*/ {
        //QMap<QString, QString> map = *it;
        std::map<std::string, std::string> map = *it;
        // QMap<QString, QString>::iterator it_child;
        std::map<std::string, std::string>::iterator it_child;
        for (it_child = map.begin(); it_child != map.end(); it_child++) {
            //��ȡ���б���ֶ���
            std::list<std::string> listfiled;

            MRUN_Sql_FindTableIdNameSql(it_child->second, "mrun", listfiled);
            //��ȡ���б���ֶ�����
            std::list<std::string>  listfiledoper;

            MRUN_Sql_FindFieldOperatorSql(it_child->second, "mrun", listfiledoper);
            QList<QStringList> list;
            list << fromstdlist(listfiled) << fromstdlist(listfiledoper);
            mergelist_table_filed_ = mergelist(list);//�ϲ��ֶ����ֶ�����list
            //���ݿ����+�ֶ�
            tablemap.insert(QString::fromLocal8Bit((it_child->second.data())), mergelist_table_filed_);
        }
    }
    tablemap_ = tablemap;
    alldatamap.insert(proname, tablemap);//������+��ṹ
    treeviewmodel_->delTreeViewData();//���������ݿ�չʾ����
    treeviewmodel_->set_sql_all_table(/*alldatamap*/tablemap);//�������ݿ�ṹչʾ
    SlotCheckedContact(0);//����������
}
//�����Ĳۺ���������id���+�������+������
void SqlShowSettingForm::SlotRelated(const QString &strrelated)
{
    if (!strrelated.isEmpty()) {
        //����id
        if (strrelated.split("|").at(0).simplified() != relatedid_) {
            return;
        }
        else
        {
            for (int i = 0; i < listtablerelatedname_.size(); i++)
            {
                std::string table;
                //�����������list��������߼������
                if (listtablerelatedname_.at(i) != relatedlevelfirsttable_) {
                    qobject_cast<SqlDataShowDialog*>(tabformmap_.value(listtablerelatedname_.at(i)))->cleartabledata();
                    table = listtablerelatedname_.at(i).toStdString();//����
                }
                QSqlQuery *ret = Q_NULLPTR;

                //QMap<QString, QString> where;
                std::map<std::string, std::string> where;
                //QList<QMap<QString, QString>> data;
                std::list<std::map<std::string, std::string>> data;
                //QStringList keyList;
                std::list<std::string> keyList;
                std::list<std::string>  filedlist;//�ֶ�
                std::list<std::string>  filedoperlist;//�ֶ�˵��
                std::map<std::string, std::string> mapwhere;
                //list �ֶ�
                QStringList list = selecttablemap_.value(listtablerelatedname_.at(i));
                for (int i = 0; i < list.size(); i++)
                {
                    QString strlist = list.at(i);
                    if (strlist.split("|").at(0).toInt() == 0) {
                        filedlist.push_back(strlist.split("|").at(1).toStdString());//�ֶ�
                    }
                    else if (strlist.split("|").at(0).toInt() == 1)
                    {
                        filedoperlist.push_back(strlist.split("|").at(1).toStdString());//�ֶ�����
                    }
                }
                keyList = filedlist;//�ֶ�
                //strrelated ���+����
                where[strrelated.split("|").at(0).simplified().toStdString()] = strrelated.split("|").at(1).simplified().toStdString();//ƥ����ڸ��ֶε�����table����
                strrelatedrecordid_ = strrelated.split("|").at(1).simplified();
                if (!table.empty()) {
                    //��ѯ�ñ��¼������
                    int sizecount = MRUN_Sql_GetCountSizeSql(table, where);
                    qobject_cast<SqlDataShowDialog*>(tabformmap_.value(QString::fromLocal8Bit(table.data())))
                        ->setlabelinfo(SQL_INFO::Record, tr("��ǰ�����ݿ��¼����:  %1").arg(sizecount));
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
                        ->setlabelinfo(SQL_INFO::Page, tr("��ǰҳ:  %1/%2").arg(1).arg(count));

                    //QString strwhere = relatedid_ + "|" + strrelated.split("|").at(1).simplified();
                    //listwhere.push_back(string((const char*)strwhere.toLocal8Bit()));
                    mapwhere[string((const char*)relatedid_.toLocal8Bit())] = string((const char*)(strrelated.split("|").at(1).simplified()).toLocal8Bit());
                    if (!MRUN_Sql_FindNoIdSql(table, 0, DATA_COUNT_PAGE, keyList, mapwhere, data))
                    {
                        setmessage(HINT_FLAG::MRUN_ERROR, "��ȡ����ʧ��");
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
//����(�ر�)������ѡ��
void SqlShowSettingForm::SlotCheckedContact(int checked)
{
    cleartab();
    QMap<QString, QStringList>::iterator it_tab;
    if (!checked) {
        treeviewmodel_->setCheckedAll(true);//ȫѡչʾ
        QMap<QString, QStringList> selecttablemap;//ѡ�е�չʾ����
        selecttablemap = treeviewmodel_->getselecttabledata();//��ȡѡ�е����ݣ�����+�ֶ�list��
        for (it_tab = selecttablemap.begin(); it_tab != selecttablemap.end(); it_tab++) {
            AddTabWidget(it_tab.key(), it_tab.value());
        }
    }
    else //����ѡ�й����ı�
    {
        treeviewmodel_->setCheckedAll(false);
        cleartab();

        QSqlQuery *ret = Q_NULLPTR;
        std::string table = "public_manage_table";//������table
        //QMap<QString, QString> where;
        std::map<std::string, std::string> where;
        //QList<QMap<QString, QString>> data;
        std::list<std::map<std::string, std::string>> data;
        // QStringList keyList;
        std::list<std::string> keyList;
        //��������<<�Ƿ����<<����id<<�����ȼ�
        //keyList << "tablename"<<"related"<<"relatedid"<<"relatedlevel";
        keyList.push_back("tablename");
        keyList.push_back("related");
        keyList.push_back("relatedid");
        keyList.push_back("relatedlevel");
        where["proname"] = ui->comboBox_pro->currentText().simplified().toStdString();//ƥ����ڸ���Ŀ������table
        if (!MRUN_Sql_FinDataMapSql(table, where, keyList, data)) {
            setmessage(HINT_FLAG::MRUN_ERROR, "��ȡ���ݱ���ʧ��");
        }
        //��������Ŀ�����б� ��ȡ�й����ı�
        QStringList listtablerelatedname;//��list
        QString relatedlevelfirsttable;//����������ߵı�
        QString relatedid;//����������ߵı��ֶ�id
        //for (int i = 0; i < data.size(); i++) 
        std::list<std::map<std::string, std::string>>::iterator it;
        for (it = data.begin(); it != data.end(); it++)
        {
            std::map<std::string, std::string> map = *it;
            if (QString::fromStdString((map["related"])).toInt() == 1) { //1���� 0������              
                QString strlist = QString::fromStdString((map["tablename"])).simplified() + "|" +
                    QString::fromStdString((map["relatedid"])).simplified()
                    + "|" + QString::fromStdString((map["relatedlevel"])).simplified();
                related_table_list_.append(strlist);//��������+�����ֶ�+��������
                listtablerelatedname.append(QString::fromStdString((map["tablename"])).simplified());//���й�������
                relatedid = QString::fromStdString((map["relatedid"])).simplified();
                relatedid_ = relatedid;//���й���id
            }
            if (QString::fromStdString((map["relatedlevel"])).toInt() == 1) {//�����ȼ�1�����
                relatedlevelfirsttable = QString::fromStdString((map["tablename"])).simplified();
                relatedlevelfirsttable_ = relatedlevelfirsttable;//����������߱���
            }
        }
        listtablerelatedname_ = listtablerelatedname;
        //������������
        treeviewmodel_->setrelatedtablelist(listtablerelatedname);
        //ѡ�й�������
        treeviewmodel_->setCheckedRelated();

        QMap<QString, QStringList>::iterator it_tab;
        QMap<QString, QStringList> selecttablemap;//ѡ�е�չʾ����
        selecttablemap = treeviewmodel_->getselecttabledata();//����������+�ֶ�list��
        selecttablemap_ = selecttablemap; //���������� + �ֶ�list��
        for (it_tab = selecttablemap.begin(); it_tab != selecttablemap.end(); it_tab++) {
            AddTabWidget(it_tab.key(), it_tab.value());
        }
        //���õ�ǰѡ�н���
        ui->tabWidget->setCurrentWidget(tabformmap_.value(relatedlevelfirsttable));
    }
}
//�������չʾ
void SqlShowSettingForm::on_toolButton_showdata_clicked()
{
    cleartab();
    QMap<QString, QStringList>::iterator it_tab;
    QMap<QString, QStringList> selecttablemap;//ѡ�е�չʾ����(����+�ֶ�)
    selecttablemap = treeviewmodel_->getselecttabledata();
    for (it_tab = selecttablemap.begin(); it_tab != selecttablemap.end(); it_tab++) {
        AddTabWidget(it_tab.key(), it_tab.value());
    }

}
//head(����) list (�ֶ���+�ֶ�˵��)
void SqlShowSettingForm::AddTabWidget(QString head, QStringList list)
{
    std::list<std::string> filedlist;//�ֶ�
    std::list<std::string> filedoperlist;//�ֶ�˵��
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
    SqlDataShowDialog * form = new SqlDataShowDialog;//�Ҳ�������ʾ����
    //���ݿ���֮�������
    QObject::connect(form, SIGNAL(signalChangeValue(int)), this, SLOT(SlotChangePageValue(int)));
    form->setObjectName(head);
    ui->tabWidget->addTab(form, head);
    //��ʼ��ÿһ�����ݿ���չʾ����
    form->initshowform(fromstdlist(filedoperlist), head);
    //tabformmap_���ÿһ�����ݿ�չʾ����ָ��
    tabformmap_.insert(form->objectName(), form);
    //QList<QMap<QString, QString>> row;
    std::list<std::map<std::string, std::string>> row;
    QSqlQuery * ret = Q_NULLPTR;

    //QMap<QString, QString> where;
std:map < std::string, std::string > where;

    std::map<std::string, std::string>  mapwhere;
    //��ѯ�ñ��¼������
    int sizecount = MRUN_Sql_GetCountSizeSql(head.toStdString(), where);
    qobject_cast<SqlDataShowDialog*>(tabformmap_.value(head))
        ->setlabelinfo(SQL_INFO::Record, tr("��ǰ�����ݿ��¼����:  %1").arg(sizecount));
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
        ->setlabelinfo(SQL_INFO::Page, tr("��ǰҳ:  %1/%2").arg(1).arg(count));
    if (sizecount > DATA_COUNT_PAGE) {//��¼�������ڸ�ҳ����

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
    else//��¼����С�ڸ�ҳ����
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

    if (ui->checkBox_table_contact->isChecked()) {//��ǰ������������Ӧ�ۺ���
        QObject::connect(form, SIGNAL(signalItemText(const QString &)), this, SLOT(SlotRelated(const QString &)));
    }
}
void SqlShowSettingForm::SlotChangePageValue(int value)
{
    QMap<QString, QStringList> map = treeviewmodel_->getselecttabledata();//��ǰ��ѡ����
    QString head = ui->tabWidget->currentWidget()->objectName();//����
    QStringList list = map.value(head);//��ǰ��
    std::list<std::string> filedlist;//�ֶ�
    std::list<std::string> filedoperlist;//�ֶ�˵��
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
    qobject_cast<SqlDataShowDialog*>(tabformmap_.value(head))->cleartabledata();//����ñ�����
    std::list<std::map<std::string, std::string>> row;//��ѯ��������
    QSqlQuery * ret = Q_NULLPTR;

    std::map<std::string, std::string> where;//��ѯ��¼��ƥ��
    std::map<std::string, std::string> mapwhere;//��ѯ����ƥ��
    //&&head!= relatedlevelfirsttable_


    //ѡ�й���
    if (ui->checkBox_table_contact->isChecked()) {

        where[string((const char*)relatedid_.toLocal8Bit())] = string((const char*)strrelatedrecordid_.toLocal8Bit());
        if (head == relatedlevelfirsttable_) {//��ǰ��������������
            //listwhere.clear();
            where.clear();
        }
        //��ѯ�ñ��¼������
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
            ->setlabelinfo(SQL_INFO::Page, tr("��ǰҳ:  %1/%2").arg(value).arg(count));
        qobject_cast<SqlDataShowDialog*>(tabformmap_.value(head))->setmaxvalue(count);


        QString strwhere = relatedid_ + "|" + strrelatedrecordid_;
        mapwhere[string((const char*)strwhere.toLocal8Bit())] = string((const char*)strrelatedrecordid_.toLocal8Bit());
        if (head == relatedlevelfirsttable_) {//��ǰ��������������
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
        //��ѯ�ñ��¼������
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
            ->setlabelinfo(SQL_INFO::Page, tr("��ǰҳ:  %1/%2").arg(value).arg(count));
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
//��������ʽ
void SqlShowSettingForm::initComboxColor(QComboBox * comBox)
{
	QStandardItemModel *pItemModel = qobject_cast<QStandardItemModel*>(comBox->model());
	//�޸�ĳ���ı�
	for (int i = 0; i < comBox->count(); i++)
	{
		pItemModel->item(i)->setForeground(QColor(255, 255, 255));					//�޸�ĳ���ı���ɫ
		pItemModel->item(i)->setBackground(QColor(29, 30, 31));						//�޸�ĳ�����ɫ    
		pItemModel->item(i)->setSizeHint(QSize(180, 40));
		pItemModel->item(i)->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);    //�޸�ĳ���ı����뷽ʽ
	}
}
void SqlShowSettingForm::on_toolButton_left_clicked(bool checked)//���������ť
{
	ui->groupBox->setVisible(false);
	ui->toolButton_right->setEnabled(true);
	ui->toolButton_left->setVisible(false);
	ui->line_2->setStyleSheet("background-color:transparent");
	ui->line_3->setStyleSheet("background-color:transparent");
}
void SqlShowSettingForm::on_toolButton_right_clicked(bool checked)//���������ť
{
	//if (checked) {
		ui->groupBox->setVisible(true);
		ui->toolButton_right->setEnabled(false);
		ui->toolButton_left->setVisible(true);
		ui->line_2->setStyleSheet("background-color:#000000");
		ui->line_3->setStyleSheet("background-color:#000000");
	//}
}