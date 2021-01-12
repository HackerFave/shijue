#include "sqlsavesettingform.h"
#include "../../sqlmethod/DatabaseInterface/mrun_sqloperation_define.h"
#include "mrun_messagebox.h"
SqlSaveSettingForm::SqlSaveSettingForm(QWidget *parent)
    : QWidget(parent)
    ,ui(new Ui::SqlSaveSettingForm)
{
    ui->setupUi(this);
    std::list<std::string> list;
    MRUN_Sql_GetProSqlList(list);
    AddItemsCombox(list);
    //�����л��ۺ���
    connect(ui->comboBox_pro, SIGNAL(currentTextChanged(const QString &)), this, SLOT(SlotProChange(const QString &)));
    SlotProChange(ui->comboBox_pro->currentText());
}

QMap<QString, QString> SqlSaveSettingForm::fromstdmap(std::map<std::string, std::string> map)
{
    std::map<std::string, std::string>::iterator it;
    QMap<QString, QString> map_q;
    for (it = map.begin(); it != map.end(); it++) {
        it->first;
        map_q[QString::fromLocal8Bit((it->first.data()))] = QString::fromLocal8Bit((it->second.data()));
    }
    return map_q;
}
QStringList SqlSaveSettingForm::fromstdlist(std::list<std::string> list)
{
    QStringList list_q;
    std::list<std::string>::iterator it;
    for (it = list.begin(); it != list.end(); it++) {

        list_q.push_back(QString::fromLocal8Bit((*it).data()));
    }
    return list_q;
}
SqlSaveSettingForm::~SqlSaveSettingForm()
{
}
void SqlSaveSettingForm::InitSqlTableData(const QMap<QString, QMap<QString, QStringList>> &mapprodata)
{
    ClearSqlTableGroup();
    QMap<QString, QMap<QString, QStringList>>::const_iterator it;
    for (it = mapprodata.cbegin(); it != mapprodata.cend(); it++) {
        QMap<QString, QStringList> mapchild = it.value();
        QMap<QString, QStringList>::iterator it_child;
        for (it_child = mapchild.begin(); it_child != mapchild.end(); it_child++) {
            SqlTableGroupItem * itemgroup = new SqlTableGroupItem;
            itemgroup->setObjectName(it_child.key());
            itemgroup->SetGroupBoxTitle(itemgroup->objectName());
            map_SqlTableGroupItem_.insert(itemgroup->objectName().simplified(), itemgroup);
            itemgroup->SetGroupBoxChecked(true);
            itemgroup->SetIdItemList(it_child.value());
            ui->verticalLayout_addgroup->addWidget(itemgroup);

        }
    }
}
//�����л�
void SqlSaveSettingForm::SlotProChange(const QString &proname)
{
    if (!MRUN_Sql_InitSqlConnect(ui->comboBox_pro->currentText().toStdString() + "write")) {
        return;
    }
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
            //mergelist_table_filed_ = mergelist(list);//�ϲ��ֶ����ֶ�����list
            //���ݿ����+�ֶ�
            tablemap.insert(QString::fromLocal8Bit((it_child->second.data())), mergelist(list));
        }
    }
   // tablemap_ = tablemap;
    alldatamap.insert(proname, tablemap);//������+��ṹ
    InitSqlTableData(alldatamap);
}
void SqlSaveSettingForm::AddItemsCombox(const std::list<std::string> list)
{
    ui->comboBox_pro->clear();
    std::list<std::string>::const_iterator it;
    for (it = list.cbegin(); it != list.cend(); it++) {
        ui->comboBox_pro->addItem(QString::fromLocal8Bit((*it).data()));
    }

}
//�ϲ�����qstringlist
QStringList SqlSaveSettingForm::mergelist(QList<QStringList> list)
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
void SqlSaveSettingForm::ClearSqlTableGroup()
{
    QMap<QString, SqlTableGroupItem*>::iterator it;
    for (it = map_SqlTableGroupItem_.begin(); it != map_SqlTableGroupItem_.end(); it++) {
        delete it.value();
    }
    map_SqlTableGroupItem_.clear();
}
//�����Ƿ���Ҫ��չ������������
void SqlSaveSettingForm::on_toolButton_savedatasql_clicked()
{
    if (!MRUN_Sql_InitSqlConnect(ui->comboBox_pro->currentText().toStdString()+"write")) {
        setmessage(HINT_FLAG::MRUN_ERROR, QString::fromLocal8Bit("д������ʧ��"));
        return;
    }
}
void SqlSaveSettingForm::on_toolButton_addnewpro_clicked()
{
   QSettings* configIni_ = new QSettings(QString("%1/sqlsetting/prosql.ini").arg(QCoreApplication::applicationDirPath())
        , QSettings::IniFormat);
    QDir dir;
    dir.setPath(QString("%1/sqlsetting").arg(QCoreApplication::applicationDirPath()));
    if (!dir.exists()) {
        dir.mkpath(QString("%1/sqlsetting").arg(QCoreApplication::applicationDirPath()));
        configIni_->setValue(QString("pro/%1").arg(configIni_->allKeys().size()+1) ,ui->lineEdit_newpro->text().simplified());  
    }
    configIni_->setValue(QString("pro/%1").arg(configIni_->allKeys().size()+1), ui->lineEdit_newpro->text().simplified());
}