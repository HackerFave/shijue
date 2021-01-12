#pragma once
#include "qsignalmapper.h"
#include "qvector.h"
#include "qtimer.h"
#include "qgridlayout.h"
#include "qlabel.h"
#pragma  comment(lib,"nvml.lib")
class GpuMemoryLabel :public QObject
{
	Q_OBJECT
public:
	GpuMemoryLabel();
	~GpuMemoryLabel();

private:
	QTimer *timerGPU;       //定时器获取GPU信息
	

private:
	int totalNew, idleNew, totalOld, idleOld;
	int cpuPercent;
	int memoryPercent;
	int memoryAll;
	int memoryUse;
	int memoryFree;
	QVector<QString> m_gpu;
	QVector<QLabel*> m_plab;


private slots:
	void GetGPU();			//获取gpu
	void GetMemory();       //获取内存
	void ReadData();        //读取数据
	void SetData();         //设置数据

public :

	QVector<QString>  getgpuinfo() { return m_gpu; }
	void Start(QVBoxLayout *layouttext, QVBoxLayout *layout);
	void Stop();

private:
	bool GetGpuInfo();
};
