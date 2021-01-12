#include "fenpIni.h"
#include "solution.h"
#include "qsettings"
#include "uicommon.h"
#include "qapplication.h"

FenpIni* FenpIni::g_fenpini = NULL;

FenpIni::FenpIni()
{
	m_isopen = false;
	m_isauto = false;
}


bool FenpIni::ReadIni()
{
	if (g_fenpini != NULL)
	{
		delete g_fenpini;
		g_fenpini = NULL;
	}
	g_fenpini = new FenpIni();
	return g_fenpini->FReadIni();
}

bool FenpIni::WriteIni(defenp & data)
{
	if(!g_fenpini)
		return false;
	return  g_fenpini->FWriteIni(data);
}

defenp FenpIni::GetData()
{
	if (!g_fenpini)
		return defenp();
	return g_fenpini->FGetData();
}

bool FenpIni::IsOpen()
{
	if (!g_fenpini)
		return false;
	return g_fenpini->FIsOpen();;
}

bool FenpIni::SetOpen(bool is)
{
	if (!g_fenpini)
		return false;
	return g_fenpini->FSetOpen(is);
}

int FenpIni::GetStationAtWin(int station)
{
	if (!g_fenpini)
		return -1;
	return g_fenpini->FGetStationAtWin(station);
}

void FenpIni::SetAutoOpen(bool is)
{
	if (!g_fenpini)
		return ;
	g_fenpini->FSetAutoOpen(is);
}

bool FenpIni::GetAutoOpen()
{
	if (!g_fenpini)
		return false;
	return g_fenpini->FGetAutoOpen();
}

FenpIni::~FenpIni()
{
}

bool FenpIni::FReadIni()
{
	if (!Solution::GetCurrentSolution())
		return false;
	m_staofwin.clear();
	m_inipathname = QCoreApplication::applicationDirPath() + QString(SOLUTION) + Solution::GetCurrentSolution()->get_solutionname() + '/' + QString(FENPINI);
	QSettings configini(m_inipathname, QSettings::IniFormat);
	// 获取所有节点
	QStringList strlist = configini.childGroups();
	int icount = strlist.count();
	//获取每个节点的数据
	//存入数据
	QStringList childlist;
	for (int i = 0; i < icount; i++)
	{
		if (!QString::compare(strlist.at(i), ISOPENCUSTOM))
			continue;
		if (!QString::compare(strlist.at(i), AUTOOPEN))
			continue;
		defenpwin wmap;
		configini.beginGroup(strlist.at(i));
		childlist = configini.allKeys();
		wmap.viewnum = configini.value(FWINDOWNUM).toInt();
		for (int j = 0; j < childlist.count(); j++)
		{
			if (!QString::compare(childlist.at(j), FWINDOWNUM))
				continue;
			int len = get_ilist().at(wmap.viewnum);
			int clen = childlist.at(j).toInt();
			if (clen < len)
			{
				wmap.dfw[clen] = configini.value(childlist.at(j)).toInt();
				if(wmap.dfw[clen]> -1)
					m_staofwin[wmap.dfw[clen]] = strlist.at(i).toInt();
			}
		}
	
		m_defenp[i] = wmap;
		wmap.dfw.clear();
		configini.endGroup();
	}
	
	configini.beginGroup(ISOPENCUSTOM);
	m_isopen = configini.value(COPEN).toBool();
	configini.endGroup();

	configini.beginGroup(AUTOOPEN);
	m_isauto = configini.value(COPEN).toBool();
	configini.endGroup();
	

	return true;
}

bool FenpIni::FWriteIni(defenp & data)
{

	m_defenp.clear();
	defenpwin fwin;
	auto iter = data.begin();
	while (iter!=data.end())
	{
		fwin.dfw.clear();
		fwin.viewnum = iter->second.viewnum;
		fwin.dfw = iter->second.dfw;
		m_defenp[iter->first] = fwin;
		iter++;
	}

	if(m_inipathname.isEmpty())
		return false;
	QSettings configini(m_inipathname, QSettings::IniFormat);


	for each (auto var in m_defenp)
	{
		configini.beginGroup(QString::number(var.first));
		
		for each (auto var in configini.allKeys())
		{
			configini.remove(var);
		}

			configini.setValue(FWINDOWNUM, var.second.viewnum);
		for each (auto var1 in var.second.dfw)
		{
			configini.setValue(QString::number(var1.first), var1.second);
		}
		configini.endGroup();
	}

	configini.beginGroup(ISOPENCUSTOM);
	configini.setValue(COPEN, m_isopen);
	configini.endGroup();

	configini.beginGroup(AUTOOPEN);
	configini.setValue(COPEN, m_isauto);
	configini.endGroup();
	
	return true;
}

defenp FenpIni::FGetData()
{
	return m_defenp;
}

bool FenpIni::FIsOpen()
{
	return m_isopen;
}

bool FenpIni::FSetOpen(bool is)
{
	m_isopen = is;
	return true;
}

int FenpIni::FGetStationAtWin(int station)
{
	if (m_staofwin.count(station) < 1)
		return -1;
	else
		return m_staofwin.at(station);
}

void FenpIni::FSetAutoOpen(bool is)
{
	m_isauto = is;
}

bool FenpIni::FGetAutoOpen()
{
	return m_isauto;
}

