#pragma once
#include <vector>
#include <functional>

#define PLC_EXPORTS __declspec(dllexport)

namespace plc {
    enum PlcResultType {
        kOk = 0,            //��Ʒ
        kRetry,             //�ز�
        kNgMeasure,         //�ߴ粻��
        kNgAppearance,      //��۲���
        kNgOther,           //��������
        kResultTypeMax,
    };

    enum PlcMessageType {
        kStart = 0,         //����
        kStop,              //ֹͣ
        kPowerOff,          //�ػ�
        kFaultAirPressure,  //��ѹ����
        kDisConnect,        //���ӶϿ�
        kQueueEmpty,        //�����ڴ���ʱ������Ϊ�ա�
        kMessageTypeMax,
		kRollStart,         //��ʼ
		kRollEnd,           //�����
    };

    enum PlcDeviceType {
        kSerial = 0,        //����
        kTcp,               //����
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

    //�����صķ���ʼ������
    PLC_EXPORTS int deinitialize();
    //�����صĳ�ʼ������
    PLC_EXPORTS int initialize();
    //��ȡ��λ������
    PLC_EXPORTS int getgroupnum();

    //��������
    PLC_EXPORTS int connect(PlcConnectInfo connectinfo);
    //�Ͽ�����
    PLC_EXPORTS int disconnect();

    //���Ĵ���
    PLC_EXPORTS std::vector<int> readreg(int plcstartaddr, int num);
    //д�Ĵ���
    PLC_EXPORTS int writereg(int plcstartaddr, int num, std::vector<int> regvals);

	//vector<int>ת��Ϊ����
	PLC_EXPORTS float getfloat(std::vector<int> regvals, std::string floattype = "dcba");
	//����ת��Ϊvector<int>
	PLC_EXPORTS std::vector<int> setfloat(float f, std::string floattype = "abcd");

    //����
    PLC_EXPORTS int start();
    //ֹͣ
    PLC_EXPORTS int stop();
    //���ͼ������PLC���ݽ��ȥ����������
    PLC_EXPORTS int push(int groupid, PlcResultType resulttype);

    //ע��ص�����������PLC���¼�������������ֹͣ�����ֹ���
    PLC_EXPORTS int registercallback(std::function<void(PlcMessageType)> callback);
}