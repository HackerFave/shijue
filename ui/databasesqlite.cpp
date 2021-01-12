#include "databasesqlite.h"
#include "qlist.h"
#include "qdebug.h"
#include "qcoreapplication.h"

DatabaseSqlite * DatabaseSqlite::g_databasesqlite = NULL;
DatabaseSqlite::DatabaseSqlite()
{
	g_databasesqlite = NULL;
}


DatabaseSqlite::~DatabaseSqlite()
{
	m_db.close();
	if (m_query != NULL)
	{
		delete m_query;
		m_query = NULL;
	}
}
DatabaseSqlite * DatabaseSqlite::get_db()
{
	if (g_databasesqlite == NULL)
		g_databasesqlite = new DatabaseSqlite();
	return g_databasesqlite;
}

bool DatabaseSqlite::DatabaseIsOK()
{
	if (g_databasesqlite == NULL)
		return false;

	if (g_databasesqlite->m_query == NULL)
		return false;

	return true;
}

bool DatabaseSqlite::Init(const QString & host, const QString & user, const QString & passwd, const QString & dataname)
{
	if (m_db.isValid())//存在连接直接退出
	{
		return false;
	}

	m_dbhost = host;
	m_dbuser = user;
	m_dbpasswd = passwd;
	m_dbdataname = dataname;
	//m_dbdataname = QCoreApplication::applicationDirPath() + '/' + dataname;

	m_db = QSqlDatabase::addDatabase("QMYSQL","cicai");
	m_db.setHostName(m_dbhost);							//数据库服务器IP
	m_db.setUserName(m_dbuser);							//数据库用户名
	m_db.setPassword(m_dbpasswd);						//数据库密码
	
	//QString str = QString("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)}; FIL={MS Access};DBQ=%1;").arg(m_dbdataname);
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

bool DatabaseSqlite::Query(const QString & sql, QSqlQuery ** ret)
{
	if (m_query == NULL)
		return false;
	bool isok = m_query->exec(sql);
	m_sql = sql;
	//QString a = m_query->value(1).toString();
	*ret = m_query;
	if (isok == false) {
		return false;
	}
	return true;
}


bool DatabaseSqlite::Add(QString table_name, std::map<QString, QString> data, QSqlQuery ** ret)
{
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

bool DatabaseSqlite::Del(QString table_name, std::map<QString, QString> where, QSqlQuery ** ret)
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

bool DatabaseSqlite::Updata(QString table_name, std::map<QString, QString> where, std::map<QString, QString> data, QSqlQuery ** ret)
{
	if (m_query == NULL)
		return false;
	QString sql = "update " + table_name + " set ";
	for (std::map<QString, QString>::const_iterator i = data.cbegin(); i != data.cend(); i++)
	{
		sql += i->first + "=";
		sql +="'"+ i->second + "',";
		
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

bool DatabaseSqlite::Find(QString table_name, std::list<QString> key, std::list<std::list<QString>>* row, QSqlQuery ** ret)
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

void DatabaseSqlite::CloseDB()
{
	m_db.close();
}
bool DatabaseSqlite::Find(QString sql, std::list<std::list<QString> > *row, QSqlQuery ** ret, int len)
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


					
bool DatabaseSqlite::Find(QString table_name, std::list<QString> key, std::map<QString, QString> where, std::list<std::list<QString> > *row, QSqlQuery ** ret)
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
		sql += i->first + " = '" + i->second +"'"+ " AND ";
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
