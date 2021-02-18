#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <time.h>
#include <opencv.hpp>
#include "yolo.h"

using namespace cv::dnn;
using namespace std; 

int main(int argc, char **argv)
{
    alginfo *palginfo = Init(
        "D:\\ainnovation\\projects\\thermometer\\face_detection_cpp\\face_location\\test\\yolov3-face_net123.cfg",
        "D:\\ainnovation\\projects\\thermometer\\face_detection_cpp\\face_location\\test\\yolov3-face_net123_50000.weights"
    );

    cv::Mat image1 = cv::imread("D:\\ainnovation\\projects\\thermometer\\face_detection_cpp\\face_location\\test\\1.jpg");

    locationinfo_s outputs;
    clock_t start, end;
    start = clock();
    outputs = Inference(palginfo, image1);
    end = clock();
    std::cout << "inference time: " << double(end - start) / CLOCKS_PER_SEC << std::endl;

    for (size_t i = 0; i < outputs.num; i++)
    {
        int classID = outputs.classIDs[i];
        std::string outname = std::to_string(i) + "_" + std::to_string(outputs.corrdinate[i][0]) + "_" + std::to_string(classID) + ".jpg";
        cv::imwrite(outname.c_str(), outputs.img[i]);
        std::cout << std::to_string(i) << " " << std::to_string(outputs.corrdinate[i][0]) << " " << std::to_string(classID) << std::endl;

    }

    UnInit(palginfo);

    system("pause");

    return 0;
}