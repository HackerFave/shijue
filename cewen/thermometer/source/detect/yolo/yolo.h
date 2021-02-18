#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <opencv.hpp>

using namespace cv::dnn;
using namespace std;

typedef struct locationinfo
{
    int num; //检测到的人脸数量
    cv::Mat img[10]; //裁剪出的人脸区域
    std::vector<std::vector<int>> corrdinate; // [10];
    std::vector<int> classIDs;
}locationinfo_s;

class alginfo
{
public:
    float confThreshold;// = 0.5; // Confidence threshold
    float nmsThreshold;// = 0.4;  // Non-maximum suppression threshold
    int inpWidth;// = 416;  // Width of network's input image
    int inpHeight;// = 416; // Height of network's input image
    vector<string> classes;
    cv::dnn::Net net;
};

// 初始化
alginfo* Init(string modelConfiguration, string modelWeights);

// 算法处理
locationinfo Inference(alginfo *palginfo, const cv::Mat& images);

// 释放
void UnInit(alginfo *palginfo);

vector<string> getOutputsNames(const Net& net);
locationinfo_s postprocess(const cv::Mat& frame, const vector<cv::Mat>& outs, float confThreshold, float nmsThreshold);


