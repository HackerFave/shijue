#include "common.h"
#include "camerafactory.h"

namespace camera {
    class CameraService {
    public:
        static CameraService& instance() {
            static CameraService the_instance;
            return the_instance;
        }

    private:
        CameraService() = default;

    public:
        int deinitialize(void) {
            cameras_[CAMERA_VISIBLE]->deinitialize();
            cameras_[CAMERA_INFRARE]->deinitialize();
            return AINNOSUCCESS;
        }

        int initialize(void) {
            cameras_[CAMERA_VISIBLE] = CCameraFactory::Get()->CreateCamera("Visible");
            cameras_[CAMERA_VISIBLE]->initialize();
            cameras_[CAMERA_INFRARE] = CCameraFactory::Get()->CreateCamera("Infrare");
            cameras_[CAMERA_INFRARE]->initialize();
            return AINNOSUCCESS;
        }

        int start(void) {
            cameras_[CAMERA_VISIBLE]->start();
            cameras_[CAMERA_INFRARE]->start();
            return AINNOSUCCESS;
        }

        int stop(void) {
            cameras_[CAMERA_VISIBLE]->stop();
            cameras_[CAMERA_INFRARE]->stop();
            return AINNOSUCCESS;
        }

        int registercallback(cameratype type, std::function<void(const capinfo&)> callback) {
            callbacks_[type] = callback;
            cameras_[type]->registercallback(callback);
            return AINNOSUCCESS;
        }
        
    private:
        std::map<cameratype,ICamera*> cameras_;
        std::map<cameratype, std::function<void(const capinfo&)> > callbacks_;
    };

    int deinitialize(void) {
        return CameraService::instance().deinitialize();
    }

    int initialize(void) {
        return CameraService::instance().initialize();
    }

    int start(void) {
        return CameraService::instance().start();
    }

    int stop(void) {
        return CameraService::instance().stop();
    }

    int registercallback(cameratype type, std::function<void(const capinfo&)> callback) {
        return CameraService::instance().registercallback(type, callback);
    }
}