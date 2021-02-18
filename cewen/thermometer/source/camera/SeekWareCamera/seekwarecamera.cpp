#include "seekwarecamera.h"
#include "SeekWare.h"



#ifdef _DEBUG
#pragma comment(lib, "../source/camera/SeekWareCamera/SeekWare.lib")
#else
#pragma comment(lib, "../source/camera/SeekWareCamera/SeekWare.lib")
#endif

int SeekWareCamera::deinitialize(void) {
	
	if (m_isopen)
		stop();

	if (m_camera != NULL)
	{
		psw* pswcamera = (psw*)m_camera;
		delete pswcamera;
		m_camera = NULL;
	}
	return AINNOSUCCESS;
}

int SeekWareCamera::initialize(void) {

	if (m_camera != NULL)
		deinitialize();

	psw pswlist[10];
	psw* pswcamera;
	int numfound, retcode;
	
	//1查找设备
	retcode = Seekware_Find(pswlist, 10, &numfound);
	if(numfound<1)
		return AINNOFAILURE;
	
	//2获取设备句柄
	if (m_camera == NULL)
	{
		pswcamera = new psw(pswlist[0]);
		m_camera = (void*)pswcamera;
	}
	return AINNOSUCCESS;
}

int SeekWareCamera::start(void) {

	if (m_camera == NULL)
		return AINNOFAILURE;
	psw* pswcamera = (psw*)m_camera;

	if (m_isopen == false)
	{
		sw_retcode retCode = Seekware_Open(*pswcamera);
		if (retCode == sw_retcode::SW_RETCODE_NONE || retCode == sw_retcode::SW_RETCODE_OPENEX)
		{
			m_isopen = true;
			m_imageheight = (*pswcamera)->frame_rows;
			m_imagewidth = (*pswcamera)->frame_cols;

			//开启线程
			m_thread = std::thread(&SeekWareCamera::run,this);
			//m_thread.detach();
			
			return AINNOSUCCESS;
		}
	}
	return AINNOFAILURE;
}

int SeekWareCamera::stop(void) {

	if (m_camera == NULL)
		return AINNOFAILURE;
	psw* pswcamera = (psw*)m_camera;

	m_isopen = false;
	m_thread.join();
	Seekware_Close(*pswcamera);
	
	return AINNOSUCCESS;
}

int SeekWareCamera::registercallback(std::function<void(const capinfo&)> callback) {
	callback_ = callback;
	return AINNOSUCCESS;
}

void SeekWareCamera::run()
{
	if (m_camera == NULL)
		return;
	psw* pswcamera = (psw*)m_camera;

	float* temperatureImage = new float[m_imagewidth * m_imageheight];

	while (m_isopen)
	{
		capinfo swcapinfo;
		sw_retcode seekRet = sw_retcode::SW_RETCODE_BPARAM;
		
		swcapinfo.img = cv::Mat(m_imageheight, m_imagewidth, CV_8UC4);
		seekRet = Seekware_GetImage(*pswcamera, NULL, temperatureImage, (uint32_t *)swcapinfo.img.data);
		if (seekRet != sw_retcode::SW_RETCODE_NONE)
			continue;

		swcapinfo.m_capturetime = std::chrono::steady_clock::now();
		swcapinfo.type = cameratype::CAMERA_INFRARE;
		callback_(swcapinfo);
	}

	if (temperatureImage != NULL)
	{
		delete[] temperatureImage;
		temperatureImage = NULL;
	}
}
