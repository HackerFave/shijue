#include "common.h"

namespace camera {
    int deinitialize(void);
    
    int initialize(void);

    int start(void);

    int stop(void);
    
    int registercallback(cameratype, std::function<void(const capinfo&)> callback);
}
