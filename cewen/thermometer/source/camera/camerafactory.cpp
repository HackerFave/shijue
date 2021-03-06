#include "camerafactory.h"
#include "visible.h"
#include "infrare.h"
#include "WindowsCamera.h"
#include "seekwarecamera.h"

CCameraFactory::CCameraFactory()
{
    Register("Visible", &CCameraVisible::Create);
    Register("Infrare", &CCameraInfrare::Create);
    Register("Infrare", &CWindowsCamera::Create);
    Register("Infrare", &SeekWareCamera::Create);
}

void CCameraFactory::Register(const std::string &cameraName, CreateCameraFunc pfnCreate)
{
    m_FactoryMap[cameraName] = pfnCreate;
}

ICamera *CCameraFactory::CreateCamera(const std::string &cameraName)
{
    FactoryMap::iterator it = m_FactoryMap.find(cameraName);
    if (it != m_FactoryMap.end())
        return it->second();
    return NULL;
}
