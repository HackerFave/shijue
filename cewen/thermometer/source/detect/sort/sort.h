#pragma once
#include <iostream>
#include <fstream>
#include <iomanip> // to format image names using setw() and setfill()
#include <io.h>    // to check file existence using POSIX function access(). On Linux include <unistd.h>.

#include <set>
#include <vector>
#include "Hungarian.h"
#include "KalmanTracker.h"

#include "opencv2/video/tracking.hpp"
#include "opencv2/highgui/highgui.hpp"

typedef struct TrackingBox
{
    int frame;
    int id;
    cv::Rect_<float> box;
}TrackingBox;

class sort {
public:
    void initialize(void);
    std::vector<TrackingBox> track(std::vector<TrackingBox> detFrameData);
    void deinitialize(void);

private:
    int frame_count = 0;
    int max_age = 1;
    int min_hits = 3;
    double iouThreshold = 0.3;
    std::vector<KalmanTracker> trackers;
    std::vector<cv::Rect_<float>> predictedBoxes;
    std::vector<std::vector<double>> iouMatrix;
    std::vector<int> assignment;
    set<int> unmatchedDetections;
    set<int> unmatchedTrajectories;
    set<int> allItems;
    set<int> matchedItems;
    std::vector<cv::Point> matchedPairs;
    std::vector<TrackingBox> frameTrackingResult;
    unsigned int trkNum = 0;
    unsigned int detNum = 0;
};