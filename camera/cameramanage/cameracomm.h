#pragma once
#include <vector>
#include <thread>
#include <string>
#include <functional>
#include <direct.h>
#include <io.h>
#include <opencv2/opencv.hpp>
//�������
enum cameratype {
    CAMERA_FILE,        //����Ŀ¼
    CAMERA_INVENTED,    //����Ŀ¼
    CAMERA_HIK,         //����
    CAMERA_BASLER,      //BASLER���
    CAMERA_COUNT,
};

enum triggersource {
    TRIGGER_SOURCE_LINE0 = 0, //LINE0Ӳ������
    TRIGGER_SOURCE_LINE1 = 1, //LINE1Ӳ������
    TRIGGER_SOURCE_LINE2 = 2, //LINE2Ӳ������
    TRIGGER_SOURCE_LINE3 = 3, //LINE3Ӳ������
    TRIGGER_SOURCE_COUNTER0 = 4, //COUNTER0Ӳ������
    TRIGGER_SOURCE_SOFTWARE = 7  //����}MV_CAM_TRIGGER_SOURCE
};


//�ɼ���ͼ����Ϣ
class capinfo {
public:
    int frameid;
    cameratype type;    //�������
    cv::Mat img;        //ͼƬ
    cv::Mat img_aux;    //ͼ��2
    std::string cameraid;
    //cv::Mat temprature; //�¶���Ϣ��ֻ�к��������
    //std::chrono::steady_clock::time_point m_capturetime; //�ɼ�ʱ��
};

//�ɼ�����
typedef struct
{
    float			nExposure;				/* �ع�    */
    float			nGain;				    /* ����    */
    float			nFrameRate;			    /* ֡��    */
}CameraPicInfo;

//�����Ϣ
typedef struct
{
    char 			chDevName[256];			/* �豸��		*/
    char			chSoftVer[256];			/* ����汾��	*/ //��Ӧ��
    char 			chHardVer[20];			/* Ӳ���汾��	*/
    char 			chSysVer[256];			/* ϵͳ�汾	    */ //�ͺ�
    int				nSdkPort;				/* sdk�˿ں�    */
    char 			chIp[16];				/* ip��ַ		*/
    char 			chGateway[16];			/* ����	        */
    char 			chNetmask[16];			/* ��������	    */
    char 			chMac[18];				/* Mac��ַ		*/
    char            chRoomID[20];			/* RooMID       */
    char            chSN[20];				/* SN           */
    char            chWifiIp[16];			/* wifi_ip      */
    char            chFileAdd[512];         /* �ļ���ַ     */
    cameratype      type;                   /* �������     */
    int             chDevIndex;             /* ������     */
    int             triggermode;            /* ����ģʽ     */
}DeviceInfo;

class ICamera
{
public:
    virtual void Test(void) = 0;
    virtual int SetParam(DeviceInfo*) = 0;
    virtual int Deinitialize(void) = 0;
    virtual int Initialize(void) = 0;
    virtual int Open(void) = 0;
    virtual int Close(void) = 0;
    virtual int Start(void) = 0;
    virtual int Stop(void) = 0;
    virtual int SetPicParam(CameraPicInfo*) = 0;
    virtual int GetPicParam(CameraPicInfo*) = 0;
    virtual int RegisterCallback(std::function<void(const capinfo&)> callback) = 0;
    //����Դ
    virtual int SetTriggerSource(triggersource) = 0;
    virtual int GetTriggerSource(triggersource*) = 0;
    //����ģʽ
    virtual int SetTriggerMode(int) = 0;
    virtual int GetTriggerMode(int*) = 0;
    //����
    virtual int Trigger() = 0;

protected:
    std::function<void(const capinfo&)> callback_;
    std::thread thread_;
    bool isrunning_;

};

// Create function pointer
//typedef ICamera* (__stdcall *CreateCameraFunc)(int);
typedef std::vector<DeviceInfo> DevList;
typedef void*(__stdcall *CameraPROC)(void*);

