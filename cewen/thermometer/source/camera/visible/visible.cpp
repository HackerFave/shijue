#include "visible.h"

int CCameraVisible::deinitialize(void){
    
    return AINNOSUCCESS;
}

int CCameraVisible::initialize(void){
    
    return AINNOSUCCESS;
}

int CCameraVisible::start(void){
    
    return AINNOSUCCESS;
}

int CCameraVisible::stop(void){
    
    return AINNOSUCCESS;
}

int CCameraVisible::registercallback(std::function<void(const capinfo&)> callback){
    callback_ = callback;
    return AINNOSUCCESS;
}
