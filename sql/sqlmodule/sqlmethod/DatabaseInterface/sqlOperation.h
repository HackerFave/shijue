#pragma once
#include "mrun_common.h"
#include <vector>
#include <list>
#include <map>
class QSqlQuery;
class QObject;
class QSqlQuery;
class QSqlDatabase;
class QSqlRecord;
class QSqlError;
using namespace std;
class /*MRUN_DLL*/ sqlOperation
{
public:
    sqlOperation(std::string type);
    ~sqlOperation();
    static bool DatabaseIsOK();
    void CreateTable();
    bool Init(const std::string &host, const std::string &user, const std::string &passwd, const std::string &dataname);

    bool Query(const std::string &sql, QSqlQuery **ret);
    //增加数据
    bool Add(std::string table_name, std::map<std::string, std::string> data, QSqlQuery ** ret);
    //批量添加数据
    bool Add(std::string table_name, std::vector<std::map<std::string, std::string>> data, QSqlQuery ** ret);
    //删除一条记录
    bool Del(std::string table_name, std::map<std::string, std::string> where, QSqlQuery ** ret);
    bool delRecord(std::string table_name, const std::string &key, const std::string &value, QSqlQuery ** ret);
    //更新数据
    bool Updata(std::string table_name, std::map<std::string, std::string> where, std::map<std::string, std::string> data, QSqlQuery ** ret);
    //查找
    bool Find(std::string table_name, std::list<std::string> key, std::map<std::string, std::string> where, std::list<std::list<std::string>> *row, QSqlQuery ** ret);
    bool Find(std::string sql, std::list<std::list<std::string>>* row, QSqlQuery ** ret, int len);
    bool Find(std::string table_name, std::list<std::string> key, std::list<std::list<std::string>>* row, QSqlQuery ** ret);
   
    //关闭数据库
    void CloseDB();
    //判断该条记录是否存在
    int isExists(std::string table_name, std::string key, std::string value, QSqlQuery ** ret);
    bool findData(std::string table_name, std::map<std::string, std::string> &where, const std::list<std::string> &keyList, /*QList<QVariantList>*/std::list<std::list<std::string>> &row, QSqlQuery ** ret);
    //查询最近n天数据
    bool findData_Date(std::string table_name, const std::list<std::string> &keyList, std::list<std::list<std::string>> &row, int days, const std::string &data_time_key, QSqlQuery ** ret);
    bool findData(std::string table_name, std::map<std::string, std::string>  &where, const std::list<std::string> &keyList, std::list<std::map<std::string, std::string>>&row, QSqlQuery ** ret);
    bool findDataRecord(std::string table_name, std::map<std::string, std::string> &where, const std::list<std::string> &keyList, std::vector<std::map<std::string, std::string>> &row, QSqlQuery ** ret);
    //查询某字段数据
    bool Find(std::string table_name, const std::list<std::string> &listkey, std::list<std::map<std::string, std::string>>&row, QSqlQuery ** ret);
    //查询指定条记录第十条到第一百条 id(字段)
    bool Find(std::string table_name, std::string id, int start_num, int end_num, const std::list<std::string>  &keyList, std::list<std::map<std::string, std::string>>&row, QSqlQuery**ret);
    //查询指定条记录第十条到第一百条且满足筛选条件
    bool Find(std::string table_name, std::string id, int start_num, int end_num, const std::list<std::string> &keyList, std::list<std::string> listvalue, std::list<std::map<std::string, std::string>>&row, QSqlQuery**ret);
    //select * from test_record where dataID between 100 and 10000 and record_Id = '20201023143531685' 
    //查询指定条记录（没有自增id的情况）
    bool sqlOperation::Find(std::string table_name, int start_num, int end_num, const std::list<std::string> &keyList, std::map<std::string, std::string> mapwhere, std::list<std::map<std::string, std::string>>&row, QSqlQuery**ret);
    //获取最后一次的查询语句
    std::string get_lastsql() { return m_sql; }
   // void setCurrentDataStatus(const QString &record_id) {
     //   _recordID = record_id;
    //}
    //void setTestRecord(std::map<QString, QString> data);
    bool clearTable(std::list<std::string> listTableName, QSqlQuery ** ret);
    //查询一张表中的字段名
    bool findtableidname(std::string table_name, std::string proname, std::list<std::string> &listrow, QSqlQuery ** ret);
    //查询一张表中的字段描述
    bool findfieldoperator(std::string table_name, std::string proname,std::list<std::string> &listrow, QSqlQuery ** ret);
    //select count(*) from 表名 where 字段 = 'value'; 
    //查询总的记录数 包含筛选条件
    int getcountsize(std::string table_name, std::map<std::string, std::string> where, QSqlQuery ** ret);
private:
    static sqlOperation * p_sqloperation_;
    QSqlDatabase	*m_db = nullptr ;
    QSqlQuery     * m_query = nullptr;
    std::string			m_dbhost;
    std::string			m_dbuser;
    std::string			m_dbpasswd;
    std::string			m_dbdataname;
    std::string			m_sql;
    std::string _recordID;
    bool _isDoWork = true;
    std::string type_;
};
