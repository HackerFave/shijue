#pragma once
#include <iostream>
#include <vector>

#include <opencv2/opencv.hpp>
#include "plc.h"

#define USE_PLC
#define USE_CUDA
#define CAMERA_SIMULATE
//#define  EXTERNAL_TRIGGER
#define USE_WHOLEIMG

#define MAX_CAMERA_NUM 12
#define MAX_STATION_NUM MAX_CAMERA_NUM
#define MAX_GROUP_NUM 6

#define MAX_ALG_NUM 6

class imginfo {
public:
    ~imginfo()
    {
        m_img.release();
        m_img_aux.release();
    }
	imginfo(const cv::Mat & img) {
        if (img.channels() == 1)
        {
            cv::cvtColor(img, m_img, cv::COLOR_GRAY2RGB);
        }
        else
        {
            m_img = img;
        }
		//m_img = img.clone();
		m_alg.clear();
		m_algdone.clear();
		m_isNg = false;
	};
	void setcapturetime() {
		m_capturetime = std::chrono::steady_clock::now();
		m_algstarttime = m_capturetime;
	}
	std::chrono::steady_clock::time_point& getcapturetime() {
		return m_capturetime;
	}
	void setalgstarttime() {
		m_algstarttime = std::chrono::steady_clock::now();
	}
	std::chrono::steady_clock::time_point& getalgstarttime() {
		return m_algstarttime;
	}
	void setalgdonetime() {
		m_algdonetime = std::chrono::steady_clock::now();
	}
	std::chrono::steady_clock::time_point& getalgdonetime() {
		return m_algdonetime;
	}
	void setdestroytime() {
		m_destroytime = std::chrono::steady_clock::now();
	}
	std::chrono::steady_clock::time_point& getdestroytime() {
		return m_destroytime;
	}
	void setalgstarttime(std::chrono::steady_clock::time_point& starttime) {
		if (0 == std::chrono::duration_cast<std::chrono::microseconds>(m_algstarttime - m_capturetime).count()) {
			m_algstarttime = starttime;
		}
	}
	void setalgdonetime(std::chrono::steady_clock::time_point& donetime) {
		m_algdonetime = donetime;
	}
	std::vector<std::string>& getalg() {
		return m_alg;
	}
	void setalg(std::vector<std::string> alg) {
		m_alg = alg;
	}
	void setalgdone(std::string alg) {
		return m_algdone.push_back(alg);
	}
	void setcuralg(std::string alg) {
		m_algcur = alg;
	}
	std::string getcuralg() {
		return m_algcur;
	}
	bool isalgdone() {
		return m_alg.size() == m_algdone.size();
	}
	int getid() {
		return m_id;
	}
	void setid(int id) {
		m_id = id;
	}
	int getframeid() {
		return m_frameid;
	}
	void setframeid(int frameid) {
		m_frameid = frameid;
	}
	cv::Mat getimg() {
		return m_img;
	}
	void setimg(cv::Mat img) {
		m_img = img;
	}
	void setNgflag(bool isNg) {
		m_isNg = isNg;
	}
	bool getNgflag() {
		return m_isNg;
	}
    void setresulttype(int resulttype) {
        m_resulttype = m_resulttype | (1 << resulttype);
    }
    int getresulttype() {
        return m_resulttype;
    }
	void setstationid(int stationid) {
		m_stationid = stationid;
	}
	int getstationid() {
		return m_stationid;
	}
    void setauximg(cv::Mat img) {
        m_img_aux = img.clone();
    }
    cv::Mat getauximg() {
        return m_img_aux;
    }
    bool bauximgexist() {
        return !m_img_aux.empty();
    }
	void setcapturelength() {
		lengthinfo_s info = plcmaster::getcameralength();
		m_capturelength = info.length;
		m_workstatus = info.workstatus;
	}
	float getcapturelength() {
		return m_capturelength;
	}

	plcworkstatus getworkstatus() {
		return m_workstatus;
	}
private:
	int m_stationid;
	cv::Mat m_img;
    cv::Mat m_img_aux;
	std::vector<std::string> m_alg;
	std::vector<std::string> m_algdone;
	bool m_isNg;
    int m_resulttype;
	std::string m_algcur;
	int m_id;
	int m_frameid;
	float m_capturelength;
	plcworkstatus m_workstatus;
	std::chrono::steady_clock::time_point m_capturetime;
	std::chrono::steady_clock::time_point m_algstarttime;
	std::chrono::steady_clock::time_point m_algdonetime;
	std::chrono::steady_clock::time_point m_destroytime;
};

typedef struct frameinfo {
	int frameid;
	cv::Mat img;
}frameinfo_s;