#include "solution.h"
#include "cameramanage.h"
CCameraManage::CCameraManage()
{
    usecameramap_.clear();
    cameramap_.clear();
    cameramapex_.clear();
    dllmap_.clear();
    cameralist_.clear();
    LoadCameraLibrary(cameratype::CAMERA_FILE);
    LoadCameraLibrary(cameratype::CAMERA_HIK);
    LoadCameraLibrary(cameratype::CAMERA_INVENTED);
    //LoadCameraLibrary(cameratype::CAMERA_BASLER);
}

void CCameraManage::CameraCallBack(const capinfo & img)
{
    int tmpstationid = sln::getstationid(img.cameraid, img.frameid % sln::getvirtstationmax(img.cameraid));
    if (tmpstationid < 0) {
        return;
    }
    if (!sln::isused(tmpstationid)) {
        return;
    }

    static int gid = 0;
    imginfo tmpimginfo(img.img);
    tmpimginfo.setframeid(img.frameid / sln::getvirtstationmax(img.cameraid));
    tmpimginfo.setid(gid++);
    tmpimginfo.setcapturetime();
    tmpimginfo.setstationid(tmpstationid);
    tmpimginfo.setalg(sln::getalgs(tmpstationid));
	tmpimginfo.setcapturelength();

    if (img.img_aux.data != NULL) {
        tmpimginfo.setauximg(img.img_aux);
    }

    callback_(tmpimginfo);
}

CCameraManage::~CCameraManage()
{
    dllmap_.clear();

    std::vector<ICamera*> vcam ;
    for (auto it = cameramap_.begin(); it != cameramap_.end(); it++)
    {
        vcam = it->second;
        for (int i = 0; i < vcam.size(); i++)
        {
            if (vcam[i] != NULL) {
                delete vcam[i];
                vcam[i] = NULL;
            }
        }
        vcam.clear();
    }
    cameramap_.clear();
    usecameramap_.clear();
    cameramapex_.clear();
}

int CCameraManage::GetCamera(cameratype type,int idx, ICamera** camera)
{
    int ret = -1;
    auto it = cameramap_.find(type);
    if (it != cameramap_.end()) {
        if (idx >= 0 && idx < it->second.size()) {
            *camera = it->second.at(idx);
            ret = 0;
        }
    }
    return ret;
}

int CCameraManage::GetCamera(cameratype type, std::string name, ICamera** camera)
{
    int ret = -1;
    if (cameramapex_.count(type) > 0) {
        if (cameramapex_[type].count(name) > 0) {
            *camera = (cameramapex_[type])[name];
            ret = 0;
        }
    }
    return ret;
}

DevList CCameraManage::GetDevicelist(cameratype type)
{
    if (dllmap_.count(type) < 1) {
        return DevList();
    }
    else {
        DevList* devlist = (DevList*)dllmap_[type].hmethod_1();
        return *devlist;
    }
}

DevList CCameraManage::RefreshList(cameratype type)
{
    //载入map设备表中
    DevList devlist = GetDevicelist(type);
    cameralist_[type] = devlist;
    if (devlist.size() < 1) {
        std::cout << "camera dev get fail:" << type << std::endl;
        return DevList();
    }
    auto dllhand = dllmap_[type];
    std::vector<ICamera*> tempvec;
    cameramapex_[type].clear();
    std::string name;
    for (int i = 0; i < devlist.size(); i++)
    {
        ICamera* camera = (ICamera*)dllhand.hmethod_2((void*)&(devlist.at(i)));
        tempvec.push_back(camera);
        name = devlist.at(i).chDevName;

        if (name != "") {
            (cameramapex_[type])[name] = camera;
        }
    }
    for (int it = 0; it < cameramap_[type].size(); it++)
    {
        auto temp = cameramap_[type].at(it);
        if (temp != NULL) {
            delete temp;
            temp = NULL;
        }
    }
    
    cameramap_[type] = tempvec;
    return devlist;
}

void CCameraManage::LoadBeUseCameras(CameraSoMap map)
{
    usecameramap_ = map;
}

bool CCameraManage::LoadCameraLibrary(cameratype type)
{
    DllHandle dllhand ;
    if(dllmap_.count(type) < 1) {
        //todo 遍历目录比较好
        switch (type)
        {
        case cameratype::CAMERA_FILE:
            dllhand.hmod = LoadLibrary(_TEXT("camerafilewatch.dll"));
            break;
        case cameratype::CAMERA_INVENTED:
            dllhand.hmod = LoadLibrary(_TEXT("cameralocal.dll"));
            break;
        case cameratype::CAMERA_BASLER:
            dllhand.hmod = LoadLibrary(_TEXT("camerabasler.dll"));
            break;
        case cameratype::CAMERA_HIK:
            dllhand.hmod = LoadLibrary(_TEXT("camerahik.dll"));
            break;
        default:
            return false;
        }
        if (NULL == dllhand.hmod) {
            std::cout << "camera dll load fail:" << type << std::endl;
            return false;
        }
        //函数地址
        dllhand.hmethod_1 = GetProcAddress(dllhand.hmod, "GetCameraList");
        dllhand.hmethod_2 = (CameraPROC)GetProcAddress(dllhand.hmod, "GetCamera");
    }
    else{
        dllhand = dllmap_[type];
    }

    dllmap_[type] = dllhand;
    return true;
}

void CCameraManage::Run()
{
    int t = triggertime_;
    while (isautotriggerruning_)
    {
        Trigger();
        std::cout << "trigger" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(t));
    }
}

void CCameraManage::OnceTriggerStop()
{
    ICamera *cam = CCameraManage::Get()->oncecamera_;
    if (cam != NULL) {
        cam->Stop();
        cam = NULL;
    }
}
void CCameraManage::OnceTrigger(std::function<void(const capinfo&)> callback, int type, int index)
{
    ICamera *cam;
    CCameraManage::Get()->GetCamera((cameratype)type, index, &cam);
    CCameraManage::Get()->oncecamera_ = cam;
    cam->RegisterCallback(callback);
    cam->Start();
    cam->Trigger();

}

void CCameraManage::LoadUseCameras(void * lmap)
{
    std::map<int, Solution_Camera> *smap = static_cast<std::map<int, Solution_Camera>*>(lmap);
    ICamera *cam;
    CameraSoMap map;
    std::map<std::string, ICamera*> *cammap;

    for (auto it = smap->begin(); it != smap->end(); it++)
    {
        cameratype type = (cameratype)it->second.get_camtype();
        if (map.count(type) > 0) {
            cammap = &map[type];
        }
        else {
            cammap = new std::map<std::string, ICamera*>();
        }

        std::string name = it->second.get_cameraindex().toLocal8Bit();

        int ret = CCameraManage::Get()->GetCamera(type, name, &cam);

        if (ret == 0) {
            (*cammap)[name] = cam;

            if (map.count(type) < 1) {
                map[type] = *cammap;
                delete cammap;
            }
        }
        else if (map.count(type) < 1) {
            delete cammap;
        }
    }

    CCameraManage::LoadUseCameras(map);
}
