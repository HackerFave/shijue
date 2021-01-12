#pragma once
#include "processdata.h"
#include "algfactory.h"
#include <map>
class AlgManage
{
public:
    
    ~AlgManage();

    static void Star();
    static void Stop();
    static void Init();
    static void DeInit();
	static AlgManage& instance();
private:
    AlgManage();
    std::map<std::string, ProcessData*> pdmap_;
    std::map<std::string, AlgFactory*> algmap_;
	
};

