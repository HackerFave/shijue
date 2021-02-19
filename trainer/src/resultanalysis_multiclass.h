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
#include <qdebug.h>
#pragma execution_character_set("utf-8")
#define ANALYSISDEBUG

void print_all_pixel(cv::Mat mat);

typedef struct
{
    float global_ok_thr; //ȫ��ok����ֵ��ͨ���϶�����
    float global_ng_thr; //ȫ��ng����ֵ��ͨ���϶�����

}Sthreshold; //�û����õ�������ֵ����Ҫ�ⲿ����

typedef struct
{
    int tp = 0; //gt=1, pred=1
    int tn = 0; //gt=0, pred=0
    int fp = 0; //gt=0, pred=1
    int fn = 0; //gt=1, pred=0
    int ok_uncertain = 0; //gt=1, pred=�м�
    int ng_uncertain = 0; //gt=0, pred=�м�
}Sconfusionmatrix; //gt, pred; 0:negative, 1:positive

typedef struct
{
    float auc;
    float best_threshold;
}Srocinfo; //����roc�ɵõ�����Ϣ

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
    QString pred_masks_dir_; //Ԥ����mask����Ŀ¼
    QString gt_masks_dir_; //��ע���mask����Ŀ¼
    std::vector<std::string> pred_masks_path_; //Ԥ��mask�ľ���·��
    std::vector<std::string> gt_masks_path_; //��ʵmask�ľ���·��

    //std::vector<int> confusion_matrix_; //�������������
    Sthreshold cm_threshold_;
    //std::vector<std::pair<float, int> > roc_data_; //���ڼ���roc�����ݶԡ�first��Ԥ��Ϊ1������Ŷ�,0-100��second����ʵ��ǩ
    //std::map<float, std::pair<float, float>> roc_result_; //roc������,<score, <fpr, tpr>>
    //Srocinfo roc_info_; //����roc�õ���auc/��ֵ�Ƚ��

    //ȱ�ݼ�ͳ�ơ�_defect_��ʾȱ�ݼ�
    //std::map<int, int> histogram_ok_defect_; //ֻ��ÿ�������Ͻ���ͳ�ƣ�û�н����ۼ�
    //std::map<int, int> histogram_ng_defect_;
    //std::map<int, int> histogram_ok_defect_accumulate_; //��ȱ�ݼ�ͳ�ƽ����ۼӵĽ��������Ҫ�����ⲿ�Ľ��
    //std::map<int, int> histogram_ng_defect_accumulate_;
    //int up_defect_ = 0; //ȱ�ݼ�ֱ��ͼͳ�ƽ������������


    //ͼƬ��ͳ�ơ�x:������y������Ԥ��������
    //std::map<int, int> histogram_ok_img_;  //ֻ��ÿ�������Ͻ���ͳ�ƣ�û�н����ۼӡ�key��0-100������value���˷�����ֵ������Ԥ��������
    //std::map<int, int> histogram_ng_img_;
    //std::map<int, int> histogram_ok_img_accumulate_; //��ͼ��ͳ�ƽ����ۼӵĽ��������Ҫ�����ⲿ�Ľ��
    //std::map<int, int> histogram_ng_img_accumulate_;
    //int up_img_ = 0; //ͼƬ��ֱ��ͼͳ�ƽ������������

    QString errcode_;

    bool UpdatePathSuccess_ = false; //��ȡԭͼ��Ԥ�����ɹ��ı�־
    bool GetRocSuccess_ = false; //����roc�ɹ��ı�־

    float PixelFilterScore_ = 0.2; //ÿ�����صĹ��˷���
    int DefectComponentScoreMode_ = 0; //ȱ����ͨ��������㷽ʽ

    omp_lock_t lock_;
    omp_lock_t lock_out_;

    /*******************************************************************/
    //������������ÿ���������Ϊmap��keyΪ�����ţ�value��ԭ��һ��
    int nclass_ = 2; //������

    //roc���
    std::map<int, std::vector<std::pair<float, int> > > roc_data_;
    std::map<int, std::map<float, std::pair<float, float>>> roc_result_;
    std::map<int, Srocinfo> roc_info_;
    //ȱ�ݼ�ͳ��
    std::map<int, std::map<int, int>> histogram_ok_defect_;
    std::map<int, std::map<int, int>> histogram_ng_defect_;
    std::map<int, std::map<int, int>> histogram_ok_defect_accumulate_;
    std::map<int, std::map<int, int>> histogram_ng_defect_accumulate_;
    std::map<int, int> up_defect_;
    //ͼƬ��ͳ��
    std::map<int, std::map<int, int>> histogram_ok_img_;
    std::map<int, std::map<int, int>> histogram_ng_img_;
    std::map<int, std::map<int, int>> histogram_ok_img_accumulate_;
    std::map<int, std::map<int, int>> histogram_ng_img_accumulate_;
    std::map<int, int> up_img_;

    //��������
    std::vector<int> confusion_matrix_; //�������������


    /*******************************************************************/


private:
    bool HaveElement(std::vector<std::string> vec, std::string element);
    std::vector<std::string> Split(const std::string& str, const std::string& pattern);
    void ListDir(std::string path, std::vector<std::string>& files);
    std::vector<std::string> VectorIntersection(std::vector<std::string> v1, std::vector<std::string> v2);

    void GenerateInfo(QString info, QString level = "ERROR"); //��ӡ��������ʾ������Ϣ
    static bool Compare(const std::pair<float, int>&i, const std::pair<float, int>&j);

    //cv::Mat MaskTransform(Sthreshold threshold); //�����й��˷���ͳһתΪmask
    //cv::Mat MaskFilter(cv::Mat pred_mask, cv::Mat filter_mask); //��Ԥ��mask���й���

    std::map<int, std::map<float, std::pair<float, float>>> CalculateRocAndHistogram(std::vector<std::string> gt_masks_path, std::vector<std::string> pred_masks_path); //����roc
    void GetRocData(std::vector<std::string> gt_masks_path, std::vector<std::string> pred_masks_path); //��ȡ���ڼ���roc��roc_data
    Sconfusionmatrix CalculateConfusionMatrix(std::map<int, std::vector<std::pair<float, int> >> roc_data, std::map<int, Sthreshold> thresholds);

    //���ڽ�����ӻ�
    void DrawROC(std::map<int, std::map<float, std::pair<float, float>>> roc_result);
    void DrawHistogram(std::map<int, std::map<int, int>>, std::string, int x_limit, std::map<int, int> y_limit);

    //ͳ��ȱ�ݼ���Ϣ
    void DefectStatistic(cv::Mat gt_mask, cv::Mat pred_mask, int class_id);
    template <typename MATTYPE>
    std::vector<std::vector<int>> np_where(const cv::Mat& img_gray, const MATTYPE& value);
    float GetScoreFromMat(cv::Mat mask, std::string mode, std::string method, std::vector<std::vector<int>> points, cv::Mat region, std::string op, int value);
    bool Intersect(std::vector<std::vector<cv::Point> > contours1, std::vector<std::vector<cv::Point> > contours2, cv::Size size);
    float ValueInContours(const cv::Mat& mask, const std::vector<std::vector<cv::Point> >& contours);
    void AddToMap(std::map<int, int>& histogram, int key, int value);
    template <typename TYPE>
    std::vector<TYPE> unique(const cv::Mat& input, bool sort = false);

    //��ȱ�ݼ���ͼƬ��ͳ�ƽ�����к����������ۼӣ�������Ҫչʾ����״������ok����Ҫreverse������ng�Ĳ���Ҫreverse
    std::map<int, int> HistogramPostProcess(std::map<int, int> histogram, bool reverse); //����С��1ms
    //���ݸ�������ͳ�ƽ������DefectComponentScoreMode_�������ܵ�ͳ�ƽ��
    void GetRocAndHistogramTotal(const std::vector<cv::Mat>& gt_pred_mask_array);

    void set_max_number_img(); //����ͼƬ��ֱ��ͼͳ�ƽ������������
    void set_max_number_defect(); //����ȱ�ݼ�ֱ��ͼͳ�ƽ������������
    void set_confusion_matrix(std::map<int, Sthreshold> thresholds);
    template <typename TYPE>
    TYPE StringToNum(const std::string& str);

    void SetRocAndHistogram(float pixel_filter_score, int defect_filter_mode, bool from_file=false);

    //����
    static void ImageReadThread(const std::string& img_path, cv::Mat* mat);
    static void FindContoursThread(const cv::Mat& mat, std::vector<std::vector<cv::Point>>* contours);



public: //�ṩ���ⲿ��ȡ����Ľӿ�

    bool UpdatePath(); //����pred_masks_path_·��������gt_masks_path_·�������صȳ���pred_masks_path_��gt_masks_path_��һ��ͼ����������·���о����ڡ�
    void SetNClass(int nclass)
    {
        nclass_ = nclass;
    }
    
    //���������� SaveResultToFile time: 0.0220218 second
    void SaveResultToFile(); //��roc��Ϣ/ͼƬ��ֱ��ͼͳ����Ϣ/ȱ�ݼ�ֱ��ͼͳ����Ϣ/�������󱣴����ļ�
    //��ȡ������� ReadResultFromFile time: 0.004063 second
    bool ReadResultFromFile(); //���ļ��л�ȡ���������roc��Ϣ/ͼƬ��ֱ��ͼͳ����Ϣ/ȱ�ݼ�ֱ��ͼͳ����Ϣ/��������

    //������get_roc����get_histogram
    std::map<int, std::map<float, std::pair<float, float>>> get_roc(float pixel_filter_score, int defect_filter_mode, bool from_file)
    {
        SetRocAndHistogram(pixel_filter_score, defect_filter_mode, from_file);

        return roc_result_;
    }

    std::map<int, std::map<int, int>> get_histogram_ok_img()
    {
        return histogram_ok_img_accumulate_;
    }

    std::map<int, std::map<int, int>> get_histogram_ng_img()
    {
        return histogram_ng_img_accumulate_;
    }

    std::map<int, int> get_max_number_img() //����ͼƬ��ֱ��ͼͳ�ƽ������������
    {
        set_max_number_img();

        return up_img_;
    }

    std::map<int, float> get_auc()
    {
        std::map<int, float> aucs;
        for (int class_id = 0; class_id < nclass_; class_id++)
        {
            aucs[class_id] = roc_info_[class_id].auc;
        }

        return aucs;
    }

    std::map<int, float> get_bestthreshold()
    {
        std::map<int, float> best_thresholds;
        for (int class_id = 0; class_id < nclass_; class_id++)
        {
            best_thresholds[class_id] = roc_info_[class_id].best_threshold;
        }

        return best_thresholds;
    }

    std::vector<int> get_confusion_matrix(std::map<int, Sthreshold> thresholds)
    {
        //cm_threshold_ = threshold;
        set_confusion_matrix(thresholds);
        return confusion_matrix_;
    }

    std::map<int, std::map<int, int>> get_histogram_ok_defect()
    {
        return histogram_ok_defect_accumulate_;
    }

    std::map<int, std::map<int, int>> get_histogram_ng_defect()
    {
        return histogram_ng_defect_accumulate_;
    }

    std::map<int, int> get_max_number_defect() //����ȱ�ݼ�ֱ��ͼͳ�ƽ������������
    {
        set_max_number_defect();

        return up_defect_;
    }

    bool run_normal()
    {
        return UpdatePathSuccess_ && GetRocSuccess_;
    }

    Sthreshold get_histogram_threshold() //��ȡ�϶�����ֵ
    {
        return cm_threshold_;
    }
};
