#include "gpumemorylabel.h"
#include "common.h"
#ifdef USE_CUDA
#include "nvml.h"
#endif

#pragma execution_character_set("utf-8")
#define MB (1024 * 1024)
#define KB (1024)
GpuMemoryLabel::GpuMemoryLabel()
{
	this->m_gpu.clear();
	this->m_plab.clear();
}

GpuMemoryLabel::~GpuMemoryLabel()
{
}

void GpuMemoryLabel::GetMemory()
{
}

void GpuMemoryLabel::ReadData()
{
}

void GpuMemoryLabel::SetData()
{
}

void GpuMemoryLabel::Start(QVBoxLayout *layouttext , QVBoxLayout *layout)
{
	if (!GetGpuInfo())
		return;
	for (size_t i = 0; i < m_gpu.count(); i++)
	{
		QString strgpu = QString("GPU[%1]").arg(i);
		//新建LAB
		QLabel *gpu = new QLabel();
		QLabel *gputext = new QLabel();
		gputext->setText(strgpu);
		m_plab.push_back(gpu);
		layouttext->addWidget(gputext, i, 0);
		layout->addWidget(gpu, i, 0);

	
	}
	timerGPU = new QTimer(this);
	connect(timerGPU, SIGNAL(timeout()), this, SLOT(GetGPU()));
	timerGPU->start(2000);
}

void GpuMemoryLabel::Stop()
{
	timerGPU->stop();
}

bool GpuMemoryLabel::GetGpuInfo()
{
#ifdef USE_CUDA
	nvmlReturn_t result;
	unsigned int device_count, i;

	result = nvmlInit();
	if (NVML_SUCCESS != result)
	{
		result = nvmlShutdown();
		return false;
	}
	result = nvmlDeviceGetCount(&device_count);
	if (NVML_SUCCESS != result)
	{
		result = nvmlShutdown();
		return false;
	}
	for (size_t i = 0; i < device_count; i++)
	{
		nvmlDevice_t device;
		char name[NVML_DEVICE_NAME_BUFFER_SIZE];
		result = nvmlDeviceGetHandleByIndex(i, &device);
		if (NVML_SUCCESS != result)
		{
			result = nvmlShutdown();
			return false;
		}
		result = nvmlDeviceGetName(device, name, NVML_DEVICE_NAME_BUFFER_SIZE);
		if (NVML_SUCCESS != result)
		{
			result = nvmlShutdown();
			return false;
		}
		m_gpu.push_back(QString(name));
	}
	result = nvmlShutdown();
#endif
	return true;
}

void GpuMemoryLabel::GetGPU()
{
#ifdef USE_CUDA
	nvmlReturn_t result;
	nvmlMemory_t memory;
	nvmlUtilization_t utilization;
	unsigned int device_count;

	result = nvmlInit();
	if (NVML_SUCCESS != result)
	{
		result = nvmlShutdown();
		return ;
	}


	for (size_t i = 0; i < m_plab.count(); i++)
	{
		nvmlDevice_t device;
		result = nvmlDeviceGetHandleByIndex(i, &device);
		result = nvmlDeviceGetMemoryInfo(device, &memory);
		result = nvmlDeviceGetUtilizationRates(device, &utilization);

		QString msg = QString("GPU %1%   Mem %2% (已用 %3 MB/共 %4 MB)").arg(utilization.gpu).arg(utilization.memory).arg(memory.used / MB).arg(memory.total/ MB);
		m_plab[i]->setText(msg);
	}
	result = nvmlShutdown();
#endif
	return;
}