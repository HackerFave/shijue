#include "resultanalysis.h"
#include "fileprojectmanager.h"
#include <qdir.h>
#include "ciwamsgdialog.h"
#include <opencv2/opencv.hpp>
#include <fstream>
#include "omp.h"
#include <time.h>
#include<io.h>
#include<algorithm>
#include<vector>
#include <sstream>
#include <QtConcurrent>

Analysiser::Analysiser()
{
    omp_init_lock(&lock_);
}

Analysiser::~Analysiser()
{
    omp_destroy_lock(&lock_);
}

bool Analysiser::UpdatePath()
{
    //首先清空gt_masks_path_和pred_masks_path_，防止多次点击测试按钮导致vector累加
    gt_masks_path_.clear();
    pred_masks_path_.clear();

    //获取两个目录
    pred_masks_dir_ = QString("%1/test_predict").arg(FileProjectManager::Instance()->ProjectGetTrainAbsPath());
    gt_masks_dir_ = QString("%1/mask").arg(FileProjectManager::Instance()->ProjectGetTrainAbsPath());

    //当两个目录不存在时的保护
    QDir dir1(pred_masks_dir_);
    if (!dir1.exists())
    {
        errcode_ = QString("预测结果目录不存在，停止结果分析。\n[%1]").arg(pred_masks_dir_);
        GenerateInfo(errcode_, "INFO");
        return false;
    }
    QDir dir2(gt_masks_dir_);
    if (!dir2.exists())
    {
        errcode_ = QString("标注结果目录不存在，停止结果分析。\n[%1]").arg(gt_masks_dir_);
        GenerateInfo(errcode_, "INFO");
        return false;
    }

    //获取两个目录下所有图片名并保护：某个目录下没有图片时返回false
    std::vector<std::string> pred_names;
    std::vector<std::string> gt_names;
    //cv::glob(pred_masks_dir_.toStdString(), pred_masks_path_);
    ListDir(pred_masks_dir_.toStdString(), pred_names);
    if (pred_names.size() == 0)
    {
        errcode_ = QString("预测结果目录不含图片，停止结果分析。\n[%1]").arg(pred_masks_dir_);
        GenerateInfo(errcode_, "INFO");
        return false;
    }
    //cv::glob(gt_masks_dir_.toStdString(), gt_masks_path_);
    ListDir(gt_masks_dir_.toStdString(), gt_names);
    if (gt_names.size() == 0)
    {
        errcode_ = QString("标注结果目录不含图片，停止结果分析。\n[%1]").arg(gt_masks_dir_);
        GenerateInfo(errcode_, "INFO");
        return false;
    }

    //两个目录下的图片求交集，并保护：若两个目录下无相同图片，则返回false
    std::vector<std::string> names_inter = VectorIntersection(pred_names, gt_names);
    if (!names_inter.size())
    {
        errcode_ = QString("标注结果和预测结果无对应图片，停止结果分析");
        GenerateInfo(errcode_, "INFO");
        return false;
    }
    std::string gt_path;
    std::string pred_path;
    for (int i = 0; i < names_inter.size(); i++)
    {
        gt_path = gt_masks_dir_.toStdString() + "/" + names_inter[i];
        pred_path = pred_masks_dir_.toStdString() + "/" + names_inter[i];
        gt_masks_path_.push_back(gt_path);
        pred_masks_path_.push_back(pred_path);
    }
    assert(gt_masks_path_.size() == pred_masks_path_.size());

    UpdatePathSuccess_ = true;
    return true;
}

std::vector<std::string> Analysiser::Split(const std::string& str, const std::string& pattern)
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

void Analysiser::ListDir(std::string path, std::vector<std::string>& files)
{
    intptr_t hFile = 0;
    _finddata_t fileinfo;
    if ((hFile = _findfirst(path.append("/*").c_str(), &fileinfo)) != -1) {
        while (_findnext(hFile, &fileinfo) == 0) {
            if (strcmp(fileinfo.name, ".."))
                files.push_back(fileinfo.name);
        }
        _findclose(hFile);
    }
}

//容器vector中元素的去重
std::vector<int> unique_element_in_vector(std::vector<int> v) 
{
    std::vector<int>::iterator vector_iterator;
    sort(v.begin(), v.end());
    vector_iterator = std::unique(v.begin(), v.end());
    if (vector_iterator != v.end()) {
        v.erase(vector_iterator, v.end());
    }
    return v;
}

//两个vector求交集
std::vector<std::string> Analysiser::VectorIntersection(std::vector<std::string> v1, std::vector<std::string> v2)
{
    std::vector<std::string> v;
    std::sort(v1.begin(), v1.end());
    std::sort(v2.begin(), v2.end());
    std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), back_inserter(v));//求交集 
    return v;
}

//两个vector求并集
std::vector<int> vectors_set_union(std::vector<int> v1, std::vector<int> v2) {
    std::vector<int> v;
    std::sort(v1.begin(), v1.end());
    std::sort(v2.begin(), v2.end());
    std::set_union(v1.begin(), v1.end(), v2.begin(), v2.end(), back_inserter(v));//求交集 
    return v;
}

/*
判断某元素是否在vector中
true为存在，false为不存在
*/
bool Analysiser::HaveElement(std::vector<std::string> vec, std::string element)
{
    std::vector<std::string>::iterator iter;
    iter = std::find(vec.begin(), vec.end(), element);
    if (iter != vec.end()) 
    {
        return true;
    }
    else 
    {
        return false;
    }
}

template <typename TYPE>
void print_all_pixel(cv::Mat mat)
{
    //打印所有唯一的像素值

    std::cout << "************************************************" << std::endl;
    std::cout << "mat pixel: " << std::endl;

    std::vector<TYPE> pixels;
    std::vector<TYPE>::iterator iter;
    for (int row = 0; row < mat.rows; row++)
    {
        for (int col = 0; col < mat.cols; col++)
        {
            TYPE pixel = mat.ptr<TYPE>(row)[col];
            iter = find(pixels.begin(), pixels.end(), pixel);
            if (iter == pixels.end())
            {
                pixels.push_back(pixel);
            }
        }
    }
    iter = pixels.begin();
    for (; iter != pixels.end(); iter++)
    {
        std::cout << *iter << std::endl;
    }
    std::cout << "************************************************" << std::endl;
}

void Analysiser::GenerateInfo(QString info, QString level)
{
    std::cout << "------------------------" << info.toStdString() << "------------------------" << std::endl;
    CiWaMsgDialog::MessageSignal(level, info);
}

bool Analysiser::Compare(const std::pair<float, int>&i, const std::pair<float, int>&j)
{
    return i.first < j.first;
}

void Analysiser::SaveResultToFile()
{
    QString roc_result_file = QString("%1/roc_result.txt").arg(FileProjectManager::Instance()->ProjectGetTrainAbsPath());
    QString histogram_ok_img_file = QString("%1/histogram_ok_img.txt").arg(FileProjectManager::Instance()->ProjectGetTrainAbsPath());
    QString histogram_ng_img_file = QString("%1/histogram_ng_img.txt").arg(FileProjectManager::Instance()->ProjectGetTrainAbsPath());
    QString histogram_ok_defect_file = QString("%1/histogram_ok_defect.txt").arg(FileProjectManager::Instance()->ProjectGetTrainAbsPath());
    QString histogram_ng_defect_file = QString("%1/histogram_ng_defect.txt").arg(FileProjectManager::Instance()->ProjectGetTrainAbsPath());
    QString cm_file = QString("%1/confusion_matrix.txt").arg(FileProjectManager::Instance()->ProjectGetTrainAbsPath());
    QString roc_data_file = QString("%1/roc_data.txt").arg(FileProjectManager::Instance()->ProjectGetTrainAbsPath());

    //保存读取roc信息。每行为score,x,y
    std::ofstream outfile0(roc_result_file.toStdString());
    if (!roc_result_.empty())
    {
        std::map<float, std::pair<float, float>>::iterator iter;
        for (iter = roc_result_.begin(); iter != roc_result_.end(); iter++)
        {
            auto score = iter->first;
            auto fpr = iter->second.first;
            auto tpr = iter->second.second;
            outfile0 << score << "," << fpr << "," << tpr << std::endl;
        }
    }
    outfile0.close();

    //保存图片级统计信息。首行为纵轴上限，其余每行为x,y
    std::ofstream outfile1(histogram_ok_img_file.toStdString());
    outfile1 << up_img_ << std::endl;
    if (!histogram_ok_img_accumulate_.empty())
    {
        std::map<int, int>::iterator iter;
        for (iter = histogram_ok_img_accumulate_.begin(); iter != histogram_ok_img_accumulate_.end(); iter++)
        {
            outfile1 << iter->first << "," << iter->second << std::endl;
        }
    }
    outfile1.close();

    std::ofstream outfile2(histogram_ng_img_file.toStdString());
    outfile2 << up_img_ << std::endl;
    if (!histogram_ng_img_accumulate_.empty())
    {
        std::map<int, int>::iterator iter;
        for (iter = histogram_ng_img_accumulate_.begin(); iter != histogram_ng_img_accumulate_.end(); iter++)
        {
            outfile2 << iter->first << "," << iter->second << std::endl;
        }
    }
    outfile2.close();

    //保存缺陷级统计信息。首行为纵轴上限，其余每行为x,y
    std::ofstream outfile3(histogram_ok_defect_file.toStdString());
    outfile3 << up_defect_ << std::endl;
    if (!histogram_ok_defect_accumulate_.empty())
    {
        std::map<int, int>::iterator iter;
        for (iter = histogram_ok_defect_accumulate_.begin(); iter != histogram_ok_defect_accumulate_.end(); iter++)
        {
            outfile3 << iter->first << "," << iter->second << std::endl;
        }
    }
    outfile3.close();

    std::ofstream outfile4(histogram_ng_defect_file.toStdString());
    outfile4 << up_defect_ << std::endl;
    if (!histogram_ng_defect_accumulate_.empty())
    {
        std::map<int, int>::iterator iter;
        for (iter = histogram_ng_defect_accumulate_.begin(); iter != histogram_ng_defect_accumulate_.end(); iter++)
        {
            outfile4 << iter->first << "," << iter->second << std::endl;
        }
    }
    outfile4.close();

    //保存混淆矩阵信息。首行为拖动时的ok阈值及ng阈值，应用时的PixelFilterScore_和DefectComponentScoreMode_，其余8行为cm的8个值
    std::ofstream outfile5(cm_file.toStdString());
    outfile5 << cm_threshold_.global_ok_thr << "," << cm_threshold_.global_ng_thr << PixelFilterScore_ << DefectComponentScoreMode_ << std::endl;
    for (int i = 0; i < confusion_matrix_.size(); i++)
    {
        outfile5 << confusion_matrix_[i] << std::endl;
    }
    outfile5.close();

    //保存roc_data_，用于调整混淆矩阵
    std::ofstream outfile6(roc_data_file.toStdString());
    for (int i = 0; i < roc_data_.size(); i++)
    {
        outfile6 << roc_data_[i].first << "," << roc_data_[i].second << std::endl;
    }
    outfile6.close();
}

bool Analysiser::ReadResultFromFile()
{
    QString roc_result_file = QString("%1/roc_result.txt").arg(FileProjectManager::Instance()->ProjectGetTrainAbsPath());
    QString histogram_ok_img_file = QString("%1/histogram_ok_img.txt").arg(FileProjectManager::Instance()->ProjectGetTrainAbsPath());
    QString histogram_ng_img_file = QString("%1/histogram_ng_img.txt").arg(FileProjectManager::Instance()->ProjectGetTrainAbsPath());
    QString histogram_ok_defect_file = QString("%1/histogram_ok_defect.txt").arg(FileProjectManager::Instance()->ProjectGetTrainAbsPath());
    QString histogram_ng_defect_file = QString("%1/histogram_ng_defect.txt").arg(FileProjectManager::Instance()->ProjectGetTrainAbsPath());
    QString cm_file = QString("%1/confusion_matrix.txt").arg(FileProjectManager::Instance()->ProjectGetTrainAbsPath());
    QString roc_data_file = QString("%1/roc_data.txt").arg(FileProjectManager::Instance()->ProjectGetTrainAbsPath());

    if (!QFile::exists(roc_result_file) || !QFile::exists(histogram_ok_img_file) || !QFile::exists(histogram_ng_img_file) || 
        !QFile::exists(histogram_ok_defect_file) || !QFile::exists(histogram_ng_defect_file) || !QFile::exists(cm_file) || !QFile::exists(roc_data_file))
    {
        std::cout << "------------------------" << "WARNING:result file not exists, can not get result from file." << "------------------------" << std::endl;
        return false;
    }

    std::string line;
    std::vector<std::string> contents;

    //读取roc信息。每行为score,x,y
    std::ifstream infile1(roc_result_file.toStdString());
    while (std::getline(infile1, line))
    {
        contents = Split(line, ",");
        roc_result_[StringToNum<float>(contents[0])] = std::make_pair(StringToNum<float>(contents[1]), StringToNum<float>(contents[2]));
    }
    infile1.close();

    //读取图片级统计信息。首行为纵轴上限，其余每行为x,y
    std::ifstream infile2(histogram_ok_img_file.toStdString());
    while (std::getline(infile2, line))
    {
        contents = Split(line, ",");
        if (contents.size() == 1)
        {
            up_img_ = StringToNum<int>(contents[0]);
        }
        else
        {
            histogram_ok_img_accumulate_[StringToNum<int>(contents[0])] = StringToNum<int>(contents[1]);
        }
    }
    infile2.close();

    std::ifstream infile3(histogram_ng_img_file.toStdString());
    while (std::getline(infile3, line))
    {
        contents = Split(line, ",");
        if (contents.size() == 1)
        {
            up_img_ = StringToNum<int>(contents[0]);
        }
        else
        {
            histogram_ng_img_accumulate_[StringToNum<int>(contents[0])] = StringToNum<int>(contents[1]);
        }
    }
    infile3.close();

    //读取缺陷级统计信息。首行为纵轴上限，其余每行为x,y
    std::ifstream infile4(histogram_ok_defect_file.toStdString());
    while (std::getline(infile4, line))
    {
        contents = Split(line, ",");
        if (contents.size() == 1)
        {
            up_defect_ = StringToNum<int>(contents[0]);
        }
        else
        {
            histogram_ok_defect_accumulate_[StringToNum<int>(contents[0])] = StringToNum<int>(contents[1]);
        }
    }
    infile4.close();

    std::ifstream infile5(histogram_ng_defect_file.toStdString());
    while (std::getline(infile5, line))
    {
        contents = Split(line, ",");
        if (contents.size() == 1)
        {
            up_defect_ = StringToNum<int>(contents[0]);
        }
        else
        {
            histogram_ng_defect_accumulate_[StringToNum<int>(contents[0])] = StringToNum<int>(contents[1]);
        }
    }
    infile5.close();

    //保存混淆矩阵信息。首行为拖动时的ok阈值及ng阈值，应用时的PixelFilterScore_和DefectComponentScoreMode_，其余8行为cm的8个值
    std::ifstream infile6(cm_file.toStdString());
    while (std::getline(infile6, line))
    {
        contents = Split(line, ",");
        if (contents.size() == 4)
        {
            cm_threshold_.global_ok_thr = StringToNum<int>(contents[0]);
            cm_threshold_.global_ng_thr = StringToNum<int>(contents[1]);
            PixelFilterScore_ = StringToNum<int>(contents[2]);
            DefectComponentScoreMode_ = StringToNum<int>(contents[3]);
        }
        else
        {
            confusion_matrix_.push_back(StringToNum<int>(contents[0]));
        }
    }
    infile6.close();

    //读取roc_data_，用于调整混淆矩阵
    std::ifstream infile7(roc_data_file.toStdString());
    while (std::getline(infile7, line))
    {
        contents = Split(line, ",");
        roc_data_.push_back(std::make_pair(StringToNum<float>(contents[0]), StringToNum<int>(contents[1])));
    }
    infile7.close();

    return true;
}

//将string转换为数值类型
template <typename TYPE>
TYPE Analysiser::StringToNum(const std::string& str)
{
    std::istringstream iss(str);
    TYPE num;
    iss >> num;
    return num;
}

void Analysiser::SetRocAndHistogram(float pixel_filter_score, int defect_filter_mode, bool from_file)
{
    bool recalculate = true;
    if (from_file)
    {
        bool read_success = ReadResultFromFile();
        if (read_success)
        {
            recalculate = false;
            GetRocSuccess_ = true;
            UpdatePathSuccess_ = true;
            std::cout << "------------------------" << "success:read infomation from file." << "------------------------" << std::endl;
        }
        else
        {
            std::cout << "------------------------" << "failed:read infomation from file failed, recalculate" << "------------------------" << std::endl;
        }

    }
    if (recalculate)
    {
        //首先清空旧状态
        histogram_ng_img_.clear();
        histogram_ok_img_.clear();
        histogram_ng_img_accumulate_.clear();
        histogram_ok_img_accumulate_.clear();
        histogram_ng_defect_.clear();
        histogram_ok_defect_.clear();
        histogram_ng_defect_accumulate_.clear();
        histogram_ok_defect_accumulate_.clear();

        //将传入的参数赋值给成员变量
        PixelFilterScore_ = pixel_filter_score;
        DefectComponentScoreMode_ = defect_filter_mode;

        try
        {
            roc_result_ = CalculateRocAndHistogram(gt_masks_path_, pred_masks_path_); //计算roc/histogram
            GetRocSuccess_ = true;
        }
        catch (...)
        {
            errcode_ = QString("get roc failed.");
            GenerateInfo(errcode_);
        }

#ifdef ANALYSISDEBUG
        try
        {
            //绘制
            DrawROC(roc_result_);
            int up = get_max_number();
            DrawHistogram(histogram_ok_img_accumulate_, "histogram_ok_img_accumulate_.png", 101, up);
            DrawHistogram(histogram_ng_img_accumulate_, "histogram_ng_img_accumulate_.png", 101, up);
            get_histogram_ok_defect();
            get_histogram_ng_defect();
        }
        catch (...)
        {
            std::cout << "------------------------" << "save roc or histogram image failed." << "------------------------" << std::endl;
        }
#endif

        std::cout << "Result Analysis Done." << std::endl;
    }
}

void Analysiser::set_max_number_img() //返回图片级直方图统计结果的纵轴上限
{
    int up = 0;
    int max_ok = 0;
    if (histogram_ok_img_accumulate_.size()) {
        max_ok = histogram_ok_img_accumulate_.begin()->second;
    }

    if (histogram_ng_img_accumulate_.size()) {
        auto max_ng = histogram_ng_img_accumulate_.end();
        max_ng--;
        up = max_ng->second;
    }
    up = std::max(max_ok, up);
    up_img_ = up + 5;
}

void Analysiser::set_max_number_defect() //返回缺陷级直方图统计结果的纵轴上限
{
    int max_ok = 0;
    int max_ng = 0;
    if (histogram_ok_defect_accumulate_.size())
    {
        max_ok = histogram_ok_defect_accumulate_.begin()->second;
    }
    if (histogram_ng_defect_accumulate_.size())
    {
        auto max_ng_tmp = histogram_ng_defect_accumulate_.end();
        max_ng_tmp--;
        max_ng = max_ng_tmp->second;
    }
    int up = std::max(max_ok, max_ng);
    up_defect_ = up + 5;
}

void Analysiser::set_confusion_matrix(Sthreshold threshold)
{
    Sconfusionmatrix confusion_matrix = CalculateConfusionMatrix(roc_data_, threshold); //计算混淆矩阵

    confusion_matrix_.clear();
    confusion_matrix_.push_back(confusion_matrix.tn);
    confusion_matrix_.push_back(confusion_matrix.fn);
    confusion_matrix_.push_back(confusion_matrix.ok_uncertain);
    confusion_matrix_.push_back(confusion_matrix.ng_uncertain);
    confusion_matrix_.push_back(confusion_matrix.fp);
    confusion_matrix_.push_back(confusion_matrix.tp);
    int gt_ok_total = confusion_matrix.tn + confusion_matrix.ok_uncertain + confusion_matrix.fp;
    confusion_matrix_.push_back(gt_ok_total);
    int gt_ng_total = confusion_matrix.tp + confusion_matrix.ng_uncertain + confusion_matrix.fn;
    confusion_matrix_.push_back(gt_ng_total);
}

void Analysiser::ImageReadThread(const std::string& img_path, cv::Mat* mat)
{
    *mat = cv::imread(img_path, 0);
}

std::vector<std::pair<float, int> > Analysiser::GetRocData(std::vector<std::string> gt_masks_path, std::vector<std::string> pred_masks_path)
{
    std::vector<std::pair<float, int> > roc_data; //用于计算roc的数据对。first：预测为1类的置信度；second：真实标签
    int total_img = gt_masks_path.size();
    for (int i = 0; i < total_img; i++)
    {
        //printf("Result Analysis Progress: %d/%d %s \r", i+1, gt_masks_path.size(), gt_masks_path[i].c_str());
        printf("Result Analysis Progress: %d/%d\r", i+1, total_img);

        cv::Mat gt_mask;
        cv::Mat pred_mask;
        QFuture<void> future1 = QtConcurrent::run(ImageReadThread, gt_masks_path[i], &gt_mask);
        QFuture<void> future2 = QtConcurrent::run(ImageReadThread, pred_masks_path[i], &pred_mask);
        while (!(future1.isFinished() && future2.isFinished()))
        {
            continue;
        }

        //读两张图约25ms
        //cv::Mat gt_mask = cv::imread(gt_masks_path[i], 0); //0,1
        //cv::Mat pred_mask = cv::imread(pred_masks_path[i], 0); //0-255，像素值为缺陷类的得分

        GtResult gt_result;
        int nonzero = cv::countNonZero(gt_mask);
        if (nonzero > 0)
        {
            gt_result = GtResult::GtNG;
        }
        else
        {
            gt_result = GtResult::GtOK;
        }
        double maxval;
        cv::minMaxLoc(pred_mask, NULL, &maxval, NULL, NULL); //maxval:0-255
        roc_data.push_back(std::make_pair(maxval / 2.55, gt_result)); //maxval由0-255转到0-100，便于后边进行roc计算

        //统计缺陷级信息，同时根据缺陷级信息统计图片级信息
        DefectStatistic(gt_mask, pred_mask); //约40-150ms
    }
    
    return roc_data;
}

std::map<int, int> Analysiser::HistogramPostProcess(std::map<int, int> histogram, bool reverse)
{
    std::map<int, int> histogram_accumulate;

    if (reverse)
    {
        std::map<int, int>::reverse_iterator iter;
        for (iter = histogram.rbegin(); iter != histogram.rend(); iter++) //histogram_ok是分数升序排列的，需要反向遍历即按分数降序遍历
        {
            if (histogram_accumulate.empty())
            {
                histogram_accumulate[iter->first] = iter->second;
            }
            else
            {
                auto start = histogram_accumulate.begin();
                histogram_accumulate[iter->first] = iter->second + start->second;
            }
        }
    }
    else
    {
        std::map<int, int>::iterator iter1;
        for (iter1 = histogram.begin(); iter1 != histogram.end(); iter1++)
        {
            if (histogram_accumulate.empty())
            {
                histogram_accumulate[iter1->first] = iter1->second;
            }
            else
            {
                auto end = histogram_accumulate.end();
                end--;
                histogram_accumulate[iter1->first] = iter1->second + end->second;
            }
        }
    }
    
    return histogram_accumulate;
}

std::map<float, std::pair<float, float>> Analysiser::CalculateRocAndHistogram(std::vector<std::string> gt_masks_path, std::vector<std::string> pred_masks_path)
{
    roc_data_ = GetRocData(gt_masks_path, pred_masks_path); //获取计算roc所需的数据

    //直方图统计信息后处理
    histogram_ok_img_accumulate_ = HistogramPostProcess(histogram_ok_img_, true);
    histogram_ng_img_accumulate_ = HistogramPostProcess(histogram_ng_img_, false);
    histogram_ok_defect_accumulate_ = HistogramPostProcess(histogram_ok_defect_, true);
    histogram_ng_defect_accumulate_ = HistogramPostProcess(histogram_ng_defect_, false);

    //根据预测为1类的置信度对预测结果升序排列
    std::sort(roc_data_.begin(), roc_data_.end(), Compare);
    
    std::vector<float> positive_score; //gt=1的样本的分数，升序
    std::vector<float> negative_score; //gt=0的样本的分数，升序
    std::vector<std::pair<float, int>> data_ok;
    int roc_data_size = roc_data_.size();
    for (int i = 0; i < roc_data_size; i++)
    {
        auto cur_data = roc_data_[i];
        if (cur_data.second == 1)
        {
            positive_score.push_back(cur_data.first);
        }
        else //(cur_data.second == 0)
        {
            negative_score.push_back(cur_data.first);
        }
    }

    //分数区间0-100，分为100份统计roc
    std::map<float, std::pair<float, float>> roc_result; // <score, <fpr, tpr>>
    float fpr;
    float tpr;
    float sensitivity;
    float specificity;
    float cur_youden; //当前阈值下的约登指数
    float best_youden = 0; //最佳（最大）的约登指数
    float best_threshold; //最佳阈值
    float auc = 0;
    float recall;
    float precision;
    float f1;
    for (int thr = 0; thr < 100; thr++) //此次计算使用的阈值
    {
        //计算每个thr下的fpr和tpr用于绘制roc
        int false_positive;
        int true_positive;
        for (int i = 0; i < negative_score.size(); i++)
        {
            if (negative_score[i] >= thr)
            {
                false_positive = negative_score.size() - i;
                break;
            }
        }
        for (int j = 0; j < positive_score.size(); j++)
        {
            if (positive_score[j] >= thr)
            {
                true_positive = positive_score.size() - j;
                break;
            }
        }

        if (negative_score.size() == 0)
        {
            fpr = 0;
        }
        else
        {
            fpr = float(false_positive) / float(negative_score.size());
        }
        if (positive_score.size() == 0)
        {
            tpr = 0;
        }
        else
        {
            tpr = float(true_positive) / float(positive_score.size());
        }
        roc_result[thr] = std::make_pair(fpr, tpr);
        auc += tpr;

        //计算每个thr下的tp,tn,fp,fn用于计算灵敏度和特异度进而计算约登指数和最佳阈值
        //约登指数 = 灵敏度sensitivity + 特异度specificity - 1 = tp / (tp + fn) + tn / (tn + fp) - 1
        int tp = 0;
        int tn = 0;
        int fp = 0;
        int fn = 0;
        for (int i = 0; i < negative_score.size(); i++)
        {
            if (negative_score[i] <= thr)
            {
                tn += 1;
            }
            else
            {
                fp += 1;
            }
        }
        for (int i = 0; i < positive_score.size(); i++)
        {
            if (positive_score[i] >= thr)
            {
                tp += 1;
            }
            else
            {
                fn += 1;
            }
        }
        //无缺陷时，tp=fn=0，需要判断
        if ((tp == 0) && (fn == 0))
        {
            sensitivity = 0;
        }
        else
        {
            sensitivity = tp / ((tp + fn) / 1.0);
        }
        //无正常时，tn=fp=0
        if ((tn == 0) && (fp == 0))
        {
            specificity = 0;
        }
        else 
        {
            specificity = tn / ((tn + fp) / 1.0);
        }
        cur_youden = sensitivity + specificity - 1;
        if (cur_youden > best_youden)
        {
            best_threshold = thr;
            best_youden = cur_youden;
        }

        //debug
        //recall = tp / ((tp + fn) / 1.0);
        //precision = tp / ((tp + fp) / 1.0);
        //f1 = 2 * precision*recall / ((precision + recall) / 1.0);
    }

    if ((negative_score.size() == 0) || (positive_score.size() == 0)) //若这批数据全为缺陷图片或全为正常图片，则阈值为0
    {
        best_threshold = 0;
    }

    auc /= 100.0;
    roc_info_.auc = auc;
    roc_info_.best_threshold = best_threshold / 100.;

    return roc_result;
}

//cv::Mat Analysiser::MaskTransform(Sthreshold threshold)
//{
//    /*
//    将用户设置的过滤方法和阈值，统一转为cv::Mat mask 0-255
//    对预测结果mask进行过滤时，无论什么过滤方法（整张图像阈值，单个像素阈值，缺陷连通域阈值avg or max or no），均先转为一张mask，然后再过滤
//    */
//    cv::Mat _gt_mask = cv::imread(gt_masks_path_[0], 0);
//    cv::Mat filter_mask_ok(_gt_mask.rows, _gt_mask.cols, CV_32FC1, threshold.global_ok_thr);
//    cv::Mat filter_mask_ng(_gt_mask.rows, _gt_mask.cols, CV_32FC1, threshold.global_ng_thr);
//    cv::Mat filter_mask_tmp(_gt_mask.rows, _gt_mask.cols, CV_32FC1, 0.0);
//
//    std::vector<cv::Mat> channels;
//    channels.push_back(filter_mask_ok);
//    channels.push_back(filter_mask_ng);
//    channels.push_back(filter_mask_tmp);
//
//    cv::Mat filter_mask;
//    cv::merge(channels, filter_mask); //0：ok阈值；1：ng阈值；2：tmp
//
//    return filter_mask;
//}

//cv::Mat Analysiser::MaskFilter(cv::Mat pred_mask, cv::Mat filter_mask)
//{
//    /*
//    根据filter_mask对pred_mask进行过滤
//    pred_mask：模型预测结果mask，0-255
//    filter_mask：用户指定过滤方法和阈值后，生成的过滤mask，0-255
//    */
//    std::vector<cv::Mat> channels;
//    cv::split(filter_mask, channels); //0：ok阈值；1：ng阈值；2：tmp
//
//    cv::Mat result_filter_ok = pred_mask > channels[0]; //0,255
//    cv::Mat result_filter_ng = pred_mask >= channels[1]; //0,255
//    result_filter_ok /= 255.0; //0,1
//    result_filter_ng /= 255.0; //0,1
//    pred_mask_filter_ = pred_mask.mul(result_filter_ok).mul(result_filter_ng);
//
//    //对于每张图，首先计算其是否属于uncertain类别。若是，则不计入positive与negative
//    cv::Mat result_filter_uncertain = channels[0] < pred_mask_filter_ < channels[1];
//    if (cv::countNonZero(result_filter_uncertain) > 0)
//    {
//        uncertain_flag_ = true;
//        return pred_mask_filter_;
//    }
//    else
//    {
//        uncertain_flag_ = false;
//    }
//
//    double minval;
//    double maxval;
//    cv::minMaxLoc(pred_mask, &minval, &maxval, NULL, NULL);
//    cv::minMaxLoc(filter_mask, &minval, &maxval, NULL, NULL);
//    cv::minMaxLoc(pred_mask_filter_, &minval, &maxval, NULL, NULL);
//
//    return pred_mask_filter_;
//}

Sconfusionmatrix Analysiser::CalculateConfusionMatrix(std::vector<std::pair<float, int> > roc_data, Sthreshold threshold)
{
    //roc_data: first：预测为1类的置信度；second：真实标签

    Sconfusionmatrix confusion_matrix;
    float ok_threshold = threshold.global_ok_thr;
    float ng_threshold = threshold.global_ng_thr;
    GtResult gt_result;
    PredResult pred_result;
    for (int i = 0; i < roc_data.size(); i++) //遍历每张图，获取最大得分和真实标签
    {
        auto cur_data = roc_data[i];
        cur_data.first /= 100.;

        //判断gt
        if (cur_data.second == 0)
        {
            gt_result = GtResult::GtOK;
        }
        else
        {
            gt_result = GtResult::GtNG;
        }

        //根据阈值判断pred
        if (cur_data.first <= ok_threshold)
        {
            pred_result = PredResult::PredOK;
        }
        else if (cur_data.first >= ng_threshold)
        {
            pred_result = PredResult::PredNG;
        }
        else
        {
            pred_result = PredResult::PredUNCERTAIN;
        }
        
        //根据gt_result和pred_reuslt计算混淆矩阵
        if ((gt_result == GtResult::GtOK) && (pred_result == PredResult::PredUNCERTAIN))
        {
            confusion_matrix.ok_uncertain += 1;
        }
        if ((gt_result == GtResult::GtNG) && (pred_result == PredResult::PredUNCERTAIN))
        {
            confusion_matrix.ng_uncertain += 1;
        }
        if ((gt_result == GtResult::GtOK) && (pred_result == PredResult::PredOK))
        {
            confusion_matrix.tn += 1;
        }
        if ((gt_result == GtResult::GtNG) && (pred_result == PredResult::PredNG))
        {
            confusion_matrix.tp += 1;
        }
        if ((gt_result == GtResult::GtNG) && (pred_result == PredResult::PredOK))
        {
            confusion_matrix.fn += 1;
        }
        if ((gt_result == GtResult::GtOK) && (pred_result == PredResult::PredNG))
        {
            confusion_matrix.fp += 1;
        }
    }

    return confusion_matrix;
}

//void Analysiser::UnitTest()
//{
//    //new
//    Analysiser* analysiser = new Analysiser();
//    
//    //点击测试，重新计算roc/histogram/混淆矩阵。测试后不删除对象，点击应用也不删除对象，重新测试时才需要del并new
//    roc_result_ = CalculateRocAndHistogram(gt_masks_path_, pred_masks_path_, true); //计算roc/histogram
//    DrawROC(roc_result_);
//    DrawHistogram(histogram_ok_, "histogram_ok.png", 101, 500);
//    DrawHistogram(histogram_ng_, "histogram_ng.png", 101, 500);
//    //计算混淆矩阵时需要阈值
//    Sthreshold sthreshold;
//    sthreshold.global_ng_thr = 0.6;
//    sthreshold.global_ok_thr = 0.4;
//    confusion_matrix_ = CalculateConfusionMatrix(roc_data_, sthreshold); //计算混淆矩阵
//
//    //点击应用，或者调整阈值线，只重新计算混淆矩阵
//    sthreshold.global_ng_thr = 0.8;
//    sthreshold.global_ok_thr = 0.3;
//    confusion_matrix_ = CalculateConfusionMatrix(roc_data_, sthreshold); //计算混淆矩阵
//}


void Analysiser::DrawROC(std::map<float, std::pair<float, float>> roc_result)
{
    std::vector<cv::Point> pts;
    std::map<float, std::pair<float, float>>::iterator iter;
    int fpr;
    int tpr;
    cv::Point p;
    for (iter = roc_result.begin(); iter != roc_result.end(); iter++)
    {
        fpr = iter->second.first *1000;
        tpr = iter->second.second * 1000;
        p.x = fpr;
        p.y = tpr;
        pts.push_back(p);
    }
    std::vector<std::vector<cv::Point>> points;
    points.push_back(pts);
    cv::Mat show_img(1001, 1001, CV_32FC3, cv::Scalar(200, 200, 200));
    cv::polylines(show_img, points, false, cv::Scalar(255, 0, 0), 1);
    cv::flip(show_img, show_img, 0);
    QString save_path = QString("%1/roc.png").arg(FileProjectManager::Instance()->ProjectGetTrainAbsPath());
    cv::imwrite(save_path.toStdString(), show_img);
}

/*
绘制直方图
*/
void Analysiser::DrawHistogram(std::map<int, int> histogram, std::string filename, int x_limit, int y_limit)
{
    cv::Mat show_img(y_limit, x_limit, CV_32FC3, cv::Scalar(200, 200, 200));
    std::map<int, int>::iterator iter;
    int score;
    int number;
    for (iter = histogram.begin(); iter != histogram.end(); iter++)
    {
        score = iter->first;
        number = iter->second;
        cv::line(show_img, cv::Point(score, 0), cv::Point(score, number), cv::Scalar(255, 0, 0), 1);
    }
    cv::flip(show_img, show_img, 0);
    QString save_path = QString("%1/%2").arg(FileProjectManager::Instance()->ProjectGetTrainAbsPath(), QString::fromStdString(filename));
    cv::imwrite(save_path.toStdString(), show_img);
}

void Analysiser::FindContoursThread(const cv::Mat& mat, std::vector<std::vector<cv::Point>>* contours)
{
    std::vector<cv::Vec4i> tmphierarchy;
    cv::findContours(mat, *contours, tmphierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
}

void Analysiser::DefectStatistic(cv::Mat gt_mask, cv::Mat pred_mask) //gt 0,1, pred 0-255
{
    cv::Mat pred_mask_origin = pred_mask.clone(); //复制一份，后边需要用

    //对每个pred mask使用很小的置信度阈值进行过滤，防止缺陷过多
    //float filter_confidence = 0.2;
    cv::threshold(pred_mask, pred_mask, PixelFilterScore_ * 255, 1, cv::THRESH_TOZERO);

    //std::vector<cv::Vec4i> hierarchy;
    ////对gt_mask求所有缺陷的边缘
    //std::vector<std::vector<cv::Point> > gt_contours1;
    //cv::findContours(gt_mask, gt_contours1, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    ////对pred_mask求所有缺陷的边缘
    //std::vector<std::vector<cv::Point> > pred_contours1;
    //cv::findContours(pred_mask, pred_contours1, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<std::vector<cv::Point> > gt_contours;
    std::vector<std::vector<cv::Point> > pred_contours;
    QFuture<void> future1 = QtConcurrent::run(FindContoursThread, gt_mask, &gt_contours);
    QFuture<void> future2 = QtConcurrent::run(FindContoursThread, pred_mask, &pred_contours);
    while (!(future1.isFinished() && future2.isFinished()))
    {
        continue;
    }

    //循环时，常量比变量快
    int gt_contours_num = gt_contours.size();
    int pred_contours_num = pred_contours.size();
#ifdef ANALYSISDEBUG
    std::cout << "gt_contours_num: " << gt_contours_num << ", pred_contours_num: " << pred_contours_num << std::endl;
#endif

    float score_img; //图片级分数，取defect_scores的最大值
    std::vector<float> defect_scores; //此图中所有缺陷的分数，包括标注的缺陷和预测缺陷。0-255

    //对gt_contours,pred_contours遍历及比较
    if (!gt_contours_num) //若gt_contours为空，即此张图无缺陷
    {
        if (!pred_contours_num) //若gt_contours和pred_contours都为空，则这张图跳过
        {
            defect_scores.push_back(0);
        }
        else //若gt_contours且pred_contours不为空，则pred_contours中的所有边缘内的最大值均放到histogram_ok中
        {
#pragma omp parallel for schedule(dynamic)
            for (int i = 0; i < pred_contours_num; i++)
            {
                std::vector< std::vector<cv::Point>> tmp_contour{ pred_contours[i] };
                float maxval = ValueInContours(pred_mask, tmp_contour); //0-255
                omp_set_lock(&lock_);
                AddToMap(histogram_ok_defect_, maxval/2.55, 1);
                defect_scores.push_back(maxval);
                omp_unset_lock(&lock_);
            }
        }
    }
    else //gt_contours不为空
    {
        if (!pred_contours_num) //gt_contours不为空，pred_contours为空，则使用gt_contours在pred_mask_origin中寻找每个区域的最大值，放到histogram_ng中
        {
#pragma omp parallel for schedule(dynamic)
            for (int i = 0; i < gt_contours_num; i++)
            {
                std::vector< std::vector<cv::Point>> tmp_contour{ gt_contours[i] };
                float maxval = ValueInContours(pred_mask_origin, tmp_contour);
                omp_set_lock(&lock_);
                AddToMap(histogram_ng_defect_, maxval/2.55, 1);
                defect_scores.push_back(maxval);
                omp_unset_lock(&lock_);
            }
        }
        else //gt_contours不为空，pred_contours不为空
        {
            /*
            对每个gt_contour，判断是否有pred_contour与其相交。
            若相交，则将此pred_contour内pred_mask的最大值放入histogram_ng。若多个相交，则第一个放入histogram_ng，剩余的放入histogram_ok
            若不相交，则将此pred_contourr内pred_mask的最大值放入histogram_ok
            
            每个gt或者pred的contour只能匹配一次。没有使用过为false，使用过置为true
            */
            std::vector<bool> gt_flags(gt_contours_num, false);
            std::vector<bool> pred_flags(pred_contours_num, false);
            for (int i = 0; i < gt_contours_num; i++)
            {
                std::vector< std::vector<cv::Point>> tmp_gt_contour{ gt_contours[i] };
                //bool gt_contour_used = false; //每个gt_contour仅能被pred_contour匹配一次，若已经匹配过，则置为true

                for (int j = 0; j < pred_contours_num; j++)
                {
                    std::vector< std::vector<cv::Point>> tmp_pred_contour{ pred_contours[j] };
                    bool inter = Intersect(tmp_gt_contour, tmp_pred_contour, pred_mask.size());
                    if (inter && (!gt_flags[i])) //相交，且这个gt_contour没有被匹配过
                    {
                        float maxval = ValueInContours(pred_mask, tmp_pred_contour);
                        omp_set_lock(&lock_);
                        AddToMap(histogram_ng_defect_, maxval/2.55, 1);
                        defect_scores.push_back(maxval);
                        omp_unset_lock(&lock_);
                        gt_flags[i] = true;
                        pred_flags[j] = true;
                        break;
                    }
                }
            }
            
            for (int i = 0; i < gt_contours_num; i++)
            {
                if (!gt_flags[i])
                {
                    std::vector< std::vector<cv::Point>> tmp_gt_contour{ gt_contours[i] };
                    float maxval = ValueInContours(pred_mask_origin, tmp_gt_contour);
                    omp_set_lock(&lock_);
                    AddToMap(histogram_ng_defect_, maxval / 2.55, 1);
                    defect_scores.push_back(maxval);
                    omp_unset_lock(&lock_);
                }
            }
            for (int i = 0; i < pred_contours_num; i++)
            {
                if (!pred_flags[i])
                {
                    std::vector< std::vector<cv::Point>> tmp_pred_contour{ pred_contours[i] };
                    float maxval = ValueInContours(pred_mask, tmp_pred_contour);
                    omp_set_lock(&lock_);
                    AddToMap(histogram_ok_defect_, maxval / 2.55, 1);
                    defect_scores.push_back(maxval);
                    omp_unset_lock(&lock_);
                }
            }
        }
    }

    //将图片级分数score_img添加到histogram中
    score_img = *std::max_element(defect_scores.begin(), defect_scores.end());
    if (gt_contours_num) //有缺陷图，分数添加到histogram_ng_img_
    {
        AddToMap(histogram_ng_img_, score_img / 2.55, 1);
    }
    else //无缺陷图，分数添加到histogram_ok_img_
    {
        AddToMap(histogram_ok_img_, score_img / 2.55, 1);
    }
}

/*
向map中添加元素
向histogram中添加key:value。若key存在，则value累加；若key不存在，则创建。
histogram传入地址，原地修改，无返回值
*/
void Analysiser::AddToMap(std::map<int, int>& histogram, int key, int value) //<1ms
{
    if (histogram.find(key) == histogram.end())
    {
        histogram[key] = value;
    }
    else
    {
        histogram[key] += value;
    }
}

/*
判断两个contours是否相交
size: width, height,能够包含两个轮廓的图像尺寸
return: true相交，false不相交
*/
bool Analysiser::Intersect(std::vector<std::vector<cv::Point> > contours1, std::vector<std::vector<cv::Point> > contours2, cv::Size size)
{
    cv::Rect rect1 = cv::boundingRect(contours1[0]);
    cv::Rect rect2 = cv::boundingRect(contours2[0]);
    cv::Rect inter_rect = rect1 & rect2;
    if (inter_rect.width == 0)
    {
        return false;
    }

    //TODO:两个轮廓可以并行画
    //cv::Mat draw1(size, CV_8UC1, cv::Scalar(0));
    //cv::Mat draw2(size, CV_8UC1, cv::Scalar(0));
    //cv::drawContours(draw1, contours1, -1, cv::Scalar(1), -1); //缺陷区域内用1填充
    //cv::drawContours(draw2, contours2, -1, cv::Scalar(1), -1); //缺陷区域内用1填充
    //cv::Mat tmp = draw1.mul(draw2);
    //if (cv::countNonZero(tmp) != 0)
    //{
    //    return true;
    //}
    //else
    //{
    //    return false;
    //}

    //改为判断点是否在轮廓内，pointPolygonTest
    std::vector<cv::Point> contour1 = contours1[0];
    std::vector<cv::Point> contour2 = contours2[0];
    int contour1_size = contour1.size();
    int contour2_size = contour2.size();
    bool inter = false;
    double position = -1;
    if (contour1_size > contour2_size) //遍历点少的那个轮廓
    {
        for (int k = 0; k < contour2_size; k++)
        {
            position = cv::pointPolygonTest(contour1, contour2[k], false); //1,-1,0分别表示在内部，外部，轮廓上
            if (position != -1)
            {
                inter = true;
                break;
            }
        }
    }
    else
    {
        for (int k = 0; k < contour1_size; k++)
        {
            position = cv::pointPolygonTest(contour2, contour1[k], false);
            if (position != -1)
            {
                inter = true;
                break;
            }
        }
    }

    return inter;
}

/*
指定coutours，在mask中寻找contours区域内的最大/最小/平均像素值
*/
float Analysiser::ValueInContours(const cv::Mat& mask, const std::vector<std::vector<cv::Point> >& contours) //2-3ms->优化为一半
{
    double result;
    cv::Mat draw(mask.rows, mask.cols, CV_8UC1, cv::Scalar(0)); //0.0008703 second
    cv::drawContours(draw, contours, -1, cv::Scalar(1), -1); //缺陷区域内用1填充 1.63e-05 second
    cv::Rect rect = cv::boundingRect(contours[0]); //1.4e-06 second

    if (DefectComponentScoreMode_ == 0)
    {
        cv::minMaxLoc(mask(rect), NULL, &result, NULL, NULL, draw(rect));
    }
    //else if (DefectComponentScoreMode_ == "min")
    //{
    //    std::vector<uchar> mask_unique;
    //    mask_unique = Analysiser::unique<uchar>(mask_roi, true); //升序
    //    result = mask_unique[1];
    //}
    else if (DefectComponentScoreMode_ == 1)
    {
        cv::Scalar mean = cv::mean(mask(rect), draw(rect));
        result = mean[0];
    }
    else
    {
        std::cout << "ERROR: unknow mode." << std::endl;
    }

    return result;
}

template <typename TYPE>
std::vector<TYPE> Analysiser::unique(const cv::Mat& input, bool sort)
{
    //if (input.channels() > 1 || input.type() != CV_32F)
    //{
    //    std::cerr << "unique !!! Only works with CV_32F 1-channel Mat" << std::endl;
    //    return std::vector<TYPE>();
    //}

    std::vector<TYPE> out;
    for (int y = 0; y < input.rows; ++y)
    {
        const TYPE* row_ptr = input.ptr<TYPE>(y);
        for (int x = 0; x < input.cols; ++x)
        {
            TYPE value = row_ptr[x];
            if (std::find(out.begin(), out.end(), value) == out.end())
                out.push_back(value);
        }
    }

    if (sort)
        std::sort(out.begin(), out.end()); //升序

    return out;
}

template <typename MATTYPE> //uchar, float
std::vector<std::vector<int>> Analysiser::np_where(const cv::Mat& img_gray, const MATTYPE& value)
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

/*
usage: 从cv::Mat中获取【前景】像素最大值or最小值or平均值作为此张mat的分数
输入:
    mask: 需要获取分数的cv::Mat, 像素值0-255，数据类型ushort
    mode: 分数计算方法，"avg", "max", "min"
    method: 获取方法，"points", "region", ""
    points: 指定在mask中获取分数的像素点坐标。为np_where的返回结果[[行索引][列索引]]
    region: 与mask同大小的cv::Mat，作为
    op: region与value的运算方法，"==", "!="
    value: region中的某个像素值

    return: float分数
*/
//float Analysiser::get_score_from_mat(cv::Mat mask, std::string mode, std::string method, std::vector<std::vector<int>> points, cv::Mat region, std::string op, int value)
//{
//    float score;
//    if (mode == "avg")
//    {
//        std::vector<int> all_pixels;
//    }
//    else
//    {
//        std::set<int> all_pixels;
//    }
//    int pixel;
//    if (method == "points") //在给定的点获取分数
//    {
//        for (int i = 0; i < points[0].size(); i++)
//        {
//            pixel = mask.ptr<ushort>(points[0][i])[points[1][i]];
//            if (pixel != 0)
//            {
//                auto ret = std::find(all_pixels.begin(), all_pixels.end(), pixel);
//                if (ret == all_pixels.end())
//                {
//                    all_pixels.push_back(pixel);
//                }
//            }
//        }
//
//        if (mode == "avg")
//        {
//            double sum = std::accumulate(std::begin(all_pixels), std::end(all_pixels), 0.0);
//            score = sum / all_pixels.size();
//        }
//        else if (mode == "max")
//        {
//            score = *std::max_element(all_pixels.begin(), all_pixels.end());
//        }
//        else if (mode == "min")
//        {
//            score = *std::min_element(all_pixels.begin(), all_pixels.end());
//        }
//        else
//        {
//            std::cout << "ERROR: unimplement mode.";
//        }
//    }
//    else if (method == "region") //在给定的区域获取分数
//    {
//        if (op == "==")
//        {
//            for (int row = 0; row < region.rows; row++)
//            {
//                for (int col = 0; col < region.cols; col++)
//                {
//                    pixel = region.ptr<ushort>(row)[col];
//                    if (pixel == value)
//                    {
//                        all_pixels.push_back(pixel);
//                    }
//                }
//            }
//        }
//        else if (op == "!=")
//        {
//
//        }
//        else
//        {
//            std::cout << "ERROR: unimplement op.";
//        }
//
//    }
//    else if (method == "") //在全图获取分数
//    {
//
//
//    }
//    else
//    {
//        std::cout << "ERROR: unimplement method.";
//    }
//
//
//
//}
