#pragma once
#include <map>
#include <vector>
#include <stdlib.h>
#include "qstring.h"
#include "qstringlist.h"
#include "qlist.h"
struct defenpwin {

	int viewnum;
	std::map<int, int> dfw;
};

typedef std::map<int, defenpwin> defenp;

class FenpIni
{
public:
	//读取
	static bool ReadIni();
	//保存
	static bool WriteIni(defenp& data);
	//数据
	static defenp GetData();
	//是否开启自定义
	static bool IsOpen();
	//设置开启/关闭
	static bool SetOpen(bool is);
	//工位所属于的屏幕
	static int GetStationAtWin(int station);
	//设置开机自动打开分屏
	static void SetAutoOpen(bool is);
	//打开开机自动打开分屏
	static bool GetAutoOpen();



	static QStringList get_slist() { 
		QStringList str;
	str << "1" << "2" << "4" << "6" << "8" << "12" << "16" << "20" << "24";
	return str;
	}

	static QList<int> get_ilist() {
		QList<int> str;
		str << 1 << 2 << 4 << 6 << 8 << 12 << 16 << 20 << 24;
		return str;
	}

	~FenpIni();

private:
	static FenpIni *g_fenpini;

private:
	QString m_inipathname;
	FenpIni();
	//读取
	bool FReadIni();
	//保存
	bool FWriteIni(defenp& data);
	//数据
	defenp FGetData();
	//是否开启自定义
	bool FIsOpen();
	//设置开启/关闭
	bool FSetOpen(bool is);
	//工位所属于的屏幕
	int FGetStationAtWin(int station);
	//设置开机自动打开分屏
	void FSetAutoOpen(bool is);
	//打开开机自动打开分屏
	bool FGetAutoOpen();
private:
	//自动加载自定义
	bool m_isopen;
	//开机自动打开分屏
	bool m_isauto;

	std::map<int, int> m_staofwin;

	defenp m_defenp;
	
	
};

