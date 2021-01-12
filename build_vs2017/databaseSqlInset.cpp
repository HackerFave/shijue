#include "databaseSqlInset.h"

databaseSqlInset * databaseSqlInset::P_databaseSqlInset = NULL;
databaseSqlInset::databaseSqlInset()
{

}
databaseSqlInset::~databaseSqlInset()
{
    m_db.close();
    if (m_query != NULL)
    {
        delete m_query;
        m_query = NULL;
    }
}
databaseSqlInset * databaseSqlInset::get_db()
{
    if (P_databaseSqlInset == NULL)
        P_databaseSqlInset = new databaseSqlInset();
    return P_databaseSqlInset;
}

bool databaseSqlInset::DatabaseIsOK()
{
    if (P_databaseSqlInset == NULL)
        return false;

    if (P_databaseSqlInset->m_query == NULL)
        return false;

    return true;
}

bool databaseSqlInset::Init(const QString & host, const QString & user, const QString & passwd, const QString & dataname)
{
    QString sql = QString("CREATE DATABASE %1;").arg(dataname);
    QSqlQuery *ret;
    Query(sql, &ret);
    if (m_db.isValid())//存在连接直接退出
    {
        return false;
    }

    m_dbhost = host;
    m_dbuser = user;
    m_dbpasswd = passwd;
    m_dbdataname = dataname;
    //m_dbdataname = QCoreApplication::applicationDirPath() + '/' + dataname;

    m_db = QSqlDatabase::addDatabase("QMYSQL","zhongji");
    m_db.setHostName(m_dbhost);							//数据库服务器IP
    m_db.setUserName(m_dbuser);							//数据库用户名
    m_db.setPassword(m_dbpasswd);						//数据库密码

    //QString str = QString("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)}; FIL={MS Access};DBQ=%1;").arg(m_dbdataname);
    //m_db.setDatabaseName(str);
    m_db.setConnectOptions("MYSQL_OPT_RECONNECT = 1");  //add 设置mysql数据断开后自动重连 [WangPing 2020/3/20]	
    m_db.setDatabaseName(m_dbdataname);
    if (!m_db.open()) {
        m_db.setConnectOptions();	//add 链接失败时清除设置 [WangPing 2020/3/20]
        QSqlError err = m_db.lastError();
        qDebug() << err.databaseText();
        return false;
    }
    //	qDebug() << QSqlDatabase::drivers();
    m_query = new QSqlQuery(m_db);
    return true;
}

bool databaseSqlInset::Query(const QString & sql, QSqlQuery ** ret)
{
    if (m_query == NULL)
        return false;
    bool isok = m_query->exec(sql);
    m_sql = sql;
    QString a = m_query->value(1).toString();
    *ret = m_query;
    if (isok == false) {
        return false;

    }
    return true;
}


bool databaseSqlInset::Add(QString table_name, std::map<QString, QString> data, QSqlQuery ** ret)
{
    
    /*if (data.count("record_ID")) {
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
    QString sql = "insert into " + table_name + "(";
    QString values = " values(";
    for (std::map<QString, QString>::const_iterator i = data.cbegin(); i != data.cend(); i++)
    {
        sql += i->first + ", ";
        values += "'" + i->second + "', ";

    }
    sql.chop(2);
    values.chop(2);
    sql += ")";
    values += ")";
    sql += values;
    m_query->prepare(sql);
    bool is = Query(sql, ret);

    return is;
}

bool databaseSqlInset::Del(QString table_name, std::map<QString, QString> where, QSqlQuery ** ret)
{
    if (m_query == NULL)
        return false;
    QString sql = "delete from ";
    sql += table_name;
    sql += " where ";
    for (std::map<QString, QString>::const_iterator i = where.cbegin(); i != where.cend(); i++)
    {
        sql += i->first + "=";
        sql += "'" + i->second + "'and ";
    }
    sql.chop(4);
    m_query->prepare(sql);
    return Query(sql, ret);
}
//删除一条记录
bool databaseSqlInset::delRecord(QString table_name, const QString &key, const QString &value, QSqlQuery ** ret)
{ 
    QString sql = QString("delete from %1 where %2 = '%3'")
        .arg(table_name)
        .arg(key)
        .arg(value);
    m_query->prepare(sql);
    return Query(sql, ret);
}

bool databaseSqlInset::Updata(QString table_name, std::map<QString, QString> where, std::map<QString, QString> data, QSqlQuery ** ret)
{
    if (m_query == NULL)
        return false;
    QString sql = "update " + table_name + " set ";
    for (std::map<QString, QString>::const_iterator i = data.cbegin(); i != data.cend(); i++)
    {
        sql += i->first + "=";
        sql += "'" + i->second + "',";

    }
    sql.chop(1);
    sql += "where ";
    for (std::map<QString, QString>::const_iterator i = where.cbegin(); i != where.cend(); i++)
    {
        sql += i->first + "=";
        sql += "'" + i->second + "'and ";
    }
    sql.chop(4);
    return Query(sql, ret);
}

bool databaseSqlInset::Find(QString table_name, std::list<QString> key, std::list<std::list<QString>>* row, QSqlQuery ** ret)
{
    if (m_query == NULL)
        return false;
    QString sql = "SELECT ";
    int len = key.size();

    for (int i = 0; i < len; i++)
    {
        sql += key.front();
        sql += ",";
        key.pop_front();
    }
    sql.chop(1);
    sql += " FROM " + table_name;

    m_query->prepare(sql);
    if (Query(sql, ret))
    {
        while (m_query->next())
        {
            std::list<QString> j;
            for (int i = 0; i < len; i++)
            {
                j.push_back(m_query->value(i).toString());
            }
            row->push_back(j);
        }
        return true;
    }
    else
        return false;
}

void databaseSqlInset::CloseDB()
{
    m_db.close();
}
bool databaseSqlInset::Find(QString sql, std::list<std::list<QString> > *row, QSqlQuery ** ret, int len)
{
    m_query->prepare(sql);
    if (Query(sql, ret))
    {
        while (m_query->next())
        {
            std::list <QString> j;
            for (int i = 0; i < len; i++)
            {
                j.push_back(m_query->value(i).toString());

            }
            row->push_back(j);
        }
        return true;
    }
    else
        return false;
}



bool databaseSqlInset::Find(QString table_name, std::list<QString> key, std::map<QString, QString> where, std::list<std::list<QString> > *row, QSqlQuery ** ret)
{
    if (m_query == NULL)
        return false;
    QString sql = "SELECT ";
    int len = key.size();
    for (int i = 0; i < len; i++)
    {
        sql += key.front();
        sql += ",";
        key.pop_front();
    }
    sql.chop(1);
    sql += " FROM " + table_name;
    sql += " WHERE ";
    for (std::map<QString, QString>::const_iterator i = where.cbegin(); i != where.cend(); i++)
    {
        sql += i->first + " = '" + i->second + "'" + " AND ";
    }
    sql.chop(5);
    //qDebug()<<sql;
    m_query->prepare(sql);
    if (Query(sql, ret))
    {
        while (m_query->next())
        {
            std::list <QString> j;
            for (int i = 0; i < len; i++)
            {
                j.push_back(m_query->value(i).toString());

            }
            row->push_back(j);
        }
        return true;
    }
    else
        return false;
}
//判断该条记录是否存在
int databaseSqlInset::isExists(QString table_name, QString key, QString value, QSqlQuery ** ret)
{
    QString sql = QString("select * from %1 where %2 = '%3'")
        .arg(table_name)
        .arg(key)
        .arg(value);
    m_query->prepare(sql);
    if (Query(sql, ret))
    {
        if (!m_query->next()) {
            return 0;
       }
        else
        {
            return 1;
        }
       
    }
}
bool databaseSqlInset::findData(QString table_name, QMap<QString, QString> &where, const QStringList &keyList, QList<QVariantList> &row, QSqlQuery ** ret)
{
   // QMap<QString, QString> map = where;
  if (where.count("record_ID")&&where.value("record_ID").simplified().isEmpty()) {
        where["record_ID"] = _recordID;
    }
    QString sql = QString("select * from %1 where ")
        .arg(table_name);
    int keySize = where.size();
    int num = 0;
    int keyNum = keyList.size();
    QMap<QString, QString>::iterator it;
    for (it = where.begin(); it != where.end(); it++) {
        num++;
        sql += it.key();
        sql += " = ";
        sql += "'" + it.value() + "'";
        if (num > keySize - 1) {
            break;
        }
        sql += " and ";
    }
    m_query->prepare(sql);
    QSqlRecord rec = m_query->record();
    //int keyNum = keyList.size();
    if (Query(sql, ret))
    {
        while (m_query->next())
        {
            QVariantList listChild;
            rec = m_query->record();

            /*QMap<QString, QString>::iterator itKey;
            for (itKey = where.begin(); itKey != where.end(); itKey++) {
                QString value = m_query->value(rec.indexOf(itKey.key().simplified())).toString();
                listChild.append(value);
            }*/
            for (int i = 0; i < keyNum; i++) {
                QString value = m_query->value(rec.indexOf(keyList.at(i).simplified())).toString();
                listChild.append(value);
            }
            row.append(listChild);
        }
        return true;
    }
    else
        return false;

}
bool databaseSqlInset::findData(QString table_name, QMap<QString, QString> &where, const QStringList &keyList, QList<QMap<QString, QString>>&row, QSqlQuery ** ret)
{
    QString sql = QString("select * from %1 where ")
        .arg(table_name);
    int keySize = where.size();
    int num = 0;
    int keyNum = keyList.size();
    QMap<QString, QString>::iterator it;
    for (it = where.begin(); it != where.end(); it++) {
        num++;
        sql += it.key();
        sql += " = ";
        sql += "'" + it.value() + "'";
        if (num > keySize - 1) {
            break;
        }
        sql += " and ";
    }
    m_query->prepare(sql);
    QSqlRecord rec = m_query->record();
    //int keyNum = keyList.size();
    if (Query(sql, ret))
    {
        while (m_query->next())
        {
            QMap<QString,QString> mapChild;
            rec = m_query->record();

            /*QMap<QString, QString>::iterator itKey;
            for (itKey = where.begin(); itKey != where.end(); itKey++) {
                QString value = m_query->value(rec.indexOf(itKey.key().simplified())).toString();
                listChild.append(value);
            }*/
            for (int i = 0; i < keyNum; i++) {
                QString value = m_query->value(rec.indexOf(keyList.at(i).simplified())).toString();
                mapChild.insert(keyList.at(i), value);
            }
            row.append(mapChild);
        }
        return true;
    }
    else
        return false;
}
//查询最近days天数据
bool databaseSqlInset::findData_Date(QString table_name, const QStringList &keyList, QList<QVariantList> &row,int days,const QString &data_time_key, QSqlQuery ** ret)
{
    QString sql = QString("select * from %1 where DATE_SUB(CURDATE(), INTERVAL %2 DAY) <= date(%3)")
        .arg(table_name)
        .arg(days)
        .arg(data_time_key);
    m_query->prepare(sql);
    QSqlRecord rec = m_query->record();
    int keySize = keyList.size();
    if (Query(sql, ret))
    {
        while (m_query->next())
        {
            QVariantList listChild;
            rec = m_query->record();
            for (int i = 0; i < keySize; i++) {
                QString value = m_query->value(rec.indexOf(keyList.at(i).simplified())).toString();
                listChild.append(value);
            }
            row.append(listChild);
        }
        return true;
    }
    else
        return false;
}
void databaseSqlInset::CreateTable()
{
    QString dataBase = "CREATE DATABASE zhongji;";
    QString daStr =   "use zhongji;";
    QString str = "source d:\\zhongji.sql";
    QString sql = "CREATE TABLE t1(id VARCHAR(100) not null primary key,name VARCHAR(100)); ";
}
void databaseSqlInset::setTestRecord(std::map<QString, QString> data)
{
    QString table = "test_record";
    QSqlQuery * ret;
    databaseSqlInset::get_db()->Add(table, data, &ret);
}