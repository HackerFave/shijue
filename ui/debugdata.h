#pragma once
#include <stdlib.h>
#include <qstring.h>
#include <map>
#include <vector>
#include "qstringlist.h"
//int ����λ ��string����λ
typedef std::map<int, std::map<std::string, std::vector<float>>> dedata;

class DebugData
{
public:
 //��ʼ�����Ҷ�ȡ
	static bool Init();
 //д�������ļ�	
	static bool SaveIni();
//��ȡ����
	static dedata GetData();
	static std::vector<float> GetData(int station, int grade);
//��������
	static bool SetData(dedata data);
//��ȡ��ǰָ��
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

