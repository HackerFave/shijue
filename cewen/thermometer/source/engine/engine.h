#include "common.h"

namespace engine {
    enum callbacktype {
        CALLBACK_DETECT = 0,
        CALLBACK_MEASURE = 1,
        CALLBACK_UI = 2,
    };
    
    int deinitialize(void);
    
    int initialize(void);

    int start(void);

    int stop(void);


    int pushvisible(const capinfo & img);

    int pushinfrare(const capinfo & img);

    int pushdetect(const imginfo & img);

    int pushmeasure(const imginfo & img);

    int registercallback(callbacktype type, std::function<void(const imginfo&)> callback);
}
