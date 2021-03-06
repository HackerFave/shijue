#include "measure.h"

namespace measure {
    class MeasureService {
    public:
        static MeasureService& instance() {
            static MeasureService the_instance;
            return the_instance;
        }

    private:
        MeasureService() = default;

    public:
        int deinitialize(void) {
            log_info("Enter\n");
            running_ = false;
            condvar_.notify_one();
            thread_.join();
            log_info("Exit\n");
            return AINNOSUCCESS;
        }
        
        int initialize(void) {
            log_info("Enter\n");
            temperaturethr_ = (float)37.3;
            running_ = true;
            thread_ = std::thread(&MeasureService::run, this);
            log_info("Exit\n");
            return AINNOSUCCESS;
        };

        int start(void) {
            log_info("Enter\n");
            log_info("Exit\n");
            return AINNOSUCCESS;
        };

        int stop(void) {
            log_info("Enter\n");
            log_info("Exit\n");
            return AINNOSUCCESS;
        };
        
        int push(const imginfo & img) {
            inque_mutex_.lock();
            inque_.push_back(img);
            inque_mutex_.unlock();
            condvar_.notify_one();
            return AINNOSUCCESS;
        }

        int registercallback(std::function<void(const imginfo&)> callback){
            log_info("Enter\n");
            callback_ = callback;
            log_info("Exit\n");
            return AINNOSUCCESS;
        }

        int settemperaturethr(float thr) {
            temperaturethr_ = thr;
            return AINNOSUCCESS;
        }
    private:
        float gettemperature(cv::Mat mat) {
            if (0) {
                double meanVal;
                cv::Scalar tempVal = cv::mean(mat);
                meanVal = tempVal.val[0];
                return float(meanVal);
            }
            else {
                double maxVal, minVal;
                cv::minMaxLoc(mat, &minVal, &maxVal);
                return float(maxVal);
            }
        }

        void run() {
            std::unique_lock<std::mutex> lock(mutex_);

            while (running_) {
                if (!inque_.empty()){
                    inque_mutex_.lock();
                    auto begin = inque_.front();
                    inque_.pop_front();
                    inque_mutex_.unlock();
                    
                    for (int i = 0; i < begin.faceinfos.size(); i++) {
                        faceinfo *pfaceinfo = &begin.faceinfos[i];

                        //可见光区域对应到红外区域
                        float scalex = (float)(begin.infrareimg.img.cols / begin.visibleimg.img.cols);
                        float scaley = (float)(begin.infrareimg.img.rows / begin.visibleimg.img.rows);
                        int x = (int)(pfaceinfo->x * scalex);
                        int y = (int)(pfaceinfo->y * scaley);
                        int w = (int)(pfaceinfo->w * scalex);
                        int h = (int)(pfaceinfo->h * scaley);

                        //测量温度
                        cv::Mat facerect = begin.infrareimg.temprature(cv::Rect(x, y, w, (int)(h / 3)));
                        pfaceinfo->temperature = gettemperature(facerect);
                        if (pfaceinfo->temperature > temperaturethr_) {
                            pfaceinfo->bfever = true;
                        }
                        else {
                            pfaceinfo->bfever = false;
                        }
                    }
                    
                    callback_((const imginfo &)begin);
                }
                
                condvar_.wait_for(lock, std::chrono::milliseconds(10));
            }
        }
        
    private:
        //线程信息
        std::mutex mutex_;
        std::condition_variable condvar_;
        std::thread thread_;
        bool running_ = true;
        
        //输入队列
        std::deque<imginfo> inque_;
        std::mutex inque_mutex_;
        
        std::function<void(const imginfo&)> callback_;

        float temperaturethr_;
    };

    int deinitialize(void) {
        return MeasureService::instance().deinitialize();
    };

    int initialize(void) {
        return MeasureService::instance().initialize();
    };

    int start(void) {
        return MeasureService::instance().start();
    };

    int stop(void) {
        return MeasureService::instance().stop();
    };

    int push(const imginfo & img) {
        return MeasureService::instance().push(img);
    }

    int registercallback(std::function<void(const imginfo&)> callback) {
        return MeasureService::instance().registercallback(callback);
    }

    int settemperaturethr(float thr) {
        return MeasureService::instance().settemperaturethr(thr);
    }
}