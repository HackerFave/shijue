#include "processdata.h"

ProcessData::ProcessData(AlgFactory* alg)
{
    if (alg != NULL) {
        alg->RegisterCallback([this](void* data) { PostPush(data); });
    }
}

void ProcessData::Push(void * data)
{
    if (data != NULL) {
        listpredata_.push(data);
    }
}

void ProcessData::PostPush(void * data)
{
    if (data != NULL) {
        listpostdata_.push(data);
    }
}

void ProcessData::PreRun()
{
    while (true)
    {
        if (alg_ == NULL) {
            continue;
        }

        if (listpredata_.size() > 0) {
            auto data = listpostdata_.front();
            alg_->Push(PreProcess(data));
        }
        
    }
}

void ProcessData::PostRun()
{
    while (true)
    {
        if (listpostdata_.size() > 0) {
            auto data = listpostdata_.front();
            callback_(PostProcess(data));
        }
    }
}
