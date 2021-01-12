#include "solution.h"
#include "qsettings.h"
#include "uicommon.h"
#include "qcoreapplication.h"
#include "qfileinfo.h"
#include "qdir.h"
#include <opencv2/opencv.hpp>

std::vector<QString> Solution::g_solution;
Solution *s_solution = NULL;
Solution::Solution()
{
	m_station = 0;
	m_group = 0;
	m_ngcnt = 0;
	m_okcnt = 0;
    m_solutionname = "defaule";
}
Solution::Solution(int station,int group)
{
	m_station = station;
	m_group = group;
	m_ngcnt = 0;
	m_okcnt = 0;
    m_solutionname = "defaule";
}
Solution::~Solution()
{
}

bool Solution::LoadIni()
{
	g_solution.clear();
	QString path = QCoreApplication::applicationDirPath() + QString(SOLUTION);
	QDir dir(path);
	QFileInfoList list = dir.entryInfoList();
	int file_count = list.count();
	if (file_count <= 0)
	{

		return false;
	}
	for (int i = 0; i < list.count(); i++)
	{
		QFileInfo file_info = list.at(i);
		QString fileName = file_info.fileName();
		if ((QString::compare(fileName, QString("."), Qt::CaseInsensitive) == 0) || (QString::compare(fileName, QString(".."), Qt::CaseInsensitive) == 0))
			continue;
		QString solutionname = file_info.fileName();
		
		g_solution.push_back(solutionname);
	}
	return true;
}

Solution * Solution::GetCurrentSolution()
{
	return s_solution != NULL ? s_solution : NULL;
}

void Solution::SetCurrentSolution(QString path)
{
    if (s_solution != NULL)
    {
        delete s_solution;
        s_solution = NULL;
    }
    Solution *solution = new Solution;
    s_solution = solution;
    if (s_solution->ReadIniFile(path))
    {
        solution->m_inipath = QCoreApplication::applicationDirPath() + QString(SOLUTION) + path + '/' + QString(ININAME);
        solution = NULL;
    }
}



bool Solution::ReadIniFile(QString inifile)
{
	m_virtstation.clear();
	m_solutionname = inifile;
	if (m_solutionname.isEmpty())
		return false;
	QString path = QCoreApplication::applicationDirPath() + QString(SOLUTION) + m_solutionname + '/' + QString(ININAME);
	//判断是否存在
    QFileInfo fileInfo(path);
    if (!fileInfo.isFile()) {
        return false;
    }
    
    QSettings configini(path, QSettings::IniFormat);
	configini.beginGroup(SOLUTION_);
	m_station = configini.value("station").toInt();
	m_group = configini.value("group").toInt();

	configini.endGroup();
	if (m_station < 1)
		return false;
	QStringList strlist = configini.childGroups();
	if (strlist.length() < 1)
	{
		return false;
	}
	for (size_t i = 0; i < strlist.length(); i++)
	{
		Solution_Camera cameraini;
		QString key = strlist[i];
		if (key == "Solution") {
			continue;
		}
		configini.beginGroup(key);
		QString alg = configini.value(ALG).toString();
		if (alg.size()) {
			cameraini.set_alg(alg.split(','));
		}
		else {
			cameraini.set_alg(QStringList());
		}
		cameraini.set_frameid(configini.value(FRAMEID).toInt());
		cameraini.set_group(configini.value(SGROUP).toInt());
		cameraini.set_islast(configini.value(ISLAST).toBool());
		QStringList roi = configini.value(ROI).toString().split(',');
		cameraini.set_station(key.toInt());
		cameraini.set_cameraindex(configini.value(CAMERAID).toString());
		cameraini.set_virtdeviceid(configini.value(FRAMEID).toInt());
		cameraini.set_grade(configini.value(GRADE).toInt());
		cameraini.set_isused(configini.value(ISUSED).toBool());
        //读取相机类型
        cameraini.set_camtype(configini.value(CAMERATYPE).toInt());
        
		if (!m_virtstation.count(configini.value(CAMERAID).toString())) {
			m_virtstation[configini.value(CAMERAID).toString()] = 0;
		}
		m_virtstation[configini.value(CAMERAID).toString()]++;
		

		if (roi.count() < 2)
		{
			configini.endGroup();
			m_camera[key.toInt()] = cameraini;
			continue;
		}
		cameraini.set_roirect(roi.at(0).toFloat(), roi.at(1).toFloat(),roi.at(2).toFloat(), roi.at(3).toFloat());
		configini.endGroup();
		m_camera[key.toInt()] = cameraini;
	}

	for (auto item = m_camera.begin(); item != m_camera.end(); item++)
	{
		auto tmpstationid = item->first;
		Solution_Camera& solutioninfo = item->second;
		solutioninfo.set_virtdevicemax(m_virtstation[solutioninfo.get_cameraindex()]);
	}
	return true;;
	
}

bool Solution::WriteIniFile(QString inifile)
{
	m_solutionname = inifile;
	if (m_solutionname.isEmpty())
		return false;
	

	QString path = QCoreApplication::applicationDirPath() + QString(SOLUTION) + m_solutionname + '/' + QString(ININAME);
	QSettings configini(path, QSettings::IniFormat);
	configini.beginGroup("Solution");
	configini.setValue("station", QString::number(m_station));
	configini.setValue("group", QString::number(m_group));
	configini.endGroup();
	if (m_camera.size() == 0)
		return true;
	for (auto it = m_camera.begin(); it != m_camera.end(); it++)
	{
		configini.beginGroup(QString::number(it->first));
		configini.setValue(ISLAST, it->second.get_islast());
		configini.setValue(CAMERAID, it->second.get_cameraindex());
		configini.setValue(SGROUP, it->second.get_group());
		configini.setValue(GRADE, it->second.get_grade());
		configini.setValue(ISUSED, it->second.get_isused());
		QString alg ;
		QStringList alglist = it->second.get_alg();
		for (size_t j = 0; j < alglist.count(); j++)
		{
			alg = alglist[j] + ',';
		}
		alg.chop(1);
		configini.setValue(ALG, alg);
		QString strroi;
		float* froi =it->second.get_roirect();
		strroi = QString::number(froi[0]) +','+ QString::number(froi[1]) +','+ QString::number(froi[2]) + ',' + QString::number(froi[3]);
		configini.setValue(ROI, strroi);
		configini.endGroup();
	}

	return false;
}

int Solution::isexist(int stationid) {
	return m_camera.count(stationid);
}

std::vector<std::string> Solution::getalgs(int stationid) {
	std::vector<std::string> alglist;
	QStringList qalglist = m_camera[stationid].get_alg();
	for (int i = 0; i < qalglist.size(); i++) {
		alglist.push_back(qalglist[i].toStdString());
	}
	return alglist;
}

cv::Rect2f Solution::getroi(int stationid) {
	cv::Rect2f cvrect;
	float* tmp = m_camera[stationid].get_roirect();
	cvrect.x = tmp[0];
	cvrect.y = tmp[1];
	cvrect.width = tmp[2] - tmp[0];
	cvrect.height = tmp[3] - tmp[1];
	return cvrect;
}

bool Solution::isused(int stationid)
{
	return m_camera[stationid].get_isused();
}

int Solution::getgrade(int stationid)
{
	return m_camera[stationid].get_grade();
}

int Solution::getgroupid(int stationid) {
	return m_camera[stationid].get_group();
}

int Solution::getstationid(std::string cameraname, int frameid) {
	std::vector<std::string> alglist;
	QString qdevice = QString::fromLocal8Bit(cameraname.c_str());
	for (auto item = m_camera.begin(); item != m_camera.end(); item++)
	{
		auto tmpstationid = item->first;
		Solution_Camera& tmpstationinfo = item->second;
		if ((qdevice == tmpstationinfo.get_cameraindex()
			&& (frameid % tmpstationinfo.get_virtdevicemax()) == tmpstationinfo.get_virtdeviceid()))
		{
			return tmpstationinfo.get_station();
		}
	}
	return -1;
}

std::vector<int> Solution::getstationlist(int groupid) {
	std::vector<int> stationids;
	for (auto item = m_camera.begin(); item != m_camera.end(); item++)
	{
		auto tmpstationid = item->first;
		auto stationsolution = item->second;
		if (groupid == stationsolution.get_group()) {
			stationids.push_back(tmpstationid);
		}
	}
	return stationids;
}

void Solution::setdeviceng(int stationid, bool isng) {
	m_camera[stationid].set_result(isng);
}

void Solution::resetcnt(void) {
	m_ngcnt = 0;
	m_okcnt = 0;
	
	for (auto item = m_camera.begin(); item != m_camera.end(); item++)
	{
		//item->second.resetcnt();
		item->second.reset_time();
	}
}

Solution_Camera& Solution::getcamera(int stationid) {
	return m_camera[stationid];
}

int Solution::getvirtstationmax(std::string cameraname) {
	QString Qcameraname = QString::fromLocal8Bit(cameraname.c_str());
	return m_virtstation[Qcameraname];
}

void Solution::set_algtime(int stationid, long long algtime) {
	m_camera[stationid].set_algtime(algtime);
}
long long Solution::get_algtime_avg(int stationid) {
	return m_camera[stationid].get_algtime_avg();
}
long long Solution::get_algtime_max(int stationid) {
	return m_camera[stationid].get_algtime_max();
}
long long Solution::get_algtime_min(int stationid) {
	return m_camera[stationid].get_algtime_min();
}
void Solution::set_lifetime(int stationid, long long lifetime) {
	m_camera[stationid].set_lifetime(lifetime);
}
long long Solution::get_lifetime_avg(int stationid) {
	return m_camera[stationid].get_lifetime_avg();
}
long long Solution::get_lifetime_max(int stationid) {
	return m_camera[stationid].get_lifetime_max();
}
long long Solution::get_lifetime_min(int stationid) {
	return m_camera[stationid].get_lifetime_min();
}
namespace sln {
	int isexist(int stationid) {
		Solution * cursolution = Solution::GetCurrentSolution();
		return cursolution->isexist(stationid);
	}

	std::vector<std::string> getalgs(int stationid) {
		Solution * cursolution = Solution::GetCurrentSolution();
		return cursolution->getalgs(stationid);
	}

	cv::Rect2f getroi(int stationid) {
		Solution * cursolution = Solution::GetCurrentSolution();
		return cursolution->getroi(stationid);
	}
	int isused(int stationid) {
		Solution * cursolution = Solution::GetCurrentSolution();
		return cursolution->isused(stationid);
	}

	int getgrade(int stationid)
	{
		Solution * cursolution = Solution::GetCurrentSolution();

		if (cursolution != NULL)
			return cursolution->getgrade(stationid);
		else
			return 0;
	}
	

	int getgroupid(int stationid) {
		Solution * cursolution = Solution::GetCurrentSolution();
		return cursolution->getgroupid(stationid);
	}

	int getstationid(std::string cameraname, int frameid) {
		Solution * cursolution = Solution::GetCurrentSolution();
		return cursolution->getstationid(cameraname, frameid);
	}
	std::vector<int> getstationlist(int groupid) {
		Solution * cursolution = Solution::GetCurrentSolution();
		return cursolution->getstationlist(groupid);
	}

	void setdeviceng(int stationid, bool isng) {
		Solution * cursolution = Solution::GetCurrentSolution();
		return cursolution->setdeviceng(stationid, isng);
	}

	void settotalng(bool isng) {
		Solution * cursolution = Solution::GetCurrentSolution();
		return cursolution->set_result(isng);
	}

	void resetcnt(void) {
		Solution * cursolution = Solution::GetCurrentSolution();
        if (!cursolution)
            return;
		cursolution->resetcnt();
	}

	Solution_Camera& getcamera(int stationid) {
		Solution * cursolution = Solution::GetCurrentSolution();
		return cursolution->getcamera(stationid);
	}


	int getvirtstationmax(std::string cameraname) {
		Solution * cursolution = Solution::GetCurrentSolution();
		return cursolution->getvirtstationmax(cameraname);
	}

	void set_algtime(int stationid, long long algtime) {
		Solution * cursolution = Solution::GetCurrentSolution();
		cursolution->set_algtime(stationid, algtime);
	}
	long long get_algtime_avg(int stationid) {
		Solution * cursolution = Solution::GetCurrentSolution();
		return cursolution->get_algtime_avg(stationid);
	}
	long long get_algtime_max(int stationid) {
		Solution * cursolution = Solution::GetCurrentSolution();
		return cursolution->get_algtime_max(stationid);
	}
	long long get_algtime_min(int stationid) {
		Solution * cursolution = Solution::GetCurrentSolution();
		return cursolution->get_algtime_min(stationid);
	}
	void set_lifetime(int stationid, long long lifetime) {
		Solution * cursolution = Solution::GetCurrentSolution();
		cursolution->set_lifetime(stationid, lifetime);
	}
	long long get_lifetime_avg(int stationid) {
		Solution * cursolution = Solution::GetCurrentSolution();
		return cursolution->get_lifetime_avg(stationid);
	}
	long long get_lifetime_max(int stationid) {
		Solution * cursolution = Solution::GetCurrentSolution();
		return cursolution->get_lifetime_max(stationid);
	}
	long long get_lifetime_min(int stationid) {
		Solution * cursolution = Solution::GetCurrentSolution();
		return cursolution->get_lifetime_min(stationid);
	}
}