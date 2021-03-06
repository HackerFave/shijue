#ifndef ICAMERA_H_
#define ICAMERA_H_

#include "common.h"

class ICamera
{
public:
    virtual int deinitialize(void) = 0;
    virtual int initialize(void) = 0;
    virtual int start(void) = 0;
    virtual int stop(void) = 0;
    virtual int registercallback(std::function<void(const capinfo&)> callback) = 0;
};

// Create function pointer
typedef ICamera* (__stdcall *CreateCameraFunc)(void);

class CCameraFactory
{
private:
    CCameraFactory();
    CCameraFactory(const CCameraFactory &) { }
    CCameraFactory &operator=(const CCameraFactory &) { return *this; }

    typedef std::map<std::string, CreateCameraFunc> FactoryMap;
    FactoryMap m_FactoryMap;
public:
    ~CCameraFactory() { m_FactoryMap.clear(); }

    void Register(const std::string &animalName, CreateCameraFunc pfnCreate);
    ICamera *CreateCamera(const std::string &cameraName);

    static CCameraFactory *Get()
    {
        static CCameraFactory instance;
        return &instance;
    }
};

#endif