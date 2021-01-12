#pragma once
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <map>
#include <list>


class DatabaseSqlite
{
public:
	~DatabaseSqlite();
private:
	DatabaseSqlite();

private:
	QSqlDatabase	m_db;
	QSqlQuery     * m_query = nullptr;
	QString			m_dbhost;
	QString			m_dbuser;
	QString			m_dbpasswd;
	QString			m_dbdataname;
	QString			m_sql;
	static DatabaseSqlite *g_databasesqlite;

public:
	static DatabaseSqlite* get_db();
	static bool DatabaseIsOK();
public:
	bool Init(const QString &host, const QString &user, const QString &passwd, const QString &dataname);

	bool Query(const QString &sql, QSqlQuery **ret);
	//��������
	bool Add(QString table_name, std::map<QString, QString> data,  QSqlQuery ** ret);
	//ɾ��һ����¼
	bool Del(QString table_name, std::map<QString, QString> where, QSqlQuery ** ret);
	//��������
	bool Updata(QString table_name, std::map<QString, QString> where, std::map<QString, QString> data, QSqlQuery ** ret);
	//����
	bool Find(QString table_name, std::list<QString> key, std::map<QString, QString> where, std::list<std::list<QString>> *row, QSqlQuery ** ret);
	bool Find(QString sql, std::list<std::list<QString>>* row, QSqlQuery ** ret,int len);
	bool Find(QString table_name, std::list<QString> key, std::list<std::list<QString>>* row, QSqlQuery ** ret);
	//�ر����ݿ�
	void CloseDB();
   



	//��ȡ���һ�εĲ�ѯ���
	QString get_lastsql() { return m_sql; }

};

