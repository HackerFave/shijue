#include "infrare.h"

int CCameraInfrare::deinitialize(void){
    
    return AINNOSUCCESS;
}

int CCameraInfrare::initialize(void){
    
    return AINNOSUCCESS;
}

int CCameraInfrare::start(void){
    
    return AINNOSUCCESS;
}

int CCameraInfrare::stop(void){
    
    return AINNOSUCCESS;
}

int CCameraInfrare::registercallback(std::function<void(const capinfo&)> callback){
    callback_ = callback;
    return AINNOSUCCESS;
}
