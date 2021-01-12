#pragma once

#include <QObject>
#include <qrect.h>
#include <vector>
#include <opencv2/opencv.hpp>
class Solution_Camera 
{
public:
	Solution_Camera() {
        reset_time();
		m_alg.clear();
	};
	~Solution_Camera(){};
private:
    int m_cameratype;
	bool m_isused;
	int m_grade;
	int m_group;
	int m_station;
	bool m_islast;
	int m_frameid;
	QString m_cameraindex;
	int m_virtdeviceid;
	int m_virtdevicemax;
	float m_roirect[4] = { 0.0 };
	QStringList m_alg;
	int m_ngcnt;
	int m_okcnt;
	long long m_algtime_total;
	long long m_algtime_max;
	long long m_algtime_min;

	long long m_lifetime_total;
	long long m_lifetime_max;
	long long m_lifetime_min;

public:
    //相机类型
    void set_camtype(int type) { m_cameratype = type; }
    int get_camtype() { return m_cameratype; }

	//是否使用
	void set_isused(bool isused) { m_isused = isused; }
	bool get_isused() { return m_isused; }

	//是否为最后一个工位
	void set_islast(bool islast) { m_islast = islast; }
	bool get_islast() { return m_islast; }

	//设置当前所在组
	void set_group(int group) { m_group = group; }
	int get_group() { return m_group; }

	//设置当前档位
	void set_grade(int grade) { m_grade = grade; };
	int get_grade() { return m_grade; }

	//设置当前所在工号
	void set_station(int station) { m_station = station; }
	int get_station() { return m_station; }

	//设置相机名称
	void set_frameid(int frameid) { m_frameid = frameid; }
	int get_frameid() { return m_frameid; }

	//设置当前的算法
	void set_alg(QStringList alg) { m_alg = alg; }
	QStringList get_alg() { return m_alg; }
	
	//设置相机索引
	void set_cameraindex(QString name) { m_cameraindex = name; }
	QString get_cameraindex() { return m_cameraindex; }

	//设置该工位对应相机的虚拟设备ID
	void set_virtdeviceid(int virtdeviceid) { m_virtdeviceid = virtdeviceid; }
	int get_virtdeviceid() { return m_virtdeviceid; }

	//设置对应相机最大虚拟设备数
	void set_virtdevicemax(int virtdevicemax) { 
		m_virtdevicemax = virtdevicemax; 
		m_virtdeviceid = m_virtdeviceid % virtdevicemax;
	}
	int get_virtdevicemax() { return m_virtdevicemax; }

	//设置当前roi矩形相对坐标
	void set_roirect(float tx,float ty,float bx,float by) { m_roirect[0] = tx; 
															m_roirect[1] = ty;
															m_roirect[2] = bx;
															m_roirect[3] = by;}
	
	float* get_roirect() { return m_roirect; }

	void set_result(bool isng) {
		if (isng){
			m_ngcnt++;
		}
		else{
			m_okcnt++;
		}
	}

	void resetcnt(void) {
		m_ngcnt = 0;
		m_okcnt = 0;
	}

	int get_ngcnt() { return m_ngcnt; }
	int get_okcnt() { return m_okcnt; }
	void set_ngcnt(int ngcnt) {  m_ngcnt = ngcnt; }
	void set_okcnt(int okcnt) {  m_okcnt = okcnt; }

	void set_algtime(long long algtime) {
		m_algtime_total += algtime;
		if (m_algtime_max < algtime) {
			m_algtime_max = algtime;
		}
		if (m_algtime_min > algtime) {
			m_algtime_min = algtime;
		}
	}
	long long get_algtime_avg() {
		if (m_okcnt + m_ngcnt) {
			return m_algtime_total / (m_okcnt + m_ngcnt);
		}
		else {
			return 0;
		}
	}
	long long get_algtime_max() {
			return m_algtime_max;
	}
	long long get_algtime_min() {
		return m_algtime_min;
	}
	void set_lifetime(long long lifetime) {
		m_lifetime_total += lifetime;
		if (m_lifetime_max < lifetime) {
			m_lifetime_max = lifetime;
		}
		if (m_lifetime_min > lifetime) {
			m_lifetime_min = lifetime;
		}
	}
	long long get_lifetime_avg() {
		if (m_okcnt + m_ngcnt) {
			return m_lifetime_total / (m_okcnt + m_ngcnt);
		}
		else {
			return 0;
		}
	}
	long long get_lifetime_max() {
		return m_lifetime_max;
	}
	long long get_lifetime_min() {
		return m_lifetime_min;
	}
	//时间复位
	void reset_time() {
		m_ngcnt = 0;
		m_okcnt = 0;
		m_algtime_total = 0;
		m_algtime_max = 0;
		m_algtime_min = 100000000000;
		m_lifetime_total = 0;
		m_lifetime_max = 0;
		m_lifetime_min = 100000000000;
	}

};



class Solution
{

public:
	Solution();
	Solution(int, int);
	~Solution();

private:
	
	int m_group;
	int m_station;
	QString m_inipath;
	QString m_solutionname;
	int m_ngcnt;
	int m_okcnt;
	
public:
	static std::vector<QString> g_solution;
	//遍历检查方案
	static bool LoadIni();
	
	static Solution* GetCurrentSolution();

	static void SetCurrentSolution(QString path);

public:
	//相机虚拟工位数
	std::map<QString, int > m_virtstation;
	//工位列表
	std::map<int, Solution_Camera> m_camera;
	//std::vector< Solution_Camera> m_camera;
	//设置当前组
	void set_group(int group) { m_group = group; }
	int  get_group() { return m_group; }
	//设置工位
	void set_station(int station) { m_station = station; }
	int  get_station() { return m_station; }

	//设置当前的检查方案名称
	void set_solutionname(QString solutionname) { m_solutionname = solutionname; }
	QString get_solutionname() { return m_solutionname; }

	//读取配置文件
	bool ReadIniFile(QString inifile);
	//写入配置文件,写入前必须设定m_group和m_solutionname
	bool WriteIniFile(QString inifile);
	
	//配置文件路径
	QString get_inipath() { return m_inipath; };

	int isexist(int stationid);
	
	std::vector<std::string> getalgs(int stationid);

	cv::Rect2f getroi(int stationid);

	//获取当前档位
	int getgrade(int stationid);
	bool isused(int stationid);

	int getgroupid(int stationid);

	std::vector<int> getstationlist(int groupid);

	void setdeviceng(int stationid, bool isng); 
	void resetcnt(void);
	void set_result(bool isng) {
		if (isng) {
			m_ngcnt++;
		}
		else {
			m_okcnt++;
		}
	}
	int get_ngcnt() { return m_ngcnt; }
	int get_okcnt() { return m_okcnt; }
	void set_ngcnt(int ngcnt) { m_ngcnt = ngcnt; }
	void set_okcnt(int okcnt) { m_okcnt = okcnt; }
	Solution_Camera& Solution::getcamera(int stationid);

	int getstationid(std::string cameraname, int frameid);
	int getvirtstationmax(std::string cameraname);

	void set_algtime(int stationid, long long algtime);
	long long get_algtime_avg(int stationid);
	long long get_algtime_max(int stationid);
	long long get_algtime_min(int stationid);
	void set_lifetime(int stationid, long long lifetime);
	long long get_lifetime_avg(int stationid);
	long long get_lifetime_max(int stationid);
	long long get_lifetime_min(int stationid);
};

namespace sln {
	int isexist(int stationid);
	int isused(int stationid);

	std::vector<std::string> getalgs(int stationid);

	cv::Rect2f getroi(int stationid);

	int getgrade(int stationid);

	int getgroupid(int stationid);

	std::vector<int> getstationlist(int groupid);

	void setdeviceng(int stationid, bool isng);

	void settotalng(bool isng);
	void resetcnt(void);
	Solution_Camera& getcamera(int stationid);


	void set_algtime(int stationid, long long algtime);
	long long get_algtime_avg(int stationid);
	long long get_algtime_max(int stationid);
	long long get_algtime_min(int stationid);
	void set_lifetime(int stationid, long long lifetime);
	long long get_lifetime_avg(int stationid);
	long long get_lifetime_max(int stationid);
	long long get_lifetime_min(int stationid);

	int getstationid(std::string cameraname, int frameid);
	int getvirtstationmax(std::string cameraname);
}
