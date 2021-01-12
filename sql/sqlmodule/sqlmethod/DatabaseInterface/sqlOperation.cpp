#include "sqlOperation.h"
#include "mrun_messagebox.h"
#include <qstring.h>

#include "mrun_sqloperation_define.h"
using namespace std;
#pragma execution_character_set("utf-8")
//QSqlDatabase	*sqlOperation::m_db = nullptr;
sqlOperation::sqlOperation(std::string type)
:type_(type)
{
    
}

sqlOperation::~sqlOperation()
{
    m_db->close();
    if (m_query != NULL)
    {
        delete m_query;
        m_query = NULL;
    }
}


bool sqlOperation::DatabaseIsOK()
{
    return true;
}

bool sqlOperation::Init(const std::string & host, const std::string & user, const std::string & passwd, const std::string & dataname)
{
    m_db = new QSqlDatabase();
    QString sql = QString("CREATE DATABASE %1;").arg(QString::fromLocal8Bit(dataname.data()));
    QSqlQuery *ret;
    Query(string((const char *)sql.toLocal8Bit()), &ret);
    
    if (m_db->isValid())//存在连接直接退出
    {
        return false;
    }

    m_dbhost = host;
    m_dbuser = user;
    m_dbpasswd = passwd;
    m_dbdataname = dataname;
    //m_dbdataname = QCoreApplication::applicationDirPath() + '/' + dataname;

    *m_db = QSqlDatabase::addDatabase("QMYSQL", QString::fromLocal8Bit(type_.data()));
    m_db->setHostName(QString::fromLocal8Bit(m_dbhost.data()));							//数据库服务器IP
    m_db->setUserName(QString::fromLocal8Bit(m_dbuser.data()));							//数据库用户名
    m_db->setPassword(QString::fromLocal8Bit(m_dbpasswd.data()));						//数据库密码

    //QString str = QString("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)}; FIL={MS Access};DBQ=%1;").arg(m_dbdataname);
    //m_db.setDatabaseName(str);
    m_db->setConnectOptions("MYSQL_OPT_RECONNECT = 1");  //add 设置mysql数据断开后自动重连 [WangPing 2020/3/20]	
    m_db->setDatabaseName(QString::fromLocal8Bit(m_dbdataname.data()));
    if (!m_db->open()) {
        m_db->setConnectOptions();	//add 链接失败时清除设置 [WangPing 2020/3/20]
        QSqlError err = m_db->lastError();
        qDebug() << err.databaseText();
        return false;
    }
    //	qDebug() << QSqlDatabase::drivers();
    m_query = new QSqlQuery(*m_db);
    return true;
}

bool sqlOperation::Query(const std::string & sql, QSqlQuery ** ret)
{
    if (m_query == NULL)
        return false;

    bool isok = m_query->exec(QString::fromLocal8Bit(sql.data()));
    m_sql = sql;
    //QString a = m_query->value(1).toString();
    *ret = m_query;
    if (isok == false) {
        return false;
    }
    return true;
}

bool sqlOperation::Add(std::string table_name, std::vector<std::map<std::string, std::string>> data, QSqlQuery ** ret)
{
    if (m_query == NULL)
        return false;
    QString sql = "insert into " + QString::fromLocal8Bit(table_name.data()) + "(";
    QString values = " values";
    QString valueData;
    for (std::vector<std::map<std::string, std::string>>::const_iterator iter = data.cbegin(); iter != data.cend(); iter++)
    {
        for (std::map<std::string, std::string>::const_iterator i = (*iter).cbegin(); i != (*iter).cend(); i++)
        {
            sql += QString::fromLocal8Bit((i->first).data()) + ", ";
        }
        break;
    }
    for (std::vector<std::map<std::string, std::string>>::const_iterator iter = data.cbegin(); iter != data.cend(); iter++)
    {
       // cout << (*iter) << endl;
        QString value;
        value += "(";
        for (std::map<std::string, std::string>::const_iterator i = (*iter).cbegin(); i != (*iter).cend(); i++)
        {
           // sql += i->first + ", ";
            value += "'" + QString::fromLocal8Bit((i->second).data()) + "', ";

        }
        value.chop(2);
        value += "),";
        valueData += value;
    }
    
    sql.chop(2);
    //values.chop(2);
    sql += ")";
    values += valueData;
    values.chop(1);
    sql += values;
    m_query->prepare(sql);
    bool is = Query(string((const char *)sql.toLocal8Bit()), ret);

    return is;
}
bool sqlOperation::Add(std::string table_name, std::map<std::string, std::string> data, QSqlQuery ** ret)
{

   /* if (data.count("record_ID")) {
        if (!_recordID.isEmpty()) {
            data["record_ID"] = _recordID;
        }
        else
        {
            return false;
        }
    }*/
    if (m_query == NULL)
        return false;
    QString sql = "insert into " + QString::fromLocal8Bit(table_name.data()) + "(";
    QString values = " values(";
    for (std::map<std::string, std::string>::const_iterator i = data.cbegin(); i != data.cend(); i++)
    {
        sql += QString::fromLocal8Bit((i->first).data()) + ", ";
        values += "'" + QString::fromLocal8Bit((i->second).data()) + "', ";

    }
    sql.chop(2);
    values.chop(2);
    sql += ")";
    values += ")";
    sql += values;
    m_query->prepare(sql);
    bool is = Query(string((const char *)sql.toLocal8Bit()), ret);

    return is;
}

bool sqlOperation::Del(std::string table_name, std::map<std::string, std::string> where, QSqlQuery ** ret)
{
    if (m_query == NULL)
        return false;
    QString sql = "delete from ";
    sql += QString::fromLocal8Bit(table_name.data());
    sql += " where ";
    for (std::map<std::string, std::string>::const_iterator i = where.cbegin(); i != where.cend(); i++)
    {
        sql += QString::fromLocal8Bit((i->first).data()) + "=";
        sql += "'" + QString::fromLocal8Bit((i->second).data()) + "'and ";
    }
    sql.chop(4);
    m_query->prepare(sql);
    return Query(string((const char *)sql.toLocal8Bit()), ret);
}
//删除一条记录
bool sqlOperation::delRecord(std::string table_name, const std::string &key, const std::string &value, QSqlQuery ** ret)
{
    QString sql = QString("delete from %1 where %2 = '%3'")
        .arg(QString::fromLocal8Bit(table_name.data()))
        .arg(QString::fromLocal8Bit(key.data()))
        .arg(QString::fromLocal8Bit(value.data()));
    m_query->prepare(sql);
    return Query(string((const char *)sql.toLocal8Bit()), ret);
}

bool sqlOperation::Updata(std::string table_name, std::map<std::string, std::string> where, std::map<std::string, std::string> data, QSqlQuery ** ret)
{
    if (m_query == NULL)
        return false;
    QString sql = "update " + QString::fromLocal8Bit(table_name.data()) + " set ";
    for (std::map<std::string, std::string>::const_iterator i = data.cbegin(); i != data.cend(); i++)
    {
        sql += QString::fromLocal8Bit((i->first).data()) + "=";
        sql += "'" + QString::fromLocal8Bit((i->second).data()) + "',";

    }
    sql.chop(1);
    sql += "where ";
    for (std::map<std::string, std::string>::const_iterator i = where.cbegin(); i != where.cend(); i++)
    {
        sql += QString::fromLocal8Bit((i->first).data()) + "=";
        sql += "'" + QString::fromLocal8Bit((i->second).data()) + "'and ";
    }
    sql.chop(4);
    return Query(string((const char *)sql.toLocal8Bit()), ret);
}

bool sqlOperation::Find(std::string table_name, std::list<std::string> key, std::list<std::list<std::string>>* row, QSqlQuery ** ret)
{
    if (m_query == NULL)
        return false;
    QString sql = "SELECT ";
    int len = key.size();

    for (int i = 0; i < len; i++)
    {
        sql += QString::fromLocal8Bit(key.front().data());
        sql += ",";
        key.pop_front();
    }
    sql.chop(1);
    sql += " FROM " + QString::fromLocal8Bit(table_name.data());

    m_query->prepare(sql);
    if (Query(string((const char *)sql.toLocal8Bit()), ret))
    {
        while (m_query->next())
        {
            std::list<std::string> j;
            for (int i = 0; i < len; i++)
            {
                
                j.push_back(string((const char *)m_query->value(i).toString().toLocal8Bit()));
            }
            row->push_back(j);
        }
        return true;
    }
    else
        return false;
}

void sqlOperation::CloseDB()
{
    m_db->close();
}
bool sqlOperation::Find(std::string sql, std::list<std::list<std::string> > *row, QSqlQuery ** ret, int len)
{
    m_query->prepare(QString::fromLocal8Bit(sql.data()));
    if (Query(sql, ret))
    {
        while (m_query->next())
        {
            std::list <std::string> j;
            for (int i = 0; i < len; i++)
            {
                //j.push_back(m_query->value(i).toString().toStdString());
                j.push_back(string((const char *)m_query->value(i).toString().toLocal8Bit()));
            }
            row->push_back(j);
        }
        return true;
    }
    else
        return false;
}



bool sqlOperation::Find(std::string table_name, std::list<std::string> key, std::map<std::string, std::string> where, std::list<std::list<std::string> > *row, QSqlQuery ** ret)
{
    if (m_query == NULL)
        return false;
    QString sql = "SELECT ";
    int len = key.size();
    for (int i = 0; i < len; i++)
    {
        sql += QString::fromLocal8Bit(key.front().data());
        sql += ",";
        key.pop_front();
    }
    sql.chop(1);
    sql += " FROM " + QString::fromLocal8Bit(table_name.data());
    sql += " WHERE ";
    for (std::map<std::string, std::string>::const_iterator i = where.cbegin(); i != where.cend(); i++)
    {
        sql += QString::fromLocal8Bit(i->first.data()) + " = '" + QString::fromLocal8Bit(i->second.data()) + "'" + " AND ";
    }
    sql.chop(5);
    //qDebug()<<sql;
    m_query->prepare(sql);
    if (Query(string((const char *)sql.toLocal8Bit()), ret))
    {
        while (m_query->next())
        {
            std::list <std::string> j;
            for (int i = 0; i < len; i++)
            {
                //j.push_back(m_query->value(i).toString().toStdString());
                j.push_back(string((const char *)m_query->value(i).toString().toLocal8Bit()));
            }
            row->push_back(j);
        }
        return true;
    }
    else
        return false;
}
//判断该条记录是否存在
int sqlOperation::isExists(std::string table_name, std::string key, std::string value, QSqlQuery ** ret)
{
    QString sql = QString("select * from %1 where %2 = '%3'")
        .arg(QString::fromLocal8Bit(table_name.data()))
        .arg(QString::fromLocal8Bit(key.data()))
        .arg(QString::fromLocal8Bit(value.data()));
    m_query->prepare(sql);
    if (Query(string((const char *)sql.toLocal8Bit()), ret))
    {
        if (!m_query->next()) {
            return 0;
        }
        else
        {
            return 1;
        }

    }
    return false;
}
bool sqlOperation::findData(std::string table_name, std::map<std::string,std::string> &where, const std::list<std::string> &keyList, /*QList<QVariantList>*/std::list<std::list<std::string>> &row, QSqlQuery ** ret)
{
    // QMap<QString, QString> map = where;
    //if (where.count("record_ID") && where.value("record_ID").simplified().isEmpty()) {
      //  where["record_ID"] = _recordID;
    //}
    QString sql = QString("select * from %1 where ")
        .arg(QString::fromLocal8Bit(table_name.data()));
    int keySize = where.size();
    int num = 0;
    int keyNum = keyList.size();
    std::map<std::string, std::string>::iterator it;
    for (it = where.begin(); it != where.end(); it++) {
        num++;
        sql += QString::fromLocal8Bit(it->first.data());
        sql += " = ";
        sql += "'" + QString::fromLocal8Bit(it->second.data()) + "'";
        if (num > keySize - 1) {
            break;
        }
        sql += " and ";
    }
    m_query->prepare(sql);
    QSqlRecord rec = m_query->record();
    //int keyNum = keyList.size();
    if (Query(string((const char *)sql.toLocal8Bit()), ret))
    {
        while (m_query->next())
        {
            std::list<std::string> listChild;
            rec = m_query->record();

            /*QMap<QString, QString>::iterator itKey;
            for (itKey = where.begin(); itKey != where.end(); itKey++) {
                QString value = m_query->value(rec.indexOf(itKey.key().simplified())).toString();
                listChild.append(value);
            }*/
            std::list<std::string>::const_iterator it_list;
            for (it_list = keyList.cbegin(); it_list != keyList.cend();it_list++) {
                QString value = m_query->value(rec.indexOf(QString::fromStdString((*it_list)).simplified())).toString();
                listChild.push_back(string((const char *)value.toLocal8Bit()));
                
            }
            row.push_back(listChild);
        }
        return true;
    }
    else
        return false;

}
bool sqlOperation::findData(std::string table_name,std::map<std::string,std::string> &where, const std::list<std::string> &keyList, std::list<std::map<std::string, std::string>>&row, QSqlQuery ** ret)
{
    // QMap<QString, QString> map = where;
    //if (where.count("record_ID") && where.value("record_ID").simplified().isEmpty()) {
      //  where["record_ID"] = _recordID;
    //}
    std::list<std::string> key_list = keyList;
    QString sql = QString("select * from %1 where ")
        .arg(QString::fromLocal8Bit(table_name.data()));
    int keySize = where.size();
    int num = 0;
    int keyNum = keyList.size();
    std::map<std::string, std::string>::iterator it;
    for (it = where.begin(); it != where.end(); it++) {
        num++;
        sql += QString::fromLocal8Bit(it->first.data());
        sql += " = ";
        sql += "'" + QString::fromLocal8Bit(it->second.data()) + "'";
        if (num > keySize - 1) {
            break;
        }
        sql += " and ";
    }
    m_query->prepare(sql);
    QSqlRecord rec = m_query->record();
    //int keyNum = keyList.size();
    if (Query(string((const char *)sql.toLocal8Bit()), ret))
    {
        while (m_query->next())
        {
            std::map<std::string, std::string> mapChild;
            rec = m_query->record();

            
            std::list<std::string>::iterator it_list;
            for (it_list = key_list.begin(); it_list!= key_list.end(); it_list++) {
                QString value = m_query->value(rec.indexOf(QString::fromStdString((*it_list)))).toString();
                //mapChild.insert(*it_list, value.toStdString());
                mapChild[*it_list] = string((const char *)value.toLocal8Bit());
            }
            row.push_back(mapChild);
        }
        return true;
    }
    else
       return false;
}
bool sqlOperation::findDataRecord(std::string table_name, std::map<std::string, std::string> &where, const std::list<std::string> &keyList, std::vector<std::map<std::string, std::string>> &row, QSqlQuery ** ret)
{
    QString sql = QString("select * from %1 where ")
        .arg(QString::fromLocal8Bit(table_name.data()));
    int keySize = where.size();
    int num = 0;
    int keyNum = keyList.size();
    std::map<std::string, std::string>::iterator it;
    for (it = where.begin(); it != where.end(); it++) {
        num++;
        sql += QString::fromLocal8Bit(it->first.data());
        sql += " = ";
        sql += "'" + QString::fromLocal8Bit(it->second.data()) + "'";
        if (num > keySize - 1) {
            break;
        }
        sql += " and ";
    }
    m_query->prepare(sql);
    QSqlRecord rec = m_query->record();
    //int keyNum = keyList.size();
    if (Query(string((const char *)sql.toLocal8Bit()), ret))
    {
        while (m_query->next())
        {
            std::map<std::string, std::string> mapChild;
            rec = m_query->record();

            /*QMap<QString, QString>::iterator itKey;
            for (itKey = where.begin(); itKey != where.end(); itKey++) {
                QString value = m_query->value(rec.indexOf(itKey.key().simplified())).toString();
                listChild.append(value);
            }*/
            std::list<std::string>::const_iterator it_list;
            for (it_list = keyList.cbegin(); it_list!= keyList.cend(); it_list++) {
                QString value = m_query->value(rec.indexOf(QString::fromStdString((*it_list)))).toString();
               // mapChild.insert(keyList.at(i), value);
                mapChild[*it_list] = string((const char*)value.toLocal8Bit());
            }
            row.push_back(mapChild);
        }
        return true;
    }
    else
        return false;
}
bool sqlOperation::Find(std::string table_name, const std::list<std::string> &listkey, std::list<std::map<std::string, std::string>>&row, QSqlQuery ** ret)
{
    QString sql = QString("select "); //area_1,area_2 from heating_zone_table
    std::list<std::string>::const_iterator it_listkey;
    for (it_listkey = listkey.cbegin(); it_listkey !=listkey.cend(); it_listkey++) {
        sql += QString::fromLocal8Bit((*it_listkey).data());
        sql += ",";
    }
    sql.chop(1);
    sql += QString(" from %1").arg(QString::fromLocal8Bit(table_name.data()));
    m_query->prepare(sql);
    int keyNum = listkey.size();
    QSqlRecord rec = m_query->record();
    if (Query(string((const char *)sql.toLocal8Bit()), ret))
    {
        while (m_query->next())
        {
            std::map<std::string, std::string> mapChild;
            rec = m_query->record();
            std::list<std::string>::const_iterator it_list;
            for (it_list = listkey.cbegin(); it_list != listkey.cend(); it_list++) {
                QString value = m_query->value(rec.indexOf(QString::fromLocal8Bit((*it_list).data()).simplified())).toString();
                //mapChild.insert(*it_list, value.toStdString());
                mapChild[*it_list] = string((const char *)value.toLocal8Bit());
            }
            row.push_back(mapChild);
        }
        return true;
    }
    else
        return false;
}
//查询最近days天数据
bool sqlOperation::findData_Date(std::string table_name, const std::list<std::string> &keyList, std::list<std::list<std::string>> &row, int days, const std::string &data_time_key, QSqlQuery ** ret)
{
    QString sql = QString("select * from %1 where DATE_SUB(CURDATE(), INTERVAL %2 DAY) <= date(%3)")
        .arg(QString::fromLocal8Bit(table_name.data()))
        .arg(days)
        .arg(QString::fromLocal8Bit(data_time_key.data()));
    m_query->prepare(sql);
    QSqlRecord rec = m_query->record();
    int keySize = keyList.size();
    if (Query(string((const char *)sql.toLocal8Bit()), ret))
    {
        while (m_query->next())
        {
            /*
            std::list <QString> j;
            for (int i = 0; i < 3; i++)
            {
                j.push_back(m_query->value(i).toString());

            }
            row->push_back(j);
            */
            std::list<std::string> listChild;
            rec = m_query->record();
            std::list<std::string>::const_iterator it_list;
            for (it_list = keyList.cbegin(); it_list!=keyList.cend(); it_list++) {
                QString value = m_query->value(rec.indexOf(QString::fromStdString((*it_list)).simplified())).toString();
                listChild.push_back(string((const char*)value.toLocal8Bit()));
            }
            /* int snocol = rec.indexOf("id");
             int snamecol = rec.indexOf("data_time");
             int sclasscol = rec.indexOf("num");
             QString value1 = m_query->value(snocol).toString();
             QString value2 = m_query->value(snamecol).toString();
             QString value3 = m_query->value(sclasscol).toString();
             qDebug() << "sno:" << value1 << "\t" << "sname:" << value2 << "\t" << "sclass:" << value3;
             listChild<<value1<<value2<<value3;*/
            row.push_back(listChild);
        }
        return true;
    }
    else
        return false;
}
void sqlOperation::CreateTable()
{
    QString dataBase = "CREATE DATABASE zhongji;";
    QString sql = "CREATE TABLE t1(id VARCHAR(100) not null primary key,name VARCHAR(100)); ";
}
/*void sqlOperation::setTestRecord(std::map<QString, QString> data)
{

    QString table = "test_record";
    QSqlQuery * ret;
   // databaseSqlInset::get_db()->Add(table, data, &ret);
}*/
//清空数据库现存数据
bool sqlOperation::clearTable(std::list<std::string> listTableName, QSqlQuery ** ret)
{
    std::list<std::string>::iterator it;
    for (it = listTableName.begin(); it!= listTableName.end(); it++) {
        QString sql = QString("truncate table %1")
            .arg(QString::fromLocal8Bit((*it).data()));
        m_query->prepare(sql);
        if (!Query(string((const char *)sql.toLocal8Bit()), ret)) {
            return false;
        }
    }
    return true;
}
//查询表字段名
bool sqlOperation::findtableidname(std::string table_name, std::string proname, std::list<std::string> &listrow, QSqlQuery ** ret)
{
    //select CONCAT(COLUMN_NAME, ',') from information_schema.COLUMNS where table_name = 'heating_zone_table' and table_schema = 'zhongji';
    QString sql = QString("select CONCAT(COLUMN_NAME,',') from information_schema.COLUMNS where table_name = '%1'and table_schema = '%2'; ")
        .arg(QString::fromLocal8Bit(table_name.data()))
        .arg(QString::fromLocal8Bit(proname.data()));
    m_query->prepare(sql);

    QSqlRecord rec = m_query->record();
   
    if (Query(string((const char *)sql.toLocal8Bit()), ret))
    {
        int size = 0;
        /*QSqlQuery返回的数据集，record是停在第一条记录之前的。
        所以，你获得数据集后，必须执行next()或first()到第一条记录，
            这时候record才是有效的。
            否则，exec完直接value肯定报"QSqlQuery::value: not positioned on a valid record"这个错
            */
        while (m_query->next())
        {
            QString name = m_query->value(0).toString(); //做你想要的数据处理    
            name.chop(1);
            listrow.push_back(string((const char*)name.toLocal8Bit()));
        }
        return true;
    }
    return false;
}
//字段描述
bool sqlOperation::findfieldoperator(std::string table_name, std::string proname, std::list<std::string> &listrow, QSqlQuery ** ret)
{
    QString sql = QString("select column_comment from information_schema.columns where table_schema = 'mrun'and table_name = '%1';")
        .arg(QString::fromLocal8Bit(table_name.data()));
    m_query->prepare(sql);
    QSqlRecord rec = m_query->record();
    if (Query(string((const char *)sql.toLocal8Bit()), ret))
    {
        int size = 0;
        /*QSqlQuery返回的数据集，record是停在第一条记录之前的。
        所以，你获得数据集后，必须执行next()或first()到第一条记录，
            这时候record才是有效的。
            否则，exec完直接value肯定报"QSqlQuery::value: not positioned on a valid record"这个错
            */
        while (m_query->next())
        {
            QString name = m_query->value(0).toString(); //做你想要的数据处理    
            //name.chop(1);
            listrow.push_back(string((const char*)name.toLocal8Bit()));
        }
        return true;
    }
    return false;
}
//查询指定条记录第十条到第一百条
bool sqlOperation::Find(std::string table_name, std::string id, int start_num, int end_num, const std::list<std::string> &keyList, std::list<std::map<std::string, std::string>> &row, QSqlQuery**ret)
{
    QString sql = QString("select * from %1 where %2 between %3 and %4 ")
        .arg(QString::fromLocal8Bit(table_name.data()))
        .arg(QString::fromLocal8Bit(id.data()))
        .arg(start_num)
        .arg(end_num);
    int num = 0;
    int keyNum = keyList.size();
    m_query->prepare(sql);
    QSqlRecord rec = m_query->record();
    if (Query(string((const char *)sql.toLocal8Bit()), ret))
    {
        while (m_query->next())
        {
            std::map<std::string, std::string> mapChild;
            rec = m_query->record();
            std::list<std::string>::const_iterator it_list;
            for (it_list = keyList.cbegin(); it_list != keyList.cend();it_list++) {

                QString value = m_query->value(rec.indexOf(QString::fromLocal8Bit((*it_list).data()).simplified())).toString();
                //mapChild.insert(*it_list, value.toStdString());
                mapChild[*it_list] = string((const char*)value.toLocal8Bit());
            }
            row.push_back(mapChild);
        }
        return true;
    }
    else
        return false;
}
//查询指定条记录第十条到第一百条且满足筛选条件
bool sqlOperation::Find(std::string table_name, std::string id, int start_num, int end_num, const std::list<std::string>  &keyList, std::list<std::string>  listvalue, std::list<std::map<std::string, std::string>> &row, QSqlQuery**ret)
{
    //select * from test_record where dataID between 100 and 10000 and record_Id = '20201023143531685'
    QString sql = QString("select * from %1 where %2 between %3 and %4 ")
        .arg(QString::fromLocal8Bit(table_name.data()))
        .arg(QString::fromLocal8Bit(id.data()))
        .arg(start_num)
        .arg(end_num);
    //and record_Id = '20201023143531685'
    std::list<std::string>::iterator it_list_value;
    for (it_list_value = listvalue.begin(); it_list_value != listvalue.end(); it_list_value++){
        sql += QString(" and %1='%2'").arg( QString::fromLocal8Bit((*it_list_value).data()).split("|").at(0))
            .arg(QString::fromLocal8Bit((*it_list_value).data()).split("|").at(1));
    }
    int num = 0;
    int keyNum = keyList.size();
    m_query->prepare(sql);
    QSqlRecord rec = m_query->record();
    if (Query(string((const char *)sql.toLocal8Bit()), ret))
    {
        while (m_query->next())
        {
            std::map<std::string, std::string> mapChild;
            rec = m_query->record();
            std::list<std::string>::const_iterator it_list;
            for (it_list = keyList.cbegin(); it_list != keyList.cend(); it_list++) {

                QString value = m_query->value(rec.indexOf(QString::fromLocal8Bit((*it_list).data()).simplified())).toString();
                mapChild[*it_list] = string((const char*)value.toLocal8Bit());
            }
            row.push_back(mapChild);
        }
        return true;
    }
    else
        return false;
}
//查询指定条记录（没有自增id的情况）
//select * from test_record where meters='100' limit 0, 10
bool sqlOperation::Find(std::string table_name,int start_num, int page_num, const std::list<std::string> &keyList, std::map<std::string,std::string> mapwhere, std::list<std::map<std::string, std::string>> &row, QSqlQuery**ret)
{
    //select * from test_record where dataID between 100 and 10000 and record_Id = '20201023143531685'
    
    QString sqlwhere1 = "where ";
    QString sqlwhere2;
	std::map<std::string, std::string>::iterator it_map_value;
    for (it_map_value = mapwhere.begin(); it_map_value != mapwhere.end(); it_map_value++)
    {
		if ((it_map_value->first).empty()||it_map_value->second.empty()) {
			mapwhere .clear();
			break;
		}
        sqlwhere2 += QString(" and %1='%2'").arg(QString::fromLocal8Bit((it_map_value->first).data()))
            .arg(QString::fromLocal8Bit((it_map_value->second).data()));
    }
    if (mapwhere.empty()) {
        sqlwhere1 = "";
    }
    else
    {
        sqlwhere2= sqlwhere2.mid(5);
        sqlwhere1 = sqlwhere1 + sqlwhere2;
    }
    QString sql = QString("select * from %1 %2 limit %3, %4 ")
        .arg(QString::fromLocal8Bit(table_name.data()))
        .arg(sqlwhere1)
        .arg(start_num)
        .arg(page_num);
    //and record_Id = '20201023143531685'
    /*std::list<std::string>::iterator it_listvalue;
    for (it_listvalue = listwhere.begin(); it_listvalue != listwhere.end(); it_listvalue++)
    {
        sql += QString(" and %1='%2'").arg(QString::fromLocal8Bit((*it_listvalue).data()).split("|").at(0))
            .arg(QString::fromLocal8Bit((*it_listvalue).data()).split("|").at(1));
    }*/
    int num = 0;
    int keyNum = keyList.size();
    m_query->prepare(sql);
    QSqlRecord rec = m_query->record();
    if (Query(string((const char *)sql.toLocal8Bit()), ret))
    {
        while (m_query->next())
        {
            std::map<std::string, std::string> mapChild;
            rec = m_query->record();
            std::list<std::string>::const_iterator it_list;
            for (it_list = keyList.cbegin(); it_list != keyList.cend();it_list++) {
                QString value = m_query->value(rec.indexOf(QString::fromLocal8Bit((*it_list).data()).simplified())).toString();
                mapChild[*it_list]= string((const char*)value.toLocal8Bit());
            }
            row.push_back(mapChild);
        }
        return true;
    }
    else
        return false;
}

int sqlOperation::getcountsize(std::string table_name, std::map<std::string, std::string> where, QSqlQuery ** ret)
{
    QString sql = QString("select count(*) from %1 where ")// where 字段 = 'value';")
        .arg(QString::fromLocal8Bit(table_name.data()));
    std::map<std::string, std::string>::iterator it;
    QString sqlwhere;
    for (it = where.begin(); it != where.end(); it++) {
		if (it->first.empty() || it->second.empty()) {
			where.clear();
			break;
		}
        sqlwhere += QString(" and %1 = '%2'").arg(QString::fromLocal8Bit((it->first).data()))
            .arg(QString::fromLocal8Bit((it->second).data()));
    }
    if (where.empty()) {
        sql.chop(6);
    }
    else
    {
        sqlwhere = sqlwhere.mid(5);
        sql += sqlwhere;
    }
    m_query->prepare(sql);
    QSqlRecord rec = m_query->record(); 
    if (Query(string((const char *)sql.toLocal8Bit()), ret))
    {
        if (m_query->next())
        {
           return m_query->value(0).toInt();
        }
    }
    return 0;
}
 