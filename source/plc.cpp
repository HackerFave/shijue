#include <iostream>
#include <thread>
#include <io.h> //access
#include "windows.h"
#include <tchar.h>
#include "common.h"
#include "log.h"
#include "plc.h"
#include "solution.h"
#include "baseparam.h"
#include "debugdata.h"
#include "uicommon.h"
#ifdef DEBUGTIME
#include "proc.h"
#endif
#ifdef USE_CUDA
#include <cuda_runtime.h>
#define USE_MANUVISION
#endif
#include <plcinterface.h>
#include "qsettings.h"
#include <QMetaType>
#include <QCoreApplication>

//寄存器列表，不想让外部看到，所以放在plc.cpp而不是plc.h里面
#define REG_SPEED							0x112C
//不是相机处的，是PLC记录的另外一套
#define REG_CURRENT_LENGTH					0x112E
#define REG_TOTAL_LENGTH					0x1130
//相机处见到的长度
#define REG_CAMERA_LENGTH					0x1132
//相机到人工裁剪处的距离L2
#define REG_L2_LENGTH					    0x1134

#define REG_AREA_TEMPERATURE_START			0x1136
#define REG_AREA_TEMPERATURE_NUM			20
#define REG_COMPOSITE_TEMPERATURE_START     0x115E
#define REG_COMPOSITE_TEMPERATURE_NUM		2
#define REG_COMPOSITE_PRESSURE_START        0x1162
#define REG_COMPOSITE_PRESSURE_NUM		    2

#define PLC_FLOAT_FORMAT                    "badc"

#define SIMULATE_ROLL_LENGTH                100
#define SIMULATE_L3                         20
#define SIMULATE_L2                         10
typedef void *HANDLE;
float g_simulate_camera_length = 0;
float g_simulate_roll_end = 0;
//左右两个相机，各读一次才更新
int g_simulate_count = 0;
std::function<void(plc::PlcMessageType)> g_callback;

plcworkstatus g_workstatus = END;
float g_last_length = 0;
float g_l2_length = 0;
float g_last_check_length = 0;
bool g_running = false;
bool g_roll_end = false;
std::mutex g_status_mutex;

namespace plcmaster {
#ifdef USE_PLC
	void plcloadini() {
		plc::PlcConnectInfo connectinfo;
		std::string plcipaddr;
		std::string com;
		QString path = QCoreApplication::applicationDirPath() + '/' + QString(ININAME_PLC);
		QSettings configini(path, QSettings::IniFormat);
		if (configini.value("connect/type").toString() == "Serial") {
			com = configini.value("serial/device").toString().toLocal8Bit();
			connectinfo.devicetype = plc::kSerial;
			connectinfo.serialinfo.device = com.c_str();
			connectinfo.serialinfo.baud = configini.value("serial/baud").toString().toInt();
			connectinfo.serialinfo.data_bit = configini.value("serial/data_bit").toString().toInt();
			connectinfo.serialinfo.stop_bit = configini.value("serial/stop_bit").toString().toInt();
			connectinfo.serialinfo.parity = configini.value("serial/parity").toString()[0].unicode();
		}
		else {
			plcipaddr = std::string(configini.value("ip/address").toString().toLocal8Bit()).c_str();
			int plcport = configini.value("ip/port").toString().toInt();
			connectinfo.devicetype = plc::kTcp;
			connectinfo.tcpinfo.plcipaddr = plcipaddr.c_str();
			connectinfo.tcpinfo.plcport = plcport;
		}
		if (plc::connect(connectinfo)) {
			std::cout << "init PLC Fail\n";
			return;
		}
		else {
			std::cout << "init PLC OK\n";
		}
		configini.beginGroup("registers");
		QStringList childlist = configini.allKeys();
		for (int i = 0; i < childlist.size(); i++) {
			std::vector<int> regvals;
			int plcstartaddr = childlist.at(i).toInt();
			regvals.push_back(configini.value(childlist.at(i)).toInt());
			int retval = plc::writereg(plcstartaddr, 1, regvals);
		}
		configini.endGroup();
	}

	bool initialize(std::function<void(plc::PlcMessageType)> callback) {
		plc::initialize();
		g_callback = callback;
		plc::registercallback(g_callback);
		plcloadini();
		//一开始是不工作的
		g_workstatus = END;
		return true;
	}

	bool deinitialize() {
		plc::disconnect();
		plc::deinitialize();
		return true;
	}

	void start() {
		plc::start();
	}

	void stop() {
		plc::stop();
	}

	void reset() {
		plc::disconnect();
		plcloadini();
		g_workstatus = END;
	}

	float getspeed() {
		std::vector<int> regvals;
		float speed;
		regvals = plc::readreg(REG_SPEED, 2);
		if (2 != regvals.size())
		{
		    std::cout << "Read REG" << REG_SPEED << "Error";
			return 0;
		}
		speed = plc::getfloat(regvals, PLC_FLOAT_FORMAT);
		std::cout << "Read SPEED" << speed << "\n";
		return speed;
	}

	float getcurrentlength() {
		std::vector<int> regvals;
		float current_length;
		regvals = plc::readreg(REG_CURRENT_LENGTH, 2);
		if (2 != regvals.size())
		{
			std::cout << "Read REG" << REG_CURRENT_LENGTH << "Error";
			return 0;
		}
		current_length = plc::getfloat(regvals, PLC_FLOAT_FORMAT);
		std::cout << "Read CURRENT_LENGTH" << current_length << "\n";
		return current_length;
	}

	lengthinfo_s getcameralength() {
		lengthinfo_s length;
		std::vector<int> regvals;
		float current_length;

/*
        //为了方便调试，先不做限制
		//没有开始工作，直接返回0
		if (END == g_workstatus) {
			length.length = 0;
			length.workstatus = g_workstatus;
			return length;
		}
*/
		regvals = plc::readreg(REG_CAMERA_LENGTH, 2);
		if (2 != regvals.size())
		{
			std::cout << "Read REG" << REG_CAMERA_LENGTH << "Error";
			current_length = 0;
		}
		else
		{
			current_length = plc::getfloat(regvals, PLC_FLOAT_FORMAT);
		}
		std::cout << "Read CAMERA_LENGTH" << current_length << "\n";
		length.length = current_length;
		length.workstatus = g_workstatus;
		return length;
	}

	float getonlycameralength() {
		float current_length;
		std::vector<int> regvals;
		regvals = plc::readreg(REG_CAMERA_LENGTH, 2);
		if (2 != regvals.size())
		{
			std::cout << "Read REG" << REG_CAMERA_LENGTH << "Error";
			return 0;
		}
		current_length = plc::getfloat(regvals, PLC_FLOAT_FORMAT);
		std::cout << "Read CAMERA_LENGTH" << current_length << "\n";
		return current_length;
	}

	float gettotallength() {
		std::vector<int> regvals;
		float total_length;
		regvals = plc::readreg(REG_TOTAL_LENGTH, 2);
		if (2 != regvals.size())
		{
			std::cout << "Read REG" << REG_TOTAL_LENGTH << "Error";
			return 0;
		}
		total_length = plc::getfloat(regvals, PLC_FLOAT_FORMAT);
		std::cout << "Read TOTAL_LENGTH" << total_length << "\n";
		return total_length;
	}

	float getl2length() {
		std::vector<int> regvals;
		float l2_length;
		regvals = plc::readreg(REG_L2_LENGTH, 2);
		if (2 != regvals.size())
		{
			std::cout << "Read REG" << REG_L2_LENGTH << "Error";
			return 0;
		}
		l2_length = plc::getfloat(regvals, PLC_FLOAT_FORMAT);
		std::cout << "Read L2_LENGTH" << l2_length << "\n";
		return l2_length;
	}

	//此时返回值应该是文档中的L3或者L4
	float setplcstatusstart()
	{
		g_workstatus = START;
		std::cout << "set plc status start \n";
		return getonlycameralength();
	 }

	endlengthinfo_s setplcstatusend()
	{
		g_workstatus = END;
		std::cout << "set plc status end \n";
		endlengthinfo_s temp_info;
		temp_info.last_check_length = getonlycameralength();
		temp_info.last_roll_length = temp_info.last_check_length + getl2length();
		return temp_info;
	}

	processinfo_s gettemperature() {
		std::vector<int> regvals;
		float total_length;
		int i;
		processinfo_s process_info;
		bool data_ok = 1;

		regvals = plc::readreg(REG_AREA_TEMPERATURE_START,
			2*(REG_AREA_TEMPERATURE_NUM + REG_COMPOSITE_TEMPERATURE_NUM + REG_COMPOSITE_PRESSURE_NUM));

		if (2 * (REG_AREA_TEMPERATURE_NUM + REG_COMPOSITE_TEMPERATURE_NUM + REG_COMPOSITE_PRESSURE_NUM) != regvals.size() ){
			std::cout << "Read TEMPERATURE Error, Only get data" << regvals.size() << "\n";
			data_ok = 0;
		}

		for(i = 0; i < REG_AREA_TEMPERATURE_NUM*2; i = i + 2) {
			float f = 0.0;
			if (data_ok) {
				std::vector<int> tempregvals;
				tempregvals.push_back(regvals[i]);
				tempregvals.push_back(regvals[i + 1]);
				f = plc::getfloat(tempregvals, PLC_FLOAT_FORMAT);
			}
			process_info.area_temperature.push_back(f);
		}

		for (i = REG_AREA_TEMPERATURE_NUM*2;
			i < 2 * (REG_AREA_TEMPERATURE_NUM + REG_COMPOSITE_TEMPERATURE_NUM); 
			i = i + 2) {
			float f = 0.0;
			if (data_ok) {
				std::vector<int> tempregvals;
				tempregvals.push_back(regvals[i]);
				tempregvals.push_back(regvals[i + 1]);
				f = plc::getfloat(tempregvals, PLC_FLOAT_FORMAT);
			}
			process_info.composite_temperature.push_back(f);
		}

		for (i = (REG_AREA_TEMPERATURE_NUM + REG_COMPOSITE_TEMPERATURE_NUM) * 2;
			i < 2 * (REG_AREA_TEMPERATURE_NUM + REG_COMPOSITE_TEMPERATURE_NUM + REG_COMPOSITE_PRESSURE_NUM);
			i = i + 2) {
			float f = 0.0;
			if (data_ok) {
				std::vector<int> tempregvals;
				tempregvals.push_back(regvals[i]);
				tempregvals.push_back(regvals[i + 1]);
				f = plc::getfloat(tempregvals, PLC_FLOAT_FORMAT);
			}
			process_info.composite_pressure.push_back(f);
		}

		return process_info;
	}

	//目前使用手工按钮来启动，所以不设置维护g_workstatus了
	void run() {
		float temp_length = 0;
		while (g_running) {
			//判断
			if (END == g_workstatus)
			{
				temp_length = getonlycameralength();

				// 长度发生反转了，则认为新的一卷开始了
				if (temp_length < g_last_length)
				{
					g_workstatus = START;
					g_callback(plc::kRollStart);
				}
				g_last_length = temp_length;
			}
			else if (START == g_workstatus)
			{
				if (true == g_roll_end)
				{
					g_workstatus = END;
					g_l2_length = getl2length();
					g_last_check_length = getonlycameralength();

					g_callback(plc::kRollEnd);
					g_roll_end = false;
				}
			}
			//以100ms轮询, 距离差一点没有关系，精度要求不高
			Sleep(100);
		}
	}

#else
	bool initialize(std::function<void(plc::PlcMessageType)> callback) {
		QString path = QCoreApplication::applicationDirPath() + '/' + QString(ININAME_PLC);
		QSettings configini(path, QSettings::IniFormat);
		//g_offset = configini.value("function/offset", QVariant(20.0)).toFloat();
		g_callback = callback;
		plc::registercallback(g_callback);
		g_simulate_camera_length = 0;
		g_workstatus = END;
		g_last_check_length = 0;
		return true;
	}

	bool deinitialize() {
		return true;
	}

	void start() {
		g_simulate_camera_length = 0;
		g_simulate_count = 0;
	}

	void stop() {
	}

	void reset() {
		g_simulate_camera_length = 0;
		g_simulate_count = 0;
		g_workstatus = END;
	}

	float getspeed() {
		return 10.0;
	}

	float getcurrentlength() {
		float plc_length = g_simulate_camera_length - SIMULATE_L3;

		return plc_length;
	}

	lengthinfo_s getcameralength() {
		lengthinfo_s length;
		float current_length = g_simulate_camera_length;

		std::cout << "current_length " << current_length << "\n";

		g_simulate_count++;

		if (!(g_simulate_count % 2))
		{
			g_simulate_camera_length += 0.5;
		}

		length.length = g_simulate_camera_length;
		length.workstatus = g_workstatus;
		return length;
	}

	float gettotallength() {
		return 0;
	}

	processinfo_s gettemperature() {
		std::vector<int> regvals;
		float total_length;
		int i;
		processinfo_s temperature_info;

		for (i = 0; i < REG_AREA_TEMPERATURE_NUM; i++) {
			float f = 66;
			temperature_info.area_temperature.push_back(f+i);
		}

		for (i = 0; i < REG_COMPOSITE_TEMPERATURE_NUM; i++) {
			float f = 88;
			temperature_info.composite_temperature.push_back(f+i);
		}

		for (i = 0; i < REG_COMPOSITE_TEMPERATURE_NUM; i++) {
			float f = 120;
			temperature_info.composite_pressure.push_back(f + i);
		}
		return temperature_info;
	}

	float setplcstatusstart()
	{
		g_workstatus = START;
		std::cout << "set plc status start \n";
		g_simulate_camera_length = SIMULATE_L3;
		return g_simulate_camera_length;
	}

	endlengthinfo_s setplcstatusend()
	{
		g_workstatus = END;
		std::cout << "set plc status end \n";
		if (false == g_roll_end)
		{
			g_roll_end = true;
		}
		endlengthinfo_s temp_info;
		temp_info.last_check_length = g_simulate_camera_length;
		temp_info.last_roll_length = g_simulate_camera_length + SIMULATE_L2;
		return temp_info;
	}
#endif
	void setoffset(float offset) {
		// update init file
		//QString path = QCoreApplication::applicationDirPath() + '/' + QString(ININAME_PLC);
		//QSettings configini(path, QSettings::IniFormat);
		//configini.setValue(QString("function/offset"), QString::number(offset, 'f', 2));
	}
}
