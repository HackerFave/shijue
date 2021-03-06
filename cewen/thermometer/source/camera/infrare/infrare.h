#ifndef CAMERA_INFRARE_H_
#define CAMERA_INFRARE_H_

#include "camerafactory.h"

class CCameraInfrare : public ICamera
{
public:
    int deinitialize(void);
    int initialize(void);
    int start(void);
    int stop(void);
    int registercallback(std::function<void(const capinfo&)> callback);

    static ICamera * __stdcall Create() { return new CCameraInfrare(); }
private:
    std::function<void(const capinfo&)> callback_;
};

#endif