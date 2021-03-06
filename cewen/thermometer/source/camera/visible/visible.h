#ifndef CAMERA_VISIBLE_H_
#define CAMERA_VISIBLE_H_

#include "camerafactory.h"

class CCameraVisible : public ICamera
{
public:
    int deinitialize(void);
    int initialize(void);
    int start(void);
    int stop(void);
    int registercallback(std::function<void(const capinfo&)> callback);

    static ICamera * __stdcall Create() { return new CCameraVisible(); }
private:
    std::function<void(const capinfo&)> callback_;
};

#endif