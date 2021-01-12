#pragma once

#ifndef CAMERA_INTERFACE_H 
#define CAMERA_INTERFACE_H
#endif

#ifdef BUILD_CAMERA_DLL // ���뵼����־��ʹ���� DLL ����ʱ����Ϊ�������� DLL ����ʱ����Ϊ����
#define IO_CAMERA_DLL __declspec(dllexport) // ��������
#else
#define IO_CAMERA_DLL __declspec(dllimport) // ���붨��
#endif

extern "C" // ��׼ C ��ʽ����ֹ�����������仯
{
    IO_CAMERA_DLL void *GetCameraList();
    IO_CAMERA_DLL void *GetCamera(void *pChar);
}