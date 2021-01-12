#include "uicommon.h"
#include <iostream>
#include <string>
#include <vector>
#include "qsettings.h"
#include "common.h"
#ifdef USECAMERADLL
#include "..\camera\cameramanage\cameramanage.h"
#endif
#include "alg.h"
//#include "qmessagebox.h"
#include "detectionscheme.h"
#include "solution.h"
#include "ciwamsgdialog.h"

DetectionScheme::DetectionScheme(QWidget *parent)
	: QWidget(parent)
{

	ui.setupUi(this);
	std::vector<std::string> cameralist;
#ifdef USECAMERADLL
    ui.comCameraType->clear();
    for (int i = 0; i < cameratype::CAMERA_COUNT; i++)
    {
        cameratype t = (cameratype)i;
        switch (t)
        {
        case CAMERA_FILE:
            ui.comCameraType->addItem(QString("网络相机"));
            break;
        case CAMERA_INVENTED:
            ui.comCameraType->addItem(QString("本地相机"));
            break;
        case CAMERA_HIK:
            ui.comCameraType->addItem(QString("海康相机"));
            break;
        case CAMERA_BASLER:
            ui.comCameraType->addItem(QString("basler"));
            break;
        case CAMERA_COUNT:
            break;
        }
    }
    ui.comCameraType->setCurrentIndex((int)cameratype::CAMERA_HIK);
    cameralist = CCameraManage::EnumerateCurrentDevices(cameratype::CAMERA_HIK);
    CCameraManage::Open();
#endif
	ui.comCameraID->clear();
	for (int i = 0; i < cameralist.size(); i++) {
		std::string cameraname = cameralist[i];
		ui.comCameraID->addItem(QString::fromLocal8Bit(cameraname.c_str()));
	}

	QObject::connect(ui.comCameraID, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_btntest_clicked()));
	QObject::connect(ui.comStation, SIGNAL(currentIndexChanged(QString)), this, SLOT(SetParam(QString)));
    QObject::connect(ui.comCameraType, SIGNAL(currentIndexChanged(QString)), this, SLOT(OnCameraType()));
	
	//隐藏标题栏
	setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
	//初始化算法列表
	InitAlgList();

	
}

DetectionScheme::~DetectionScheme()
{
	if (ui.dsplan != NULL)
	{
		delete ui.dsplan;
		ui.dsplan = NULL;
	}

}
void DetectionScheme::on_checkLast_clicked()
{
	int stationid = ui.comStation->currentText().toInt();
	if (m_vectorSolu.size() < stationid+1)
		return;

	if (ui.checkLast->isChecked())
	{
		m_vectorSolu[stationid]->SetLastIcon();
		if((stationid + 1)< m_vectorSolu.size())
			m_vectorSolu[stationid+1]->SetFristIcon();
	}
	else
	{
		m_vectorSolu[stationid]->SetNormalIcon();
		if ((stationid + 1) < m_vectorSolu.size())
		{
			m_vectorSolu[stationid + 1]->SetNormalIcon();
			if (Solution::GetCurrentSolution()->m_camera[stationid + 1].get_islast())
			{
				m_vectorSolu[stationid+1]->SetLastIcon();
			}
		
		}
		if (stationid == 0)
			m_vectorSolu[stationid]->SetFristIcon();
		if (stationid > 1)
		{
			if (Solution::GetCurrentSolution()->m_camera[stationid - 1].get_islast())
			{
				m_vectorSolu[stationid]->SetFristIcon();

			}
	
		}

	}

}

void DetectionScheme::OnSendTriggerClose(int)
{
#ifdef USECAMERADLL
    CCameraManage::OnceTriggerStop();
    QObject::disconnect(this, SIGNAL(SendTriggerClose(int)), this, SLOT(OnSendTriggerClose(int)));
#endif
}

void DetectionScheme::on_btntest_clicked()
{
	std::string cameraname = ui.comCameraID->currentText().toLocal8Bit();
#ifdef USECAMERADLL
    QObject::connect(this, SIGNAL(SendTriggerClose(int)), this, SLOT(OnSendTriggerClose(int)));
    int num = ui.comCameraID->currentIndex();
    CCameraManage::OnceTrigger([this](const capinfo& img) {
        cv::Mat temp(img.img);
        ui.editFrameId->setText(QString::number(img.frameid));
        ui.dsplan->ShowPic(temp);
        emit this->SendTriggerClose(img.frameid); },
        ui.comCameraType->currentIndex(), num);
#endif
}

//设置读取的参数
void DetectionScheme::SetParam(QString text)
{
	Solution* solution = Solution::GetCurrentSolution();
	if (!solution)
		return;
	bool temp = solution->m_camera[text.toInt()].get_isused();
	ui.checkLast->setChecked(solution->m_camera[text.toInt()].get_islast());
	ui.checkUseStation->setChecked(solution->m_camera[text.toInt()].get_isused());
	ui.comGroup->setCurrentText(QString::number(solution->m_camera[text.toInt()].get_group()));
	ui.editFrameId->setText(QString::number(solution->m_camera[text.toInt()].get_frameid()));
	ui.comStation->setCurrentText(QString::number(solution->m_camera[text.toInt()].get_station()));
	ui.comCameraID->setCurrentText(solution->m_camera[text.toInt()].get_cameraindex());
    ui.comCameraType->setCurrentIndex(solution->m_camera[text.toInt()].get_camtype());
	//算法
	QToolButton  *p;
	for (size_t j = 0; j < ui.gridLayout_4->count(); j++)
	{
		p = (QToolButton*)ui.gridLayout_4->itemAt(j)->widget();
		p->setChecked(false);
	}
	QStringList list = solution->m_camera[text.toInt()].get_alg();
	for (size_t i = 0; i < list.count(); i++)
	{
		for (size_t j = 0; j < ui.gridLayout_4->count(); j++)
		{
			p = (QToolButton*)ui.gridLayout_4->itemAt(j)->widget();
			if (QString::compare(p->text(), list.at(i), Qt::CaseSensitive) == 0)
				p->setChecked(true);
		}
		update();
	}

	//图标
	m_vectorSolu[text.toInt()]->m_issettled = false;
	for (size_t i = 0; i < m_vectorSolu.size(); i++)
	{
		if(!m_vectorSolu[i]->m_issettled)
			m_vectorSolu[i]->UnSettled();

	}
	m_vectorSolu[text.toInt()]->Setting();

	ui.dsplan->DelRoi();
	float * roi = solution->m_camera[text.toInt()].get_roirect();

	//切换图像
	ui.dsplan->set_issetingroi(true);
	ui.dsplan->set_grect(roi[0], roi[1], roi[2], roi[3]);

	this->UpdataIcon();
}

void DetectionScheme::InitAlgList()
{
	QToolButton  *p;

	//去读算法
	std::vector<std::string> alglist;
	alglist = alg::enumerate();
	int algcount = alglist.size();
	
	for (int j = 0; j < ui.gridLayout_4->count(); j++)
	{

		p = (QToolButton*)ui.gridLayout_4->itemAt(j)->widget();
			
		if (j < algcount)
		{
			QString algname = QString::fromLocal8Bit(alglist[j].c_str());
			p->setText(algname);
		}
		else
		{
			//隐藏预留的算法
			p->hide();
		}
	}




}

void DetectionScheme::on_btnDelRoi_clicked()
{
	ui.dsplan->DelRoi();
}

void DetectionScheme::on_btnSave_clicked()
{
	QString ininame = Solution::GetCurrentSolution()->get_solutionname();
	QString alg,roi;
	//roi
	roi = ui.dsplan->GetRoiRect();
	if (roi.isEmpty())
	{
		CiWaMsgDialog::MessageT("保存失败", "roi未设置");
		//QMessageBox::warning(NULL, "", "保存失败:roi未设置");
		return;
	}
	QString stationid = ui.comStation->currentText();
	QString groupid = ui.comGroup->currentText();
	QString cameraid = ui.comCameraID->currentText();
	QString frameid = ui.editFrameId->text();
	bool islastcamera = ui.checkLast->isChecked();
	bool isusestation = ui.checkUseStation->isChecked();
    int camtype = ui.comCameraType->currentIndex();
	QSettings configini(Solution::GetCurrentSolution()->get_inipath(), QSettings::IniFormat);
	configini.beginGroup(stationid);
	configini.setValue(SGROUP, groupid);
	configini.setValue(CAMERAID, cameraid);
	configini.setValue(FRAMEID, frameid);
	configini.setValue(ISLAST, islastcamera);
	configini.setValue(ISUSED, isusestation);
    configini.setValue(CAMERATYPE, camtype);
	for (size_t i = 0; i < ui.gridLayout_4->count(); i++)
	{
		QToolButton *p = (QToolButton*)ui.gridLayout_4->itemAt(i)->widget();
		if (p->isChecked())
			alg.append(p->text() + ',');

	}

	alg.chop(1);
	configini.setValue(ALG, alg);
	configini.setValue(ROI, roi);
	configini.endGroup();

	Solution::SetCurrentSolution(ininame);

	//图标设置为已保存
	if (m_vectorSolu[stationid.toInt()])
	{
		m_vectorSolu[stationid.toInt()]->Settled();
	}
	CiWaMsgDialog::MessageT("保存成功", "");
}

void DetectionScheme::on_btnRoi_clicked()
{
	ui.dsplan->DrawRoi();

}


void DetectionScheme::mouseMoveEvent(QMouseEvent *event)
{
}

void DetectionScheme::closeEvent(QCloseEvent * event)
{

}
void DetectionScheme::UpdataIcon()
{
	//分组
	Solution* solution = Solution::GetCurrentSolution();
	for (size_t i = 0; i < m_vectorSolu.size(); i++)
	{
		m_vectorSolu[i]->SetNormalIcon();
	}

	m_vectorSolu[0]->SetFristIcon();
	if (m_stationnum > 0)
		m_vectorSolu[m_stationnum - 1]->SetLastIcon();
	
	if (solution == NULL)
		return;
	for (auto it = solution->m_camera.begin(); it != solution->m_camera.end(); it++)
	{
		if (it->first >= m_stationnum)
			return;
		if (it->second.get_islast())
		{
			m_vectorSolu[it->first]->SetLastIcon();
			if ((it->first + 1) < m_stationnum)
			{
				m_vectorSolu[it->first + 1]->SetFristIcon();
			}
		}
	}
}



void DetectionScheme::AddStationPro(int stationnum)
{
	SolutionIcon *lab;
	for (int i = 0; i < m_vectorSolu.size(); i++)
	{
		delete m_vectorSolu[i];
		m_vectorSolu[i] = NULL;
	}
	m_vectorSolu.clear();

	int num = ui.gridLayout_8->count();
	for (int i = 1; i <= num; i++)
	{
		QWidget  *p = ui.gridLayout_8->itemAt(num - i)->widget();
		ui.gridLayout_8->removeWidget(p);
		delete p;
	}

	for (size_t n = 0; n < 3; n++)
	{
		for (size_t m = 0; m < 10; m++)
		{
			int index = n * 10 + m;
			if (index < stationnum)
			{
				lab = new SolutionIcon();
				lab->m_lab->setText(QString::number(index));
				ui.gridLayout_8->addWidget(lab->m_widget, n, m);
				m_vectorSolu.push_back(lab);
			}
		}
	}

	this->UpdataIcon();
}

void DetectionScheme::setStationNum(int stationnum) {
	m_stationnum = stationnum;
	ui.labStation->setText(QString("共 %1 个工位").arg(m_stationnum));
	//添加进度图标
	AddStationPro(stationnum);

	ui.comStation->clear();
	for (int i = 0; i < m_stationnum; i++) {
		std::string stationname = std::to_string(i);
		ui.comStation->addItem(QString::asprintf(stationname.c_str()));
	}

}


void DetectionScheme::setGroupNum(int groupnum) {
	m_groupnum = groupnum;
	ui.comGroup->clear();
	for (int i = 0; i < m_groupnum; i++) {
		std::string groupname = std::to_string(i);
		ui.comGroup->addItem(QString::asprintf(groupname.c_str()));
	}
}

void DetectionScheme::OnCameraType()
{
#ifdef USECAMERADLL
    std::vector<std::string> cameralist;
    cameralist = CCameraManage::EnumerateCurrentDevices((cameratype)ui.comCameraType->currentIndex());
    QObject::disconnect(ui.comCameraID, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_btntest_clicked()));
    ui.comCameraID->clear();
    for (int i = 0; i < cameralist.size(); i++) {
        std::string cameraname = cameralist[i];
        ui.comCameraID->addItem(QString::fromLocal8Bit(cameraname.c_str()));
    }
    QObject::connect(ui.comCameraID, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_btntest_clicked()));
#endif
}

void DetectionScheme::on_btnExit_clicked()
{
	this->close();
}