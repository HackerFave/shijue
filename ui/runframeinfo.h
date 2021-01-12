#pragma once

#include <QWidget>
#include "ui_runframeinfo.h"
#include "solution.h"
#include "qtimer.h"
class RunFrameInfo : public QWidget
{
	Q_OBJECT

public:
	RunFrameInfo(QWidget *parent = Q_NULLPTR, int stationid = -1);

	~RunFrameInfo();

public:
	//相机配置文件
	int m_stationid;
	//设置group框的相机名
	void SetGroupCameraName(QString name);
	Ui::RunFrameInfo ui;


private:
	
	void InitDialog();
	
};
