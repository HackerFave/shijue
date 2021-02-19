#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <math.h>
#include <map>
#include "fileprojectmanager.h"
#include <typeinfo>
#include <omp.h>

//#define ANALYSISDEBUG

void print_all_pixel(cv::Mat mat);

typedef struct
{
    float global_ok_thr; //全局ok的阈值，通过拖动设置
    float global_ng_thr; //全局ng的阈值，通过拖动设置

}Sthreshold; //用户设置的所有阈值，需要外部传入

typedef struct
{
    int tp = 0; //gt=1, pred=1
    int tn = 0; //gt=0, pred=0
    int fp = 0; //gt=0, pred=1
    int fn = 0; //gt=1, pred=0
    int ok_uncertain = 0; //gt=1, pred=中间
    int ng_uncertain = 0; //gt=0, pred=中间
}Sconfusionmatrix; //gt, pred; 0:negative, 1:positive

typedef struct
{
    float auc;
    float best_threshold;
}Srocinfo; //根据roc可得到的信息

enum PredResult
{
    PredOK = 0,
    PredNG = 1,
    PredUNCERTAIN = 2
};

enum GtResult
{
    GtOK = 0,
    GtNG = 1
};

template<class T>
struct DisableCompare :public std::binary_function<T, T, bool>
{
    bool operator()(T lhs, T rhs) const
    {
        return true;
    }
};

class Analysiser
{
public:
    Analysiser();
    ~Analysiser();

private:
    QString pred_masks_dir_; //预测结果mask保存目录
    QString gt_masks_dir_; //标注结果mask保存目录
    std::vector<std::string> pred_masks_path_; //预测mask的绝对路径
    std::vector<std::string> gt_masks_path_; //真实mask的绝对路径

    std::vector<int> confusion_matrix_; //混淆矩阵计算结果
    Sthreshold cm_threshold_;
    std::vector<std::pair<float, int> > roc_data_; //用于计算roc的数据对。first：预测为1类的置信度,0-100；second：真实标签
    std::map<float, std::pair<float, float>> roc_result_; //roc计算结果,<score, <fpr, tpr>>
    Srocinfo roc_info_; //根据roc得到的auc/阈值等结果

    //缺陷级统计。_defect_表示缺陷级
    std::map<int, int> histogram_ok_defect_; //只是每个分数上进行统计，没有进行累加
    std::map<int, int> histogram_ng_defect_;
    std::map<int, int> histogram_ok_defect_accumulate_; //对缺陷级统计进行累加的结果，即需要返回外部的结果
    std::map<int, int> histogram_ng_defect_accumulate_;
    int up_defect_ = 0; //缺陷级直方图统计结果的纵轴上限


    //图片级统计。x:分数，y：样本预测错误个数
    std::map<int, int> histogram_ok_img_;  //只是每个分数上进行统计，没有进行累加。key：0-100分数；value：此分数阈值下样本预测错误个数
    std::map<int, int> histogram_ng_img_;
    std::map<int, int> histogram_ok_img_accumulate_; //对图像级统计进行累加的结果，即需要返回外部的结果
    std::map<int, int> histogram_ng_img_accumulate_;
    int up_img_ = 0; //图片级直方图统计结果的纵轴上限

    QString errcode_;

    bool UpdatePathSuccess_ = false; //获取原图及预测结果成功的标志
    bool GetRocSuccess_ = false; //计算roc成功的标志

    float PixelFilterScore_ = 0.2; //每个像素的过滤分数
    int DefectComponentScoreMode_ = 0; //缺陷连通域分数计算方式

    omp_lock_t lock_;

private:
    void UnitTest();

    bool HaveElement(std::vector<std::string> vec, std::string element);
    std::vector<std::string> Split(const std::string& str, const std::string& pattern);
    void ListDir(std::string path, std::vector<std::string>& files);
    std::vector<std::string> VectorIntersection(std::vector<std::string> v1, std::vector<std::string> v2);

    void GenerateInfo(QString info, QString level = "ERROR"); //打印及弹窗显示错误信息
    static bool Compare(const std::pair<float, int>&i, const std::pair<float, int>&j);

    //cv::Mat MaskTransform(Sthreshold threshold); //将所有过滤方法统一转为mask
    //cv::Mat MaskFilter(cv::Mat pred_mask, cv::Mat filter_mask); //对预测mask进行过滤

    std::map<float, std::pair<float, float>> CalculateRocAndHistogram(std::vector<std::string> gt_masks_path, std::vector<std::string> pred_masks_path); //计算roc
    std::vector<std::pair<float, int> > GetRocData(std::vector<std::string> gt_masks_path, std::vector<std::string> pred_masks_path); //获取用于计算roc的roc_data
    Sconfusionmatrix CalculateConfusionMatrix(std::vector<std::pair<float, int> > roc_data, Sthreshold threshold);

    //用于结果可视化
    void DrawROC(std::map<float, std::pair<float, float>> roc_result);
    void DrawHistogram(std::map<int, int>, std::string, int x_limit, int y_limit);

    //统计缺陷级信息
    void DefectStatistic(cv::Mat gt_mask, cv::Mat pred_mask);
    template <typename MATTYPE>
    std::vector<std::vector<int>> np_where(const cv::Mat& img_gray, const MATTYPE& value);
    float GetScoreFromMat(cv::Mat mask, std::string mode, std::string method, std::vector<std::vector<int>> points, cv::Mat region, std::string op, int value);
    bool Intersect(std::vector<std::vector<cv::Point> > contours1, std::vector<std::vector<cv::Point> > contours2, cv::Size size);
    float ValueInContours(const cv::Mat& mask, const std::vector<std::vector<cv::Point> >& contours);
    void AddToMap(std::map<int, int>& histogram, int key, int value);
    template <typename TYPE>
    std::vector<TYPE> unique(const cv::Mat& input, bool sort = false);

    //对缺陷级和图片级统计结果进行后处理，即进行累加，返回需要展示的形状。对于ok的需要reverse，对于ng的不需要reverse
    std::map<int, int> HistogramPostProcess(std::map<int, int> histogram, bool reverse); //后处理小于1ms

    void set_max_number_img(); //返回图片级直方图统计结果的纵轴上限
    void set_max_number_defect(); //返回缺陷级直方图统计结果的纵轴上限
    void set_confusion_matrix(Sthreshold threshold);
    template <typename TYPE>
    TYPE StringToNum(const std::string& str);

    void SetRocAndHistogram(float pixel_filter_score, int defect_filter_mode, bool from_file=false);

    //并发
    static void ImageReadThread(const std::string& img_path, cv::Mat* mat);
    static void FindContoursThread(const cv::Mat& mat, std::vector<std::vector<cv::Point>>* contours);



public: //提供给外部获取结果的接口

    bool UpdatePath(); //更新pred_masks_path_路径，更新gt_masks_path_路径。返回等长的pred_masks_path_和gt_masks_path_，一张图必须在两个路径中均存在。
    
    //保存分析结果 SaveResultToFile time: 0.0220218 second
    void SaveResultToFile(); //将roc信息/图片级直方图统计信息/缺陷级直方图统计信息/混淆矩阵保存至文件
    //读取分析结果 ReadResultFromFile time: 0.004063 second
    bool ReadResultFromFile(); //从文件中获取分析结果：roc信息/图片级直方图统计信息/缺陷级直方图统计信息/混淆矩阵

    //必须先get_roc才能get_histogram
    std::map<float, std::pair<float, float>> get_roc(float pixel_filter_score, int defect_filter_mode, bool from_file)
    {
        SetRocAndHistogram(pixel_filter_score, defect_filter_mode, from_file);

        return roc_result_;
    }

    std::map<int, int> get_histogram_ok()
    {
        return histogram_ok_img_accumulate_;
    }

    std::map<int, int> get_histogram_ng()
    {
        return histogram_ng_img_accumulate_;
    }

    int get_max_number() //返回图片级直方图统计结果的纵轴上限
    {
        set_max_number_img();

        return up_img_;
    }

    float get_auc()
    {
        return roc_info_.auc;
    }

    float get_bestthreshold()
    {
        return roc_info_.best_threshold;
    }

    std::vector<int> get_confusion_matrix(Sthreshold threshold)
    {
        cm_threshold_ = threshold;
        set_confusion_matrix(threshold);
        return confusion_matrix_;
    }

    std::map<int, int> get_histogram_ok_defect()
    {
        return histogram_ok_defect_accumulate_;
    }

    std::map<int, int> get_histogram_ng_defect()
    {
#ifdef ANALYSISDEBUG
        int up = get_max_number_defect();
        DrawHistogram(histogram_ng_defect_accumulate_, "histogram_ng_defect.png", 101, up);
        DrawHistogram(histogram_ok_defect_accumulate_, "histogram_ok_defect.png", 101, up);
#endif

        return histogram_ng_defect_accumulate_;
    }

    int get_max_number_defect() //返回缺陷级直方图统计结果的纵轴上限
    {
        set_max_number_defect();

        return up_defect_;
    }

    bool run_normal()
    {
        return UpdatePathSuccess_ && GetRocSuccess_;
    }

    Sthreshold get_histogram_threshold() //获取拖动的阈值
    {
        return cm_threshold_;
    }
};
