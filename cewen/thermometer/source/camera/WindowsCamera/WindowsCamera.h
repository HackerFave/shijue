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

	//��ȡ����ͷ�豸�����б�
	int listDevices(std::vector<std::string>& vecDevices);

	static ICamera * __stdcall Create() { return new CWindowsCamera(); }
private:
	int m_nWinCameraId;					//����ͷid��Ĭ��-1��Ϊ���²����USB����ͷ�豸
	std::string m_strWinCameraName;		//����ͷ����
	cv::VideoCapture m_videoCap;		//��������ͷ
	bool m_bStop;						//���ƿ�ʼ��ֹͣ
	int m_nFPS;							//֡��
	int m_nHeight;						//��
	int m_nWidth;						//��
	int m_nReverSal;					//��ת 1��ˮƽ��ת 0����ֱ��ת  -1��ˮƽ��ֱ��ת
	std::thread m_thread;
	std::function<void(const capinfo&)> callback_;
};

#endif