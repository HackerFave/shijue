#include "common.h"

namespace measure {
    int deinitialize(void);
    
    int initialize(void);

    int start(void);

    int stop(void);

    int push(const imginfo& img);

    int registercallback(std::function<void(const imginfo&)> callback);

    int settemperaturethr(float thr);
}
