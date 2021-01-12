#pragma once
#include <queue>
#include "algfactory.h"

class ProcessData
{
public:
    ProcessData(AlgFactory* alg);
    virtual ~ProcessData() = 0;

    void Push(void* data);

    void PostPush(void* data);
private:

    virtual void* PreProcess(void* data) = 0;

    virtual void* PostProcess(void* data) = 0;

    void PreRun();

    void PostRun();

protected:

    AlgFactory* alg_;
    std::queue<void*> listpredata_;
    std::queue<void*> listpostdata_;
    std::function<void(void*)> callback_;
};

