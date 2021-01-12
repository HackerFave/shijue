#pragma once
#include "QString.h"
#include "QSerialPort"      //串口访问 
#include "QSerialPortInfo"  //串口端口信息访问
#include <mutex>
#include <thread>

enum Light_cmd
{
    CMD_RED_OPEN = 0,
	CMD_YELLOW_OPEN,
	CMD_GREEN_OPEN,
	CMD_RED_CLOSE,
	CMD_YELLOW_CLOSE,
	CMD_GREEN_CLOSE,
};

namespace light {
	bool deinitialize();

	bool initialize();

	void push(Light_cmd cmd);
};

