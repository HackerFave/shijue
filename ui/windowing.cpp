#include "windowing.h"
#include "widgetpic.h"
#include "uicommon.h"
#include "qlineedit.h"
#include "fenpIni.h"
#include "qvalidator.h"
Windowing::Windowing(QWidget *parent,int window,int idwindow,int type,bool ismast)
	: QDialog(parent)
{
	m_ismaste = ismast;
	m_idwindow = idwindow;
	m_windowtype = type;
	ui.setupUi(this);
	Init(window);
}

Windowing::~Windowing()
{
}

void Windowing::Init(int window)
{
	//设置多个窗口
	if (window < 2)				 //1
	{
		DistributeView(1, 1);	
	}
	else if (window < 3)		//1*2
	{
		DistributeView(1, 2);
	}
	else if (window < 5)		//2*2
	{
		DistributeView(2,2);
	}
	else if (window < 7)		//3*2
	{
		DistributeView(2, 3);
	}
	else if (window < 9)		//4*2
	{
		DistributeView(2, 4);
	}
	else if (window < 13)		//4*3
	{
		DistributeView(3, 4);
	}
	else if (window < 17)		//4*4
	{
		DistributeView(4, 4);
	}
	else if (window < 21)		//4*5
	{
		DistributeView(4, 5);
	}
	else if (window < 25)		//4*6
	{
		DistributeView(4, 6);
	}
}

void Windowing::DistributeView(int row, int col)
{
	m_window = row * col;
	int i = 0;
	int num = ui.gridLayout->count();
	QLineEdit * plabel;
	WidgetPic* ppic;

	for (int i = 1; i <= num; i++)
	{
		QWidget  *p = ui.gridLayout->itemAt(num - i)->widget();
		ui.gridLayout->removeWidget(p);
		delete p;
		p = NULL;
	}
	
	m_mapview.clear();
	
	for (size_t j = 0; j < row; j++)
	{
		for (size_t z = 0; z < col; z++)
		{
			if(m_windowtype != 0)
			{
				plabel = new QLineEdit(this);
				m_mapview[col*j + z] = (void*)plabel;
				plabel->setValidator(new QIntValidator(0, MAXCAMERA, this));
				plabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
				plabel->setAlignment(Qt::AlignCenter);
				plabel->setStyleSheet("font: 75 20pt \"PingFangSC-Medium FB\";color: rgba(255,255,255,0.80);background-color: rgb(21, 21, 21);");
				plabel->setText(QString::number(col*j + z));
				
				ui.gridLayout->addWidget(plabel, j, z);
			}
			else
			{ 
				ppic = new WidgetPic(this);
				ppic->set_bestaion(i);
				ppic->set_bewindow(m_idwindow);
				ui.gridLayout->addWidget(ppic, j, z);
				m_mapview[col*j + z] = (void*)ppic;
			}
		}

	}
	if (FenpIni::IsOpen())
		SetCustomeWin();
}

void Windowing::SetCustomeWin()
{
	if (m_windowtype != 0)
		return;
	int idwindow = m_idwindow;
	if (m_ismaste)
		idwindow = m_idwindow - 1;

	defenp fdata = FenpIni::GetData();
	
	//1是否有屏幕的id
	if (fdata.count(idwindow) < 1)
		return;
	//2屏幕数量和当前配置文件数量是否相等
	
	int viewnum = FenpIni::get_ilist().at(fdata.at(idwindow).viewnum);
	std::map<int, int> dfw = fdata.at(idwindow).dfw;
	if (m_mapview.size() != viewnum)
		return;
	//3遍历配置，将屏幕指针和工位对应上，工位可否对应多个屏幕？不可以这样
	m_stationmap.clear();
	m_stationfenp.clear();
	for (int i = 0; i < viewnum; i++)
	{
		m_stationmap[dfw.at(i)] = m_mapview[i];
		m_stationfenp[dfw.at(i)] = idwindow;
	}
}


void Windowing::show()
{
	QDialog::show();
	
	int num = ui.gridLayout->count();
	for (int i = 1; i <= num; i++)
	{
		QWidget  *p = ui.gridLayout->itemAt(num - i)->widget();
		p->repaint();
	}

}



