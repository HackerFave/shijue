#pragma once
#include <string>
#include <map>
#include <vector>
#include <io.h>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "log.h"

#define AINNOSUCCESS 0
#define AINNOFAILURE 1

//相机类型
enum cameratype {
    CAMERA_VISIBLE = 0, //可见光相机
    CAMERA_INFRARE = 1, //红外相机
};

//采集的图像信息
class capinfo {
public:
    cameratype type;    //相机类型
    cv::Mat img;        //图片
    cv::Mat temprature; //温度信息，只有红外相机有
    std::chrono::steady_clock::time_point m_capturetime; //采集时间
};

//人脸信息
class faceinfo{
public:
    int id;             //人脸ID
    int x;              //人脸框在可见光图片中的水平偏移
    int y;              //人脸框在可见光图片中的垂直偏移
    int h;              //人脸框高度
    int w;              //人脸框宽度
    float temperature;  //温度
    bool bfever;        //是否发热
    bool bmask;         //是否戴口罩
};

class imginfo {
public:
    capinfo visibleimg; //可见光图像信息
    capinfo infrareimg; //红外图像信息
    std::vector< faceinfo > faceinfos; //可见光图片中检测出来的所有人脸信息
};