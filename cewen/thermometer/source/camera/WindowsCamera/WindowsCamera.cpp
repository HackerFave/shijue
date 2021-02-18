#include "WindowsCamera.h"
#include "common.h"
#include <strmif.h>
#include <initguid.h>

#define VI_MAX_CAMERAS 20
DEFINE_GUID(CLSID_SystemDeviceEnum, 0x62be5d10, 0x60eb, 0x11d0, 0xbd, 0x3b, 0x00, 0xa0, 0xc9, 0x11, 0xce, 0x86);
DEFINE_GUID(CLSID_VideoInputDeviceCategory, 0x860bb310, 0x5d01, 0x11d0, 0xbd, 0x3b, 0x00, 0xa0, 0xc9, 0x11, 0xce, 0x86);
DEFINE_GUID(IID_ICreateDevEnum, 0x29840822, 0x5b84, 0x11d0, 0xbd, 0x3b, 0x00, 0xa0, 0xc9, 0x11, 0xce, 0x86);

CWindowsCamera::CWindowsCamera() :
	m_nWinCameraId(-1),
	m_strWinCameraName(/*"USB2.0_CAM1"*/"Integrated Webcam"),
	m_bStop(true),
	m_nFPS(25),
	m_nWidth(0),
	m_nHeight(0),
	m_nReverSal(1)
{

}

CWindowsCamera::~CWindowsCamera()
{

}

int CWindowsCamera::deinitialize(void)
{
	m_videoCap.release();
	return AINNOSUCCESS;
}

int CWindowsCamera::initialize(void)
{
	std::vector<std::string> CameraName;
	int num = listDevices(CameraName);
	if (num <= 0)
	{
		std::cout << "无摄像头设备！" << std::endl;
		return AINNOFAILURE;
	}
	std::cout << "摄像头个数：" << num << std::endl;
	for (int i = 0; i < num; i++) 
	{
		//获取选择摄像头id
		if (m_strWinCameraName == CameraName[i])
		{
			m_nWinCameraId = i;	
			std::cout << " ID: " << i << " : " << " Name: " << CameraName[i] << std::endl;
			break;
		}	
	}

	std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
	if (!m_videoCap.open(m_nWinCameraId))
	{
		std::cout << "打开摄像头失败!" << std::endl;
		return AINNOFAILURE;
	}
	std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
	std::cout << time_span.count() << " seconds." << std::endl;

	m_nWidth = m_videoCap.get(cv::CAP_PROP_FRAME_WIDTH);//获取默认宽高
	m_nHeight = m_videoCap.get(cv::CAP_PROP_FRAME_HEIGHT);

	m_videoCap.set(cv::CAP_PROP_FRAME_WIDTH, m_nWidth);//设置宽高及其他参数，具体可参考set的说明
	m_videoCap.set(cv::CAP_PROP_FRAME_HEIGHT, m_nHeight);

	return AINNOSUCCESS;
}

int CWindowsCamera::start(void)
{
	if (m_videoCap.isOpened())
	{
		m_thread = std::thread(&CWindowsCamera::run, this);
		//m_thread.detach();
	}
	else
	{
		std::cout << "摄像头未打开!" << std::endl;
	}
	return AINNOSUCCESS;
}

int CWindowsCamera::stop(void)
{
	m_bStop = false;
	return AINNOSUCCESS;
}

int CWindowsCamera::registercallback(std::function<void(const capinfo&)> callback)
{
	callback_ = callback;
	return AINNOSUCCESS;
}

void CWindowsCamera::run()
{
	m_bStop = true;
	while (m_bStop)
	{
		cv::Mat frame;
		m_videoCap >> frame;
		std::chrono::steady_clock::time_point time = std::chrono::steady_clock::now();
		if (frame.empty())
		{
			std::cout << "获取一帧图片失败!" << std::endl;
			continue;
		}

		//进行水平翻转
		cv::Mat mat;
		cv::flip(frame, mat, m_nReverSal);

		capinfo winCapInfo;
		winCapInfo.type = cameratype::CAMERA_VISIBLE;
		winCapInfo.img = mat;
		winCapInfo.m_capturetime = time;

		//发送图片
		callback_(winCapInfo);

		//设置帧率
		cv::waitKey(m_nFPS);
	}
}

void CWindowsCamera::join()
{
	m_thread.join();
}

int CWindowsCamera::listDevices(std::vector<std::string>& vecDevices)
{
	ICreateDevEnum *pDevEnum = NULL;
	IEnumMoniker *pEnum = NULL;
	int deviceCounter = 0;
	CoInitialize(NULL);

	HRESULT hr = CoCreateInstance(
		CLSID_SystemDeviceEnum,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum,
		reinterpret_cast<void**>(&pDevEnum)
	);

	if (SUCCEEDED(hr))
	{
		hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
		if (hr == S_OK) {

			IMoniker *pMoniker = NULL;
			while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
			{
				IPropertyBag *pPropBag;
				hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag,
					(void**)(&pPropBag));

				if (FAILED(hr)) {
					pMoniker->Release();
					continue; // Skip this one, maybe the next one will work.
				}

				VARIANT varName;
				VariantInit(&varName);
				hr = pPropBag->Read(L"Description", &varName, 0);
				if (FAILED(hr))
				{
					hr = pPropBag->Read(L"FriendlyName", &varName, 0);
				}

				if (SUCCEEDED(hr))
				{
					hr = pPropBag->Read(L"FriendlyName", &varName, 0);
					int count = 0;
					char tmp[255] = { 0 };
					while (varName.bstrVal[count] != 0x00 && count < 255)
					{
						tmp[count] = (char)varName.bstrVal[count];
						count++;
					}
					vecDevices.push_back(tmp);
				}

				pPropBag->Release();
				pPropBag = NULL;
				pMoniker->Release();
				pMoniker = NULL;

				deviceCounter++;
			}

			pDevEnum->Release();
			pDevEnum = NULL;
			pEnum->Release();
			pEnum = NULL;
		}
	}
	return deviceCounter;
}

