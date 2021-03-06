#include "engine.h"

namespace engine {
    class EngineService {
    public:
        static EngineService& instance() {
            static EngineService the_instance;
            return the_instance;
        }

    private:
        EngineService() = default;

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
            running_ = true;
            thread_ = std::thread(&EngineService::run, this);
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
        
        int pushvisible(const capinfo & img) {
            visible_mutex_.lock();
            visible_.push_back(img);
            visible_mutex_.unlock();
            condvar_.notify_one();
            return AINNOSUCCESS;
        }

        int pushinfrare(const capinfo & img) {
            infrare_mutex_.lock();
            infrare_.push_back(img);
            infrare_mutex_.unlock();
            condvar_.notify_one();
            return AINNOSUCCESS;
        }

        int pushdetect(const imginfo & img) {
            detect_mutex_.lock();
            detect_.push_back(img);
            detect_mutex_.unlock();
            condvar_.notify_one();
            return AINNOSUCCESS;
        }

        int pushmeasure(const imginfo & img) {
            measure_mutex_.lock();
            measure_.push_back(img);
            measure_mutex_.unlock();
            condvar_.notify_one();
            return AINNOSUCCESS;
        }

        int registercallback(callbacktype type, std::function<void(const imginfo&)> callback){
            log_info("Enter\n");
            callbacks_[type] = callback;
            log_info("Exit\n");
            return AINNOSUCCESS;
        }

    private:
        int getinfrareidx(capinfo& visible) {
            if (infrare_.size() == 0) {
                return -1;
            }

            int idx = int(infrare_.size());
            for (int i = 0; i < infrare_.size(); i++) {
                if (infrare_[i].m_capturetime > visible.m_capturetime) {
                    idx = i;
                    break;
                }
            }

            //删除旧的，最多只保留一个早于当前时间的
            for (int j = idx-2; j >= 0; j--) {
                std::vector<capinfo>::iterator it = infrare_.begin() + j;
                infrare_.erase(it);
            }

            return 0;
        }
        void run() {
            std::unique_lock<std::mutex> lock(mutex_);

            while (running_) {
                //如果有可见光图片，则发送给检测模块
                if (!visible_.empty() && !infrare_.empty()){
                    visible_mutex_.lock();
                    auto begin = visible_.front();
                    visible_.pop_front();
                    visible_mutex_.unlock();

                    int idx = getinfrareidx(begin);
                    imginfo img;
                    img.visibleimg = begin;
                    img.infrareimg = visible_[idx];
                    callbacks_[CALLBACK_DETECT](img);
                }

                //如果检测模块有结果，则查找时间戳最近的红外图片，一起送给测量模块
                if (!detect_.empty()){
                    detect_mutex_.lock();
                    auto begin = detect_.front();
                    detect_.pop_front();
                    detect_mutex_.unlock();
                    
                    callbacks_[CALLBACK_MEASURE](begin);
                }
                
                //如果测量模块有结果，则送给UI
                if (!measure_.empty()){
                    measure_mutex_.lock();
                    auto begin = measure_.front();
                    measure_.pop_front();
                    measure_mutex_.unlock();
                    
                    callbacks_[CALLBACK_UI](begin);
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
        
        //可见光图片，输入队列
        std::deque<capinfo> visible_;
        std::mutex visible_mutex_;
        
        //红外图片，输入队列
        std::vector<capinfo> infrare_;
        std::mutex infrare_mutex_;
        
        //检测结果，输入队列
        std::deque<imginfo> detect_;
        std::mutex detect_mutex_;
        
        //温度测量结果，输入队列
        std::deque<imginfo> measure_;
        std::mutex measure_mutex_;
        
        std::map<callbacktype, std::function<void(const imginfo&)> > callbacks_;
    };

    int deinitialize(void) {
        return EngineService::instance().deinitialize();
    };

    int initialize(void) {
        return EngineService::instance().initialize();
    };

    int start(void) {
        return EngineService::instance().start();
    };

    int stop(void) {
        return EngineService::instance().stop();
    };

    int pushvisible(const capinfo & img) {
        return EngineService::instance().pushvisible(img);
    }

    int pushinfrare(const capinfo & img) {
        return EngineService::instance().pushinfrare(img);
    }

    int pushdetect(const imginfo & img) {
        return EngineService::instance().pushdetect(img);
    }

    int pushmeasure(const imginfo & img) {
        return EngineService::instance().pushmeasure(img);
    }

    int registercallback(callbacktype type, std::function<void(const imginfo&)> callback) {
        return EngineService::instance().registercallback(type, callback);
    }
}