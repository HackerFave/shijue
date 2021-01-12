#pragma once

#ifndef CAMERA_INTERFACE_H 
#define CAMERA_INTERFACE_H
#endif

#ifdef BUILD_CAMERA_DLL // 导入导出标志，使其在 DLL 定义时声明为导出，再 DLL 调用时声明为导入
#define IO_CAMERA_DLL __declspec(dllexport) // 导出定义
#else
#define IO_CAMERA_DLL __declspec(dllimport) // 导入定义
#endif

extern "C" // 标准 C 格式，防止导出函数名变化
{
    IO_CAMERA_DLL void *GetCameraList();
    IO_CAMERA_DLL void *GetCamera(void *pChar);
}