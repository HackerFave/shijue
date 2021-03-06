#include "detect.h"

namespace detect {
    class DetectService {
    public:
        static DetectService& instance() {
            static DetectService the_instance;
            return the_instance;
        }

    private:
        DetectService() = default;

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
            thread_ = std::thread(&DetectService::run, this);
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

    private:
        void run() {
            std::unique_lock<std::mutex> lock(mutex_);

            while (running_) {
                if (!inque_.empty()){
                    inque_mutex_.lock();
                    auto begin = inque_.front();
                    inque_.pop_front();
                    inque_mutex_.unlock();
                    
                    //TODO
                    
                    
                    callback_(begin);
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
    };

    int deinitialize(void) {
        return DetectService::instance().deinitialize();
    };

    int initialize(void) {
        return DetectService::instance().initialize();
    };

    int start(void) {
        return DetectService::instance().start();
    };

    int stop(void) {
        return DetectService::instance().stop();
    };

    int push(const imginfo & img) {
        return DetectService::instance().push(img);
    }

    int registercallback(std::function<void(const imginfo&)> callback) {
        return DetectService::instance().registercallback(callback);
    }
}