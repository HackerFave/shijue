#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <plcinterface.h>
#include <thread>

typedef struct processinfo {
	std::vector<float> area_temperature;
	std::vector<float> composite_temperature;
	std::vector<float> composite_pressure;
}processinfo_s;

enum plcworkstatus {
	START = 0,
	END = 1
};

typedef struct lengthinfo {
	float length;
	plcworkstatus workstatus;
}lengthinfo_s;

typedef struct endlengthinfo {
	//检查的最后坐标
	float last_check_length;
	//这一卷的长度
	float last_roll_length;
}endlengthinfo_s;

namespace plcmaster {
	void run();
	bool initialize(std::function<void(plc::PlcMessageType)> callback);
	bool deinitialize();
	void start();
	void stop();
	void reset();
	// 从PLC记录距离，到相机拍图的行程
	void setoffset(float offset);
	float getspeed();
	float getcurrentlength();
	lengthinfo_s getcameralength();
	float getonlycameralength();
	float gettotallength();
	processinfo_s gettemperature();
	// 由SW或者HW按键处理函数调用，设置一卷已经开始或结束了
	float setplcstatusstart();
	endlengthinfo_s setplcstatusend();
}

