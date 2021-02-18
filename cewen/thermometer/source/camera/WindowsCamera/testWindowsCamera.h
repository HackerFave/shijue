#pragma once
#include "WindowsCamera.h"
#include <windows.h>

CWindowsCamera windowsCamera;
int count = 0;
void showImage(const capinfo& capInfoIn)
{
	cv::Mat edges;
	cv::namedWindow("edges");

	cv::imshow("edges", capInfoIn.img);		
}

void Text()
{
//	CWindowsCamera windowsCamera;

	if (!windowsCamera.initialize())
	{
		std::cout << "��ʼ��ʧ�ܣ�" << std::endl;
	}

	windowsCamera.registercallback(showImage);

	windowsCamera.start();
	count = 0;
	std::cout << "��ͣ" << std::endl;
	Sleep(10000);
	windowsCamera.stop();
	windowsCamera.join();

 	windowsCamera.deinitialize();
}
