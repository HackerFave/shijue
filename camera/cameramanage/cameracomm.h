#pragma once
#include <vector>
#include <thread>
#include <string>
#include <functional>
#include <direct.h>
#include <io.h>
#include <opencv2/opencv.hpp>
//相机类型
enum cameratype {
    CAMERA_FILE,        //网络目录
    CAMERA_INVENTED,    //本地目录
    CAMERA_HIK,         //海康
    CAMERA_BASLER,      //BASLER相机
    CAMERA_COUNT,
};

enum triggersource {
    TRIGGER_SOURCE_LINE0 = 0, //LINE0硬件触发
    TRIGGER_SOURCE_LINE1 = 1, //LINE1硬件触发
    TRIGGER_SOURCE_LINE2 = 2, //LINE2硬件触发
    TRIGGER_SOURCE_LINE3 = 3, //LINE3硬件触发
    TRIGGER_SOURCE_COUNTER0 = 4, //COUNTER0硬件触发
    TRIGGER_SOURCE_SOFTWARE = 7  //软触发}MV_CAM_TRIGGER_SOURCE
};


//采集的图像信息
class capinfo {
public:
    int frameid;
    cameratype type;    //相机类型
    cv::Mat img;        //图片
    cv::Mat img_aux;    //图像2
    std::string cameraid;
    //cv::Mat temprature; //温度信息，只有红外相机有
    //std::chrono::steady_clock::time_point m_capturetime; //采集时间
};

//采集参数
typedef struct
{
    float			nExposure;				/* 曝光    */
    float			nGain;				    /* 增益    */
    float			nFrameRate;			    /* 帧数    */
}CameraPicInfo;

//相机信息
typedef struct
{
    char 			chDevName[256];			/* 设备名		*/
    char			chSoftVer[256];			/* 软件版本号	*/ //供应商
    char 			chHardVer[20];			/* 硬件版本号	*/
    char 			chSysVer[256];			/* 系统版本	    */ //型号
    int				nSdkPort;				/* sdk端口号    */
    char 			chIp[16];				/* ip地址		*/
    char 			chGateway[16];			/* 网关	        */
    char 			chNetmask[16];			/* 子网掩码	    */
    char 			chMac[18];				/* Mac地址		*/
    char            chRoomID[20];			/* RooMID       */
    char            chSN[20];				/* SN           */
    char            chWifiIp[16];			/* wifi_ip      */
    char            chFileAdd[512];         /* 文件地址     */
    cameratype      type;                   /* 相机类型     */
    int             chDevIndex;             /* 相机序号     */
    int             triggermode;            /* 触发模式     */
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
    //触发源
    virtual int SetTriggerSource(triggersource) = 0;
    virtual int GetTriggerSource(triggersource*) = 0;
    //触发模式
    virtual int SetTriggerMode(int) = 0;
    virtual int GetTriggerMode(int*) = 0;
    //触发
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

