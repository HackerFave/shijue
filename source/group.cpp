#include <iostream>
#include <string>
#include <filesystem>

#include "opencv2/opencv.hpp"

#include "common.h"
#include "log.h"
#include "solution.h"
#include "group.h"
#include "plcinterface.h"

namespace group {
	class GroupService {
	public:
		static GroupService& instance() {
			static GroupService the_instance;
			return the_instance;
		}

	private:
		GroupService() = default;

	public:
		bool deinitialize() {
			log_info("Enter\n");
			stop();
			log_info("Exit\n");
			return true;
		};

		bool initialize(/*std::function<void(imginfo&)> callback*/) {
			log_info("Enter\n");
			running_ = false;
			device_to_imgque.clear();
			group_.clear();
			log_info("Exit\n");
			return true;
		};

		bool start() {
			log_info("Enter\n");
			std::lock_guard<std::mutex> lock(mutex_);
			device_to_imgque.clear();
			group_.clear();
			if (running_) {
				log_detail("Already start\n");
				log_info("Exit\n");
				return true;
			}

			running_ = true;
			thread_ = std::thread(&GroupService::run, this);
			log_info("Exit\n");
			return true;
		}

		void stop() {
			log_info("Enter\n");
			std::lock_guard<std::mutex> lock(mutex_);
			if (!running_) {
				log_detail("Already stoped\n");
				log_info("Exit\n");
				return;
			}

			running_ = false;
			condvar_.notify_one();
			mutex_.unlock();
			thread_.join();
			mutex_.lock();
			log_info("Exit\n");
		}
		void push(const imginfo & img) {
			imginfo tmpimginfo = img;
			int tmpstationid = tmpimginfo.getstationid();
			image_mutex_.lock();
			device_to_imgque[tmpstationid].push_back(tmpimginfo);
			image_mutex_.unlock();
			group_mutex_.lock();
			int groupid = sln::getgroupid(tmpstationid);
			group_.push_back(groupid);
			group_mutex_.unlock();
			condvar_.notify_one();
            std::cout << "INFO: device_to_imgque[tmpstationid].size" << device_to_imgque[tmpstationid].size() << std::endl;
		}
	private:
		void run() {
			log_info("Enter\n");
			std::unique_lock<std::mutex> lock(mutex_);
			cv::Mat mat;

			while (running_) {
				if (group_.empty()) {
					condvar_.wait_for(lock, std::chrono::milliseconds(500));
					continue;
				}

				group_mutex_.lock();
				int groupid = group_.front();
				group_.pop_front(); 
				group_mutex_.unlock();

				std::vector<int> stationlist = sln::getstationlist(groupid);
				bool readyflag = 1;
				image_mutex_.lock();
				for (int i=0; i< stationlist.size(); i++){
					if (!sln::isused(stationlist[i])) {
						return;
					}
					if (device_to_imgque[stationlist[i]].empty()) {
						readyflag = 0;
						continue;
					}
				}

				if (readyflag) {
                    int ngtype = 0;
					for (int i = 0; i < stationlist.size(); i++) {
						if (!sln::isused(stationlist[i])) {
							continue;
						}
						auto begin = device_to_imgque[stationlist[i]].front();
						device_to_imgque[stationlist[i]].pop_front();
						begin.setdestroytime();
						sln::set_lifetime(begin.getstationid(), (begin.getdestroytime() - begin.getcapturetime()).count()/1000000);
                        ngtype = ngtype | begin.getresulttype();
					}

                    if (ngtype & (1 << plc::kNgMeasure)) {
                        sln::settotalng(true);
                    }
                    else if (ngtype & (1 << plc::kNgAppearance)) {
                        sln::settotalng(true);
                    }
                    else if (ngtype & (1 << plc::kNgOther)) {
                        sln::settotalng(true);
                    }
                    else if (ngtype & (1 << plc::kRetry)) {
                        sln::settotalng(true);
                    }
					else {
					}

				}
				image_mutex_.unlock();
			}
			log_info("Exit\n");
		}
	private:
		std::function<void(imginfo&)> callback_;
		std::mutex mutex_;
		std::condition_variable condvar_;
		std::thread thread_;
		bool running_;

		std::mutex image_mutex_;
		std::map<int, std::deque<imginfo> > device_to_imgque;
		std::mutex group_mutex_;
		std::deque<int> group_;
	};

	bool deinitialize(/*std::function<void(imginfo&)> callback*/) {
		return GroupService::instance().deinitialize(/*callback*/);
	};

	bool initialize(/*std::function<void(imginfo&)> callback*/) {
		return GroupService::instance().initialize(/*callback*/);
	};

	bool start() {
		return GroupService::instance().start();
	}

	void stop() {
		GroupService::instance().stop();
	}

	void push(const imginfo & img) {
		GroupService::instance().push(img);
	}
}