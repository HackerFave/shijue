#pragma once
#include "mrun_common.h"
#include <map>
#include <vector>
#include <list>
class sqlOperation;
using namespace std;
extern"C"
{
    /***************************************************************
    *  @brief     数据写入（在线检测数据）
    *  @param    
    *  @note      
    *  @return	  成功：TRUE。失败：FALSE。
    **************************************************************/
    MRUN_DLL bool MRUN_Sql_SetSqlProData(const std::vector<std::map<std::string, std::string>> &mapdata);
     
    /***************************************************************
    *  @brief     数据写入（用户配置） 
    *  @param    
    *  @note      
    *  @return	  成功：TRUE。失败：FALSE。
    **************************************************************/
    MRUN_DLL bool MRUN_Sql_SetSqlProDataUser(const std::map<std::string, std::vector<std::map<std::string, std::string>>> &mapdata);
    
    /***************************************************************
    *  @brief     更新一条记录（）
    *  @param    
    *  @note     
    *  @return	  成功：TRUE。失败：FALSE。
    **************************************************************/
    MRUN_DLL bool MRUN_Sql_Updata(std::string table_name, std::map<std::string, std::string> where, std::map<std::string, std::string> data);
    
    /***************************************************************
    *  @brief     判断一条记录是否存在（）
    *  @param    
    *  @note     
    *  @return	  存在：TRUE。不存在：FALSE。
    **************************************************************/
    MRUN_DLL bool  MRUN_Sql_IsExists(std::string table_name, std::string key, std::string value);
    
    /***************************************************************
    *  @brief     获取当前可写入的数据库相关项目列表
    *  @param     list为返回的列表
    *  @note      
    *  @return	  成功：TRUE。失败：FALSE。
    **************************************************************/
    MRUN_DLL bool MRUN_Sql_GetProSqlList(std::list<std::string> &list);
    
    /***************************************************************
    *  @brief     获取当前操作的数据库指针（也可根据需要传入字符串返回一个新的实例,用返回的指针做）
    *  @param     proname为传入的项目名称（项目名称必须在数据库开发时做关联）
    *  @note     
    *  @return	成功：返回指定地址。失败：返回空指针。
    **************************************************************/
    MRUN_DLL sqlOperation*  MRUN_Sql_InitSqlConnect(const std::string proname);
    
    /***************************************************************
    *  @brief     设置当前操作的连接（必须在MRUN_Sql_InitSqlConnect之后，发走则返回空）
    *  @param     contype::创建不同的连接，字符串来区分，proname为传入的项目名称（项目名称必须在数据库开发时做关联）
    *  @note     
    *  @return	成功：true。失败：false。
    **************************************************************/
    MRUN_DLL bool  MRUN_Sql_SetCurrentConnect(const std::string contype, const std::string proname);

    /***************************************************************
    *  @brief     获取当前操作的项目名称
    *  @param    
    *  @note     
    *  @return	成功：返回字符串。失败：返回空。
    **************************************************************/
    MRUN_DLL bool MRUN_Sql_GetCurrentProStr(std::string &str);
   
    /***************************************************************
    *  @brief     查询字段描述
    *  @param    table_name：要查询的表格名称，proname:项目名称 listresult:结果集
    *  @note     
    *  @return	成功：true。失败：false。
    **************************************************************/
    MRUN_DLL bool MRUN_Sql_FindFieldOperatorSql(std::string table_name, std::string proname, std::list<std::string> &listresult);
    
    /***************************************************************
    *  @brief     返回一个MRUN_Sql_InitSqlConnect（）之后的实例指针
    *  @param    
    *  @note     
    *  @return	成功：指针地址。失败：空。
    **************************************************************/
    MRUN_DLL sqlOperation * MRUN_Sql_SqlPoint();

    /***************************************************************
    *  @brief     数据查询
    *  @param    table_name:表名 where:匹配字段，条件查询 keyList:需要查询的字段 row：结果集
    *  @note     
    *  @return	成功：true。失败：false。
    **************************************************************/
    MRUN_DLL bool MRUN_Sql_FindDataListSql(std::string table_name, std::map<std::string,std::string> &where, const std::list<std::string> &keyList, std::list<std::list<std::string>> &row);
    
    /***************************************************************
    *  @brief     数据查询
    *  @param    table_name:表名 where:匹配字段，条件查询 keyList:需要查询的字段 row：结果集
    *  @note     
    *  @return	成功：true。失败：false。
    **************************************************************/
    MRUN_DLL bool MRUN_Sql_FinDataMapSql(std::string table_name, std::map<std::string, std::string> &where,  std::list<std::string> &keyList, std::list<std::map<std::string, std::string>>&row);

    /***************************************************************
     *  @brief        //查询一张表中的字段名
     *  @param    table_name:表名 proname:项目名称，listresult：结果集
     *  @note     
     *  @return	成功：true。失败：false。
     **************************************************************/
    MRUN_DLL bool MRUN_Sql_FindTableIdNameSql(std::string table_name, std::string proname, std::list<std::string> &listresult);
    
    /***************************************************************
    *  @brief        查询总的记录数 包含筛选条件
    *  @param    table_name:表名 where：条件匹配
    *  @note     
    *  @return	返回记录数
    **************************************************************/
    MRUN_DLL int MRUN_Sql_GetCountSizeSql(std::string table_name, std::map<std::string, std::string> where);
    
    /***************************************************************
    *  @brief         //查询指定条记录（没有自增id的情况）
    *  @param    table_name:表名 id：字段名 start_num：开始于第几条 page_num：每一页的记录条数 keyList：查询的字段 mapwhere：条件匹配 row：结果集
    *  @note     
    *  @return	返回记录数
    **************************************************************/
    MRUN_DLL bool MRUN_Sql_FindNoIdSql(std::string table_name, int start_num, int page_num, const std::list<std::string> &keyList, std::map<std::string, std::string> mapwhere, std::list<std::map<std::string, std::string>>&row);
   
}
