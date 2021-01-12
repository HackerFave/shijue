#include "runframenewui.h"
#include "qheaderview.h"
#include "widgetpic.h"
#include "solution.h"
#include "qdesktopwidget.h"
#include "fenpIni.h"
#include "../socket/ciwasocket.h"
RunFrameNewUi::RunFrameNewUi(QWidget *parent, QRect rect)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->resize(rect.width(), rect.height());

	m_isfpshow = false;
	m_windowing = new Windowing(this, 12);
	
	m_listwindow = FenpIni::get_ilist();
	m_runtoprightdialog = new RunTopRightDialog();
	ui.layoutRight->addWidget(m_runtoprightdialog);
	QObject::connect(m_runtoprightdialog, SIGNAL(SendUpdataWindow()), this, SLOT(OnSendUpdataWindow()));
	QObject::connect(m_runtoprightdialog, SIGNAL(SendCustomWindow()), this, SLOT(OnSendCustomWindow()));
	QObject::connect(m_runtoprightdialog, SIGNAL(SendSingleSignal()), this, SLOT(OnSendSingleSignal()));
	QObject::connect(m_runtoprightdialog, SIGNAL(SendMultiSignal()), this, SLOT(OnSendMultiSignal()));
	m_runframetablewidget = new RunFrameTableWidget(NULL,QSize(rect.width()*0.8,rect.height()));

	
	if (FenpIni::GetAutoOpen())
		ui.layoutLeft->addWidget(m_runframetablewidget);
	else
		ui.layoutLeft->addWidget(m_windowing);
	
	if (FenpIni::IsOpen())
		OnSendCustomWindow();
	else
		OnSendUpdataWindow();

}

RunFrameNewUi::~RunFrameNewUi()
{
	if (m_windowing)
	{
		delete m_windowing;
		m_windowing = NULL;
	}
	for (size_t j = 0; j < m_windowing_list.size(); j++)
	{
		delete m_windowing_list[j];
		m_windowing_list[j] = NULL;
	}
}

void RunFrameNewUi::OnSendMultiSignal()
{
	mutex.lock();
	QList<int> list;
	ui.layoutLeft->removeWidget(m_windowing);
	
	m_windowing->hide();
	ui.layoutLeft->addWidget(m_runframetablewidget);
	RunFrameTable * runframetable = m_runtoprightdialog->get_table();
	m_runframetablewidget->SetTable(runframetable);
	m_runframetablewidget->show();
	list << m_runframetablewidget->ui.label_3->width()
		 << m_runframetablewidget->ui.label_2->width()
		 << m_runframetablewidget->ui.label_4->width()
		 << m_runframetablewidget->ui.label->width()
		 << m_runframetablewidget->ui.label_5->width()
		 << m_runframetablewidget->ui.label_6->width();
	runframetable->SetColWidth(list);
	runframetable->SetTableStyle(true);
	runframetable->show();
	m_isfpshow = true;
	//显示多显示器
	if (FenpIni::IsOpen())
		ShowMdisplay(2);
	else
		ShowMdisplay();
	mutex.unlock();
}

void RunFrameNewUi::OnSendSingleSignal()
{
	mutex.lock();
	int station = Solution::GetCurrentSolution()->get_station();
	QList<int> list;
	RunFrameTable * runframetable = m_runtoprightdialog->get_table();
	runframetable->SetColWidth(runframetable->m_swidth_list);
	runframetable->SetTableStyle(false);
	m_runframetablewidget->hide();
	ui.layoutLeft->removeWidget(m_runframetablewidget);
	m_runtoprightdialog->ShowRightBottomTable();
	ui.layoutLeft->addWidget(m_windowing);
	m_isfpshow = false;
	//显示单显示器
	for (size_t j = 0; j < m_windowing_list.size(); j++)
	{
		m_windowing_list[j]->hide();
	}
	m_windowing->show();
	mutex.unlock();
}

void RunFrameNewUi::show()
{
    QWidget::show();
}

//自定义显示
void RunFrameNewUi::OnSendCustomWindow()
{
	defenp fdata = FenpIni::GetData();
	if (fdata.size() < 1)
		return;
	//2显示(单屏)(读取配置文件)
	int winid = 0;
	int viewnum = 0;
	//单显示器
	if (m_windowing != NULL)
	{
		if (fdata.count(0) < 1)
			return;
		delete m_windowing;
		viewnum = fdata.at(0).viewnum;
		m_windowing = new Windowing(this, m_listwindow.at(viewnum),0);
	}

	if (!m_isfpshow)
	{
		ui.layoutLeft->addWidget(m_windowing);
		m_windowing->show();
	}

	//2显示(多屏)
	if (m_windowing_list.size() > 0)
	{
		ShowMdisplay(2);
	}
	
}

void RunFrameNewUi::OnSendUpdataWindow()
{
	
	if (!Solution::GetCurrentSolution())
		return;
	
	if (m_windowing_list.size() > 0)
	{
		ShowMdisplay();
	}

	int station = Solution::GetCurrentSolution()->get_station();
	if (m_windowing != NULL)
	{
		delete m_windowing;
		m_windowing = new Windowing(this, station,0);
	}
	if (!m_isfpshow)
	{
		ui.layoutLeft->addWidget(m_windowing);
		m_windowing->show();
	}
}


//显示主屏图像
void RunFrameNewUi::ShowCameraPicMain(const imginfo& img)
{
	imginfo tmpimginfo = img;
	int winidx = tmpimginfo.getstationid();
	if (m_windowing == NULL)
		return;
	WidgetPic  *p;
	if ((m_windowing->m_stationmap.count(winidx) < 1) && FenpIni::IsOpen())
		return;
	mutex.lock();
	if (FenpIni::IsOpen())
	{

		p = (WidgetPic*)m_windowing->m_stationmap.at(winidx);
	}
	else
		p = (WidgetPic*)m_windowing->ui.gridLayout->itemAt(winidx)->widget();
	cv::Mat tmpimg = tmpimginfo.getimg();
    QImage  disImage = QImage((const unsigned char*)(tmpimg.data), tmpimg.cols, tmpimg.rows, tmpimg.cols*tmpimg.channels(), QImage::Format_RGB888);
	p->SetQimage(disImage);
	mutex.unlock();
}


//显示分屏图像
void RunFrameNewUi::ShowCameraPic(const imginfo& img)
{
	imginfo tmpimginfo = img;
	cv::cvtColor(tmpimginfo.getimg(), tmpimginfo.getimg(), cv::COLOR_BGR2RGB);
	if (!m_isfpshow)
	{
		this->ShowCameraPicMain(img);
		return;
	}
	int winidx = tmpimginfo.getstationid();
	//给分屏显示

	WidgetPic *pw = NULL;
	if (m_windowing_list.size() > 0)
	{
		if (FenpIni::IsOpen())
		{
			int pingmudix = FenpIni::GetStationAtWin(winidx);
			
			if (pingmudix > -1 && m_windowing_list.size() > pingmudix)
			{
				if(m_windowing_list[pingmudix]->m_stationmap.count(winidx))
				{ 
					mutex.lock();
					pw = (WidgetPic*)m_windowing_list[pingmudix]->m_stationmap.at(winidx);
				}
				else
					return;
			}
			else
				return;
		}
		else
		{ 
			int i = winidx / m_windowing_list[0]->m_window;
			int j = winidx % m_windowing_list[0]->m_window;
			mutex.lock();
			pw = (WidgetPic*)m_windowing_list[i]->ui.gridLayout->itemAt(j)->widget();
		}
	}
	else
	{
		return;
	}
	
	cv::Mat tmpimg = tmpimginfo.getimg();
	QImage  disImage = QImage((const unsigned char*)(tmpimg.data), tmpimg.cols, tmpimg.rows, tmpimg.cols*tmpimg.channels(), QImage::Format_RGB888);

	if (pw)
	{
		pw->SetQimage(disImage);

	}
	mutex.unlock();
}



/***************************************************************
*  @brief     分屏
*  @param     int 1:默认,2自定义
*  @note     
*  @Sample usage:     
**************************************************************/
void RunFrameNewUi::ShowMdisplay(int iscustom)
{
	for (size_t j = 0; j < m_windowing_list.size(); j++)
	{
		delete m_windowing_list[j];
		m_windowing_list[j] = NULL;
	}
	m_windowing_list.clear();

	QDesktopWidget* desktop = QApplication::desktop();
	int n = desktop->screenCount();
	int station = Solution::GetCurrentSolution()->get_station(); 
	int w = 0;
	int i = 0;
	bool ismaste = false;
	//主从分别
	if (CiWaSocket::GetCurrMaster()== HostState::MASTE)
	{
		ismaste = true;
		if (n < 2)
			return;
		w = ceil((float)station / (n - 1));
		i = 1;

	}
	else
	{
		ismaste = false;
		if (n < 1)
			return;
		w = ceil((float)station / n);
		i = 0;
	}
	defenp fdata = FenpIni::GetData();
	while (i < n)
	{
		if (iscustom != 1)
		{
			if (fdata.count(i) < 1)
			{ 
				i++;
				continue;
			}
			if (ismaste)
				w = m_listwindow.at(fdata.at(i-1).viewnum);
			else
				w = m_listwindow.at(fdata.at(i).viewnum);
		}
		Windowing* dlg = new Windowing(this, w, i,0, ismaste);
		dlg->setGeometry(desktop->screenGeometry(i));
		if (m_isfpshow)
		{
			dlg->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
			dlg->showMaximized();
			dlg->show();
		}
		m_windowing_list.push_back(dlg);
		i++;
	}
	
}
QString RunFrameNewUi::getModelName()
{
  return m_runtoprightdialog->getProgramName();
}


