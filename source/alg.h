#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "common.h"
namespace alg {
	bool initialize(std::function<void(const imginfo& img)> callback);

	std::vector<std::string> enumerate();

	void start();

	void stop();

	void push(const imginfo & input);

	void registercallback(std::function<void(const imginfo&)> callback);

	bool deinitialize();

	std::string getimgsavedir();

    bool reloadmodel();

    bool unloadmodel();
}
