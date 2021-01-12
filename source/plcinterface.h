#pragma once
#include <vector>
#include <functional>

#define PLC_EXPORTS __declspec(dllexport)

namespace plc {
    enum PlcResultType {
        kOk = 0,            //良品
        kRetry,             //重测
        kNgMeasure,         //尺寸不良
        kNgAppearance,      //外观不良
        kNgOther,           //其他不良
        kResultTypeMax,
    };

    enum PlcMessageType {
        kStart = 0,         //启动
        kStop,              //停止
        kPowerOff,          //关机
        kFaultAirPressure,  //气压不足
        kDisConnect,        //连接断开
        kQueueEmpty,        //吹气口触发时，队列为空。
        kMessageTypeMax,
		kRollStart,         //卷开始
		kRollEnd,           //卷结束
    };

    enum PlcDeviceType {
        kSerial = 0,        //串口
        kTcp,               //网口
    };

    typedef struct {
        const char *device;
        int baud;
        int data_bit;
        int stop_bit;
        char parity;
    }PlcSerialInfo;

    typedef struct {
        const char* plcipaddr;
        int plcport;
    }PlcTcpInfo;

    typedef struct {
        PlcDeviceType devicetype;
        union {
            PlcTcpInfo tcpinfo;
            PlcSerialInfo serialinfo;
        };
    }PlcConnectInfo;

    //软件相关的反初始化操作
    PLC_EXPORTS int deinitialize();
    //软件相关的初始化操作
    PLC_EXPORTS int initialize();
    //获取工位组数量
    PLC_EXPORTS int getgroupnum();

    //建立连接
    PLC_EXPORTS int connect(PlcConnectInfo connectinfo);
    //断开连接
    PLC_EXPORTS int disconnect();

    //读寄存器
    PLC_EXPORTS std::vector<int> readreg(int plcstartaddr, int num);
    //写寄存器
    PLC_EXPORTS int writereg(int plcstartaddr, int num, std::vector<int> regvals);

	//vector<int>转换为浮点
	PLC_EXPORTS float getfloat(std::vector<int> regvals, std::string floattype = "dcba");
	//浮点转换为vector<int>
	PLC_EXPORTS std::vector<int> setfloat(float f, std::string floattype = "abcd");

    //启动
    PLC_EXPORTS int start();
    //停止
    PLC_EXPORTS int stop();
    //发送检测结果，PLC根据结果去操作吹气口
    PLC_EXPORTS int push(int groupid, PlcResultType resulttype);

    //注册回调函数，处理PLC的事件，比如启动，停止，各种故障
    PLC_EXPORTS int registercallback(std::function<void(PlcMessageType)> callback);
}