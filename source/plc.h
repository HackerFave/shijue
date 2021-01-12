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
	//�����������
	float last_check_length;
	//��һ��ĳ���
	float last_roll_length;
}endlengthinfo_s;

namespace plcmaster {
	void run();
	bool initialize(std::function<void(plc::PlcMessageType)> callback);
	bool deinitialize();
	void start();
	void stop();
	void reset();
	// ��PLC��¼���룬�������ͼ���г�
	void setoffset(float offset);
	float getspeed();
	float getcurrentlength();
	lengthinfo_s getcameralength();
	float getonlycameralength();
	float gettotallength();
	processinfo_s gettemperature();
	// ��SW����HW�������������ã�����һ���Ѿ���ʼ�������
	float setplcstatusstart();
	endlengthinfo_s setplcstatusend();
}

