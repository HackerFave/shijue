#pragma once

#include <QObject>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlError>
#include <QMap>
#include <map>
#include <QMutexLocker>
#include <list>
#include <qdebug.h>
class sqlOperation : public QObject
{
    Q_OBJECT

public:
    sqlOperation(QObject *parent);
    ~sqlOperation();
    static bool DatabaseIsOK();
    void CreateTable();
    bool Init(const QString &host, const QString &user, const QString &passwd, const QString &dataname,const QString & type);

    bool Query(const QString &sql, QSqlQuery **ret);
    //增加数据
    bool Add(QString table_name, std::map<QString, QString> data, QSqlQuery ** ret);
    //批量添加数据
    bool Add(QString table_name, std::vector<std::map<QString, QString>> data, QSqlQuery ** ret);
    //删除一条记录
    bool Del(QString table_name, std::map<QString, QString> where, QSqlQuery ** ret);
    bool delRecord(QString table_name, const QString &key, const QString &value, QSqlQuery ** ret);
    //更新数据
    bool Updata(QString table_name, std::map<QString, QString> where, std::map<QString, QString> data, QSqlQuery ** ret);
    //查找
    bool Find(QString table_name, std::list<QString> key, std::map<QString, QString> where, std::list<std::list<QString>> *row, QSqlQuery ** ret);
    bool Find(QString sql, std::list<std::list<QString>>* row, QSqlQuery ** ret, int len);
    bool Find(QString table_name, std::list<QString> key, std::list<std::list<QString>>* row, QSqlQuery ** ret);
    //关闭数据库
    void CloseDB();
    //判断该条记录是否存在
    int isExists(QString table_name, QString key, QString value, QSqlQuery ** ret);
    bool findData(QString table_name, QMap<QString, QString> &where, const QStringList &keyList, QList<QVariantList> &row, QSqlQuery ** ret);
    //查询最近n天数据
    bool findData_Date(QString table_name, const QStringList &keyList, QList<QVariantList> &row, int days, const QString &data_time_key, QSqlQuery ** ret);
    bool findData(QString table_name, QMap<QString, QString> &where, const QStringList &keyList, QList<QMap<QString, QString>>&row, QSqlQuery ** ret);
    bool findDataRecord(QString table_name, QMap<QString, QString> &where, const QStringList &keyList, std::vector<std::map<QString,QString>> &row, QSqlQuery ** ret);
    //获取最后一次的查询语句
    QString get_lastsql() { return m_sql; }
    void setCurrentDataStatus(const QString &record_id) {
        _recordID = record_id;
    }
    void setTestRecord(std::map<QString, QString> data);
    bool clearTable(QStringList listTableName, QSqlQuery ** ret);
private:
    QSqlDatabase	m_db;
    QSqlQuery     * m_query = nullptr;
    QString			m_dbhost;
    QString			m_dbuser;
    QString			m_dbpasswd;
    QString			m_dbdataname;
    QString			m_sql;
    QString _recordID;
    bool _isDoWork = true;
};
