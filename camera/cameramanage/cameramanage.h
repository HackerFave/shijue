#pragma once
#include <map>
#include <vector>
#include "cameracomm.h"
#include "camerainterface.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <tchar.h>
#include "common.h"

typedef struct
{
    HMODULE hmod;
    FARPROC hmethod_1;      //GetCameraList获取相机列表方法
    CameraPROC hmethod_2;   //GetCamera获取指定相机实例
}DllHandle;

typedef std::map<cameratype, std::vector<ICamera*>> CameraMap;
typedef std::map < cameratype, std::map<std::string, ICamera*>> CameraSoMap;

class CCameraManage
{
private:
    CCameraManage();
    CCameraManage(const CCameraManage &) { }

    CCameraManage &operator=(const CCameraManage &) { return *this; }

    CameraMap cameramap_;
    CameraSoMap cameramapex_;
    CameraSoMap usecameramap_;
    std::map<cameratype, DevList> cameralist_;

    bool isopen_ = false;
    bool isstart_ = false;
    bool isautotriggerruning_ = false;
    int triggertime_ = 2000;
    ICamera* oncecamera_ = NULL;

    std::thread thread_;

    typedef std::map<cameratype, DllHandle> DLLMap;
    DLLMap dllmap_;

    std::function<void(const imginfo&)> callback_;

    
public:
    ~CCameraManage();
    //回调
    void CameraCallBack(const capinfo & img);

    //获得相机
    int GetCamera(cameratype type, std::string name, ICamera** camera);
    int GetCamera(cameratype type, int idx, ICamera** camera);
    //获得设备列表
    DevList GetDevicelist(cameratype type);
    //刷新设备列表，重载设备
    DevList RefreshList(cameratype type);
    //载入使用设备
    void LoadBeUseCameras(CameraSoMap map);

    bool LoadCameraLibrary(cameratype type);

    void Run();
    
    void RegisterCallback(std::function<void(const imginfo&)> callback) {
        callback_ = callback;
    }
    static CCameraManage *Get()
    {
        static CCameraManage instance;
        return &instance;
    }

    static void Initialize(std::function<void(const imginfo&)> callback) {
        auto cam = CCameraManage::Get();
        cam->RegisterCallback(callback);
        auto list = cam->RefreshList(cameratype::CAMERA_BASLER);
        list = cam->RefreshList(cameratype::CAMERA_HIK);
        list = cam->RefreshList(cameratype::CAMERA_INVENTED);
        list = cam->RefreshList(cameratype::CAMERA_FILE);
    }

    static int Open() {
        auto cam = CCameraManage::Get();
        if (cam->isopen_) {
            return -1;
        }
        for (auto it = cam->cameramapex_.begin();
            it != cam->cameramapex_.end(); it++)
        {
            for (auto i = it->second.begin();
                i != it->second.end(); i++)
            {
                i->second->Open();
            }
        }
        if (cam->cameramapex_.size() > 0) {
            cam->isopen_ = true;
        }
        return 0;
    }

    static int Close() {
        Stop();
        auto cam = CCameraManage::Get();
        if (!cam->isopen_) {
            return -1;
        }
        for (auto it = cam->cameramapex_.begin();
            it != cam->cameramapex_.end(); it++)
        {
            for (auto i = it->second.begin();
                i != it->second.end(); i++)
            {
                i->second->Close();
            }
        }
        cam->isopen_ = false;
        return 0;
    }
    
    static int StartAutoTrigger() {
        auto cam = CCameraManage::Get();
        int ret = 0;
        if (!cam->isopen_) {
            ret = Open();
        }
        //if ((!cam->isstart_)&&(ret == 0)) {
            ret = Start();
        //}
        if ((!cam->isautotriggerruning_) && (ret == 0)) {
            cam->isautotriggerruning_ = true;
            cam->thread_ = std::thread(&CCameraManage::Run, cam);           
        }
        return ret;
    }

    static int Start() {
        auto cam = CCameraManage::Get();
        //if (cam->isstart_) {
        //    return -1;
        //}
        for (auto it = cam->usecameramap_.begin();
            it != cam->usecameramap_.end(); it++)
        {
            for (auto i = it->second.begin();
                i != it->second.end(); i++)
            {
                i->second->RegisterCallback([cam](const capinfo& img) {
                    cam->CameraCallBack(img);
                });
                i->second->Start();
            }
        }
        if (cam->usecameramap_.size() > 0) {
            cam->isstart_ = true;
        }
        return 0;
    }

    static int StopAutoTrigger() {
        auto cam = CCameraManage::Get();
        if (!cam->isstart_) {
            return -1;
        }
        if (cam->isautotriggerruning_) {
            cam->isautotriggerruning_ = false;
            cam->thread_.join();
        }
        return 0;
    }

    static int Stop() {
        StopAutoTrigger();
        auto cam = CCameraManage::Get();
        if (!cam->isstart_) {
            return -1;
        }
        for (auto it = cam->usecameramap_.begin();
            it != cam->usecameramap_.end(); it++)
        {
            for (auto i = it->second.begin();
                i != it->second.end(); i++)
            {
                i->second->Stop();
            }
        }
        cam->isstart_ = false;
        return 0;
    }

    static void OnceTriggerStop();

    static void OnceTrigger(std::function<void(const capinfo&)> callback, int type, int index);

    static void LoadUseCameras(void* map);

    static void LoadUseCameras(CameraSoMap map) {
        auto cam = CCameraManage::Get();
        cam->LoadBeUseCameras(map);
    }

    static void ReLoadAllDevices() {
        for (int i = 0; i < (int)cameratype::CAMERA_COUNT; i++)
        {
            EnumerateDevices((cameratype)i);
        }
    }
    
    static DevList EnumerateALLDevices(){
        DevList list;
        auto cam = CCameraManage::Get();
        for (int i = 0; i < (int)cameratype::CAMERA_COUNT; i++)
        {
            if (i == (int)cameratype::CAMERA_BASLER) {
                continue;
            }
            DevList temp;
            temp = cam->GetDevicelist((cameratype)i);
            list.insert(list.end(), temp.begin(), temp.end());
        }
        return list;
    }

    //获得当前设备列表
    static std::vector<std::string> EnumerateCurrentDevices(cameratype type) {
        std::vector<std::string> vec;
        vec.clear();
        auto cam = CCameraManage::Get();
        if (cam->cameralist_.count(type) > 0) {
            auto list = cam->cameralist_.at(type);
            for (auto it = list.begin(); it != list.end(); it++)
            {
                vec.push_back(it->chDevName);
            }
        }
        return vec;
    }


    
    static std::vector<std::string> EnumerateDevices(cameratype type) {
        Close();
        auto cam = CCameraManage::Get();
        auto list = cam->RefreshList(type);
        std::vector<std::string> rlist;
        for (auto it = list.begin(); it != list.end(); it++)
        {
            rlist.push_back(it->chDevName);
        }
        return rlist;
    }

    static int Trigger() {
        auto cam = CCameraManage::Get();
        if (!cam->isstart_) {
            return -1;
        }
        for (auto it = cam->usecameramap_.begin();
            it != cam->usecameramap_.end(); it++){
            for (auto i = it->second.begin();
                i != it->second.end(); i++) {
                i->second->Trigger();
            }
        }
        return 0;
    }

    static void Deinitialize(){
        Close();
    }

    static bool CameraIsStart() {
        auto cam = CCameraManage::Get();
        return cam->isstart_;
    }

    static bool CameraIsOpen() {
        auto cam = CCameraManage::Get();
        return cam->isopen_;
    }
};
