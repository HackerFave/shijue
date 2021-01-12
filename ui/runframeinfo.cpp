#include "runframeinfo.h"

RunFrameInfo::RunFrameInfo(QWidget *parent, int stationid)
	: QWidget(parent)
{
	m_stationid = stationid;
	//Òþ²Ø±êÌâÀ¸
	setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
	ui.setupUi(this);
	InitDialog();

}

RunFrameInfo::~RunFrameInfo()
{

}

void RunFrameInfo::SetGroupCameraName(QString name)
{
	//ui.groupBox->setTitle(name);
}

void RunFrameInfo::InitDialog()
{
	if (m_stationid == -1)
		return;
	Solution_Camera &cam = sln::getcamera(m_stationid);
	ui.groupBox->setTitle(cam.get_cameraindex());
}



