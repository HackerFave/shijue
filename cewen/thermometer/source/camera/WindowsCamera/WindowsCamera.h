#ifndef CAMERA_WINDOWS_H_
#define CAMERA_WINDOWS_H_

#include "camerafactory.h"
#include <vector>

class CWindowsCamera : public ICamera
{
public:
	CWindowsCamera();
	~CWindowsCamera();

	int deinitialize(void);
	int initialize(void);
	int start(void);
	int stop(void);
	int registercallback(std::function<void(const capinfo&)> callback);

	void run();

	void join();

	//获取摄像头设备名称列表
	int listDevices(std::vector<std::string>& vecDevices);

	static ICamera * __stdcall Create() { return new CWindowsCamera(); }
private:
	int m_nWinCameraId;					//摄像头id，默认-1，为最新插入的USB摄像头设备
	std::string m_strWinCameraName;		//摄像头名称
	cv::VideoCapture m_videoCap;		//连接摄像头
	bool m_bStop;						//控制开始、停止
	int m_nFPS;							//帧率
	int m_nHeight;						//高
	int m_nWidth;						//宽
	int m_nReverSal;					//翻转 1：水平翻转 0：垂直翻转  -1：水平垂直翻转
	std::thread m_thread;
	std::function<void(const capinfo&)> callback_;
};

#endif