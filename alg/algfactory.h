#pragma once
#include <queue>
#include <thread>
#include <functional>

class AlgFactory
{
public:
    AlgFactory();
    virtual ~AlgFactory() = 0;

    void RegisterCallback(std::function<void(void*)> callback);

    bool Initialize();

    bool DeInitialize();

    void Push(void* data);

private:

    virtual void* DoProcess(void* data) = 0;

	virtual bool InitAlg() = 0;

	virtual bool DeInitAlg() = 0;
	
    void Run();

    std::queue<void*> listpredata_;
    std::function<void(void*)> callback_;

protected:
	std::thread thread_;
};

