#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <opencv.hpp>
#include "yolo.h"

using namespace cv::dnn;
using namespace std; 

alginfo* Init(string modelConfiguration, string modelWeights)
{
    alginfo* palginfo = new alginfo();
    palginfo->confThreshold = 0.2;
    palginfo->nmsThreshold = 0.4;
    palginfo->inpWidth = 416;
    palginfo->inpHeight = 416;
    palginfo->classes.push_back("face");
    palginfo->classes.push_back("mask");

    // Load the network
    palginfo->net = readNetFromDarknet(modelConfiguration, modelWeights);
    palginfo->net.setPreferableBackend(DNN_BACKEND_OPENCV);
    palginfo->net.setPreferableTarget(DNN_TARGET_CPU);

    return palginfo;
}

// 算法处理
locationinfo_s Inference(alginfo *palginfo, const cv::Mat& images)
{
    locationinfo_s res;

    cv::Mat blob;
    blobFromImage(images, blob, 1 / 255.0, cv::Size(palginfo->inpWidth, palginfo->inpHeight), cv::Scalar(0, 0, 0), true, false);

    //Sets the input to the network
    palginfo->net.setInput(blob);

    // Runs the forward pass to get output of the output layers
    vector<cv::Mat> outs;
    palginfo->net.forward(outs, getOutputsNames(palginfo->net));

    // Remove the bounding boxes with low confidence
    res = postprocess(images, outs, palginfo->confThreshold, palginfo->nmsThreshold);

    return res;
}

// 释放
void UnInit(alginfo* palginfo)
{
    if (&palginfo != NULL)
    {
        return delete palginfo;
    }
}

// Remove the bounding boxes with low confidence using non-maxima suppression
// void postprocess(cv::Mat& frame, const vector<cv::Mat>& outs)
locationinfo_s postprocess(const cv::Mat& frame, const vector<cv::Mat>& outs, float confThreshold, float nmsThreshold)
{
    vector<int> classIds;
    vector<float> confidences;
    vector<cv::Rect> boxes;

    locationinfo_s res;

    int height = frame.rows;
    int width = frame.cols;

    for (size_t i = 0; i < outs.size(); ++i)
    {
        // Scan through all the bounding boxes output from the network and keep only the
        // ones with high confidence scores. Assign the box's class label as the class
        // with the highest score for the box.
        float* data = (float*)outs[i].data;
        for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
        {
            cv::Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
            cv::Point classIdPoint;
            double confidence;
            // Get the value and location of the maximum score
            minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
            if (confidence > confThreshold)
            {
                int centerX = (int)(data[0] * frame.cols);
                int centerY = (int)(data[1] * frame.rows);
                int width = (int)(data[2] * frame.cols);
                int height = (int)(data[3] * frame.rows);
                int left = centerX - width / 2;
                int top = centerY - height / 2;

                classIds.push_back(classIdPoint.x);
                confidences.push_back((float)confidence);
                boxes.push_back(cv::Rect(left, top, width, height));
            }
        }
    }

    // Perform non maximum suppression to eliminate redundant overlapping boxes with lower confidences
    vector<int> indices;
    NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);
    cout << "total number of person:" << indices.size() << endl << endl;
    res.num = indices.size();

    for (size_t i = 0; i < indices.size(); ++i)
    {
        int idx = indices[i];
        cv::Rect box = boxes[idx];

        // crop region
        int top = box.y - 0.2*box.height;
        int bottom = box.y + 1.4*box.height;

        if (top < 0)
        {
            top = 0;
        }
        if (bottom > (height - 1))
        {
            bottom = height - 1;
        }
        //cout << "top: " << top << ", bottom: " << bottom << endl;

        //cv::Rect area(box.x - 0.2*box.width, top, 1.4*box.width, bottom - top); //左上点x,y,  w,h
        cv::Rect area(box.x, box.y, box.width, box.height);

        cv::Mat person = frame(area);

        res.img[i] = person;

        std::vector<int> position;
        position.push_back(box.x);
        position.push_back(box.y);
        position.push_back(box.width);
        position.push_back(box.height);
        res.corrdinate.push_back(position);

        res.classIDs.push_back(classIds[idx]);

        // imshow("person", person);
        // waitKey(0);
    }

    return res;

}

// Get the names of the output layers
vector<string> getOutputsNames(const Net& net)
{
    static vector<string> names;
    if (names.empty())
    {
        //Get the indices of the output layers, i.e. the layers with unconnected outputs
        vector<int> outLayers = net.getUnconnectedOutLayers();

        //get the names of all the layers in the network
        vector<string> layersNames = net.getLayerNames();

        // Get the names of the output layers in names
        names.resize(outLayers.size());
        for (size_t i = 0; i < outLayers.size(); ++i)
            names[i] = layersNames[outLayers[i] - 1];
    }
    return names;
}

