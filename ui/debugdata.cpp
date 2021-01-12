#include "debugdata.h"
#include "qapplication.h"
#include "uicommon.h"
#include "solution.h"
#include "qsettings.h"

DebugData* g_debugdata = NULL;
DebugData::DebugData()
{
	m_inipathname = QCoreApplication::applicationDirPath() + QString(SOLUTION) + Solution::GetCurrentSolution()->get_solutionname() + '/' + QString(THRESHOLDNAME);
	m_debugdatamap.clear();
	
	gearlist << GEAR0 << GEAR1 << GEAR2 << GEAR3 << GEAR4;
}


bool DebugData::Init()
{
	if (g_debugdata == NULL)
	{
		g_debugdata = new DebugData();
	}
	g_debugdata->m_debugdatamap.clear();

	std::vector<float> flist(4,0.0);
	std::map<std::string,std::vector<float>> gearmap;
	g_debugdata->m_inipathname = QCoreApplication::applicationDirPath() + QString(SOLUTION) + Solution::GetCurrentSolution()->get_solutionname() + '/' + QString(THRESHOLDNAME);

	QStringList list;


	int station = Solution::GetCurrentSolution()->get_station();

	//¶ÁÅäÖÃÎÄ¼þ
	QSettings configini(g_debugdata->m_inipathname, QSettings::IniFormat);
	for (int i = 0; i < station; i++)
	{
		configini.beginGroup(QString::number(i));
		for (int j = 0; j < g_debugdata->gearlist.count(); j++)
		{
			flist.clear();
			list = configini.value(g_debugdata->gearlist[j]).toString().split(',');
			for (size_t z = 0; z < list.count(); z++)
			{
				if (list.count() < 4)
					list << "0";
				if (list[z].isEmpty())
					list[z] = "0";

				flist.push_back(list[z].toFloat());
			}
			gearmap[g_debugdata->gearlist[j].toStdString()] = flist;
		}

		g_debugdata->m_debugdatamap[i] = gearmap;
		configini.endGroup();
	}

	return true;
}

bool DebugData::SaveIni()
{
	if (g_debugdata == NULL)
		return false;
	if (g_debugdata->m_debugdatamap.size() < 1)
		return false;

	QString str;
	QSettings configini(g_debugdata->m_inipathname, QSettings::IniFormat);
	for (auto iter = g_debugdata->m_debugdatamap.cbegin(); iter != g_debugdata->m_debugdatamap.cend(); iter++)
	{
		configini.beginGroup(QString::number(iter->first));
		for (auto iter2 = iter->second.cbegin(); iter2 != iter->second.cend(); iter2++)
		{
			str = "";
			for (int i= 0; i < iter2->second.size(); i++)
			{
				str.append(QString::number(iter2->second[i], 'f', 2));
				str.append(',');

			}
			str.chop(1);
			configini.setValue(QString::fromLocal8Bit(iter2->first.c_str()), str);
		}
		configini.endGroup();
	}
	return true;
}

dedata DebugData::GetData()
{
	if (g_debugdata == NULL)
		return dedata();
	else
		return dedata(g_debugdata->m_debugdatamap);
}

std::vector<float> DebugData::GetData(int station, int grade)
{
	if (g_debugdata == NULL)
		return std::vector<float>();
	
	std::string str = g_debugdata->gearlist[grade].toStdString();
	return std::vector<float>(g_debugdata->m_debugdatamap.at(station).at(str));
}



bool DebugData::SetData(dedata data)
{
	if (g_debugdata == NULL)
		return false;
	g_debugdata->m_debugdatamap.clear();
	g_debugdata->m_debugdatamap = data;
	return SaveIni();

}

DebugData* DebugData::GetPDebugData()
{
	if (g_debugdata != NULL)
		return g_debugdata;
	return NULL;
}

DebugData::~DebugData()
{
}
