#ifndef CAMERA_SEEKWARE_H_
#define CAMERA_SEEKWARE_H_

#include "camerafactory.h"

class SeekWareCamera : public ICamera
{
public:
	int deinitialize(void);
	int initialize(void);
	int start(void);
	int stop(void);
	int registercallback(std::function<void(const capinfo&)> callback);

	static ICamera * __stdcall Create() { return new SeekWareCamera(); }
private:
	std::function<void(const capinfo&)> callback_;

private:
	bool m_isopen;
	int m_imageheight, m_imagewidth;
	void* m_camera = NULL;
	std::thread m_thread;
private:
	void run();
	
};
#endif