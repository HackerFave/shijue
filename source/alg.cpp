#include <iostream>
#include <thread>
#include <io.h> //access
#include <direct.h> //mkdir
#include "windows.h"
#include <tchar.h>
#include <opencv2/opencv.hpp>
#include "common.h"
#include "log.h"
#include "alg.h"
#include "solution.h"
#include "baseparam.h"
#include "debugdata.h"
#include "uicommon.h"
#include "qsettings.h"
#include "qcoreapplication.h"
#include "qfileinfo.h"
#include "qdir.h"
//#include <qmessagebox.h>
#include "ciwamsgdialog.h"
#ifdef DEBUGTIME
#include "proc.h"
#endif
#ifdef USE_CUDA
#include <cuda_runtime.h>
#define USE_MANUVISION
#endif
#include "plcinterface.h"
#include "threshold.h"
#include "wantusblight.h"
#include "postprocess.h"
#include <QtConcurrent>

typedef void *HANDLE;
typedef struct
{
	std::vector<cv::Mat> result_imgs;
	std::vector<bool> isok;
	std::vector<float> score_per_class;
	void *private_data;
}SResult;

std::map<int, std::pair<std::string, cv::Scalar>> label_color_map_ = {
	{0, std::make_pair("bg", cv::Scalar(0, 0, 0))},         //黑色
	{1, std::make_pair("gansha", cv::Scalar(0, 0, 255))}, //蓝色，显示为红色
	{2, std::make_pair("kailie", cv::Scalar(0, 0, 255))},
	{3, std::make_pair("huangban", cv::Scalar(0, 0, 255))},
	{4, std::make_pair("xuhua", cv::Scalar(0, 0, 255))},
	{5, std::make_pair("wuwu", cv::Scalar(0, 0, 255))},
	{6, std::make_pair("bujun", cv::Scalar(0, 0, 255))},
	{7, std::make_pair("wanzhe", cv::Scalar(0, 0, 255))}
};

typedef HANDLE(_cdecl *LPINIT)(std::string algName, std::string configFileName, int gpu);
typedef SResult(_cdecl *LPINFERENCE)(HANDLE handle, const std::vector<cv::Mat> & images);
typedef void(_cdecl *LPUNINIT)(HANDLE handle);
typedef std::string(_cdecl *LPGETLASTERR)(HANDLE handle); 
typedef void(_cdecl *LPCLEARLASTERR)(HANDLE handle);
typedef void(_cdecl *LPINITTHREAD)(int num);
bool contourscomp(cv::InputArray a, cv::InputArray b) {
	return cv::contourArea(a) > cv::contourArea(b);
}
namespace alg {
	class AlgService {
	public:
		static AlgService& instance() {
			static AlgService the_instance;
			return the_instance;
		}

	public:
		AlgService() = default;

	public:
		void deinitialize() {
            if (!binitialized) {
                return;
            }
			log_info("Enter\n");
			running_ = false;
			condvar_.notify_one();
			thread_.join();

			for (auto i = 0; i < algthreads_.size(); ++i)
			{
				int idx = (i != 0);//0是CPU线程，其他都是GPU线程
				algcondvar_[idx].notify_one();
				algthreads_[i]->join();
			}
            algthreads_.clear();

            ::FreeLibrary(m_hmod);
            binitialized = false;
			if (postprocesser) {
				delete postprocesser;
			}
			log_info("Exit\n");
		}
		bool initialize(std::function<void(const imginfo&)> callback) {
            if (binitialized) {
                return true;
            }
			log_info("Enter\n");
			loadini();
#ifdef USE_CUDA
			cudaGetDeviceCount(&cudadevice);
#else
			cudadevice = 1;
#endif
			cudadevice = 1;
#ifdef USE_MANUVISION
			m_hmod = ::LoadLibrary(_TEXT("manuvision.dll"));
			if (NULL == m_hmod)
			{
				DWORD dwErr = ::GetLastError();
				log_error("Load manuvision.dll fail!!!\n");
				CiWaMsgDialog::MessageSignal("初始化算法失败", "加载manuvision.dll失败！");
				exit(1);
			}

			m_lpInit = (LPINIT)GetProcAddress(m_hmod, "Init");
			m_lpInference = (LPINFERENCE)GetProcAddress(m_hmod, "Inference");
			m_lpUnInit = (LPUNINIT)GetProcAddress(m_hmod, "UnInit");
			m_lpGetLastErr = (LPGETLASTERR)GetProcAddress(m_hmod, "GetLastErr");
			m_lpClearLastErr = (LPCLEARLASTERR)GetProcAddress(m_hmod, "ClearLastErr");
			m_lpInitThread = (LPINITTHREAD)GetProcAddress(m_hmod, "InitThread");

			if (NULL == m_lpInit || NULL == m_lpInference || NULL == m_lpUnInit) {
				log_error("GetProcAddress fail!!!\n");
				CiWaMsgDialog::MessageSignal("初始化算法失败", "GetProcAddress失败！");
				exit(1);
			}

#endif
			postprocesser = new PostProcesser();
			callback_ = callback;
			running_ = true;
			thread_ = std::thread(&AlgService::run, this);

			for (auto i = 0; i < 1 + cudadevice; ++i)
			{
				algthreads_.emplace_back(new std::thread(&AlgService::algrun, this, i));
			}

			std::string dstdir = "saveimg/";
			if (access(dstdir.c_str(), 0) == -1) {
				mkdir(dstdir.c_str());
			}

            binitialized = true;
			log_info("Exit\n");
			return true;
		};

		std::vector<std::string> enumerate() {
			return alglist;
		};

		void start() {
			log_info("Enter\n");
			time_t t = time(0);
			char ch[64];
			strftime(ch, sizeof(ch), "%Y-%m-%d %H-%M-%S", localtime(&t)); //年-月-日 时-分-秒
			imgsavedir = "saveimg/" + std::string(ch);
			if (access(imgsavedir.c_str(), 0) == -1) {
				mkdir(imgsavedir.c_str());
			}
            imgsavedir_all = "saveimg/" + std::string(ch) + "_all";
            if (access(imgsavedir_all.c_str(), 0) == -1) {
                mkdir(imgsavedir_all.c_str());
            }
			log_info("Exit\n");
		};

		void stop() {
			log_info("Enter\n");

			log_info("Exit\n");
		};

		void push(const imginfo & tmpimginfo) {
			image_mutex_.lock();
			image_.push_back(tmpimginfo);
			image_mutex_.unlock();
			condvar_.notify_one();
		};
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
		void registercallback(std::function<void(const imginfo&)> callback) {
			callback_ = callback;
		};
	
		//获取保存图像路径名
		std::string getimgsavedir() { return imgsavedir;}

        bool reloadmodel() {
            deinitialize();
            std::function<void(const imginfo&)> callback = callback_;
            initialize(callback);
            return true;
        }
        bool unloadmodel() {
            deinitialize();
            return true;
        }
	private:
		//contours降序排列
		static inline bool ContoursSortFun(std::vector<cv::Point> contour1, std::vector<cv::Point> contour2)
		{
			//return (contour1.size() > contour2.size());
			return (cv::contourArea(contour1) > cv::contourArea(contour2));
		}

		void run() {
			std::unique_lock<std::mutex> lock(mutex_);

			while (running_) {
				//查询是否有数据要处理，没有则休眠
				if (image_.empty() && result_.empty()) {
					condvar_.wait_for(lock, std::chrono::milliseconds(10));
					continue;
				}

				if (!image_.empty()){
					image_mutex_.lock();
					auto begin = image_.front();
					image_.pop_front();
					image_mutex_.unlock();
					//枚举用到的所有算法
					std::vector<std::string>&tmpalg = begin.getalg();
					for (auto iter = tmpalg.cbegin(); iter != tmpalg.cend(); iter++)
					{
						std::string curalg = *iter;
						std::string curalgtype = getalgtype(curalg);
						int queid;
						//如果使用GPU，提交一个数据到GPU队列，唤醒一个GPU线程去执行
						if ((curalgtype == "Segmentation") || (curalgtype == "Classification")) {
							queid = 1;
						}
						//如果不用GPU，提交一个数据到CPU队列，唤醒一个CPU线程去执行
						else {
							queid = 0;
						}
						imginfo tmpimg = begin;
						tmpimg.setcuralg(*iter);
						algmutex_[queid].lock();
						algimage_[queid].push_back(tmpimg);
						algmutex_[queid].unlock();
						algcondvar_[queid].notify_one();
					}

					if (tmpalg.size()){
						//保存已提交数据;
						submit_mutex_.lock();
						submit_.push_back(begin);
						submit_mutex_.unlock();
					}
					else{
						callback_(begin);
					}
				}

				if (!result_.empty()) {
					result_mutex_.lock();
					auto begin = result_.front();
					result_.pop_front();
					result_mutex_.unlock();
					callback_(begin);
				}
			}
		}

        int postprocess(cv::Mat maskimg, cv::Mat scoreimg, int stationid) {
            int resulttype = 0;
            double minVal;
            double maxVal;
            int minIdx;
            int maxIdx;
            cv::Scalar meanVal;
            std::vector<std::vector<cv::Point> > tmpcontours;
            std::vector<cv::Vec4i> tmphierarchy;
            cv::findContours(maskimg, tmpcontours, tmphierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
            for (int i = tmpcontours.size() - 1; i >= 0; i--){
                int defectid = maskimg.at<uchar>(tmpcontours[i][0]);

                int retval;
                threshold::thresholdval thrval;
                //像素面积
                retval = threshold::get(stationid, defectid, "area", &thrval);
                if (0 == retval) {
                    double area = cv::contourArea(tmpcontours[i]);
                    if (area > thrval.ng) {
                        resulttype = resulttype | plc::kNgAppearance;
                        return resulttype;
                    }
                    else if (area > thrval.ok) {
                        resulttype = resulttype | plc::kRetry;
                    }
                }
                
                //长
                retval = threshold::get(stationid, defectid, "long", &thrval);
                if (0 == retval) {
                    cv::RotatedRect rect = cv::minAreaRect(tmpcontours[i]);
                    int longside = std::max(rect.size.width, rect.size.height);
                    if (longside > thrval.ng) {
                        resulttype = resulttype | plc::kNgAppearance;
                        return resulttype;
                    }
                    else if (longside > thrval.ok) {
                        resulttype = resulttype | plc::kRetry;
                    }
                }

                //宽
                retval = threshold::get(stationid, defectid, "wide", &thrval);
                if (0 == retval) {
                    cv::RotatedRect rect = cv::minAreaRect(tmpcontours[i]);
                    int shortside = std::min(rect.size.width, rect.size.height);
                    if (shortside > thrval.ng) {
                        resulttype = resulttype | plc::kNgAppearance;
                        return resulttype;
                    }
                    else if (shortside > thrval.ok) {
                        resulttype = resulttype | plc::kRetry;
                    }
                }

                //平均得分
                retval = threshold::get(stationid, defectid, "meanscore", &thrval);
                if (0 == retval) {
                    double score;
                    cv::Rect minboundingRect = cv::boundingRect(tmpcontours[i]);
                    cv::Mat defectmat = cv::Mat(minboundingRect.height, minboundingRect.width, CV_8UC1, cv::Scalar(0));
                    cv::drawContours(defectmat, tmpcontours, i, cv::Scalar(1), -1, 8,
                    cv::noArray(), INT_MAX, cv::Point(-minboundingRect.x, -minboundingRect.y));
                    meanVal = cv::mean(scoreimg(minboundingRect), defectmat);
                    score = meanVal[0];
                    if (score > thrval.ng) {
                        resulttype = resulttype | plc::kNgAppearance;
                        return resulttype;
                    }
                    else if (score > thrval.ok) {
                        resulttype = resulttype | plc::kRetry;
                    }
                }

                //得分
                retval = threshold::get(stationid, defectid, "maxscore", &thrval);
                if (0 == retval) {
                    double score;
                    cv::Rect minboundingRect = cv::boundingRect(tmpcontours[i]);
                    cv::Mat defectmat = cv::Mat(minboundingRect.height, minboundingRect.width, CV_8UC1, cv::Scalar(0));
                    cv::drawContours(defectmat, tmpcontours, i, cv::Scalar(1), -1, 8,
                        cv::noArray(), INT_MAX, cv::Point(-minboundingRect.x, -minboundingRect.y));
                    cv::minMaxIdx(scoreimg, &minVal, &maxVal, &minIdx, &maxIdx, defectmat);
                    score = maxVal;
                    if (score > thrval.ng) {
                        resulttype = resulttype | plc::kNgAppearance;
                        return resulttype;
                    }
                    else if (score > thrval.ok) {
                        resulttype = resulttype | plc::kRetry;
                    }
                }
            }

			//if (tmpcontours>0)
			//{
			//	resulttype = resulttype | plc::kNgAppearance;
			//}
            return resulttype;
        }

		void algrun(int i) {
			log_info("thread %d Enter\n", i);
			int idx = (i != 0);//0是CPU线程，其他都是GPU线程
			std::unique_lock<std::mutex> lock(algmutex_[idx]);
			bool ngflag;
            int resulttype;
			std::vector<std::vector<cv::Point> > contours;
            std::map<int, std::vector<std::vector<cv::Point> >> allclass_contours;
			std::string errcode;
			
			std::map<std::string, std::vector<HANDLE>> alghandle;
#ifdef USE_MANUVISION
			m_lpInitThread(4);
			for (int j = 0; j < alglist.size(); j++) {
				log_info("thread %d Enter Init %s\n", i, alglist[j].c_str());
				HANDLE tmphandle;
				std::string algname = alglist[j];
				std::string algtype = getalgtype(algname);
				if (i == 0) {
					if ((algname == "SaveImg") || (algtype == "Segmentation") || (algtype == "Classification")) {
						continue;
					}
					tmphandle = m_lpInit(algtypelist[j], algxmllist[j], (i - 1));
					std::string errcode = m_lpGetLastErr(tmphandle);
					if (errcode != "")
					{
						m_lpClearLastErr(tmphandle);
						log_error("m_lpInit fail!!! %s\n", errcode.c_str());
						//QMessageBox::critical(NULL, "", QString(errcode.c_str()));
						CiWaMsgDialog::MessageSignal("运行错误", QString(errcode.c_str()));
						exit(1);
					}
					alghandle[algname].push_back(tmphandle);
				}
				else {
					if ((algtype != "Segmentation") && (algtype != "Classification")) {
						continue;
					}
					tmphandle = m_lpInit(algtypelist[j], algxmllist[j], (i - 1));
					errcode = m_lpGetLastErr(tmphandle);
					if (errcode != "")
					{
						m_lpClearLastErr(tmphandle);
						log_error("m_lpInference fail!!! %s\n", (algname + " " + errcode).c_str());
						//QMessageBox::critical(NULL, "m_lpInference", QString((algname + " " + errcode).c_str()));
						CiWaMsgDialog::MessageSignal("运行错误:m_lpInference", QString((algname + " " + errcode).c_str()));
						return;
					}
					alghandle[algname].push_back(tmphandle);
					std::vector<cv::Mat> tmpimages;
					cv::Mat m = cv::Mat::eye(1024, 1024, CV_8UC3);
					tmpimages.push_back(m);
					SResult output = m_lpInference(tmphandle, tmpimages);
					errcode = m_lpGetLastErr(tmphandle);
					if (errcode != "")
					{
						m_lpClearLastErr(tmphandle);
						log_error("m_lpInference fail!!! %s\n", (algname + " " + errcode).c_str());
						//QMessageBox::critical(NULL, "m_lpInference", QString((algname + " " + errcode).c_str()));
						CiWaMsgDialog::MessageSignal("运行错误:m_lpInference", QString((algname + " " + errcode).c_str()));
						return;
					}
				
				}
				log_info("thread %d Exit init %s\n", i, alglist[j].c_str());
			}
#endif

			while (running_) {
				//查询是否有数据要处理，没有则休眠
				if (algimage_[idx].empty()) {
					algcondvar_[idx].wait_for(lock, std::chrono::milliseconds(10));
					continue;
				}
				std::cout << "algimage_[idx]: " << idx << ": " << algimage_[idx].size() << std::endl;
                int64 ts = cv::getTickCount();
				//获取一个数据
				auto begin = algimage_[idx].front();
				algimage_[idx].pop_front();

				//从plc获取距离信息
				float plc_distance = begin.getcapturelength();

                if (begin.getworkstatus() == plcworkstatus::START)
                {
                    //检测，拼接等正常进行
                }
                else
                {
                    //状态复位
                    postprocesser->PLCEndAction(plc_distance);
                    //不检测。删除submit_
                    std::vector<imginfo>::iterator it = submit_.begin() + 0;
                    submit_.erase(it);

                    continue;
                }

				//开始处理
				algmutex_[idx].unlock();
				//log_info("thread %d Start %d %s\n", i, begin.getstationid(), begin.getcuralg().c_str());
				begin.setalgstarttime();
				std::string curalg = begin.getcuralg();
				std::string curalgtype = getalgtype(curalg);
				int tmpstationid = begin.getstationid();
				cv::Rect2f tmprect = sln::getroi(tmpstationid);
				ngflag = false;
                resulttype = 0;
				contours.clear();
                cv::Mat saveimg = begin.getimg().clone();
				if (curalg == "SaveImg") {
#ifdef DEBUGTIME
					QTime temp;
					temp.start();
#endif // DEBUGTIME
					int ret;
					imginfo tmpimg = begin;
					std::string dstdir = imgsavedir + "\\" + std::to_string(tmpstationid) + "\\";
					char buf[1024];
					sprintf(buf, "%08d_%08d.jpg", tmpstationid, tmpimg.getframeid());
					std::string filename = dstdir + buf;
					if (access(dstdir.c_str(), 0) == -1) {
						ret = mkdir(dstdir.c_str());
					}
					//cv::imwrite(filename, tmpimg.getimg());
                    //postprocesser->PushImwrite(filename, tmpimg.getimg());
#ifdef DEBUGTIME
					DebugPROC::EndTime(temp, TIMEID::T_SAVEIMG);
#endif // DEBUGTIME
				}
				else if ((curalgtype == "Segmentation") || (curalgtype == "Classification")) {
#ifdef DEBUGTIME
					QTime temp;
					temp.start();
#endif // DEBUGTIME
					std::vector<cv::Mat> tmpimages;
					auto tmpimg = begin.getimg();
					tmprect.x = int(tmprect.x * tmpimg.cols);
					tmprect.y = int(tmprect.y * tmpimg.rows);
					tmprect.width = int(tmprect.width * tmpimg.cols);
					tmprect.height = int(tmprect.height * tmpimg.rows);
#ifdef USE_WHOLEIMG
					tmpimages.push_back(tmpimg);
#else
					tmpimages.push_back(tmpimg(tmprect));
#endif
					int64 t1 = cv::getTickCount();
					SResult output = m_lpInference(alghandle[curalg][0], tmpimages);
					int64 t2 = cv::getTickCount();
					std::cout << "TIME: m_lpInference time " << (double)(t2 - t1) / cv::getTickFrequency() << " s" << std::endl;
					errcode = m_lpGetLastErr(alghandle[curalg][0]);
					if (errcode != "")
					{
						m_lpClearLastErr(alghandle[curalg][0]);
						log_error("m_lpInference fail!!! %s\n", (curalg + " " + errcode).c_str());
						//QMessageBox::critical(NULL, "m_lpInference", QString((curalg + " " + errcode).c_str()));
						CiWaMsgDialog::MessageSignal("运行错误:m_lpInference", QString((curalg + " " + errcode).c_str()));
						return;
					}
					if (curalgtype == "Segmentation") {
						std::vector<cv::Vec4i> hierarchy;
#ifdef USE_WHOLEIMG
						use_restore = output.result_imgs[0].size() == begin.getimg().size() ? true : false;
						if (!use_restore)
						{
							//resize roi rect
							tmprect.x = (tmprect.x / tmpimg.cols)*output.result_imgs[0].cols;
							tmprect.y = (tmprect.y / tmpimg.rows)*output.result_imgs[0].rows;
							tmprect.width = (tmprect.width / tmpimg.cols)*output.result_imgs[0].cols;
							tmprect.height = (tmprect.height / tmpimg.rows)*output.result_imgs[0].rows;

							//resize origin image
							int tmp_h = output.result_imgs[0].size().height;
							int tmp_w = output.result_imgs[0].size().width;
							cv::Mat tmp = begin.getimg();
							cv::resize(tmp, tmp, cv::Size(tmp_w, tmp_h));
							begin.setimg(tmp);
						}

                        cv::Mat maskimg = output.result_imgs[0](tmprect);
                        //cv::Mat scoreimg = output.result_imgs[1](tmprect);
#else
                        cv::Mat maskimg = output.result_imgs[0];
                        cv::Mat scoreimg = output.result_imgs[1];
#endif //USE_WHOLEIMG
                        //1.缺陷拼接；2.缺陷过滤；3.判断过滤后有缺陷；4.记入数据库及存图

                        int64 t_stitch = cv::getTickCount();
						postprocesser->DefectStitch(begin, output.result_imgs[0].clone(), plc_distance);
                        std::cout << "TIME: DefectStitch out " << (double)(cv::getTickCount() - t_stitch) / cv::getTickFrequency() << std::endl;
						
						//过滤，原地修改maskimg，返回保留的缺陷边缘alldefect_contours
                        int64 ts_ff = cv::getTickCount();
						allclass_contours = postprocesser->FilterFactory(maskimg, begin.getimg()(tmprect), THRESHOLDTYPE::ALARM);
						int64 te_ff = cv::getTickCount();
						std::cout << "TIME: FilterFactory out " << (double)(te_ff - ts_ff) / cv::getTickFrequency() << std::endl;

						if (allclass_contours.size() != 0)
						{
							resulttype = resulttype | (1 << plc::kNgAppearance);
							ngflag = true;
						}

						//cv::findContours(maskimg, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
						//resulttype = postprocess(maskimg, scoreimg, tmpstationid);
						//if (contours.size() > 0)
						//{
						//	resulttype = resulttype | (1 << plc::kNgAppearance);
						//	ngflag = true;
						//}
                        //if (0 != resulttype) {
                        //    ngflag = true;
                        //}
#ifdef DEBUGTIME
						DebugPROC::EndTime(temp, TIMEID::T_ALG_S_P);
#endif // DEBUGTIME
					}
					else if (curalgtype == "Classification")
					{
						if (!output.isok[0]) {
							ngflag = true;
						}
#ifdef DEBUGTIME
						DebugPROC::EndTime(temp, T_ALG_P);
#endif // DEBUGTIME
					}
				}
				else {
#ifdef DEBUGTIME
					QTime temp;
					temp.start();
#endif // DEBUGTIME

					std::vector<cv::Mat> tmpimages;
					auto tmpimg = begin.getimg();
					tmprect.x = int(tmprect.x * tmpimg.cols);
					tmprect.y = int(tmprect.y * tmpimg.rows);
					tmprect.width = int(tmprect.width * tmpimg.cols);
					tmprect.height = int(tmprect.height * tmpimg.rows);
					tmpimages.push_back(tmpimg(tmprect));
					SResult output = m_lpInference(alghandle[curalg][0], tmpimages);
					errcode = m_lpGetLastErr(alghandle[curalg][0]);
					if (errcode != "")
					{
						m_lpClearLastErr(alghandle[curalg][0]);
						log_error("m_lpInference fail!!! %s\n", (curalg + " " + errcode).c_str());
						//QMessageBox::critical(NULL, "m_lpInference ", QString((curalg+" "+errcode).c_str()));
						CiWaMsgDialog::MessageSignal("运行错误:m_lpInference", QString((curalg + " " + errcode).c_str()));
						return;
					}
					if (!output.isok[0]) {
                        if (algdefectmap[curalg] == "Measure") {
                            resulttype = resulttype | (1 << plc::kNgMeasure);
                        }
                        else {
                            resulttype = resulttype | (1 << plc::kNgAppearance);
                        }
						ngflag = true;
					}
#ifdef DEBUGTIME
					DebugPROC::EndTime(temp, T_ALG_F_B);
#endif // DEBUGTIME
				}
                int64 t_draw = cv::getTickCount();
				if (baseparam::get_autosave()) {
#ifdef DEBUGTIME
					QTime temp;
					temp.start();
#endif // DEBUGTIME
					PRODUCTTYPE savetype = baseparam::get_autosavetype();
					if (((BULIANGPIN == savetype) && ngflag) || ((LIANGPIN == savetype) && !ngflag)) 
                    {
						int ret;
						std::string dstdir = imgsavedir + "\\" + std::to_string(tmpstationid) + "\\";
						if (access(dstdir.c_str(), 0) == -1) {
							ret = mkdir(dstdir.c_str());
						}
						char buf[1024];
						sprintf(buf, "%s_%08d_%08d.jpg", (savetype==LIANGPIN ? "OK":"NG"), tmpstationid, begin.getframeid());
						std::string filename = dstdir + buf;

						if (autosavefilelist[tmpstationid].size() >= baseparam::get_autosavenum()) {
							auto tmpfilename = autosavefilelist[tmpstationid].front();
							ret = remove(tmpfilename.c_str());
							autosavefilelist[tmpstationid].erase(autosavefilelist[tmpstationid].begin());
						}
						//cv::imwrite(filename, saveimg/*, std::vector<int>{cv::IMWRITE_JPEG_QUALITY, 100}*/);
                        postprocesser->PushImwrite(filename, saveimg);
						autosavefilelist[tmpstationid].push_back(filename);
					}
                    //保存所有图片
                    int ret;
                    std::string dstdir = imgsavedir_all + "\\" + std::to_string(tmpstationid) + "\\";
                    if (access(dstdir.c_str(), 0) == -1) {
                        ret = mkdir(dstdir.c_str());
                    }
                    char buf[1024];
                    sprintf(buf, "%s_%08d_%08d.jpg", (ngflag == false ? "OK" : "NG"), tmpstationid, begin.getframeid());
                    std::string filename = dstdir + buf;
                    //cv::imwrite(filename, saveimg/*, std::vector<int>{cv::IMWRITE_JPEG_QUALITY, 100}*/);
                    postprocesser->PushImwrite(filename, saveimg);

#ifdef DEBUGTIME
					DebugPROC::EndTime(temp,TIMEID::T_AUTOSAVE);
#endif // DEBUGTIME
				}

#ifdef DEBUGTIME
				QTime temp;
				temp.start();
#endif // DEBUGTIME
				if (ngflag) 
                {
					std::map<int, std::vector<std::vector<cv::Point>>>::iterator iter;
					for (iter = allclass_contours.begin(); iter != allclass_contours.end(); iter++)
					{
						int class_id = iter->first;
						std::string label = label_color_map_[class_id].first;
						cv::Scalar color = label_color_map_[class_id].second;
						std::vector<std::vector<cv::Point>> curclass_contours = iter->second;
						//画缺陷轮廓
						if (use_restore)
						{
							cv::drawContours(begin.getimg()(tmprect), curclass_contours, -1, color, 15);
						}
						else
						{
							cv::drawContours(begin.getimg()(tmprect), curclass_contours, -1, color, 7);
						}
						
						for (int i_curclass = 0; i_curclass < curclass_contours.size(); i_curclass++)
						{
							//获取轮廓质心并写入类别
							cv::Moments moment = cv::moments(curclass_contours[i_curclass]);
							if (moment.m00 == 0)
							{
								//特征矩为0，不操作
							}
							else
							{
								cv::Point center = cv::Point(moment.m10 / moment.m00, moment.m01 / moment.m00);
								//cv::circle(begin.getimg()(tmprect), center, 15, color, -1);
								if (use_restore)
								{
									cv::putText(begin.getimg()(tmprect), label + std::to_string(i_curclass), center, cv::FONT_HERSHEY_SIMPLEX, 5, color, 7);
								}
								else
								{
									cv::putText(begin.getimg()(tmprect), label + std::to_string(i_curclass), center, cv::FONT_HERSHEY_SIMPLEX, 2, color, 3);
								}
							}
						}
					}

					auto tmpimg = begin.getimg();
					cv::rectangle(tmpimg, cv::Rect(0, 0, tmpimg.cols, tmpimg.rows), cv::Scalar(0, 0, 255), 5);
                    light::push(CMD_RED_OPEN);
				}
				else {
					auto tmpimg = begin.getimg();
					cv::rectangle(tmpimg, cv::Rect(0, 0, tmpimg.cols, tmpimg.rows), cv::Scalar(0, 255, 0), 5);
                    light::push(CMD_GREEN_OPEN);
				}
                int64 te_draw = cv::getTickCount();
#ifdef DEBUGTIME
				DebugPROC::EndTime(temp,TIMEID::T_NGFLAG);
#endif // DEBUGTIME
				//log_info("thread %d End %d %s\n", i, begin.getstationid(), begin.getcuralg().c_str());
                
				algmutex_[idx].lock();

				begin.setalgdonetime();
				
				//处理结束，把结果反馈给主线程
                int64 t_put = cv::getTickCount();
				putresult(begin, resulttype);
                std::cout << "TIME: alg draw " << (double)(te_draw - t_draw) / cv::getTickFrequency() << " second" << std::endl;
                std::cout << "TIME: putresult " << (double)(cv::getTickCount() - t_put) / cv::getTickFrequency() << " second" << std::endl;
				Sleep(0);
                std::cout << "TIME: alg one img " << (double)(cv::getTickCount() - ts) / cv::getTickFrequency() << std::endl;
                std::cout << "*********************************************************************************" << std::endl;
			}

#ifdef USE_MANUVISION
			for (auto item = alghandle.begin(); item != alghandle.end(); item++) {
				auto value = item->second;
				for (int j = 0; j < value.size(); j++) {
					m_lpUnInit(value[j]);
				}
			}
#endif
			log_info("thread %d Exit\n", i);
		}

		void putresult(imginfo& img, int resulttype) {
			//设置某一算法处理完毕标志
			std::string curalg = img.getcuralg();

			//判断这个数据是否所需算法都处理完毕
			submit_mutex_.lock();
			for (size_t i = 0; i < submit_.size(); i++) {
				if (submit_[i].getid() == img.getid())
				{
					submit_[i].setalgstarttime(img.getalgstarttime());
					submit_[i].setalgdonetime(img.getalgdonetime());
					submit_[i].setalgdone(curalg);
                    submit_[i].setresulttype(resulttype);
					if (!use_restore)
					{
						submit_[i].setimg(img.getimg());
					}
					if (0 != resulttype) {
						submit_[i].setNgflag(true);
					}
					if (submit_[i].isalgdone()) {
						if (submit_[i].getNgflag()) {
							sln::setdeviceng(submit_[i].getstationid(), 1);
						}
						else {
							sln::setdeviceng(submit_[i].getstationid(), 0);
						}
						sln::set_algtime(submit_[i].getstationid(), (submit_[i].getalgdonetime()- submit_[i].getalgstarttime()).count()/1000000);
						//处理完毕，调用回调函数
						result_mutex_.lock();
						result_.push_back(submit_[i]);
						result_mutex_.unlock();
						//唤醒主线程去处理
						condvar_.notify_one();
						std::vector<imginfo>::iterator it = submit_.begin() + i;
						submit_.erase(it);
					}
					break;
				}
			}
			submit_mutex_.unlock();
		}

		void loadini() {
			//QString path = QCoreApplication::applicationDirPath() + "/Model/alg.ini";
			QString path = "Model\\alg.ini";
			if (access(path.toStdString().c_str(), 0) == -1) {
				//QMessageBox::critical(NULL, "", path + " not exist!");
				CiWaMsgDialog::MessageSignal("加载错误:loadini", path + " not exist!");
				exit(1);
			}
			QSettings configini(path, QSettings::IniFormat);
			// 获取所有节点
			QStringList strlist = configini.childGroups();
			int icount = strlist.count();
			//获取每个节点的数据
			QStringList childlist;
			alglist.clear();
			algtypelist.clear();
			algxmllist.clear();
            algdefectmap.clear();
			for (int i = 0; i < icount; i++)
			{
				configini.beginGroup(strlist.at(i));
				QString name = configini.value("name").toString();
				if (name.size()) {
					alglist.push_back(name.toStdString());
				}
				else {
					alglist.push_back("");
				}
				QString type = configini.value("type").toString();
				if (type.size()) {
					algtypelist.push_back(type.toStdString());
				}
				else {
					algtypelist.push_back("");
				}
				QString xml = configini.value("xml").toString();
				if (xml.size()) {
					//xml = QCoreApplication::applicationDirPath() + "/Model/" + xml;
					xml = "Model\\" + xml;
					if (access(xml.toStdString().c_str(), 0) == -1) {
						//QMessageBox::critical(NULL, "", xml+" not exist!");
						CiWaMsgDialog::MessageSignal("加载错误:loadini", xml + " not exist!");
						exit(1);
					}
					algxmllist.push_back(xml.toStdString());
				}
				else {
					algxmllist.push_back("");
				}
                QString defect = configini.value("defect").toString();
                if (defect.size()) {
                    algdefectmap[type.toStdString()] = defect.toStdString();
                }
                else {
                    algdefectmap[type.toStdString()] = "";
                }
				configini.endGroup();
			}
		}

		std::string getalgtype(std::string algname) {
			std::vector <std::string>::iterator iElement = find(alglist.begin(),
				alglist.end(), algname);
			int idx = std::distance(alglist.begin(), iElement);
			return algtypelist[idx];
		}

	private:
		std::mutex mutex_;
		std::condition_variable condvar_;
		std::thread thread_;
		bool running_ = true;
		std::deque<imginfo> image_;
		std::mutex image_mutex_;
		std::deque<imginfo> result_;
		std::mutex result_mutex_;
		std::vector<imginfo> submit_;
		std::mutex submit_mutex_;
		std::function<void(const imginfo&)> callback_;
        bool binitialized = false;

		HINSTANCE m_hmod;
		LPINIT 			m_lpInit;
		LPINFERENCE		m_lpInference;
		LPUNINIT		m_lpUnInit;
		LPGETLASTERR	m_lpGetLastErr;
		LPCLEARLASTERR	m_lpClearLastErr;
		LPINITTHREAD	m_lpInitThread;
		//std::map<std::string, std::vector<HANDLE>> alghandle;

		std::vector<std::unique_ptr<std::thread> > algthreads_;
		std::deque<imginfo> algimage_[2];
		std::condition_variable algcondvar_[2];
		std::mutex algmutex_[2];
		int cudadevice;

		std::string imgsavedir;
		std::string imgsavedir_all;
		std::map<int, std::vector<std::string> > autosavefilelist;

		std::vector<std::string> alglist;
		std::vector<std::string> algtypelist;
		std::vector<std::string> algxmllist;
        std::map<std::string, std::string> algdefectmap;

		PostProcesser * postprocesser = NULL;
		bool use_restore;
	};

	bool deinitialize() {
		AlgService::instance().deinitialize();
		return true;
	};

	bool initialize(std::function<void(const imginfo&)> callback) {
		return AlgService::instance().initialize(callback);
	};

	std::vector<std::string> enumerate() {
		return AlgService::instance().enumerate();
	};

	void start() {
		return AlgService::instance().start();
	};

	void stop() {
		return AlgService::instance().stop();
	};

	void push(const imginfo & input) {
		AlgService::instance().push(input);
	}

	void registercallback(std::function<void(const imginfo&)> callback) {
		AlgService::instance().registercallback(callback);
	}

	//获取自动保存图像的路径名
	std::string getimgsavedir()
	{
		return AlgService::instance().getimgsavedir();
	}

    bool reloadmodel() {
        return AlgService::instance().reloadmodel();
    }

    bool unloadmodel() {
        return AlgService::instance().unloadmodel();
    }
}