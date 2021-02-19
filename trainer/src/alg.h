#pragma once
#include <iostream>
#include <vector>
#include <string>

#define USE_ALG

namespace alg {
	bool initialize();

	void start();

	void stop();

	bool deinitialize();

    unsigned char getstatus();
    
    void train();

    void test();

    void convert(int mode);
}
