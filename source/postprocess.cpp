#include "postprocess.h"
#include <direct.h>
#include <iomanip>
#include <io.h>
#include <chrono>
#include <math.h>

PostProcesser::PostProcesser()
{
	omp_init_lock(&lock_);
	omp_init_lock(&lock_inner_);

    lut_ = CreateLUT(8);

    if (access(dstdir_.c_str(), 0) == -1) 
    {
        mkdir(dstdir_.c_str());
    }

    watchflag_ = true;
    thread_ = std::thread(&PostProcesser::WatchImwrite, this);
}

PostProcesser::~PostProcesser()
{
	omp_destroy_lock(&lock_);
	omp_destroy_lock(&lock_inner_);

    while (true)
    {
        if (saveimglist_.empty())
        {
            watchflag_ = false;
            break;
        }
    }
    thread_.join();
}

template <typename TYPE>
TYPE StringToNum(const std::string& str)
{
    std::istringstream iss(str);
    TYPE num;
    iss >> num;
    return num;
}

bool PostProcesser::GetModified(std::string filepath, THRESHOLDTYPE type)
{
#ifdef DEBUG_TIME
    t1 = cv::getTickCount();
#endif // DEBUG_TIME

	struct stat buf;
	FILE *p_file;
	//char buff[1000];
	//_getcwd(buff, 1000);
	//filepath = buff + filepath;
	if (access(filepath.c_str(), 0) == -1)
	{
		std::cout << "ERROR: file not exist, " << filepath << std::endl;
	}
	p_file = fopen(filepath.c_str(), "r");
	int fd = fileno(p_file);
	fstat(fd, &buf);
	long time = buf.st_mtime;

	fclose(p_file);
	p_file = NULL;
	
	long * modified_time;
	switch (type)
	{
	case ALARM:
		modified_time = &alarm_modified_time_;
		break;
	case DATABASE:
		modified_time = &database_modified_time_;
		break;
	case EXPORT:
		modified_time = &export_modified_time_;
		break;
	}

	if (time == *modified_time)
	{
		std::cout << "INFO: threshold not modified, " << filepath << std::endl;
		return false;
	}
	else
	{
		std::cout << "INFO: threshold modified, " << filepath << std::endl;
		*modified_time = time;
		return true;
	}
#ifdef DEBUG_TIME
    std::cout << "TIME: GetModufied " << (double)(cv::getTickCount() - t1) / cv::getTickFrequency() << " second" << std::endl;
#endif // DEBUG_TIME

}

template<typename keytype, typename valuetype>
int MapSize(std::map<keytype, valuetype> map)
{
    int size = 0;
    std::map<keytype, valuetype>::iterator iter;
    for (iter = map.begin(); iter != map.end(); iter++)
    {
        size += iter->second.size();
    }

    return size;
}

//index_pred原地修改，origin_img不修改
std::map<int, std::vector<std::vector<cv::Point>>> PostProcesser::FilterFactory(cv::Mat& index_pred, cv::Mat& origin_img, THRESHOLDTYPE type)
{
	if (type != THRESHOLDTYPE::ALARM && type != THRESHOLDTYPE::DATABASE && type != THRESHOLDTYPE::EXPORT)
	{
		std::cout << "ERROR: unknown filter type: " << type << std::endl;
	}

    std::map<int, std::vector<std::vector<cv::Point>>> allclass_contours;
    std::vector<int> class_ids = MatUnique(index_pred, true);
    if (class_ids.size() == 0)
    {
        return allclass_contours;
    }

#ifdef DEBUG_TIME
    int64 t_filterfactory = cv::getTickCount();
    int64 t_getthr = cv::getTickCount();
#endif // DEBUG_TIME

    QMap<QString, QString> thr = dataSql::getInstance()->getThreshold(threshold_type_[type]); //宽度：mm，长度：cm
    thr_.gansha_width = StringToNum<float>(thr[QString("gansha_width")].toStdString()) / mm_per_pixel_w_;
    thr_.gansha_height = StringToNum<float>(thr[QString("gansha_height")].toStdString()) * 10 / mm_per_pixel_h_;
    thr_.kailie_width = StringToNum<float>(thr[QString("kailie_width")].toStdString()) / mm_per_pixel_w_;
    thr_.kailie_height = StringToNum<float>(thr[QString("kailie_height")].toStdString()) * 10 / mm_per_pixel_h_;
    thr_.huangban_color = StringToNum<float>(thr[QString("huangban_color")].toStdString());
    thr_.xuhua_width = StringToNum<float>(thr[QString("xuhua_width")].toStdString()) / mm_per_pixel_w_;
    thr_.wuwu_area = StringToNum<float>(thr[QString("wuwu_area")].toStdString()) / (mm_per_pixel_w_*mm_per_pixel_h_);
    thr_.bujun_width = StringToNum<float>(thr[QString("bujun_width")].toStdString()) / mm_per_pixel_w_;
    thr_.wanzhe_curvature = StringToNum<float>(thr[QString("wanzhe_curvature")].toStdString()) / mm_per_pixel_w_;

#ifdef DEBUG_TIME
    std::cout << "TIME: get threshold " << (double)(cv::getTickCount() - t_getthr) / cv::getTickFrequency() << std::endl;
#endif // DEBUG_TIME

    //如果是记录数据库，则清空旧的缺陷参数。记录和报警使用同样的流程，导出不走这个流程
    if (type == THRESHOLDTYPE::DATABASE)
    {
        cur_img_defects_info_.clear();
    }
    else if (type==THRESHOLDTYPE::ALARM)
    {
        
    }
    else
    {
        std::cout << "ERROR: unsupport threshold type, support database and alarm, current is " << type << endl;
    }

#ifdef OPENMP
#pragma omp parallel for schedule(dynamic)
#endif // OPENMP
	for (int i_class_ids = 0; i_class_ids < class_ids.size(); i_class_ids++)
	{
		int class_id = class_ids[i_class_ids];
		//只保留当前类别像素值
		cv::Mat mask_img_class;
		cv::threshold(index_pred, mask_img_class, class_id - 1, 0, cv::THRESH_TOZERO);
		cv::threshold(mask_img_class, mask_img_class, class_id, 0, cv::THRESH_TOZERO_INV);

		std::vector<std::vector<cv::Point> > all_contours;
		std::vector<cv::Vec4i> hierarchy;
		cv::findContours(mask_img_class, all_contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

		//对除污物外的所有缺陷先进行面积过滤
		std::vector<std::vector<cv::Point> > contours;
		if (class_id != DEFECTTYPE::WUWU)
		{
			for (int i = 0; i < all_contours.size(); i++)
			{
				if (cv::contourArea(all_contours[i]) > thr_.allclass_minarea)
				{
					contours.push_back(all_contours[i]);
				}
				else
				{
					cv::drawContours(index_pred, all_contours, i, cv::Scalar(0), -1);
				}
			}
			if (contours.size() == 0) //此类缺陷被过滤，直接下一类
			{
				continue;
			}
		}
		else
		{
			contours = all_contours;
		}

        //根据设置的阈值过滤
        if (class_id == DEFECTTYPE::GANSHA)
        {
            for (int i = 0; i < contours.size(); i++)
            {
                bool filtered = FilterGansha(index_pred, contours[i]);
                if (!filtered)
                {
    #ifdef OPENMP
                    omp_set_lock(&lock_);
    #endif // OPENMP
                    allclass_contours[class_id].push_back(contours[i]);
    #ifdef OPENMP
                    omp_unset_lock(&lock_);
    #endif // OPENMP
                }
            }
        }
        else if (class_id == DEFECTTYPE::KAILIE)
        {
            for (int i = 0; i < contours.size(); i++)
            {
                bool filtered = FilterKailie(index_pred, contours[i]);
                if (!filtered)
                {
#ifdef OPENMP
                    omp_set_lock(&lock_);
#endif // OPENMP
                    allclass_contours[class_id].push_back(contours[i]);
#ifdef OPENMP
                    omp_unset_lock(&lock_);
#endif // OPENMP
                }
            }
        }
		else if (class_id == DEFECTTYPE::HUANGBAN)
		{
			for (int i = 0; i < contours.size(); i++)
			{
				bool filtered = FilterHuangban(index_pred, origin_img, contours[i]);
				if (!filtered)
				{
#ifdef OPENMP
					omp_set_lock(&lock_);
#endif // OPENMP
					allclass_contours[class_id].push_back(contours[i]);
#ifdef OPENMP
					omp_unset_lock(&lock_);
#endif // OPENMP
				}
			}
		}
        else if (class_id == DEFECTTYPE::XUHUA)
        {
            for (int i = 0; i < contours.size(); i++)
            {
                bool filtered = FilterXuhua(index_pred, contours[i]);
                if (!filtered)
                {
#ifdef OPENMP
                    omp_set_lock(&lock_);
#endif // OPENMP
                    allclass_contours[class_id].push_back(contours[i]);
#ifdef OPENMP
                    omp_unset_lock(&lock_);
#endif // OPENMP
                }
            }
        }
        else if (class_id == DEFECTTYPE::WUWU)
        {
            for (int i = 0; i < contours.size(); i++)
            {
                bool filtered = FilterWuwu(index_pred, contours[i]);
                if (!filtered)
                {
#ifdef OPENMP
                    omp_set_lock(&lock_);
#endif // OPENMP
                    allclass_contours[class_id].push_back(contours[i]);
#ifdef OPENMP
                    omp_unset_lock(&lock_);
#endif // OPENMP
                }
            }
        }
		else if (class_id == DEFECTTYPE::BUJUN)
		{
			for (int i = 0; i < contours.size(); i++)
			{
				bool filtered = FilterBujun(index_pred, contours[i]);
				if (!filtered)
				{
#ifdef OPENMP
					omp_set_lock(&lock_);
#endif // OPENMP
					allclass_contours[class_id].push_back(contours[i]);
#ifdef OPENMP
					omp_unset_lock(&lock_);
#endif // OPENMP
				}
			}
		}
        else if (class_id == DEFECTTYPE::WANZHE)
        {
            for (int i = 0; i < contours.size(); i++)
            {
                bool filtered = FilterWanzhe(index_pred, contours[i]);
                if (!filtered)
                {
#ifdef OPENMP
                    omp_set_lock(&lock_);
#endif // OPENMP
                    allclass_contours[class_id].push_back(contours[i]);
#ifdef OPENMP
                    omp_unset_lock(&lock_);
#endif // OPENMP
                }
            }
        }
		else
		{
			std::cout << "ERROR: class_id=" << class_id << " " << DEFECTTYPE(class_id) << ", can not find a filter function." << std::endl;
		}
	}

	//每类取根据面积取前topN个缺陷
	//std::map<int, std::vector<std::vector<cv::Point>>>::iterator iter;
	//for (iter = allclass_contours.begin(); iter != allclass_contours.end(); iter++)
	//{
	//	int class_id = iter->first;
	//	std::vector<std::vector<cv::Point>> contours = iter->second;
	//	if (contours.size() > topn_per_class_[class_id])
	//	{
	//		//对contours面积降序排列
	//		std::sort(contours.begin(), contours.end(), ContourAreaCompare);

	//		std::vector<std::vector<cv::Point>>::const_iterator first = contours.begin();
	//		std::vector<std::vector<cv::Point>>::const_iterator last = contours.begin() + topn_per_class_[class_id];
	//		std::vector<std::vector<cv::Point>> tmpcontours(first, last);
	//		allclass_contours[class_id] = tmpcontours;
	//	}
	//}

#ifdef DEBUG_TIME
    std::cout << "TIME: FilterFactory in " << (double)(cv::getTickCount() - t_filterfactory) / cv::getTickFrequency() << std::endl;
#endif // DEBUG_TIME

    if (type == THRESHOLDTYPE::DATABASE && cur_img_defects_info_.size() != MapSize<int, std::vector<std::vector<cv::Point>>>(allclass_contours))
    {
        std::cout << "ERROR: cur_img_defects_info_.size() != allclass_contours.size()" << std::endl;
    }

	return allclass_contours;
}

bool PostProcesser::ContourAreaCompare(cv::InputArray contour1, cv::InputArray contour2)
{
	return cv::contourArea(contour1) > cv::contourArea(contour2);
}

bool PostProcesser::FilterXuhua(cv::Mat& index_pred, const std::vector<cv::Point>& contour)
{
	cv::Rect rect = cv::boundingRect(contour);
	if (rect.height < 400 || rect.width < thr_.xuhua_width)
	{
		std::vector<std::vector<cv::Point>> tmpcontours{ contour };
		cv::drawContours(index_pred, tmpcontours, -1, cv::Scalar(0), -1);

		return true;
	}
	else
	{
        export_param_["width"] = QString("%1").arg(rect.width);
        export_param_["height"] = QString("-1");
        export_param_["color"] = QString("-1");
        export_param_["area"] = QString("-1");
        export_param_["curvature"] = QString("-1");
        cur_img_defects_info_.push_back(export_param_);
		return false;
	}
}

bool PostProcesser::FilterWuwu(cv::Mat& index_pred, const std::vector<cv::Point>& contour)
{
    float area = cv::contourArea(contour);
    if (area < thr_.wuwu_area)
    {
        std::vector<std::vector<cv::Point>> tmpcontours{ contour };
        cv::drawContours(index_pred, tmpcontours, -1, cv::Scalar(0), -1);

        return true;
    }
    else
    {
        export_param_["width"] = QString("-1");
        export_param_["height"] = QString("-1");
        export_param_["color"] = QString("-1");
        export_param_["area"] = QString("%1").arg(area);
        export_param_["curvature"] = QString("-1");
        cur_img_defects_info_.push_back(export_param_);
        return false;
    }
}

bool PostProcesser::FilterBujun(cv::Mat& index_pred, const std::vector<cv::Point>& contour)
{
	cv::Rect rect = cv::boundingRect(contour);
	if (rect.height < 400 || rect.width < thr_.bujun_width)
	{
		std::vector<std::vector<cv::Point>> tmpcontours{ contour };
		cv::drawContours(index_pred, tmpcontours, -1, cv::Scalar(0), -1);

		return true;
	}
	else
	{
        export_param_["width"] = QString("%1").arg(rect.width);
        export_param_["height"] = QString("-1");
        export_param_["color"] = QString("-1");
        export_param_["area"] = QString("-1");
        export_param_["curvature"] = QString("-1");
        cur_img_defects_info_.push_back(export_param_);
		return false;
	}
}

bool PostProcesser::FilterHuangban(cv::Mat& index_pred, const cv::Mat& origin_img, const std::vector<cv::Point>& contour)
{
	//根据黄斑的最小外接矩形，裁剪原图区域并转正
	cv::Mat crop_img = CropMinAreaRect(origin_img, contour, std::vector<int>{1, 1, 1, 1});

	//计算黄斑严重程度
    cv::Mat defect_img(crop_img.rows, crop_img.cols, CV_8UC3, cv::Scalar(0));
    int rows = crop_img.rows;
    int cols = crop_img.cols;
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            cv::Vec3b pixel = crop_img.ptr<cv::Vec3b>(row)[col];
            defect_img.ptr<cv::Vec3b>(row)[col] = JudgeColor(pixel);
        }
    }

    std::vector<cv::Mat> defect_img_chns;
    cv::split(defect_img, defect_img_chns);
    //if (cv::countNonZero(defect_img_chns[0] == 0) && cv::countNonZero(defect_img_chns[1] == 0) && cv::countNonZero(defect_img_chns[2] == 0))
    //{
    //    std::vector<std::vector<cv::Point>> tmpcontours{ contour };
    //    cv::drawContours(index_pred, tmpcontours, -1, cv::Scalar(0), -1);

    //    return true;
    //}
    //else
    //{
    //    export_param_["color"] = 1; //TODO
    //    cur_img_defects_info_.push_back(export_param_);
    //    return false;
    //}

    export_param_["width"] = QString("-1");
    export_param_["height"] = QString("-1");
    export_param_["color"] = QString("%1").arg(1);
    export_param_["area"] = QString("-1");
    export_param_["curvature"] = QString("-1");
    cur_img_defects_info_.push_back(export_param_);
    return false;
}

bool PostProcesser::FilterGansha(cv::Mat& index_pred, const std::vector<cv::Point>& contour)
{
	cv::Rect rect = cv::boundingRect(contour);
    float ratio = rect.width / (rect.height / 1.0);
	int area = cv::contourArea(contour);
	if ((ratio > thr_.gansha_maxratio) || (area < thr_.gansha_minarea) || rect.width < thr_.gansha_width || rect.height < thr_.gansha_height)
	{
		std::vector<std::vector<cv::Point>> tmpcontours{ contour };
		cv::drawContours(index_pred, tmpcontours, -1, cv::Scalar(0), -1);

		return true;
	}
	else
	{
        export_param_["width"] = QString("%1").arg(rect.width);
        export_param_["height"] = QString("%1").arg(rect.height);
        export_param_["color"] = QString("-1");
        export_param_["area"] = QString("-1");
        export_param_["curvature"] = QString("-1");
        cur_img_defects_info_.push_back(export_param_);
		return false;
	}
}

bool PostProcesser::FilterWanzhe(cv::Mat& index_pred, const std::vector<cv::Point>& contour)
{
    cv::RotatedRect circle = cv::fitEllipse(contour);
    float curvature = max(circle.size.width, circle.size.height);

    if (curvature < thr_.wanzhe_curvature)
    {
        std::vector<std::vector<cv::Point>> tmpcontours{ contour };
        cv::drawContours(index_pred, tmpcontours, -1, cv::Scalar(0), -1);

        return true;
    }
    else
    {
        export_param_["width"] = QString("-1");
        export_param_["height"] = QString("-1");
        export_param_["color"] = QString("-1");
        export_param_["area"] = QString("-1");
        export_param_["curvature"] = QString("%1").arg(curvature);
        cur_img_defects_info_.push_back(export_param_);
        return false;
    }
}

/*
基于分割结果寻找的轮廓，如果是开裂，则在原图上外扩再裁剪。
外扩的大小取决于开裂的过滤条件，规定w<0.5mm && h<30mm时可被过滤，像素精度0.073mm/pixel，即w<7pixel && h<410pixel可被过滤
故左右各扩10pixel，上下各扩200pixel。上边不能扩大？？
*/
bool PostProcesser::FilterKailie(cv::Mat& index_pred, const cv::Mat& origin_img, const std::vector<cv::Point>& contour)
{
	int step = 100; //行方向上的遍历步长
	int maxdiff = 20; //开裂颜色的像素与正常像素的最小差距，也即所有开裂像素的最大差距
	std::vector<int> topk_pixels(0, 5);
	std::vector<int> padding{ 200, 200, 10, 10 };

	cv::Mat crop_img = CropMinAreaRect(origin_img, contour, padding);
	cv::Mat crop_img_gray;
	cv::cvtColor(crop_img, crop_img_gray, cv::COLOR_BGR2GRAY);

	int rows = crop_img_gray.rows;
	int cols = crop_img_gray.cols;
	cv::Mat crop_img_bin(1, cols, CV_8UC1);

	for (int row = 0; row < rows; row = row + step)
	{
		cv::Rect rect(0, row, cols, step);
		cv::Mat step_img = crop_img_gray(rect);
		step_img.reshape(1, -1);
		cv::sort(step_img, step_img, cv::SORT_EVERY_ROW + cv::SORT_DESCENDING);
		cv::Mat top_pixels = step_img(cv::Rect(0, 0, 1, 200));
#ifdef DEBUG_POSTPROCESS
		std::vector<int> tmp = step_img(cv::Rect(0, 0, 1, 200));
#endif // DEBUG_POSTPROCESS
		int max_pixel = cv::mean(top_pixels)[0];
		std::vector<int>::iterator min_topk = std::max_element(topk_pixels.begin(), topk_pixels.end());
		if ((*min_topk - max_pixel) > maxdiff)
		{
			max_pixel = 255;
		}
		if (*min_topk < max_pixel)
		{
			int min_loc = std::distance(std::begin(topk_pixels), min_topk);
			topk_pixels[min_loc] = max_pixel;
		}

		cv::threshold(step_img, step_img, max_pixel, 255, cv::THRESH_TOZERO);
		std::vector<cv::Mat> tmp1{ crop_img_bin, step_img };
		cv::vconcat(tmp1, crop_img_bin);
	}
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(crop_img_bin, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
	contours = MergeContours(contours);
	cv::RotatedRect minrect = cv::minAreaRect(contours);
	
#ifdef DEBUG_POSTPROCESS
	cv::imwrite("crop_img_bin.png", crop_img_bin); //分步二值化结果
	cv::drawContours(crop_img_gray, contours, -1, cv::Scalar(0), 3); //边缘检测结果

	std::vector<cv::Mat> tmp2;
	cv::boxPoints(minrect, tmp2[0]);
	cv::drawContours(crop_img_gray, tmp2, -1, cv::Scalar(30), 5); //最小外接矩形结果
	cv::imwrite("crop_img_gray_with_box.png", crop_img_gray);
#endif // DEBUG_POSTPROCESS

	cv::Size size = minrect.size;
	if (size.width < thr_.kailie_width && size.height < thr_.kailie_height) //合格
	{
		std::vector<std::vector<cv::Point>> tmpcontours{ contour };
		cv::drawContours(index_pred, tmpcontours, -1, cv::Scalar(0), -1);
	}

	return false;
}

bool PostProcesser::FilterKailie(cv::Mat& index_pred, const std::vector<cv::Point>& contour)
{
    cv::Rect rect = cv::boundingRect(contour);

    if (rect.width < thr_.kailie_width && rect.height < thr_.kailie_height)
    {
        std::vector<std::vector<cv::Point>> tmpcontours{ contour };
        cv::drawContours(index_pred, tmpcontours, -1, cv::Scalar(0), -1);

        return true;
    }
    else
    {
        export_param_["width"] = QString("%1").arg(rect.width);
        export_param_["height"] = QString("%1").arg(rect.height);
        export_param_["color"] = QString("-1");
        export_param_["area"] = QString("-1");
        export_param_["curvature"] = QString("-1");
        cur_img_defects_info_.push_back(export_param_);
        return false;
    }
}

std::vector<std::vector<cv::Point>> PostProcesser::MergeContours(std::vector<std::vector<cv::Point>> contours)
{
	std::vector<std::vector<cv::Point>> allcontours;
	std::vector<cv::Point> contourslist;

	for (int i = 0; i < contours.size(); i++)
	{
		std::vector<cv::Point> vec_i;
		vec_i = contours[i];
		if (vec_i.size() > 1) //过滤一些噪点
		{
			contourslist.insert(contourslist.end(), vec_i.begin(), vec_i.end());
		}
	}
	allcontours.emplace_back(contourslist);

	return allcontours;
}

/*
根据contour裁剪出最小外接矩形
padding:上下左右各扩大多少进行裁剪
*/
cv::Mat PostProcesser::CropMinAreaRect(const cv::Mat& image, const std::vector<cv::Point>& contour, const std::vector<int>& padding)
{
	//根据黄斑的最小外接矩形，裁剪原图区域并转正
	//cv::RotatedRect rotatedrect = cv::minAreaRect(contour);
	//cv::Mat matrix = cv::getRotationMatrix2D(rotatedrect.center, rotatedrect.angle, 1);
	//cv::Mat rotate_img;
	//cv::warpAffine(image, rotate_img, matrix, rotatedrect.size);
	//cv::Mat crop_img;
	//cv::getRectSubPix(rotate_img, cv::Size(rotatedrect.size.width+padding[2]+padding[3], rotatedrect.size.height + padding[0] + padding[1]), rotatedrect.center, crop_img);

    cv::Rect rect = cv::boundingRect(contour);
    cv::Mat crop_img = image.clone()(rect);

	return crop_img;
}

bool PostProcesser::IsFiltered(cv::Mat img)
{
	cv::Mat defect_img(img.rows, img.cols, CV_8UC3, cv::Scalar(0));
	int rows = img.rows;
	int cols = img.cols;
	for (int row = 0; row < rows; row++)
	{
		for (int col = 0; col < cols; col++)
		{
			cv::Vec3b pixel = img.ptr<cv::Vec3b>(row)[col];
			defect_img.ptr<cv::Vec3b>(row)[col] = JudgeColor(pixel);
		}
	}

	std::vector<cv::Mat> defect_img_chns;
	cv::split(defect_img, defect_img_chns);
	if (cv::countNonZero(defect_img_chns[0] == 0) && cv::countNonZero(defect_img_chns[1] == 0) && cv::countNonZero(defect_img_chns[2] == 0))
	{
		return true;
	}
	else
	{
		return false;
	}
}

cv::Vec3b PostProcesser::JudgeColor(cv::Vec3b pixel_defect)
{
	int b = pixel_defect[0];
	int g = pixel_defect[1];
	int r = pixel_defect[2];

	cv::Vec3b retval;
	if ((b > normal_pixel_.min_normal_pixel.blue && b < normal_pixel_.max_normal_pixel.blue) 
		&& (g > normal_pixel_.min_normal_pixel.green && g < normal_pixel_.max_normal_pixel.green) 
		&& (r > normal_pixel_.min_normal_pixel.red && r < normal_pixel_.max_normal_pixel.red) 
		&& (abs(r - b) < 20) 
		&& (abs(g - b) < 20))
	{
		//正常像素
		retval = (0, 0, 0);
	}
	else
	{
		retval = pixel_defect;
	}

	return retval;
}

void PostProcesser::SetNormalpixel(cv::Mat normal_img)
{
	cv::Vec3b min_bgr;
	cv::Vec3b max_bgr;

	std::vector<cv::Mat> bgr;
	cv::split(normal_img, bgr);

	double* b_min = (double*)normal_pixel_.min_normal_pixel.blue;
	double* b_max = (double*)normal_pixel_.max_normal_pixel.blue;
	cv::minMaxLoc(bgr[0], b_min, b_max, NULL, NULL);

	double* g_min = (double*)normal_pixel_.min_normal_pixel.green;
	double* g_max = (double*)normal_pixel_.max_normal_pixel.green;
	cv::minMaxLoc(bgr[1], g_min, g_max, NULL, NULL);

	double* r_min = (double*)normal_pixel_.min_normal_pixel.red;
	double* r_max = (double*)normal_pixel_.max_normal_pixel.red;
	cv::minMaxLoc(bgr[2], r_min, r_max, NULL, NULL);
}

bool PostProcesser::DefectMerge(cv::Mat& index_pred, const int& distance, const int& class_id)
{
	bool merged;

	cv::Mat index_pred_dst;
	cv::threshold(index_pred, index_pred_dst, class_id-1, 0, cv::THRESH_TOZERO);
	cv::threshold(index_pred_dst, index_pred_dst, class_id, 0, cv::THRESH_TOZERO_INV);

	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(index_pred_dst, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

	if (contours.size() == 1)
	{
		return false;
	}
	for (int i = 0; i < contours.size(); i++)
	{
		std::vector<cv::Point> contour_a = contours[i];
		while ((i + 1) < contours.size())
		{
			std::vector<cv::Point> contour_b = contours[i+1];
			for (int j = 0; j < contour_b.size(); j++)
			{
				int distance = cv::pointPolygonTest(contour_a, contour_b[j], true);
			}
		}
	}
}

void PostProcesser::ImageMaskResize(cv::Mat& image, cv::Mat& mask, float ratio)
{
#ifdef DEBUG_TIME
    int64 t_resize = cv::getTickCount();
#endif // DEBUG_TIME
    cv::resize(image, image, cv::Size(0, 0), ratio, ratio, cv::INTER_LINEAR);
    cv::resize(mask, mask, cv::Size(0, 0), ratio, ratio, cv::INTER_NEAREST);
#ifdef DEBUG_TIME
    std::cout << "TIME: resize img and mask " << mask.size() << " " << ratio << " " << (double)(cv::getTickCount() - t_resize) / cv::getTickFrequency() << std::endl;
#endif // DEBUG_TIME

}

void PostProcesser::ImwriteThread(const std::string& img_path, const cv::Mat& mat)
{
    cv::imwrite(img_path, mat.clone());
}

void PostProcesser::DefectStitch(imginfo origin_imginfo, cv::Mat pred_mask, float plc_dist)
{
#ifdef DEBUG_TIME
    int64 t_defectstitch = cv::getTickCount();
    t3 = cv::getTickCount();
#endif // DEBUG_TIME

    cv::Mat origin_img = origin_imginfo.getimg().clone();
#ifdef DEBUG_TIME
    std::cout << "TIME: img clone with size" << origin_img.size() << " " << (double)(cv::getTickCount() - t3) / cv::getTickFrequency() << " second" << std::endl;
#endif // DEBUG_TIME

	CalcShiftX(3.65);
	CalcShiftY(plc_dist);

    int station_id = origin_imginfo.getstationid();

    stitch_imgs_[station_id] = origin_img;
    stitch_masks_[station_id] = pred_mask;

    if (!(stitch_img_result_last_->size() == stitch_mask_result_last_->size() \
        && stitch_img_result_last_->size() <= 1 \
        && stitch_mask_result_last_->size() <= 1))
    {
        std::cout << "ERROR: error stitch_last size " << stitch_img_result_last_->size() << " " << stitch_mask_result_last_->size() << std::endl;
    }
    if (!(stitch_imgs_.size() == stitch_masks_.size() \
        && stitch_imgs_.size() <= 2 \
        && stitch_masks_.size() <= 2))
    {
        std::cout << "ERROR: error stitch size " << stitch_imgs_.size() << " " << stitch_masks_.size() << std::endl;
    }
    
    if (stitch_imgs_.size() < 2)
	{
		std::cout << "INFO: wait for next image to stitch." << std::endl;

		return;
	}

	if (stitch_masks_.size() == 2)
	{
		//拼接当前的左右两张图
		cv::Mat left_right_stitch_mask = ImageStitchX(stitch_masks_[0], stitch_masks_[1]);
        cv::Mat left_right_stitch_img = ImageStitchX(stitch_imgs_[0], stitch_imgs_[1]);
//#ifdef DEBUG_TIME
//        t4 = cv::getTickCount();
//#endif // DEBUG_TIME
//        int cur_nonzero = cv::countNonZero(left_right_stitch_mask);
//#ifdef DEBUG_TIME
//        std::cout << "TIME: countNonZero with size" << left_right_stitch_mask.size() << " " << (double)(cv::getTickCount() - t4) / cv::getTickFrequency() << " second" << std::endl;
//#endif // DEBUG_TIME

        auto tmp_contours = FilterFactory(left_right_stitch_mask.clone(), left_right_stitch_img.clone(), THRESHOLDTYPE::DATABASE);
        if (tmp_contours.size() == 0) //当前帧无缺陷，则清空旧状态，直接返回
        {
            PLCEndAction(plc_dist);

            return;
        }

		if (cv::countNonZero(left_right_stitch_mask(bottom_overrect_)) == 0) //说明当前图像与上一帧无重叠缺陷
		{
			if (stitch_img_result_last_->size() == 1) //若上一帧保存了图，则上一帧的处理可以结束，需要记录并存图并清空
			{
				std::map<int, std::vector<std::vector<cv::Point>>> allclass_contours = FilterFactory(stitch_mask_result_last_->at(0),\
																									 stitch_img_result_last_->at(0),\
																									 THRESHOLDTYPE::DATABASE);
                if (allclass_contours.size() != 0)
                {
                    //存图
                    if (access(dstdir_.c_str(), 0) == -1)
                    {
                        mkdir(dstdir_.c_str());
                    }
                    std::chrono::system_clock::duration d = std::chrono::system_clock::now().time_since_epoch();
                    std::chrono::microseconds microsecond = std::chrono::duration_cast<std::chrono::microseconds>(d);
                    auto savename = microsecond.count();

                    //QtConcurrent::run(ImwriteThread, dstdir_ + "\\" + std::to_string(savename) + ".jpg", stitch_img_result_last_->at(0));
                    ImageMaskResize(stitch_img_result_last_->at(0), stitch_mask_result_last_->at(0), 0.5);
                    PushImwrite(dstdir_ + "\\" + std::to_string(savename) + ".jpg", stitch_img_result_last_->at(0));
                    cv::LUT(stitch_mask_result_last_->at(0), lut_, stitch_mask_result_last_->at(0));
                    //QtConcurrent::run(ImwriteThread, dstdir_ + "\\" + std::to_string(savename) + ".png", stitch_mask_result_last_->at(0));
                    PushImwrite(dstdir_ + "\\" + std::to_string(savename) + ".png", stitch_mask_result_last_->at(0));
                    
                    //遍历缺陷，记录
                    std::map<int, std::vector<std::vector<cv::Point>>>::iterator iter;
                    for (iter = allclass_contours.begin(); iter != allclass_contours.end(); iter++)
                    {
                        std::vector<std::vector<cv::Point>> curclass_contours = iter->second;
                        for (int i_curclass = 0; i_curclass < curclass_contours.size(); i_curclass++)
                        {
                            //缺陷类别
                            DEFECTTYPE defect_type = DEFECTTYPE(iter->first);
                            std::vector<cv::Point> curclass_contour = curclass_contours[i_curclass];
                            //生成一条数据库记录
                            DatabaseItem(/*meters*/plc_dist, defect_type, curclass_contour, savename);
                        }
                    }
                }

                stitch_img_result_last_->clear();
                stitch_mask_result_last_->clear();
			}
			else
			{
				//上一帧未保存结果，则不操作
                if (!(stitch_img_result_last_->size() == 0 && stitch_mask_result_last_->size() == 0))
                {
                    std::cout << "ERROR: expect stitch_last size == 0, but get " << stitch_img_result_last_->size() << " " << stitch_mask_result_last_->size() << std::endl;
                }
			}

            //上一帧处理完成。处理当前帧
            //先判断当前图像与下一帧是否有重叠
            if (cv::countNonZero(left_right_stitch_mask(top_overrect_)) == 0) //若无重叠，则与上下均无重叠，记录并保存
            {
                std::map<int, std::vector<std::vector<cv::Point>>> allclass_contours = FilterFactory(left_right_stitch_mask, \
                                                                                                    left_right_stitch_img, \
                                                                                                    THRESHOLDTYPE::DATABASE);
                //std::map<int, std::vector<std::vector<cv::Point>>> allclass_contours = tmp_contours;
                if (allclass_contours.size() != 0)
                {
                    //存图
                    if (access(dstdir_.c_str(), 0) == -1)
                    {
                        mkdir(dstdir_.c_str());
                    }
                    std::chrono::system_clock::duration d = std::chrono::system_clock::now().time_since_epoch();
                    std::chrono::microseconds microsecond = std::chrono::duration_cast<std::chrono::microseconds>(d);
                    auto savename = microsecond.count();
                    ImageMaskResize(left_right_stitch_img, left_right_stitch_mask, 0.5);
#ifdef DEBUG_TIME
                    t8 = cv::getTickCount();
#endif // DEBUG_TIME
                    //QtConcurrent::run(ImwriteThread, dstdir_ + "\\" + std::to_string(savename) + ".jpg", left_right_stitch_img);
                    PushImwrite(dstdir_ + "\\" + std::to_string(savename) + ".jpg", left_right_stitch_img);
#ifdef DEBUG_TIME
                    std::cout << "TIME: imwrite left_right_stitch_img" << left_right_stitch_img.size() << " " << (double)(cv::getTickCount() - t8) / cv::getTickFrequency() << " second" << std::endl;
                    t8 = cv::getTickCount();
#endif // DEBUG_TIME
                    cv::LUT(left_right_stitch_mask, lut_, left_right_stitch_mask);
#ifdef DEBUG_TIME
                    std::cout << "TIME: LUT left_right_stitch_mask" << left_right_stitch_mask.size() << " " << (double)(cv::getTickCount() - t8) / cv::getTickFrequency() << " second" << std::endl;
                    t8 = cv::getTickCount();
#endif // DEBUG_TIME
                    //QtConcurrent::run(ImwriteThread, dstdir_ + "\\" + std::to_string(savename) + ".png", left_right_stitch_mask);
                    PushImwrite(dstdir_ + "\\" + std::to_string(savename) + ".png", left_right_stitch_mask);
#ifdef DEBUG_TIME
                    std::cout << "TIME: imwrite left_right_stitch_mask" << left_right_stitch_mask.size() << " " << (double)(cv::getTickCount() - t8) / cv::getTickFrequency() << " second" << std::endl;
#endif // DEBUG_TIME

                    std::map<int, std::vector<std::vector<cv::Point>>>::iterator iter;
                    for (iter = allclass_contours.begin(); iter != allclass_contours.end(); iter++)
                    {
                        std::vector<std::vector<cv::Point>> curclass_contours = iter->second;
                        for (int i_curclass = 0; i_curclass < curclass_contours.size(); i_curclass++)
                        {
                            //缺陷类别
                            DEFECTTYPE defect_type = DEFECTTYPE(iter->first);
                            std::vector<cv::Point> curclass_contour = curclass_contours[i_curclass];
                            //生成一条数据库记录
                            DatabaseItem(/*meters*/plc_dist, defect_type, curclass_contour, savename);
                        }
                    }
                }
            }
            else //若有重叠，则放入队列，等待下一帧拼接
            {
                stitch_img_result_last_->emplace_front(left_right_stitch_img);
                stitch_mask_result_last_->emplace_front(left_right_stitch_mask);
            }
		}
		else //说明当前图像与上一帧有重叠缺陷，需要拼接。此处假设只要重叠部分有一张图预测为缺陷，则两张图都有缺陷，即可以拼接
		{
			//先判断当前图像与下一帧是否有重叠
			if (cv::countNonZero(left_right_stitch_mask(top_overrect_)) == 0) //若无重叠，则直接与上一帧拼接，记录并保存，并清空队列
			{
                cv::Mat bottom_top_stitch_img;
                cv::Mat bottom_top_stitch_mask;
                if (stitch_img_result_last_->empty())
                {
                    bottom_top_stitch_img = left_right_stitch_img;
                    bottom_top_stitch_mask = left_right_stitch_mask;
                }
                else
                {
                    bottom_top_stitch_img = ImageStitchY(stitch_img_result_last_->at(0), left_right_stitch_img);
                    bottom_top_stitch_mask = ImageStitchY(stitch_mask_result_last_->at(0), left_right_stitch_mask);
                }

                std::map<int, std::vector<std::vector<cv::Point>>> allclass_contours = FilterFactory(bottom_top_stitch_mask, \
                                                                                                        bottom_top_stitch_img, \
                                                                                                        THRESHOLDTYPE::DATABASE);
                if (allclass_contours.size() != 0)
                {
                    //存图
                    if (access(dstdir_.c_str(), 0) == -1)
                    {
                        mkdir(dstdir_.c_str());
                    }
                    std::chrono::system_clock::duration d = std::chrono::system_clock::now().time_since_epoch();
                    std::chrono::microseconds microsecond = std::chrono::duration_cast<std::chrono::microseconds>(d);
                    auto savename = microsecond.count();
                    ImageMaskResize(bottom_top_stitch_img, bottom_top_stitch_mask, 0.5);
                    //QtConcurrent::run(ImwriteThread, dstdir_ + "\\" + std::to_string(savename) + ".jpg", bottom_top_stitch_img);
                    PushImwrite(dstdir_ + "\\" + std::to_string(savename) + ".jpg", bottom_top_stitch_img);
#ifdef DEBUG_TIME
                    t9 = cv::getTickCount();
#endif // DEBUG_TIME
                    cv::LUT(bottom_top_stitch_mask, lut_, bottom_top_stitch_mask);
#ifdef DEBUG_TIME
                    std::cout << "TIME: LUT with size" << bottom_top_stitch_mask.size() << " " << (double)(cv::getTickCount() - t9) / cv::getTickFrequency() << " second" << std::endl;
#endif // DEBUG_TIME
                    //QtConcurrent::run(ImwriteThread, dstdir_ + "\\" + std::to_string(savename) + ".png", bottom_top_stitch_mask);
                    PushImwrite(dstdir_ + "\\" + std::to_string(savename) + ".png", bottom_top_stitch_mask);

                    std::map<int, std::vector<std::vector<cv::Point>>>::iterator iter;
                    for (iter = allclass_contours.begin(); iter != allclass_contours.end(); iter++)
                    {
                        std::vector<std::vector<cv::Point>> curclass_contours = iter->second;
                        for (int i_curclass = 0; i_curclass < curclass_contours.size(); i_curclass++)
                        {
                            //缺陷类别
                            DEFECTTYPE defect_type = DEFECTTYPE(iter->first);
                            std::vector<cv::Point> curclass_contour = curclass_contours[i_curclass];
                            //生成一条数据库记录
                            DatabaseItem(/*meters*/plc_dist, defect_type, curclass_contour, savename);
                        }
                    }
                }

				stitch_img_result_last_->clear();
				stitch_mask_result_last_->clear();
			}
			else //若有重叠，则拼接，且放入队列，等待下一帧
			{
                if (stitch_img_result_last_->empty())
                {
                    stitch_img_result_last_->emplace_front(left_right_stitch_img);
                    stitch_mask_result_last_->emplace_front(left_right_stitch_mask);
                }
                else
                {
                    cv::Mat tmp;
                    tmp = ImageStitchY(stitch_img_result_last_->at(0), left_right_stitch_img);
                    stitch_img_result_last_->clear();
                    stitch_img_result_last_->emplace_front(tmp);

                    tmp = ImageStitchY(stitch_mask_result_last_->at(0), left_right_stitch_mask);
                    stitch_mask_result_last_->clear();
                    stitch_mask_result_last_->emplace_front(tmp);
                }
			}
		}

		stitch_imgs_.clear();
		stitch_masks_.clear();
	}
#ifdef DEBUG_TIME
    std::cout << "TIME: defectstitch in " << (double)(cv::getTickCount() - t_defectstitch) / cv::getTickFrequency() << " second" << std::endl;
#endif // DEBUG_TIME
}

cv::Mat PostProcesser::ImageStitchX(cv::Mat image_left, cv::Mat image_right)
{
	if (!(image_left.rows == image_right.rows && image_left.channels() == image_right.channels()))
	{
		std::cout << "ERROR: ImageStitchX needs two images with the same rows and chns, please check. Do resize now. Error size:" << image_left.size() << image_right.size() << std::endl;
        cv::resize(image_right, image_right, cv::Size(image_right.size().width, image_left.size().height), cv::INTER_NEAREST);
    }

#ifdef DEBUG_TIME
    t5 = cv::getTickCount();
#endif // DEBUG_TIME

	cv::Mat left_right_stitch(image_left.rows, image_left.cols + image_right.cols - overlap_w_, image_left.type());
	image_left.copyTo(left_right_stitch(cv::Rect(0, 0, image_left.cols, image_left.rows)));
	image_right.copyTo(left_right_stitch(cv::Rect(cv::Point(left_right_stitch.cols-image_right.cols, 0), cv::Point(left_right_stitch.cols, left_right_stitch.rows))));
#ifdef DEBUG_TIME
    if (image_left.channels() == 1)
    {
        std::cout << "TIME: ImageStitchX mask" << image_left.size() << image_right.size() << " " << (double)(cv::getTickCount() - t5) / cv::getTickFrequency() << " second" << std::endl;
    }
    else
    {
        std::cout << "TIME: ImageStitchX image" << image_left.size() << image_right.size() << " " << (double)(cv::getTickCount() - t5) / cv::getTickFrequency() << " second" << std::endl;
    }
#endif // DEBUG_TIME

	return left_right_stitch;
}

cv::Mat PostProcesser::ImageStitchY(cv::Mat image_bottom, cv::Mat image_top)
{
	if (!(image_bottom.cols == image_top.cols && image_bottom.channels() == image_top.channels()))
	{
		std::cout << "ERROR: ImageStitchY needs two images with the same cols and chns, please check. Do resize now. Error size:" << image_bottom.size() << image_top.size() << std::endl;
        cv::resize(image_top, image_top, cv::Size(image_bottom.size().width, image_top.size().height), cv::INTER_NEAREST);
    }

#ifdef DEBUG_TIME
    int64 t_stitchy = cv::getTickCount();
#endif // DEBUG_TIME
	cv::Mat bottom_top_stitch(image_bottom.rows + image_top.rows - overlap_h_, image_bottom.cols, image_bottom.type());
	image_bottom.copyTo(bottom_top_stitch(cv::Rect(0, bottom_top_stitch.rows - image_bottom.rows, image_bottom.cols, image_bottom.rows)));
	image_top.copyTo(bottom_top_stitch(cv::Rect(0, 0, image_top.cols, image_top.rows)));
#ifdef DEBUG_TIME
    if (image_bottom.channels() == 1)
    {
        std::cout << "TIME: ImageStitchY mask" << image_bottom.size() << image_top.size() << " " << (double)(cv::getTickCount() - t_stitchy) / cv::getTickFrequency() << " second" << std::endl;
    }
    else
    {
        std::cout << "TIME: ImageStitchY image" << image_bottom.size() << image_top.size() << " " << (double)(cv::getTickCount() - t_stitchy) / cv::getTickFrequency() << " second" << std::endl;
    }
#endif // DEBUG_TIME

	return bottom_top_stitch;
}

//判断两个mask中是否有同一个缺陷，进而判断是否需要拼接
std::map<int, bool> PostProcesser::DefectOverlap(cv::Mat mask1, cv::Mat mask2)
{
	std::map<int, bool> result;

	std::vector<int> pixels_1 = MatUnique(mask1);
	std::vector<int> pixels_2 = MatUnique(mask2);
	std::vector<int> pixels_inter = VectorIntersection(pixels_1, pixels_2);
	if (pixels_inter.empty())
	{
		return result;
	}
	else
	{
		for (int i = 0; i < pixels_inter.size(); i++)
		{
			int class_id = pixels_inter[i];
			//两个mask过滤，只保留当前class_id像素值
			cv::Mat mask1_class;
			cv::threshold(mask1, mask1_class, class_id - 1, 0, cv::THRESH_TOZERO);
			cv::threshold(mask1_class, mask1_class, class_id, 0, cv::THRESH_TOZERO_INV);
			cv::Mat mask2_class;
			cv::threshold(mask1, mask2_class, class_id - 1, 0, cv::THRESH_TOZERO);
			cv::threshold(mask2_class, mask2_class, class_id, 0, cv::THRESH_TOZERO_INV);
			//过滤结果做位与，有非零值则有重叠
			cv::Mat mask_1and2;
			cv::bitwise_and(mask1_class, mask2_class, mask_1and2);
			if (cv::countNonZero(mask_1and2) != 0)
			{
				result[class_id] = true;
			}
			else
			{
				result[class_id] = false;
			}
		}

		return result;
	}

}

std::vector<int> PostProcesser::MatUnique(const cv::Mat& input, bool sort)
{
#ifdef DEBUG_TIME
    t2 = cv::getTickCount();
#endif // DEBUG_TIME

	std::vector<int> result;
	if (input.channels() > 1 || input.type() != CV_8UC1)
	{
		std::cout << "ERROR: PostProcesser::MatUnique Only works with CV_8UC1 Mat, but get " << input.type() << std::endl;
		return result;
	}

	cv::Mat tmpinput;
	cv::resize(input, tmpinput, cv::Size(0, 0), 0.25, 0.25, 0);

	for (int row = 0; row < tmpinput.rows; row++)
	{
		const uchar* row_ptr = tmpinput.ptr<uchar>(row);
		for (int col = 0; col < tmpinput.cols; col++)
		{
			int pixel = row_ptr[col];
			if (pixel != 0)
			{
				if (std::find(result.begin(), result.end(), pixel) == result.end())
				{
					result.push_back(pixel);
				}
			}
		}
	}

	if (sort)
		std::sort(result.begin(), result.end()); //升序
#ifdef DEBUG_TIME
    std::cout << "TIME: MatUnique with size" << tmpinput.size() << " " << (double)(cv::getTickCount() - t2) / cv::getTickFrequency() << " second" << std::endl;
#endif // DEBUG_TIME

	return result;
}

//两个vector求交集
std::vector<int> PostProcesser::VectorIntersection(std::vector<int> v1, std::vector<int> v2)
{
	std::vector<int> v;
	std::sort(v1.begin(), v1.end());
	std::sort(v2.begin(), v2.end());
	std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), back_inserter(v));//求交集 

	return v;
}

QMap<QString, QVariant> PostProcesser::ReadJson(const std::string &jsonFile)
{
	QFile loadFile(QString::fromStdString(jsonFile));
	if (!loadFile.open(QIODevice::ReadOnly)) {
		//qDebug() << "Json File open failed!";
		std::cout << "ERROR: Json file open failed, " << jsonFile << std::endl;
	}
	else {
		//qDebug() << "File open successfully!";
	}
	QMap<QString, QVariant> map;
	QByteArray allData = loadFile.readAll();
	loadFile.close();
	QJsonParseError json_error;
	QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &json_error));

	if (json_error.error != QJsonParseError::NoError)
	{
		//qDebug() << "json error!";
        std::cout << "ERROR: Json error, " << jsonFile << std::endl;
	}
	QJsonObject rootObj = jsonDoc.object();

	QStringList keys = rootObj.keys();

	for (int i = 0; i < keys.size(); i++)
	{
		//qDebug() << "key" << i << " is:" << keys.at(i);

		QString key = keys.at(i);
		if (rootObj.contains(key))
		{
			QJsonObject subObj = rootObj.value(key).toObject();
			if (subObj.isEmpty()) {
				map.insert(key, rootObj.value(key).toVariant());
			}
			QStringList keysChild = subObj.keys();
			for (int j = 0; j < keysChild.size(); j++) {
				//qDebug() << "keysChild" << keysChild.at(j) << subObj.value(keysChild.at(j)).toString() << "LLLLLLLLL";
				//map.insert(keys.at(i), rootObj.value(keys.at(i)).toVariant());
				map.insert(keysChild.at(j), subObj.value(keysChild.at(j)).toVariant());
			}
		}
	}

	return map;
}

template <typename MATTYPE> //uchar, float
std::vector<std::vector<int>> np_where(const cv::Mat& img_gray, const MATTYPE& value)
{
	//返回：行索引，列索引
	std::vector<std::vector<int>> res;
	std::vector<int> res_row;
	std::vector<int> res_col;
	MATTYPE* pixel;
	int rows = img_gray.rows;
	int cols = img_gray.cols;
	for (int row = 0; row < rows; row++)
	{
		pixel = img_gray.data + row * img_gray.step;
		for (int col = 0; col < cols; col++)
		{
			if (pixel[0] == value)
			{
				res_row.push_back(row);
				res_col.push_back(col);
			}
			pixel += 1;
		}
	}
	res.push_back(res_row);
	res.push_back(res_col);

	return res;
}

cv::Mat get_edge(cv::Mat img)
{
	cv::Mat canvas;
	cv::GaussianBlur(img, canvas, cv::Size(3, 3), 0);

	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(21, 21));
	cv::dilate(canvas, canvas, kernel);
	cv::erode(canvas, canvas, kernel);
	cv::Mat edge;
	cv::Canny(canvas, edge, 20, 30);

	return edge;
}


//img: 单通道图像或bgr通道
int get_zero_point(const cv::Mat &stitchImg)
{
	if (stitchImg.rows == 0)
	{
		return 0;
	}
	cv::Mat img = stitchImg.clone();
	if (img.channels() > 2)
	{
		cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
	}
	int h = img.rows;
	int w = img.cols;
	int x_min = w * 0.06;
	int x_max = x_min * 4;
	int y_min = floor(h / 6);
	int y_max = y_min * 2;
	
	cv::Rect roi(cv::Point(x_min, y_min), cv::Point(x_max, y_max));
	cv::Mat canvas = img(roi);
	cv::Mat edge = get_edge(canvas);
	std::vector<std::vector<int>> res = np_where<uchar>(edge, 255);
	std::vector<int> i = res[0];
	float sum_i = std::accumulate(std::begin(i), std::end(i), 0.0);
	float idx_x = sum_i / i.size();
	int zeroPoint = idx_x + x_min;

	return zeroPoint;
}


void PostProcesser::DatabaseItem(float meters, DEFECTTYPE defect_type, std::vector<cv::Point> contour, int64 savename, const cv::Mat &stitchImg)
{
#ifdef DEBUG_TIME
    t7 = cv::getTickCount();
#endif // DEBUG_TIME

	DatabaseMap databasemap;
    
    //缺陷类型

    //缺陷位置
	int zeroPoint = get_zero_point(stitchImg); //寻找零点
    cv::Rect defect_rect = cv::boundingRect(contour);
	int position_left = (defect_rect.x - zeroPoint) * mm_per_pixel_w_;
    int position_right = (defect_rect.x - zeroPoint + defect_rect.width) * mm_per_pixel_w_;
    QString position = QString::number(position_left) + "-" + QString::number(position_right);

    //缺陷长度
    int length = defect_rect.height * mm_per_pixel_h_;
    if (length == 0)
    {
        length = 1;
    }

    //文件名 savename

	//databasemap["dataID"] = QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz"); //ID
	databasemap["meters"] = QString::number(int(meters)); //米数
	databasemap["thickness"] = "0.29"; //厚度
	databasemap["defect"] = QString::fromStdString(defect_type_[defect_type]); //缺陷
	databasemap["position"] = position; //位置
	databasemap["length"] = QString::number(int(length)); //长度
	databasemap["description"] = QString::fromStdString(std::string()); //描述
    databasemap["remarks"] = QString::fromStdString(std::string()); //备注
    databasemap["production_time"] = QString::fromStdString(std::to_string(-1)); //生产时间
    databasemap["operator"] = QString::fromStdString(std::to_string(-1)); //操作员
    databasemap["record_ID"] = QString::fromStdString(std::to_string(-1));
	databasemap["savename"] = QString::fromStdString(std::to_string(savename)); //图片文件名
    
    //数据库导出时需要的字段
    databasemap["width"] = cur_img_defects_info_[0]["width"];
    databasemap["height"] = cur_img_defects_info_[0]["height"];
    databasemap["color"] = cur_img_defects_info_[0]["color"];
    databasemap["area"] = cur_img_defects_info_[0]["area"];
    databasemap["curvature"] = cur_img_defects_info_[0]["curvature"];
    std::vector< std::map<QString, QString>>::iterator it = cur_img_defects_info_.begin();
    cur_img_defects_info_.erase(it); //记录后删除第0个元素

    databasemap["add_type"] = QString("0");

    dataSql::getInstance()->setData(databasemap);
#ifdef DEBUG_TIME
    std::cout << "TIME: DatabaseItem " << (double)(cv::getTickCount() - t7) / cv::getTickFrequency() << " second" << std::endl;
#endif // DEBUG_TIME

}

double PostProcesser::Round(float number)
{
    char str[20];
    sprintf(str, "%.3f", number);
    float c = (float)(atof(str));
    return c;
}

cv::Mat PostProcesser::CreateLUT(int nclass)
{
    if (nclass != 8)
    {
        std::cout << "ERROR: unimplement lut classes, save result maybe wrong.";
    }

    //查找表，数组的下标对应图片里面的灰度值
    cv::Mat lut(1, 256, CV_8UC1, cv::Scalar(0));

    for (int i = 0; i < 256; i++)
    {
        if (i < nclass)
        {
            lut.ptr<uchar>(0)[i] = i * 30; //0-7映射到0-210
        }
        else
        {
            break;
        }
    }

    return lut;
}

void PostProcesser::PLCEndAction(float plc_distance)
{
    //处理postprocesser中队列的所有剩余数据，存图，存数据库
    //清空postprocesser的队列及状态
    //恢复到类初始化的状态

#ifdef DEBUG_TIME
    t6 = cv::getTickCount();
#endif // DEBUG_TIME

    //处理上一帧
    if (!(stitch_img_result_last_->size() == stitch_mask_result_last_->size() \
        && stitch_img_result_last_->size() <= 1 \
        && stitch_mask_result_last_->size() <= 1))
    {
        std::cout << "ERROR: error stitch last size in plcendaction " << stitch_img_result_last_->size() << " " << stitch_mask_result_last_->size() << std::endl;
    }

    if (!stitch_img_result_last_->empty())
    {
        std::map<int, std::vector<std::vector<cv::Point>>> allclass_contours = FilterFactory(stitch_mask_result_last_->at(0), \
                                                                                                stitch_img_result_last_->at(0), \
                                                                                                THRESHOLDTYPE::DATABASE);
        if (allclass_contours.size() != 0)
        {
            //存图
            if (access(dstdir_.c_str(), 0) == -1)
            {
                mkdir(dstdir_.c_str());
            }
            std::chrono::system_clock::duration d = std::chrono::system_clock::now().time_since_epoch();
            std::chrono::microseconds microsecond = std::chrono::duration_cast<std::chrono::microseconds>(d);
            auto savename = microsecond.count();
            ImageMaskResize(stitch_img_result_last_->at(0), stitch_mask_result_last_->at(0), 0.5);
            //QtConcurrent::run(ImwriteThread, dstdir_ + "\\" + std::to_string(savename) + ".jpg", stitch_img_result_last_->at(0));
            PushImwrite(dstdir_ + "\\" + std::to_string(savename) + ".jpg", stitch_img_result_last_->at(0));
            cv::LUT(stitch_mask_result_last_->at(0), lut_, stitch_mask_result_last_->at(0));
            //QtConcurrent::run(ImwriteThread, dstdir_ + "\\" + std::to_string(savename) + ".png", stitch_mask_result_last_->at(0));
            PushImwrite(dstdir_ + "\\" + std::to_string(savename) + ".png", stitch_mask_result_last_->at(0));

            std::map<int, std::vector<std::vector<cv::Point>>>::iterator iter;
            for (iter = allclass_contours.begin(); iter != allclass_contours.end(); iter++)
            {
                std::vector<std::vector<cv::Point>> curclass_contours = iter->second;
                for (int i_curclass = 0; i_curclass < curclass_contours.size(); i_curclass++)
                {
                    //缺陷类别
                    DEFECTTYPE defect_type = DEFECTTYPE(iter->first);
                    std::vector<cv::Point> curclass_contour = curclass_contours[i_curclass];
                    //生成一条数据库记录
                    DatabaseItem(/*meters*/plc_distance - 0.5, defect_type, curclass_contour, savename);
                }
            }
        }

        stitch_img_result_last_->clear();
        stitch_mask_result_last_->clear();
    }

    //处理当前帧
    if (!(stitch_imgs_.size() == stitch_masks_.size() \
        && stitch_imgs_.size() <= 2 \
        && stitch_masks_.size() <= 2))
    {
        std::cout << "ERROR: error stitch last size in plcendaction " << stitch_img_result_last_->size() << " " << stitch_mask_result_last_->size() << std::endl;
    }

    if (stitch_imgs_.size() == 0)
    {
        //pass
    }
    else if ((stitch_imgs_.size() == 1) || (stitch_imgs_.size() == 2 && stitch_imgs_[0].cols==0) || (stitch_imgs_.size() == 2 && stitch_imgs_[1].cols == 0))
    {
        cv::Mat stitch_img = stitch_imgs_[0].cols != 0 ? stitch_imgs_[0] : stitch_imgs_[1];
        cv::Mat stitch_mask = stitch_masks_[0].cols != 0 ? stitch_masks_[0] : stitch_masks_[1];
        std::map<int, std::vector<std::vector<cv::Point>>> allclass_contours = FilterFactory(stitch_mask, stitch_img, THRESHOLDTYPE::DATABASE);
        if (allclass_contours.size() != 0)
        {
            if (access(dstdir_.c_str(), 0) == -1)
            {
                mkdir(dstdir_.c_str());
            }
            std::chrono::system_clock::duration d = std::chrono::system_clock::now().time_since_epoch();
            std::chrono::microseconds microsecond = std::chrono::duration_cast<std::chrono::microseconds>(d);
            auto savename = microsecond.count();
            ImageMaskResize(stitch_img, stitch_mask, 0.5);
            //QtConcurrent::run(ImwriteThread, dstdir_ + "\\" + std::to_string(savename) + ".jpg", stitch_img);
            PushImwrite(dstdir_ + "\\" + std::to_string(savename) + ".jpg", stitch_img);
            cv::LUT(stitch_mask, lut_, stitch_mask);
            //QtConcurrent::run(ImwriteThread, dstdir_ + "\\" + std::to_string(savename) + ".png", stitch_mask);
            PushImwrite(dstdir_ + "\\" + std::to_string(savename) + ".png", stitch_mask);
            
            std::map<int, std::vector<std::vector<cv::Point>>>::iterator iter;
            for (iter = allclass_contours.begin(); iter != allclass_contours.end(); iter++)
            {
                std::vector<std::vector<cv::Point>> curclass_contours = iter->second;
                for (int i_curclass = 0; i_curclass < curclass_contours.size(); i_curclass++)
                {
                    //缺陷类别
                    DEFECTTYPE defect_type = DEFECTTYPE(iter->first);
                    std::vector<cv::Point> curclass_contour = curclass_contours[i_curclass];
                    //生成一条数据库记录
                    DatabaseItem(/*meters*/plc_distance, defect_type, curclass_contour, savename);
                }
            }
        }

        stitch_imgs_.clear();
        stitch_masks_.clear();
    }
    else if (stitch_imgs_.size() == 2)
    {
        cv::Mat tmpimg = ImageStitchX(stitch_imgs_[0], stitch_imgs_[0]);
        cv::Mat tmpmask = ImageStitchX(stitch_masks_[0], stitch_masks_[1]);
        std::map<int, std::vector<std::vector<cv::Point>>> allclass_contours = FilterFactory(tmpmask, tmpimg, THRESHOLDTYPE::DATABASE);
        if (allclass_contours.size() != 0)
        {
            if (access(dstdir_.c_str(), 0) == -1)
            {
                mkdir(dstdir_.c_str());
            }
            std::chrono::system_clock::duration d = std::chrono::system_clock::now().time_since_epoch();
            std::chrono::microseconds microsecond = std::chrono::duration_cast<std::chrono::microseconds>(d);
            auto savename = microsecond.count();
            ImageMaskResize(tmpimg, tmpmask, 0.5);
            //QtConcurrent::run(ImwriteThread, dstdir_ + "\\" + std::to_string(savename) + ".jpg", tmpimg);
            PushImwrite(dstdir_ + "\\" + std::to_string(savename) + ".jpg", tmpimg);
            cv::LUT(tmpmask, lut_, tmpmask);
            //QtConcurrent::run(ImwriteThread, dstdir_ + "\\" + std::to_string(savename) + ".png", tmpmask);
            PushImwrite(dstdir_ + "\\" + std::to_string(savename) + ".png", tmpmask);

            std::map<int, std::vector<std::vector<cv::Point>>>::iterator iter;
            for (iter = allclass_contours.begin(); iter != allclass_contours.end(); iter++)
            {
                std::vector<std::vector<cv::Point>> curclass_contours = iter->second;
                for (int i_curclass = 0; i_curclass < curclass_contours.size(); i_curclass++)
                {
                    //缺陷类别
                    DEFECTTYPE defect_type = DEFECTTYPE(iter->first);
                    std::vector<cv::Point> curclass_contour = curclass_contours[i_curclass];
                    //生成一条数据库记录
                    DatabaseItem(/*meters*/plc_distance, defect_type, curclass_contour, savename);
                }
            }
        }

        stitch_imgs_.clear();
        stitch_masks_.clear();
    }
    else
    {
        std::cout << "ERROR: suppose stitch_imgs_.size() <= 2, but get " << stitch_imgs_.size() << std::endl;

        stitch_imgs_.clear();
        stitch_masks_.clear();
    }

#ifdef DEBUG_TIME
    std::cout << "TIME: PLCEndAction " << (double)(cv::getTickCount() - t6) / cv::getTickFrequency() << " second" << std::endl;
#endif // DEBUG_TIME
}

void PostProcesser::CalcShiftX(float overlap_w/*cm*/)
{
	if (overlap_w < 0)
	{
		std::cout << "WARNING: support overlap_w>0, but get overlap_w=" << overlap_w << std::endl;
	}

	overlap_w = (overlap_w * 10) / mm_per_pixel_w_;
	overlap_w_ = overlap_w / reduce_ratio_w_;

	left_overrect_ = cv::Rect(0, 0, overlap_w_, input_height_);
	right_overrect_ = cv::Rect(input_width_ - overlap_w_, 0, overlap_w_, input_height_);
}

void PostProcesser::CalcShiftY(float plc_distance_current)
{
    if (plc_distance_last_ == -1)
    {
        plc_distance_last_ = plc_distance_current;
        return;
    }

    float dist_once = plc_distance_current - plc_distance_last_; //上下相邻两帧的距离，单位：米
    int dist_once_pixel = (dist_once * 1000) / mm_per_pixel_h_; //单位：像素
    int overlap_h = camera_height_ - dist_once_pixel; //上下相邻两帧的重叠像素数

    if (overlap_h < 0)
    {
        std::cout << "WARNING: plc speed is too fast to cover all material. Check plc speed and process speed." << std::endl;
    }

    overlap_h_ = overlap_h / reduce_ratio_h_; //纵向重叠像素数进行resize
	overlap_h_ = 500;
    top_overrect_ = cv::Rect(0, 0, input_width_ * 2 - overlap_w_, overlap_h_);
    bottom_overrect_ = cv::Rect(0, input_height_ - overlap_h_, input_width_ * 2 - overlap_w_, overlap_h_);

    plc_distance_last_ = plc_distance_current;
}

void PostProcesser::WatchImwrite()
{
    std::unique_lock<std::mutex> imwrite_lock(mutex_);
    while (watchflag_)
    {
        std::pair<std::string, cv::Mat> tmpimg;
        condvar_.wait_for(imwrite_lock, std::chrono::milliseconds(1));
        imwrite_mutex_.lock();
        if (!saveimglist_.empty())
        {
            tmpimg = saveimglist_.front();
            saveimglist_.pop_front();
        }
        imwrite_mutex_.unlock();

        if (tmpimg.first != "")
        {
#ifdef DEBUG_TIME
            int64 t_imwrite = cv::getTickCount();
#endif // DEBUG_TIME
            cv::imwrite(tmpimg.first, tmpimg.second);
#ifdef DEBUG_TIME
            std::cout << "TIME: imwrite with size " << tmpimg.second.size() << " " << (double)(cv::getTickCount() - t_imwrite) / cv::getTickFrequency() << std::endl;
#endif // DEBUG_TIME
        }
    }
}

void PostProcesser::PushImwrite(std::string savepath, cv::Mat img)
{
    imwrite_mutex_.lock();
    saveimglist_.push_back(std::make_pair(savepath, img));
    imwrite_mutex_.unlock();
    std::cout << "INFO: saveimglist_.size " << saveimglist_.size() << std::endl;
    condvar_.notify_one();
}

std::vector<std::string> Split(const std::string& str, const std::string& pattern)
{
    std::vector<std::string> result;

    if (str != "")
    {
        std::string strs = str + pattern; //在字符串末尾也加入分隔符，方便截取最后一段
        size_t position = strs.find(pattern); //find函数返回的是其下标
        while (position != std::string::npos) //使用循环将每一段都存放如vector中
        {
            std::string tmp = strs.substr(0, position);
            result.push_back(tmp);
            strs = strs.substr(position + 1); //然后拷贝第一个分隔符后面的所有元素，循环
            position = strs.find(pattern);
        }
    }

    return result;
}

std::vector<DatabaseMap> DefectMap(std::vector<DatabaseMap> database_record, int img_height, int img_width, std::string savename)
{
    std::vector<DatabaseMap> database_record_filter;

    //获取导出阈值
    Sthreshold export_thr;
    QMap<QString, QString> thr = dataSql::getInstance()->getThreshold(QString("export")); //宽度：mm，长度：cm
    export_thr.gansha_width = StringToNum<float>(thr[QString("gansha_width")].toStdString()) / mm_per_pixel_w_;
    export_thr.gansha_height = StringToNum<float>(thr[QString("gansha_height")].toStdString()) * 10 / mm_per_pixel_h_;
    export_thr.kailie_width = StringToNum<float>(thr[QString("kailie_width")].toStdString()) / mm_per_pixel_w_;
    export_thr.kailie_height = StringToNum<float>(thr[QString("kailie_height")].toStdString()) * 10 / mm_per_pixel_h_;
    export_thr.huangban_color = StringToNum<float>(thr[QString("huangban_color")].toStdString());
    export_thr.xuhua_width = StringToNum<float>(thr[QString("xuhua_width")].toStdString()) / mm_per_pixel_w_;
    export_thr.wuwu_area = StringToNum<float>(thr[QString("wuwu_area")].toStdString()) / (mm_per_pixel_w_*mm_per_pixel_h_);
    export_thr.bujun_width = StringToNum<float>(thr[QString("bujun_width")].toStdString()) / mm_per_pixel_w_;
    export_thr.wanzhe_curvature = StringToNum<float>(thr[QString("wanzhe_curvature")].toStdString()) / mm_per_pixel_w_;

    //每个方向扩大，防止文字越界
    int expand_top = 200;
    int expand_bottom = 200;
    int expand_left = 200;
    int expand_right = 200;

    //设置横向、纵向长度
    cv::Mat defectmap(img_height + expand_top + expand_bottom, img_width + expand_left + expand_right, CV_8UC3, cv::Scalar(255, 255, 255));
    cv::Rect roi(expand_left, expand_top, img_width, img_height); //有效区域，即绘制缺陷的区域

    //绘制坐标轴及轴上坐标值
    int line_thickness_axis = 1;
    int font_scale_axis = 1;
    int font_thickness_axis = 1;

    cv::putText(defectmap, "x: mm, y: m", cv::Point(expand_left, img_height+expand_top+50), cv::FONT_HERSHEY_TRIPLEX, font_scale_axis, cv::Scalar(0, 0, 0), font_thickness_axis);
    cv::rectangle(defectmap, roi, cv::Scalar(0, 0, 0), line_thickness_axis); //外框

    for (int row = 0; row < img_height; row += 500)
    {
        cv::putText(defectmap, std::to_string(row), cv::Point(expand_left - 100, row + expand_top), cv::FONT_HERSHEY_TRIPLEX, font_scale_axis, cv::Scalar(0, 0, 0), font_thickness_axis);
        cv::line(defectmap, cv::Point(expand_left - 20, row + expand_top), cv::Point(expand_left, row + expand_top), cv::Scalar(0, 0, 0), line_thickness_axis);
    }
    if (img_height % 500 != 0)
    {
        cv::putText(defectmap, std::to_string(img_height), cv::Point(expand_left - 100, img_height + expand_top), cv::FONT_HERSHEY_TRIPLEX, font_scale_axis, cv::Scalar(0, 0, 0), font_thickness_axis);
        cv::line(defectmap, cv::Point(expand_left - 20, img_height + expand_top), cv::Point(expand_left, img_height + expand_top), cv::Scalar(0, 0, 0), line_thickness_axis);
    }

    for (int col = 0; col < img_width; col += 100)
    {
        cv::putText(defectmap, std::to_string(col), cv::Point(col + expand_left, expand_top - 50), cv::FONT_HERSHEY_TRIPLEX, font_scale_axis, cv::Scalar(0, 0, 2), font_thickness_axis);
        cv::line(defectmap, cv::Point(col + expand_left, expand_top - 20), cv::Point(col + expand_left, expand_top), cv::Scalar(0, 0, 0), line_thickness_axis);
    }
    if (img_width % 100 != 0)
    {
        cv::putText(defectmap, std::to_string(img_width), cv::Point(img_width + expand_left, expand_top - 50), cv::FONT_HERSHEY_TRIPLEX, font_scale_axis, cv::Scalar(0, 0, 0), font_thickness_axis);
        cv::line(defectmap, cv::Point(img_width + expand_left, expand_top - 20), cv::Point(img_width + expand_left, expand_top), cv::Scalar(0, 0, 0), line_thickness_axis);
    }

    //绘制每个缺陷。所有操作都在roi内进行
    int font_scale_defect = 1;
    int font_thickness_defect = 1;

    for (int i = 0; i < database_record.size(); i++)
    {
        DatabaseMap item = database_record[i];
        std::string meters = item["meters"].toStdString(); //m
        std::string position = item["position"].toStdString(); //mm
        std::string length = item["length"].toStdString(); //mm
        std::string defect = item["defect"].toStdString();
		//qstring转std::string 乱码问题
		//std::string defect = std::string((const char *)(item["defect"]).toLocal8Bit().constData());
        if (defect == "干纱")
        {
			defect = "gansha";
            if (StringToNum<float>(item["width"].toStdString()) < export_thr.gansha_width && StringToNum<float>(item["height"].toStdString()) < export_thr.gansha_height)
            {
                continue;
            }
        }
        else if (defect == "开裂")
        {
			defect = "kailie";
            if (StringToNum<float>(item["width"].toStdString()) < export_thr.kailie_width && StringToNum<float>(item["height"].toStdString()) < export_thr.kailie_height)
            {
                continue;
            }
        }
        else if (defect == "黄斑")
        {
			defect = "huangban";
            if (StringToNum<float>(item["color"].toStdString()) < export_thr.huangban_color)
            {
                continue;
            }
        }
        else if (defect == "虚化")
        {
			defect = "xuhua";
            if (StringToNum<float>(item["width"].toStdString()) < export_thr.xuhua_width)
            {
                continue;
            }
        }
        else if (defect == "污物")
        {
			defect = "wuwu";
            if (StringToNum<float>(item["area"].toStdString()) < export_thr.wuwu_area)
            {
                continue;
            }
        }
        else if (defect == "不均")
        {
			defect = "bujun";
            if (StringToNum<float>(item["width"].toStdString()) < export_thr.bujun_width)
            {
                continue;
            }
        }
        else if (defect == "弯折")
        {
			defect = "wanzhe";
            if (StringToNum<float>(item["curvature"].toStdString()) < export_thr.wanzhe_curvature)
            {
                continue;
            }
        }
        else
        {
            std::cout << "ERROR: unknown defect type " << defect << std::endl;
        }

        std::vector<std::string> left_right = Split(position, "-");
        int left = StringToNum<int>(left_right[0]); //缺陷左侧坐标 mm
        int right = StringToNum<int>(left_right[1]); //缺陷右侧坐标 mm
        int top = StringToNum<int>(meters); //缺陷顶部的plc距离 m
        float height = (float)(StringToNum<float>(length) / 1000); //缺陷长度 m

        cv::rectangle(defectmap(roi), cv::Point(left, top), cv::Point(right, top+ height), cv::Scalar(50, 50, 50), -1); //绘制缺陷块，填充
        //为防止文本越界，需要在defectmap上写
        cv::putText(defectmap, meters, cv::Point(expand_left+left, expand_top+top - 20), cv::FONT_HERSHEY_TRIPLEX, font_scale_defect, cv::Scalar(0, 0, 0), font_thickness_defect); //缺陷块上方写米数
        cv::putText(defectmap, position, cv::Point(expand_left+left, expand_top+top + height + 40), cv::FONT_HERSHEY_TRIPLEX, font_scale_defect, cv::Scalar(0, 0, 0), font_thickness_defect); //缺陷块下方写位置
        cv::putText(defectmap, /*defect*/(defect) , cv::Point(expand_left+right + 5, expand_top+top + height+10), cv::FONT_HERSHEY_TRIPLEX, font_scale_defect, cv::Scalar(0, 0, 0), font_thickness_defect); //缺陷块右侧写缺陷类型

        database_record_filter.push_back(item);
    }
    cv::imwrite(savename, defectmap);

    return database_record_filter;
}