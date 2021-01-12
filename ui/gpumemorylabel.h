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
	QTimer *timerGPU;       //��ʱ����ȡGPU��Ϣ
	

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
	void GetGPU();			//��ȡgpu
	void GetMemory();       //��ȡ�ڴ�
	void ReadData();        //��ȡ����
	void SetData();         //��������

public :

	QVector<QString>  getgpuinfo() { return m_gpu; }
	void Start(QVBoxLayout *layouttext, QVBoxLayout *layout);
	void Stop();

private:
	bool GetGpuInfo();
};
