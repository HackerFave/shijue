#include "processdata.h"
#include "algfactory.h"

AlgFactory::AlgFactory()
{
    
}

void AlgFactory::RegisterCallback(std::function<void(void*)> callback)
{
    callback_ = callback;
}

bool AlgFactory::Initialize()
{
	bool ret = InitAlg();
	
	if(ret){
		thread_ = std::thread(&AlgFactory::Run, this);
	}
    return ret;
}

bool AlgFactory::DeInitialize()
{
	thread_.join();
	bool ret = DeInitAlg();
    return ret;	
}

void AlgFactory::Push(void * data)
{
    if (data != NULL) {
        listpredata_.push(data);
    }
}

void AlgFactory::Run()
{
    while (true)
    {
        if (listpredata_.size() > 0) {
            auto data = DoProcess(listpredata_.front());
            callback_(data);
        }
    }
}
