#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <deque>
#include <QFileDialog>
#include <QJsonParseError>
#include <QJsonObject>
#include <QVariant>
#include <qdebug.h>
#include "databaseSqlInset.h"
#include "opencv2/opencv.hpp"
#include "omp.h"
#include "dataSql.h"
#include <QtConcurrent>
#include "common.h"
#include <thread>
#include <Windows.h>
#include <functional>

#pragma execution_character_set("utf-8")
//#define DEBUG_POSTPROCESS
#define DEBUG_TIME
//#define OPENMP

#define camera_width_ 5472 //相机采图尺寸
#define camera_height_ 3648
#define input_width_ 2560 //后处理模块输入尺寸       //TODO：每换模型都需要手动修改，如何自动获取？？？？
#define input_height_ 1792
#define reduce_ratio_w_ ((float)camera_width_ / input_width_)
#define reduce_ratio_h_ ((float)camera_height_ / input_height_)

#define mm_per_pixel_w_ (0.073 * reduce_ratio_w_)
#define mm_per_pixel_h_ (0.073 * reduce_ratio_h_)

typedef std::map<QString, QString> DatabaseMap;
std::vector<DatabaseMap> DefectMap(std::vector<DatabaseMap> database_record, int height, int width, std::string savename);

template <typename TYPE>
TYPE StringToNum(const std::string& str);

typedef struct
{
	int blue;
	int green;
	int red;
}Spixelbgr;

typedef struct
{
	Spixelbgr min_normal_pixel;
	Spixelbgr max_normal_pixel;
}Snormalpixel;

//std::map<float/*用户设定的0-1的分数阈值*/, int/*像素值*/> color_lut_ = 
//{
//	//TODO
//	{0.0, 5},
//	{0.1, 10},
//	{0.2, 12},
//
//}

enum DEFECTTYPE
{
	GANSHA = 1,
	KAILIE,
	HUANGBAN,
	XUHUA,
	WUWU,
	BUJUN,
	WANZHE
};

enum THRESHOLDTYPE
{
	ALARM,
	DATABASE,
	EXPORT
};

typedef struct
{
	//用户给定的过滤条件,单位均为毫米
	std::string type = "alarm";
	std::string updatetime = "2020-07-13 18:35:44";
	float gansha_width = 0;
	float gansha_height = 0;
	float kailie_width = 0.5;
	float kailie_height = 30;
	float huangban_color = 0.7;
	float xuhua_width = 1.0;
	float wuwu_area = 16;
	float bujun_width = 0.5;
	float wanzhe_curvature = 3;

	//内部设定的过滤条件
	float gansha_maxratio = 1.0; //干纱一定是纵向的，宽高比一定小于1.0
	int gansha_minarea = 200 * 200; //干纱最小像素面积
	float allclass_minarea = 50 * 50; //除污物外的其他缺陷均需要大于此阈值

}Sthreshold; //TODO：根据input size自动适应

class PostProcesser
{
public:
    PostProcesser();
	~PostProcesser();

private:
	void SetNormalpixel(cv::Mat); //根据划定的一块正常区域，计算正常材料像素值的上下限

	bool FilterHuangban(cv::Mat& index_pred, const cv::Mat& origin_img, const std::vector<cv::Point>& contour);
	bool FilterGansha(cv::Mat& index_pred, const std::vector<cv::Point>& contour);
	bool FilterWanzhe(cv::Mat& index_pred, const std::vector<cv::Point>& contour);
    bool FilterWuwu(cv::Mat& index_pred, const std::vector<cv::Point>& contour);
	bool FilterKailie(cv::Mat& index_pred, const std::vector<cv::Point>& contour);
	bool FilterKailie(cv::Mat& index_pred, const cv::Mat& origin_img, const std::vector<cv::Point>& contour);
    bool FilterXuhua(cv::Mat& index_pred, const std::vector<cv::Point>& contour);
	bool FilterBujun(cv::Mat& index_pred, const std::vector<cv::Point>& contour);

	std::vector<std::vector<cv::Point>> MergeContours(std::vector<std::vector<cv::Point>> contours);
	cv::Mat CropMinAreaRect(const cv::Mat& image, const std::vector<cv::Point>& contour, const std::vector<int>& padding = { 0, 0, 0, 0 });
	bool IsFiltered(cv::Mat img);
	cv::Vec3b JudgeColor(cv::Vec3b pixel_defect);

	bool DefectMerge(cv::Mat& index_pred, const int& distance, const int& class_id);
	std::vector<int> MatUnique(const cv::Mat& input, bool sort=false);
	std::map<int, bool> DefectOverlap(cv::Mat mask1, cv::Mat mask2);
	std::vector<int> VectorIntersection(std::vector<int> v1, std::vector<int> v2);

	QMap<QString, QVariant> ReadJson(const std::string &jsonFile);
	bool GetModified(std::string filepath, THRESHOLDTYPE type);

	static bool ContourAreaCompare(cv::InputArray contour1, cv::InputArray contour2);

	cv::Mat ImageStitchX(cv::Mat image_left, cv::Mat image_right);
	cv::Mat ImageStitchY(cv::Mat image_bottom, cv::Mat image_top);

    cv::Mat CreateLUT(int nclass = 8);
    double Round(float number);

    //计算上下、左右的重叠像素
    void CalcShiftX(float overlap_w);
    void CalcShiftY(float plc_distance_current);

    static void ImwriteThread(const std::string& img_path, const cv::Mat& mat);
    void WatchImwrite();

    void ImageMaskResize(cv::Mat& image, cv::Mat& mask, float ratio);

public:
	std::map<int, std::vector<std::vector<cv::Point>>> FilterFactory(cv::Mat& index_pred, cv::Mat& origin_img, THRESHOLDTYPE type);
	void DefectStitch(imginfo origin_imginfo, cv::Mat pred_mask, float plc_dist);
	void DatabaseItem(float meters, DEFECTTYPE defect_type, std::vector<cv::Point> contour, int64 savename, const cv::Mat &stitchImg = cv::Mat());
    void PLCEndAction(float plc_distance);
    void PushImwrite(std::string savepath, cv::Mat img);

private:
	omp_lock_t lock_;
	omp_lock_t lock_inner_;

	Snormalpixel normal_pixel_;

	Sthreshold thr_;

	long alarm_modified_time_;
	long database_modified_time_;
	long export_modified_time_;

    int nclass_ = 8;
	std::vector<int> topn_per_class_{ 0, 3, 4, 4, 4, 8, 4, 3 }; //每类取根据面积取前topN个缺陷
    cv::Mat lut_;
    std::string dstdir_ = "defectstitch/";

    std::deque<std::pair<std::string, cv::Mat>> saveimglist_;
    std::thread thread_;
    std::mutex mutex_;
    std::mutex imwrite_mutex_;
    bool watchflag_;
    std::condition_variable condvar_;

    std::vector< DatabaseMap> cur_img_defects_info_;

    std::map<int, std::string> defect_type_ =
    {
		//中文转换
       /* {1, "gansha"},
        {2, "kailie"},
        {3, "huangban"},
        {4, "xuhua"},
        {5, "wuwu"},
        {6, "bujun"},
        {7, "wanzhe"}*/
		{1, "干纱"},
		{2, "开裂"},
		{3, "黄斑"},
		{4, "虚化"},
		{5, "污物"},
		{6, "不均"},
		{7, "弯折"}
    };

    DatabaseMap export_param_ =
    {
        {"width", QString("-1")},
        {"height", QString("-1")},
        {"color", QString("-1")},
        {"area", QString("-1")},
        {"curvature", QString("-1")}
    }; //每个缺陷的参数信息，存入数据库，导出时需要用

    std::map<THRESHOLDTYPE, QString> threshold_type_ =
    {
        {THRESHOLDTYPE::ALARM, QString("alarm")},
        {THRESHOLDTYPE::DATABASE, QString("database")},
        {THRESHOLDTYPE::EXPORT, QString("export")}
    };

#ifdef DEBUG_TIME
    int64 t1, t2, t3, t4, t5, t6, t7, t8, t9, t10;
#endif // DEBUG_TIME

	/*******************缺陷拼接*******************/
    int overlap_w_ = 100 / reduce_ratio_w_; //横向重叠像素数
	int overlap_h_ = 100 / reduce_ratio_h_; //纵向重叠像素数

	//4个位置拼接时的重叠矩形
	cv::Rect left_overrect_ = cv::Rect(0, 0, overlap_w_, input_height_);
	cv::Rect right_overrect_ = cv::Rect(input_width_ - overlap_w_, 0, overlap_w_, input_height_);
    cv::Rect top_overrect_ = cv::Rect(0, 0, input_width_ * 2 - overlap_w_, overlap_h_);
	cv::Rect bottom_overrect_ = cv::Rect(0, input_height_ - overlap_h_, input_width_ * 2 - overlap_w_, overlap_h_);
	//cv::Rect leftbottom_overrect_ = cv::Rect(0, height_ - overlap_h_, overlap_w_, overlap_h_);
	//cv::Rect rightbottom_overrect_ = cv::Rect(width_ - overlap_w_, height_ - overlap_h_, overlap_w_, overlap_h_);
	//cv::Rect lefttop_overrect_ = cv::Rect(0, 0, overlap_w_, overlap_w_);
	//cv::Rect righttop_overrect_ = cv::Rect(width_ - overlap_w_, 0, overlap_w_, overlap_h_);

	//图像队列
	std::deque<cv::Mat> stitch_img_result_last_[1]; //上一次的拼接结果，长度固定为1
	std::deque<cv::Mat> stitch_mask_result_last_[1]; //上一次的拼接结果，长度固定为1
    std::map<int, cv::Mat> stitch_imgs_;
	std::map<int, cv::Mat> stitch_masks_;

    //计算上下、左右的重叠
    float plc_distance_last_ = -1;

	/*******************缺陷拼接*******************/


    //typedef bool (PostProcesser::*m_func)(cv::Mat& index_pred, const std::vector<cv::Point>& contour);
    //std::map<DEFECTTYPE, std::function<int(cv::Mat)> > filter_function_map_ =
    //{
    //    {DEFECTTYPE::GANSHA, &PostProcesser::FilterGansha},
    //    //{DEFECTTYPE::KAILIE, &PostProcesser::FilterKailie},
    //    //{DEFECTTYPE::HUANGBAN, &PostProcesser::FilterHuangban},
    //    {DEFECTTYPE::XUHUA, &PostProcesser::FilterXuhua},
    //    {DEFECTTYPE::WUWU, &PostProcesser::FilterWuwu},
    //    {DEFECTTYPE::BUJUN, &PostProcesser::FilterBujun},
    //    {DEFECTTYPE::WANZHE, &PostProcesser::FilterWanzhe}
    //};






};