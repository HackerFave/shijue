#define BUILD_DLL 
#include "mrun_sqlmethod.h"
#include "mrun_messagebox.h"
#include "mrun_sqloperation_define.h"
#include "SqlMethod.h"
#include <qdebug.h>
sqlOperation* p_sqloperation = Q_NULLPTR;
std::list<std::string> tableidname_;//字段
std::list<std::string> fieldoperator_;//字段描述
QSettings *configIni_;
std::string currentpro_;
std::map<std::string, sqlOperation*> sqloperation_map_;
bool initIni()
{
	configIni_ = new QSettings(QString("%1/sqlsetting/prosql.ini").arg(QCoreApplication::applicationDirPath())
		, QSettings::IniFormat);
    QDir dir;
    dir.setPath(QString("%1/sqlsetting").arg(QCoreApplication::applicationDirPath()));
    if (!dir.exists()) {
        dir.mkpath(QString("%1/sqlsetting").arg(QCoreApplication::applicationDirPath()));
    }
    return true;

}
//数据写入
 bool MRUN_Sql_SetSqlProData( const std::vector<std::map<std::string, std::string>> &mapdata)
 {
     
      std::string table = "test_record"+ currentpro_;
     QSqlQuery *ret = Q_NULLPTR;
     if (p_sqloperation->Add(table, mapdata, &ret)) {
         return true;
    }
     return false;
 }
//用户配置数据写入
 bool MRUN_Sql_SetSqlProDataUser(const std::map<std::string, std::vector<std::map<std::string, std::string>>> &mapdata)
 {
     std::map<std::string, std::vector<std::map<std::string, std::string>>>::const_iterator iter;
     for (iter = mapdata.cbegin(); iter!= mapdata.cend(); iter++)
     {
         QSqlQuery *ret = Q_NULLPTR;
         if (p_sqloperation->Add(iter->first, iter->second, &ret)) {
             return true;
        }
     }
     return false;
 }
 //判断一条记录是否存在
 bool  MRUN_Sql_IsExists(std::string table_name, std::string key, std::string value)
 {
     if (p_sqloperation) {
         QSqlQuery *ret = Q_NULLPTR;
         if (p_sqloperation->isExists(table_name, key, value, &ret)) {
             return true;
         }
     }
         return false;
 }
 bool MRUN_Sql_Updata(std::string table_name, std::map<std::string, std::string> where, std::map<std::string, std::string> data)
 {
     if (p_sqloperation) {
         QSqlQuery *ret = Q_NULLPTR;
         return p_sqloperation->Updata(table_name, where, data, &ret);
     }
     return false;
 }
 sqlOperation* MRUN_Sql_InitSqlConnect(const std::string proname)
 {
     currentpro_ = proname;
     p_sqloperation = SqlMethod::sqlmethod_instance()->getsqloperationinstance(proname);
     sqloperation_map_[proname] = p_sqloperation;
     return p_sqloperation;
 }
 bool  MRUN_Sql_SetCurrentConnect(const std::string contype, const std::string proname)
 {
      currentpro_ = proname;
     if (sqloperation_map_[contype] != nullptr) {
         p_sqloperation = sqloperation_map_[contype];
         return true;
     }
     return false;
 }
 //返回字段描述
 bool MRUN_Sql_FindFieldOperatorSql(std::string table_name, std::string proname, std::list<std::string> &listresult)
 {
    
	 if (p_sqloperation) {
		 QSqlQuery *ret = Q_NULLPTR;
		 return p_sqloperation->findfieldoperator(table_name, proname, listresult, &ret);
	} 
     return false;
 }
 sqlOperation *MRUN_Sql_SqlPoint()
 {
     return p_sqloperation;
 }
 bool MRUN_Sql_FindDataListSql(std::string table_name, std::map<std::string, std::string> &where, const std::list<std::string>  &keyList, std::list<std::list<std::string>>  &row)
 {
     
	 if (p_sqloperation) {
		 QSqlQuery *ret = Q_NULLPTR;
		 return p_sqloperation->findData(table_name, where, keyList, row, &ret);
	 }
     return false;
 }
 bool MRUN_Sql_FinDataMapSql(std::string table_name, std::map<std::string, std::string> &where, std::list<std::string> &keyList, std::list<std::map<std::string, std::string>>&row)
 {
     
	 if (p_sqloperation) {
		 QSqlQuery *ret = Q_NULLPTR;
		 return p_sqloperation->findData(table_name, where, keyList, row, &ret);
	 }
     return false;
 }
 bool MRUN_Sql_FindTableIdNameSql(std::string table_name, std::string proname, std::list<std::string> &listresult)
 {
    
	 if (p_sqloperation) {
		 QSqlQuery *ret = Q_NULLPTR;
		 return p_sqloperation->findtableidname(table_name, proname, listresult, &ret);
	 }
     return false;
   
 }
 int MRUN_Sql_GetCountSizeSql(std::string table_name, std::map<std::string, std::string>  where)
 {
    
	 if (p_sqloperation) {
		 QSqlQuery *ret = Q_NULLPTR;
		 return p_sqloperation->getcountsize(table_name, where, &ret);
	 }
     return false;
 }
 bool find_add_filter_sql(std::string table_name, std::string id, int start_num, int end_num, const std::list<std::string> &keyList, std::list<std::string> listvalue, std::list<std::map<std::string, std::string>>&row)
 {
    
	 if (p_sqloperation) {
		 QSqlQuery *ret = Q_NULLPTR;
		 return p_sqloperation->Find(table_name, id, start_num, end_num, keyList, listvalue, row, &ret);
	 }
     return false;
 }
 bool find_normal_sql(std::string table_name, std::string id, int start_num, int end_num, const std::list<std::string> &keyList, std::list<std::map<std::string, std::string>>&row)
 {
     
	 if (p_sqloperation) {
		 QSqlQuery *ret = Q_NULLPTR;
		 return p_sqloperation->Find(table_name, id, start_num, end_num, keyList, row, &ret);
	 }
     return false;
 }
 bool MRUN_Sql_FindNoIdSql(std::string table_name, int start_num, int page_num, const std::list<std::string> &keyList, std::map<std::string,std::string> mapwhere, std::list<std::map<std::string, std::string>>&row)
 {
	 if (p_sqloperation) {
		 QSqlQuery *ret = Q_NULLPTR;
		 return p_sqloperation->Find(table_name, start_num, page_num, keyList, mapwhere, row, &ret);
	 }
     return false;
 }
 bool MRUN_Sql_GetProSqlList(std::list<std::string> &list)
 {
     initIni();
     //configIni_->clear();
     for (int i = 1; i <= configIni_->allKeys().size(); i++) {
         list.push_back(string((const char*)(configIni_->value(QString("pro/%1").arg(i)).toString().toLocal8Bit())));
     }
     if (list.empty()) {
         return false;
     }
     delete configIni_;
     return true;
 }
 bool  MRUN_Sql_GetCurrentProStr( std::string &str)
 {
	 if (!currentpro_.empty()) {
		 str = currentpro_;
		 return true;
	 }
     return false;
 }