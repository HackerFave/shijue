#pragma once
#include <iostream>
#include <vector>

#include <opencv2/opencv.hpp>

#include "common.h"

namespace group {
	bool initialize(/*std::function<void(imginfo&)> callback*/);

	bool start();

	void stop();

	void push(const imginfo & img);

	bool deinitialize();
}
