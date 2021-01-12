#pragma once
#include "qdatetime.h"
#include "qstring.h"
#include <map>
enum TIMEID { T_INICONFIG, T_ALGINIT, T_UI, T_PLC, T_CALLBACK, T_GRADEFILTER, T_ALG, T_ALG_F_B, T_ALG_S_P, T_ALG_P, T_SAVEIMG, T_AUTOSAVE, T_NGFLAG, T_SYSTEM, T_BTNJIANCHE,T_BTNSYSTEM, T_ADDDATABASE,T_CHANGE_SOLUTION,_MAX};

class DebugPROC
{
public:
	~DebugPROC();
	static void StartTime();
	static void EndTime(TIMEID id);
	static void EndTime(QTime& dtime, TIMEID id);
	//设置耗时
	static void SetRunTime(TIMEID id,int data);
	//获得耗时
	static int GetRunTime(TIMEID id);
	//buf
	static QString GetTextBUff();
private: 
	std::map<int, int> m_debugtime;
	QTime *time;
	DebugPROC();
};

