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
	//��ȡ
	static bool ReadIni();
	//����
	static bool WriteIni(defenp& data);
	//����
	static defenp GetData();
	//�Ƿ����Զ���
	static bool IsOpen();
	//���ÿ���/�ر�
	static bool SetOpen(bool is);
	//��λ�����ڵ���Ļ
	static int GetStationAtWin(int station);
	//���ÿ����Զ��򿪷���
	static void SetAutoOpen(bool is);
	//�򿪿����Զ��򿪷���
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
	//��ȡ
	bool FReadIni();
	//����
	bool FWriteIni(defenp& data);
	//����
	defenp FGetData();
	//�Ƿ����Զ���
	bool FIsOpen();
	//���ÿ���/�ر�
	bool FSetOpen(bool is);
	//��λ�����ڵ���Ļ
	int FGetStationAtWin(int station);
	//���ÿ����Զ��򿪷���
	void FSetAutoOpen(bool is);
	//�򿪿����Զ��򿪷���
	bool FGetAutoOpen();
private:
	//�Զ������Զ���
	bool m_isopen;
	//�����Զ��򿪷���
	bool m_isauto;

	std::map<int, int> m_staofwin;

	defenp m_defenp;
	
	
};

