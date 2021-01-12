#pragma once
#include <stdlib.h>
#include <qstring.h>
#include <map>
#include <vector>
#include "qstringlist.h"
//int ：机位 ，string，档位
typedef std::map<int, std::map<std::string, std::vector<float>>> dedata;

class DebugData
{
public:
 //初始化并且读取
	static bool Init();
 //写入配置文件	
	static bool SaveIni();
//读取数据
	static dedata GetData();
	static std::vector<float> GetData(int station, int grade);
//设置数据
	static bool SetData(dedata data);
//获取当前指针
	static DebugData* GetPDebugData();
public:
	~DebugData();


private:
	DebugData();

private:
	QString m_inipathname;
	dedata m_debugdatamap;
	QStringList gearlist;

};

