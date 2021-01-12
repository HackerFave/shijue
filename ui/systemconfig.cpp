#include "systemconfig.h"
#include "common.h"
#include "uicommon.h"
#ifdef USECAMERADLL
#include "..\camera\cameramanage\cameramanage.h"
#endif
#include "databasesqlite.h"
//#include "qmessagebox.h"
#include <QCoreApplication>
#include "solution.h"
#include "baseparam.h"
#include "tableWidgetParent.h"
#include "fenpIni.h"
#include "windowing.h"
#include "ciwamsgdialog.h"
#include "commonconfigini.h"
SystemConfig::SystemConfig(QWidget *parent, QRect rect)
	: QWidget(parent)
{
	m_plcindex = -1;
    m_cameratable = NULL;
	m_gpu = NULL;
	m_isnetwork = false;
	//隐藏标题栏
	setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
	ui.setupUi(this);
	//重置大小
	this->resize(rect.width(), rect.height());


	//connect(ui.cameraTable, SIGNAL(cellClicked(int, int)), this, SLOT(OnCellClicked(int, int)));
	connect(ui.pushButtonConnect, SIGNAL(clicked()), this, SLOT(on_pushButtonConnect_clicked()));
	connect(ui.pushButton_6, SIGNAL(clicked()), this, SLOT(on_read_clicked()));

	connect(ui.comboBoxRegisterType, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBoxRegisterType_currentIndexChanged(int)));
	connect(ui.pushButton_7, SIGNAL(clicked()), this, SLOT(on_write_clicked()));

	connect(ui.savebaseparam, SIGNAL(clicked()), this, SLOT(OnSaveBaseParam()));
	connect(ui.readbaseparam, SIGNAL(clicked()), this, SLOT(OnReadBaseParam()));

	InitTab();
}

SystemConfig::~SystemConfig()
{
	if (m_systeminfo != NULL)
	{
		delete m_systeminfo;
		m_systeminfo = NULL;
	}
	if (m_gpu != NULL)
	{
		m_gpu->Stop();
		delete m_gpu;
		m_gpu = NULL;
	}
}

void SystemConfig::SetSocketLab(bool is)
{
	if (is)
		on_btnUITest_clicked();
}

void SystemConfig::OnCellClicked(int r, int c)
{
	//QString camname = ui.cameraTable->item(r, 1)->text();
	//m_currcam = camname;
	//GetCameraParam(camname);
}



void SystemConfig::InitTab()
{
	for (int i = 0; i < 8; i++)
	{
		m_istabinit[i] = false;
	}
	ui.tabWidget->tabBar()->setStyle(new CustomTabStyle);//注意，设置上述代码风格 就可以实现QTabBar横向

	m_systeminfo = new SystemInfo(NULL);
	//默认选择基本餐数
	InitTab0();
	ui.tabWidget->setCurrentIndex(0);
	connect(ui.tabWidget, SIGNAL(currentChanged(int)), this, SLOT(OnTabChange(int)));

	ui.tabWidget->removeTab(5);
}

//基本参数
void SystemConfig::InitTab1()
{
	//if (CiWaSocket::GetCurrMaster() == HostState::MASTE)
	//	ui.checkIsMaster->setChecked(true);
	//else
	//	ui.checkIsMaster->setChecked(false);
}

//用户
void SystemConfig::InitTab2()
{
	ui.oldpwd->setEchoMode(QLineEdit::Password);//设置密码隐藏
	ui.oldpwd->setMaxLength(16);//设置最大长度16位

	ui.newpwd->setEchoMode(QLineEdit::Password);//设置密码隐藏
	ui.newpwd->setMaxLength(16);//设置最大长度16位

	ui.renewpwd->setEchoMode(QLineEdit::Password);//设置密码隐藏
	ui.renewpwd->setMaxLength(16);//设置最大长度16位

	ui.gpwd->setEchoMode(QLineEdit::Password);//设置密码隐藏
	ui.gpwd->setMaxLength(16);//设置最大长度16位

	ui.grpwd->setEchoMode(QLineEdit::Password);//设置密码隐藏
	ui.grpwd->setMaxLength(16);//设置最大长度16位

	ui.username->clear();
	ui.username->addItem(QString("请选择账号"));

	QSqlQuery *ret;
	std::list<QString> key;
	std::list<std::list<QString>> row;
	//遍历用户名
	key.push_back("name");
	DatabaseSqlite::get_db()->Find("user_login", key, &row, &ret);

	if (row.size() < 1)
		return;

	while (row.size() > 1)
	{
		ui.username->addItem(row.back().back());
		row.pop_back();
	}

}

//网络
void SystemConfig::InitTab3()
{
	
	tableWidgetParent* table = new tableWidgetParent();
	table->setRowColumn(30, 9);
	table->init(QStringList() << "序号"
		<< "名称"
		<< "描述"
		<< "MAC地址"
		<< "IP地址"
		<< "子网掩码"
		<< "网关地址"
		<< "DNS"
		<< "状态");

	table->initHeaderStyle(":/system/resources/icons/system/line.png");
	table->firstLineCenter(true);
	table->setFirstLineCenter();

	QTableWidget* ptable = (QTableWidget*)table->getTablePoint();
	ptable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);


	//添加数据
	ULONG netcardnum = m_systeminfo->IPCard();
	ULONG friendnamenum = m_systeminfo->IPCardFriendlyName();

	if (!m_systeminfo->get_network_info() || !m_systeminfo->get_network_name())
		return;

	PIP_ADAPTER_INFO netcard = (IP_ADAPTER_INFO *)malloc(netcardnum);
	if (!netcard)
		return;
	PIP_ADAPTER_ADDRESSES friendname = (PIP_ADAPTER_ADDRESSES)malloc(friendnamenum);
	if (!friendname)
		return;

	memcpy((void*)netcard, (void*)m_systeminfo->get_network_info(), netcardnum);
	memcpy((void*)friendname, (void*)m_systeminfo->get_network_name(), netcardnum);

	QString strmac;
	QTableWidgetItem *newItem;
	int i = 0;
	std::map<QString, QString> tempname;
	while (friendname)
	{
		tempname[friendname->AdapterName] = QString::fromWCharArray(friendname->FriendlyName);
		if (friendname->Next)
			memcpy((void*)friendname, (void*)friendname->Next, sizeof(*friendname));
		else
		{
			free(friendname);
			friendname = NULL;
		}
	}
	while (netcard)
	{
		strmac.clear();
		newItem = new QTableWidgetItem(QString::number(i + 1));
		table->setItem(i, 0, newItem);
		table->setItemPosition(i, 0, Qt::AlignCenter);

		newItem = new QTableWidgetItem(tempname[netcard->AdapterName]);
		table->setItem(i, 1, newItem);

		newItem = new QTableWidgetItem(netcard->Description);
		table->setItem(i, 2, newItem);

		for (int j = 0; j < netcard->AddressLength; j++) {
			if (j == (netcard->AddressLength - 1))
				strmac.append(QString::number(netcard->Address[j], 16));
			else
				strmac.append(QString::number(netcard->Address[j], 16) + '-');
		}
		newItem = new QTableWidgetItem(strmac);
		table->setItem(i, 3, newItem);

		newItem = new QTableWidgetItem(netcard->IpAddressList.IpAddress.String);
		table->setItem(i, 4, newItem);

		newItem = new QTableWidgetItem(netcard->IpAddressList.IpMask.String);
		table->setItem(i, 5, newItem);

		newItem = new QTableWidgetItem(netcard->GatewayList.IpAddress.String);
		table->setItem(i, 6, newItem);

		QStringList strlist = m_systeminfo->GetDns(netcard);
		QString str;
		for (auto it = strlist.cbegin(); it != strlist.cend(); it++)
		{
			str.append(*it);
			str.append("\n");
		}
		str.chop(1);
		newItem = new QTableWidgetItem(str);
		table->setItem(i, 7, newItem);

		if (m_systeminfo->GetAdapterState(netcard))

			newItem = new QTableWidgetItem(QString("已连接"));
		else
			newItem = new QTableWidgetItem(QString("未连接"));
		table->setItem(i, 8, newItem);



		i++;
		if (netcard->Next)
		{
			memcpy((void*)(netcard), (void*)netcard->Next, sizeof(*netcard));
		}

		else
		{
			free(netcard);
			netcard = NULL;

		}

	}

	ui.gridLayout_4->addWidget(table,1,0);
	

/*
	ui.netconfig->setHorizontalHeaderLabels(QStringList() << "编号"
		<< "名称"
		<< "描述"
		<< "MAC地址"
		<< "IP地址"
		<< "子网掩码"
		<< "网关地址"
		<< "DNS"
		<< "状态");

	//ui.netconfig->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	//ui.netconfig->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.netconfig->horizontalHeader()->setStretchLastSection(true);
	ui.netconfig->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui.netconfig->setColumnWidth(0, 100);
	ui.netconfig->setColumnWidth(1, 200);
	ui.netconfig->setColumnWidth(2, 300);
	ui.netconfig->setColumnWidth(3, 200);
	ui.netconfig->setColumnWidth(4, 200);
	ui.netconfig->setColumnWidth(5, 200);
	ui.netconfig->setColumnWidth(6, 200);
	ui.netconfig->setColumnWidth(7, 200);
	ui.netconfig->setColumnWidth(8, 200);
	ULONG netcardnum = m_systeminfo->IPCard();
	ULONG friendnamenum = m_systeminfo->IPCardFriendlyName();

	if (!m_systeminfo->get_network_info() || !m_systeminfo->get_network_name())
		return;

	PIP_ADAPTER_INFO netcard = (IP_ADAPTER_INFO *)malloc(netcardnum);
	if (!netcard)
		return;
	PIP_ADAPTER_ADDRESSES friendname = (PIP_ADAPTER_ADDRESSES)malloc(friendnamenum);
	if (!friendname)
		return;

	memcpy((void*)netcard, (void*)m_systeminfo->get_network_info(), netcardnum);
	memcpy((void*)friendname, (void*)m_systeminfo->get_network_name(), netcardnum);

	QString strmac;
	QTableWidgetItem *newItem;
	int i = 0;
	std::map<QString, QString> tempname;
	while (friendname)
	{
		tempname[friendname->AdapterName] = QString::fromWCharArray(friendname->FriendlyName);
		if (friendname->Next)
			memcpy((void*)friendname, (void*)friendname->Next, sizeof(*friendname));
		else
		{
			free(friendname);
			friendname = NULL;
		}
	}

	while (netcard)
	{
		strmac.clear();
		newItem = new QTableWidgetItem(QString::number(i + 1));
		ui.netconfig->setItem(i, 0, newItem);

		newItem = new QTableWidgetItem(tempname[netcard->AdapterName]);
		ui.netconfig->setItem(i, 1, newItem);

		newItem = new QTableWidgetItem(netcard->Description);
		ui.netconfig->setItem(i, 2, newItem);

		for (int j = 0; j < netcard->AddressLength; j++) {
			if (j == (netcard->AddressLength - 1))
				strmac.append(QString::number(netcard->Address[j], 16));
			else
				strmac.append(QString::number(netcard->Address[j], 16) + '-');
		}
		newItem = new QTableWidgetItem(strmac);
		ui.netconfig->setItem(i, 3, newItem);

		newItem = new QTableWidgetItem(netcard->IpAddressList.IpAddress.String);
		ui.netconfig->setItem(i, 4, newItem);

		newItem = new QTableWidgetItem(netcard->IpAddressList.IpMask.String);
		ui.netconfig->setItem(i, 5, newItem);

		newItem = new QTableWidgetItem(netcard->GatewayList.IpAddress.String);
		ui.netconfig->setItem(i, 6, newItem);

		newItem = new QTableWidgetItem(m_systeminfo->GetDns(netcard));
		ui.netconfig->setItem(i, 7, newItem);

		if (m_systeminfo->GetAdapterState(netcard))
			newItem = new QTableWidgetItem(QString("已连接"));
		else
			newItem = new QTableWidgetItem(QString("未连接"));
		ui.netconfig->setItem(i, 8, newItem);



		i++;
		if (netcard->Next)
		{
			memcpy((void*)(netcard), (void*)netcard->Next, sizeof(*netcard));
		}

		else
		{
			free(netcard);
			netcard = NULL;

		}

	}
	*/
	//1 网络设置
	CiWaSocket::CiWaSocketIni();
	ui.serverIpEdit->setText(CiWaSocket::GetServerIp());
	ui.serverPortEdit->setText(QString::number(CiWaSocket::GetServerPort()));
	ui.localIpEdit->setText(CiWaSocket::GetLocalIp());
	ui.localPortEdit->setText(QString::number(CiWaSocket::GetLocalPort()));

	if (CiWaSocket::GetHostState() == HostState::MASTE)
	{
		ui.radioIsMaster->setChecked(true);
		this->on_radioIsMaster_clicked();
	}
	else
	{
		ui.radioIsSlave->setChecked(true);
		this->on_radioIsSlave_clicked();
	}

	if (CiWaSocket::GetIsOpenNet() == 1)
	{
		m_isnetwork = false;
		ui.btnSwitch->setChecked(true);
	}
	else
	{
		m_isnetwork = true;
		ui.btnSwitch->setChecked(false);
	}
	this->on_ptnNetwork_clicked();

	//2 PLC设置
	ui.plcIpEdit->setText(CiWaSocket::GetPLCIp());
	ui.PLCPortEdit->setText(QString::number(CiWaSocket::GetPLCPort()));

	//设置单选滑块的大小
	ui.btnSwitch->resize(44,22);
	ui.btnSwitch->setMouseTracking(true);
	//隐藏关闭网络的按钮
	ui.ptnNetwork->hide();
	//隐藏连接未成功和提示
	ui.textBrowserFail->hide();
	ui.labSuccess->hide();
	ui.labFail->hide();
	ui.checkIsMaster->hide();
	ui.btnUITest->hide();
	ui.btnUITest_2->hide();

	//编辑设置
	on_btnEdit_clicked();
	//网络连接开关
	connect(ui.btnSwitch, SIGNAL(checkedChanged(bool)), this, SLOT(on_btnSwitch_change(bool)));


}

//相机
void SystemConfig::InitTab4()
{

	ui.ReverseX->hide();
	ui.external_trigger->hide();
	 
	ui.btnCameraEdit->hide();
	ui.btnSet->hide();
	ui.btnReload->hide();
	

#ifdef USECAMERADLL
    auto cameralist = CCameraManage::EnumerateALLDevices();

    tableWidgetParent* table;
    if (m_cameratable == NULL) {
        table = new tableWidgetParent();
        m_cameratable = static_cast<void*>(table);
    }
    else {
        table = static_cast<tableWidgetParent*>(m_cameratable);
        
    }
    QTableWidget* ptable = (QTableWidget*)table->getTablePoint();
    ptable->clear(); 

	table->setRowColumn(30, 8);
	table->init(QStringList() << "序号"
		<< "设备ID"
		<< "供应商"
		<< "型号"
		<< "序列号"
		<< "固件版本"
		<< "地址"
		<< "操作");

	table->initHeaderStyle(":/system/resources/icons/system/line.png");
	table->firstLineCenter(true);
	table->setFirstLineCenter();
	
	ptable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
	table->setHeaderColumnPosition(7,Qt::AlignCenter);
	QString strmac;
	QTableWidgetItem *newItem;
	int i = 0;
	for (int i = 0; i < cameralist.size(); i++)
	{
		strmac.clear();
		newItem = new QTableWidgetItem(QString::number(i + 1));
		table->setItem(i, 0, newItem);
		table->setItemPosition(i, 0, Qt::AlignCenter);

        newItem = new QTableWidgetItem(QString::fromLocal8Bit(cameralist[i].chDevName));
        table->setItem(i, 1, newItem);
        //供应商
        newItem = new QTableWidgetItem(QString::fromLocal8Bit(cameralist[i].chSoftVer));
        table->setItem(i, 2, newItem);
        //型号
        newItem = new QTableWidgetItem(QString::fromLocal8Bit(cameralist[i].chSysVer));
        table->setItem(i, 3, newItem);

        newItem = new QTableWidgetItem(QString::fromLocal8Bit(cameralist[i].chSN));
        table->setItem(i, 4, newItem);

        newItem = new QTableWidgetItem(QString::fromLocal8Bit(cameralist[i].chHardVer));
        table->setItem(i, 5, newItem);

        switch (cameralist[i].type)
        {
        case cameratype::CAMERA_FILE:
            newItem = new QTableWidgetItem(QString::fromLocal8Bit(cameralist[i].chFileAdd));
            break;
        case cameratype::CAMERA_INVENTED:
            newItem = new QTableWidgetItem(QString::fromLocal8Bit(cameralist[i].chFileAdd));
            break;
        case cameratype::CAMERA_BASLER:
            newItem = new QTableWidgetItem(QString::fromLocal8Bit(cameralist[i].chIp));
            break;
        case cameratype::CAMERA_HIK:
            newItem = new QTableWidgetItem(QString::fromLocal8Bit(cameralist[i].chIp));
            break;
        }
        table->setItem(i, 6, newItem);
#endif

        QToolButton* toolbutton = new QToolButton();
		toolbutton->setText("查看配置");
		
		toolbutton->setStyleSheet("color: #77B4BE;border:none;background-color: rgba(0, 0, 0,0);text-align:left; ");
		table->addTtemButton(i,7,toolbutton);
		QObject::connect(toolbutton, SIGNAL(clicked()), this, SLOT(OnCameraConfigClicked()));
	}
	
	//ui.verticalLayout->addWidget(table)
	ui.gridLayout_5->addWidget(table, 1, 0);

	connect(ui.btnSPFZ, SIGNAL(checkedChanged(bool)), this, SLOT(on_btnSPFZ_change(bool)));

/*
	ui.cameraTable->horizontalHeader()->setVisible(true);
	ui.cameraTable->setHorizontalHeaderLabels(QStringList() << "序号"
		<< "设备ID"
		<< "供应商"
		<< "型号"
		<< "序列号"
		<< "固件版本"
		<< "工位");
	ui.cameraTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	std::vector<std::string> cameralist;
	cameralist = camera::enumeratedevices();

	QString strmac;
	QTableWidgetItem *newItem;
	int i = 0;

	for (int i = 0; i < cameralist.size(); i++)
	{
		strmac.clear();
		newItem = new QTableWidgetItem(QString::number(i + 1));
		ui.cameraTable->setItem(i, 0, newItem);

		CameraInfo camerainfo;
		camerainfo.devicename = cameralist[i];
		camera::get_deviceinfo(camerainfo);

		newItem = new QTableWidgetItem(QString::fromStdString(camerainfo.devicename));
		ui.cameraTable->setItem(i, 1, newItem);

		newItem = new QTableWidgetItem(QString::fromStdString(camerainfo.vendor));
		ui.cameraTable->setItem(i, 2, newItem);

		newItem = new QTableWidgetItem(QString::fromStdString(camerainfo.model));
		ui.cameraTable->setItem(i, 3, newItem);

		newItem = new QTableWidgetItem(QString::fromStdString(camerainfo.id));
		ui.cameraTable->setItem(i, 4, newItem);

		newItem = new QTableWidgetItem(QString::fromStdString(camerainfo.version));
		ui.cameraTable->setItem(i, 5, newItem);
	}
*/
}

//系统
void SystemConfig::InitTab5()
{
	ui.hostname->setText(m_systeminfo->LocalMachineName());
	ui.system->setText(m_systeminfo->OSVersion());
	ui.cpu->setText(m_systeminfo->CPUType());
	ui.showview->setText(m_systeminfo->GraphicsCard());
	ui.perform->start(2000);
	if (m_gpu == NULL)
	{
		m_gpu = new GpuMemoryLabel();
		m_gpu->Start(ui.GpuLayoutText, ui.GpuLayout);
	}


}

//工位显示
void SystemConfig::InitTab8()
{
	m_pwindow = NULL;
	//1获得显示器到下拉列表
	QDesktopWidget* pdesktop = QApplication::desktop();
	for (int i = 0; i < pdesktop->screenCount(); i++)
	{
		ui.comDisplay->addItem(QString::number(i));
	}
	//2获得工位到下拉列表
	m_listwindow = FenpIni::get_slist();
	ui.comWindow->addItems(m_listwindow);

	//3建立连接两个连接
	QObject::connect(ui.comDisplay, SIGNAL(currentIndexChanged(int)), this, SLOT(OnDDisplay(int)));
	QObject::connect(ui.comWindow, SIGNAL(currentIndexChanged(int)), this, SLOT(OnDWindow(int)));
	
	on_btnShowRefresh_clicked();
}

void SystemConfig::GetCameraParam(QString camname)
{
	double value = 0;
//
//#ifndef USECAMERADLL
//	CameraParam tempcam;
//	tempcam.devicename = camname.toStdString();
//
//	tempcam.type = GET_EXPOSURETIME;
//	camera::parameteraccess(tempcam);
//	value = tempcam.exposure_time;
//	//曝光
//	ui.exposure_time->setValue(value);
//
//	tempcam.type = GET_ANALOGGAIN;
//	camera::parameteraccess(tempcam);
//	value = tempcam.analog_gain;
//	//模拟
//	ui.analog_gain->setValue(value);
//
//	tempcam.type = GET_DIGITALGAIN;
//	camera::parameteraccess(tempcam);
//	value = tempcam.digital_gain;
//	//数字
//	ui.digital_gain->setValue(value);
//
//	bool ischecked = false;
//	tempcam.type = GET_REVERSEX;
//	camera::parameteraccess(tempcam);
//	ischecked = tempcam.ReverseX;
//	//翻转X
//	ui.ReverseX->setChecked(ischecked);
//	ui.btnSPFZ->setChecked(ischecked);
//	
//	tempcam.type = GET_TRIGGERMODE;
//	camera::parameteraccess(tempcam);
//	ischecked = tempcam.external_trigger;
//	//触发模式
//	ui.external_trigger->setChecked(ischecked);
//	//true 外部 false 内部
//	if (ischecked)
//	{
//		ui.radioExternalTrigger->setChecked(true);
//		ui.radioInternalTrigger->setChecked(false);
//		
//	}
//	else
//	{
//		ui.radioInternalTrigger->setChecked(true);
//		ui.radioExternalTrigger->setChecked(false);
//	}
//#endif
}

void SystemConfig::SetCameraParam(QString camname)
{
	if (camname.isEmpty())
		return;
//#ifndef USECAMERADLL
//	CameraParam tempcam;
//	tempcam.devicename = camname.toStdString();
//
//	//曝光
//	tempcam.type = SET_EXPOSURETIME;
//	tempcam.exposure_time = ui.exposure_time->value();
//	camera::parameteraccess(tempcam);
//
//	//模拟
//	tempcam.analog_gain = (int)ui.analog_gain->value();
//	tempcam.type = SET_ANALOGGAIN;
//	camera::parameteraccess(tempcam);
//
//	//数字	
//	tempcam.digital_gain = (int)ui.digital_gain->value();
//	tempcam.type = SET_DIGITALGAIN;
//	camera::parameteraccess(tempcam);
//
//	//翻转X
//	//tempcam.ReverseX = ui.ReverseX->isChecked();
//	tempcam.ReverseX = ui.btnSPFZ->isChecked();
//	tempcam.type = SET_REVERSEX;
//	camera::parameteraccess(tempcam);
//
//	//触发模式
//	//tempcam.external_trigger = ui.external_trigger->isChecked();
//	
//	tempcam.external_trigger = ui.radioExternalTrigger->isChecked();
//	tempcam.type = SET_TRIGGERMODE;
//	camera::parameteraccess(tempcam);
//#endif
}

void SystemConfig::on_btnSet_clicked()
{
	SetCameraParam(m_currcam);
	//ui.btnCameraEdit->show();
	ui.btnSet->hide();
	ui.btnReload->hide();
	ui.btnCameraEdit->hide();
}

void SystemConfig::on_modifypwd_clicked()
{
	QSqlQuery *ret;
	std::map<QString, QString> dbwhere;
	std::map<QString, QString> dbdata;
	QString oldpwd = ui.oldpwd->text();
	QString newpwd = ui.newpwd->text();
	QString renewpwd = ui.renewpwd->text();

	if (oldpwd == "" || newpwd == "" || renewpwd == "")
	{
		//QMessageBox::warning(this, "", "密码不能为空");
		CiWaMsgDialog::MessageT("修改失败", "密码不能为空");
		return;
	}
	if (QString::compare(newpwd, renewpwd) != 0)
	{
		//QMessageBox::warning(this, "", "两次密码不一致");
		CiWaMsgDialog::MessageT("修改失败", "两次密码不一致");
		return;
	}
	//查询数据库
	QString user = ui.username->currentText();
	QString str = QString("select * from user_login where name='%1' and pwd='%2' ").arg(user).arg(oldpwd);
	if (DatabaseSqlite::get_db()->Query(str, &ret))
	{
		dbwhere["name"] = user;
		dbdata["pwd"] = newpwd;
		//修改密码
		if (DatabaseSqlite::get_db()->Updata("user_login", dbwhere, dbdata, &ret))
			CiWaMsgDialog::MessageT("修改成功", "");
		else
			CiWaMsgDialog::MessageT("修改失败", "数据库更新失败");
	}
	else
	{
		CiWaMsgDialog::MessageT("修改失败", "密码错误");
	}
	ClearUserEdit();
	InitTab2();
}
/***************************************************************
*  @brief     账号管理编辑栏合法检测,添加删除用户
*  @param     true添加用户，false删除用户
*  @note      
**************************************************************/
bool SystemConfig::UserManage(bool is)
{
	QString mes;
	mes = is ? QString("添加") : QString("删除");

	if (ui.gpwd->text().isEmpty() || ui.grpwd->text().isEmpty() || ui.gusername->text().isEmpty())
	{
		CiWaMsgDialog::MessageT(mes + "失败", "输入不能为空");
		return false;
	}

	QString user = ui.gusername->text();
	QString pwd = ui.gpwd->text();
	QString rpwd = ui.grpwd->text();

	if (QString::compare(pwd, rpwd) != 0)
	{
		CiWaMsgDialog::MessageT(mes + "失败", "两次密码不一致");
		return false;
	}

	QSqlQuery * ret;
	std::list<QString> key;
	std::list<std::list<QString>> row;
	std::map<QString, QString> dbwhere;
	std::map<QString, QString> dbdata;

	dbwhere["name"] = user;
	dbdata["name"] = user;
	dbdata["pwd"] = pwd;
	key.push_back("*");
	if (!DatabaseSqlite::get_db()->Find("user_login", key, dbwhere, &row, &ret))
	{
		CiWaMsgDialog::MessageT(mes + "失败", "数据库查询失败");
		return false;
	}

	if (is)
	{//添加

		if (row.size() > 0)
		{
			CiWaMsgDialog::MessageT(mes + "失败", "该用户名已经存在");
			return false;
		}
		else
		{

			if (DatabaseSqlite::get_db()->Add("user_login", dbdata, &ret))
			{
				CiWaMsgDialog::MessageT(mes + "成功");
				return true;
			}
			else
			{
				CiWaMsgDialog::MessageT(mes + "失败", "数据库添加失败");
				return false;
			}
		}
		
	}
	else
	{//删除
		if (row.size() > 0)
		{
			if (DatabaseSqlite::get_db()->Del("user_login", dbwhere, &ret))
			{
				CiWaMsgDialog::MessageT(mes + "成功");
				return true;
			}
			else 
			{
				CiWaMsgDialog::MessageT(mes + "失败", "数据库删除失败");
				return false;
			}
		}
		else
		{
			CiWaMsgDialog::MessageT(mes + "失败", "该用户名已经存在");
			return false;
		}
	}


}
//清空输入栏
void SystemConfig::ClearUserEdit()
{
	ui.oldpwd->setText("");
	ui.newpwd->setText("");
	ui.renewpwd->setText("");
	ui.gusername->setText("");
	ui.gpwd->setText("");
	ui.grpwd->setText("");
}

//添加用户按钮
void SystemConfig::on_addpwd_clicked()
{
	UserManage(true);
	ClearUserEdit();
	InitTab2();
}
//删除用户按钮
void SystemConfig::on_delpwd_clicked()
{
	UserManage(false);
	ClearUserEdit();
	InitTab2();

}

void SystemConfig::on_btnNetTest_clicked()
{
	this->on_btnUITest_2_clicked();
	CiWaSocket::NetWorkTest();
}

void SystemConfig::on_ptnNetwork_clicked()
{
	if (!m_isnetwork)	//未开启
	{
		//开启
		m_isnetwork = true;
		HostState state;
		if (ui.radioIsMaster->isChecked())
		{
			state = HostState::MASTE;
			//2
			CiWaSocket::NetServerWorkParam(ui.localIpEdit->text(), ui.localPortEdit->text().toUInt());
		}
		else
		{
			state = HostState::SLAVE;
			//2
			CiWaSocket::NetServerWorkParam(ui.serverIpEdit->text(), ui.serverPortEdit->text().toUInt());
			CiWaSocket::NetLocalWorkParam(ui.localIpEdit->text(), ui.localPortEdit->text().toUInt());
		}

		//3
		CiWaSocket::SetCurrMaster(state);
		//4
		CiWaSocket::NetWorkStart();
		ui.ptnNetwork->setText(QString("关闭网络"));

		ui.labIsOpenNet->setText("开");

	}
	else
	{
		ui.ptnNetwork->setText(QString("开启网络"));
		m_isnetwork = false;
		CiWaSocket::NetWorkStop();

		ui.labIsOpenNet->setText("关");
	}
}

void SystemConfig::on_btnSocketSave_clicked()
{

	CiWaSocket::NetServerWorkParam(ui.serverIpEdit->text(), ui.serverPortEdit->text().toUInt());
	CiWaSocket::NetLocalWorkParam(ui.localIpEdit->text(), ui.localPortEdit->text().toUInt());
	
	HostState state;
	if (ui.radioIsMaster->isChecked())
		state = HostState::MASTE;
	else
		state = HostState::SLAVE;
	CiWaSocket::SetCurrMaster(state);

	//是否打开网络
	if (m_isnetwork)
		CiWaSocket::SetIsOpenNet(1);
	else
		CiWaSocket::SetIsOpenNet(0);

	CiWaSocket::CiWaSocketSetIni();

	//编辑框启用
	ui.btnEdit->setChecked(false);
	on_btnEdit_clicked();


}

//设为主机槽
void SystemConfig::on_radioIsMaster_clicked()
{
	if (ui.radioIsMaster->isChecked())
	{
		//隐藏主机地址等设置
		ui.labMaster->hide();
		ui.serverIpEdit->hide();
		ui.serverPortEdit->hide();
		ui.labMasterPort->hide();
		ui.btnNetTest->hide();
		ui.labSuccess->hide();
		ui.labFail->hide();
		ui.textBrowserFail->hide();
		update();
	}
}

//设为从机槽
void SystemConfig::on_radioIsSlave_clicked()
{
	if (ui.radioIsSlave->isChecked())
	{
		//显示主机地址等设置
		ui.labMaster->show();
		ui.serverIpEdit->show();
		ui.serverPortEdit->show();
		ui.labMasterPort->show();
		ui.btnNetTest->show();
		//ui.labSuccess->show();
		//ui.labFail->show();
		//ui.textBrowserFail->show();
		update();
	}

}

//连接成功显示
void SystemConfig::on_btnUITest_clicked()
{
	ui.labSuccess->show();
	ui.labFail->hide();
	update();
}

//连接失败显示
void SystemConfig::on_btnUITest_2_clicked()
{
	ui.labFail->show();
	ui.labSuccess->hide();
	update();
}

void SystemConfig::on_btnEdit_clicked()
{
	QString styt = "border:1px solid;border-color:rgb(72, 72, 72);color: rgba(255, 255, 255, 153); background-color: rgb(21, 21, 21);";
	QString styf = "border:1px solid;border-color:rgb(72, 72, 72);color: rgba(255, 255, 255, 153); background-color: rgba(67,67,69,0.40);";
	if (ui.btnEdit->isChecked())
	{
		ui.btnEdit->setStyleSheet("	color: rgba(72, 162, 255, 0.40);border: none; ");
		ui.btnEdit->setEnabled(false);
		//置恢复
		ui.plcIpEdit->setEnabled(true);
		ui.plcIpEdit->setStyleSheet(styt);
	
		ui.PLCPortEdit->setEnabled(true);
		ui.PLCPortEdit->setStyleSheet(styt);

		ui.localIpEdit->setEnabled(true);
		ui.localIpEdit->setStyleSheet(styt);

		ui.localPortEdit->setEnabled(true);
		ui.localPortEdit->setStyleSheet(styt);

		ui.serverIpEdit->setEnabled(true);
		ui.serverIpEdit->setStyleSheet(styt);

		ui.serverPortEdit->setEnabled(true);
		ui.serverPortEdit->setStyleSheet(styt);

		ui.radioIsMaster->setEnabled(true);
		ui.radioIsSlave->setEnabled(true);
		ui.btnSwitch->setEnabled(true);

		ui.btnSocketSave->show();
		ui.btnSocketChanel->show();

	}
	else
	{
	
		ui.btnEdit->setStyleSheet("color: rgb(72, 162, 255);border: none; ");
		ui.btnEdit->setEnabled(true);
		
		//灰色
		ui.plcIpEdit->setEnabled(false);
		ui.plcIpEdit->setStyleSheet(styf);

		ui.PLCPortEdit->setEnabled(false);
		ui.PLCPortEdit->setStyleSheet(styf);

		ui.localIpEdit->setEnabled(false);
		ui.localIpEdit->setStyleSheet(styf);

		ui.localPortEdit->setEnabled(false);
		ui.localPortEdit->setStyleSheet(styf);

		ui.serverIpEdit->setEnabled(false);
		ui.serverIpEdit->setStyleSheet(styf);

		ui.serverPortEdit->setEnabled(false);
		ui.serverPortEdit->setStyleSheet(styf);

		ui.radioIsMaster->setEnabled(false);
		ui.radioIsSlave->setEnabled(false);
		ui.btnSwitch->setEnabled(false);
	
		ui.btnSocketSave->hide();
		ui.btnSocketChanel->hide();

	}
}

void SystemConfig::on_btnSocketChanel_clicked()
{
	ui.serverIpEdit->setText(CiWaSocket::GetServerIp());
	ui.serverPortEdit->setText(QString::number(CiWaSocket::GetServerPort()));
	ui.localIpEdit->setText(CiWaSocket::GetLocalIp());
	ui.localPortEdit->setText(QString::number(CiWaSocket::GetLocalPort()));

	if (CiWaSocket::GetHostState() == HostState::MASTE)
	{
		ui.radioIsMaster->setChecked(true);
		this->on_radioIsMaster_clicked();
	}
	else
	{
		ui.radioIsSlave->setChecked(true);
		this->on_radioIsSlave_clicked();
	}

	if (CiWaSocket::GetIsOpenNet() == 1)
	{
		m_isnetwork = false;
		ui.btnSwitch->setChecked(true);
	}
	else
	{
		m_isnetwork = true;
		ui.btnSwitch->setChecked(false);
	}

	this->on_ptnNetwork_clicked();

	//2 PLC设置
	ui.plcIpEdit->setText(CiWaSocket::GetPLCIp());
	ui.PLCPortEdit->setText(QString::number(CiWaSocket::GetPLCPort()));


	//编辑框启用
	ui.btnEdit->setChecked(false);
	on_btnEdit_clicked();
}

void SystemConfig::on_btnSwitch_change(bool is)
{
	if (is == m_isnetwork)
		return;
	on_ptnNetwork_clicked();
}

void SystemConfig::on_btnSPFZ_change(bool is)
{
	if (is == ui.ReverseX->isChecked())
		return;
	if(is)
		ui.labSPFZ->setText("开");
	else
		ui.labSPFZ->setText("关");
}

void SystemConfig::OnCameraConfigClicked()
{
	QToolButton* toolbutton = qobject_cast<QToolButton *>(sender());
	int x = toolbutton->frameGeometry().x();
	int y = toolbutton->frameGeometry().y();

	tableWidgetParent* table = (tableWidgetParent*) ui.gridLayout_5->itemAt(1)->widget();
	if (!table)
		return;
	QModelIndex index = table->getIndex(x, y);
	int row = index.row();
	
	QString cameraid = table->getTableData(row, 1);
	m_currcam = cameraid;
	GetCameraParam(m_currcam);

	ui.labCamera->setText(m_currcam);
	ui.btnCameraEdit->show();
	ui.btnSet->hide();
	ui.btnReload->hide();
}

void SystemConfig::on_btnCameraEdit_clicked()
{
	ui.btnCameraEdit->hide();
	ui.btnSet->show();
	ui.btnReload->show();
}

void SystemConfig::on_btnReload_clicked()
{
	GetCameraParam(m_currcam);
	ui.btnSet->hide();
	ui.btnReload->hide();
	ui.btnCameraEdit->hide();
}

//显示工位刷新
void SystemConfig::on_btnShowRefresh_clicked()
{
	int index = ui.comDisplay->currentIndex();
	OnDDisplay(index);
	ui.checkAutoLoad->setChecked(FenpIni::IsOpen());
	ui.checkAutoLoadOpen->setChecked(FenpIni::GetAutoOpen());
}

//屏幕选择
void SystemConfig::OnDDisplay(int index)
{
	defenp fdata = FenpIni::GetData();

	if (fdata.count(index) < 1)
		return;

	int view = fdata.at(index).viewnum;

	if (view < m_listwindow.count())
	{
		if (view != ui.comWindow->currentIndex())
			ui.comWindow->setCurrentIndex(view);
		else
			OnDWindow(view);
	}
}

//重置
void SystemConfig::on_btnShowReset_clicked()
{
	Windowing* pwi = (Windowing*)m_pwindow;
	QLineEdit* plab;
	int i = 0;
	auto iter = pwi->m_mapview.begin();
	while (iter != pwi->m_mapview.end())
	{
		plab = (QLineEdit*)iter->second;
		plab->setText(QString::number(i++));
		iter++;
	}
	ui.checkAutoLoad->setChecked(false);
}

//视口保存
void SystemConfig::on_btnShowSave_clicked()
{
	defenp fdata = FenpIni::GetData();
	defenpwin fwdata;
	Windowing* pwi = (Windowing*)m_pwindow;
	QLineEdit* plab;
	if (pwi == NULL)
		return;
	int dis = ui.comDisplay->currentIndex();
	int win = ui.comWindow->currentIndex();

	fwdata.viewnum = win;
	auto iter = pwi->m_mapview.begin();
	while (iter != pwi->m_mapview.end())
	{
		plab = (QLineEdit*)iter->second;
		if (plab->text().isEmpty())
			fwdata.dfw[iter->first] = -1;
		else
			fwdata.dfw[iter->first] = plab->text().toInt();
		iter++;
	}
	fdata[dis].dfw.clear();
	fdata[dis] = fwdata;
	FenpIni::SetOpen(ui.checkAutoLoad->isChecked());
	FenpIni::SetAutoOpen(ui.checkAutoLoadOpen->isChecked());
	FenpIni::WriteIni(fdata);

	//发送信号刷新主界面窗口
	emit SendCustomFenpSignal();
}

//相机断开
void SystemConfig::on_btnCloseDevice_clicked()
{
#ifdef USECAMERADLL
    CCameraManage::Close();
#endif
}

//相机重载
void SystemConfig::on_btnReLoadDevice_clicked()
{
#ifdef USECAMERADLL
    //1重载
    CCameraManage::ReLoadAllDevices();
    //2刷新界面
    this->InitTab4();
    //3将重载的设备建立连接
    CCameraManage::Open();
#endif
}

//视口选择
void SystemConfig::OnDWindow(int index)
{
	int winid, i;
	//1界面移除
	Windowing* pwi = (Windowing*)m_pwindow;
	if (pwi != NULL)
	{
		ui.layoutWindow->removeWidget(pwi);
		delete pwi;
		m_pwindow = NULL;
	}
	winid = ui.comDisplay->currentIndex();
	i = m_listwindow.at(ui.comWindow->currentIndex()).toInt();
	pwi = new Windowing(this, i, winid, 1);
	ui.layoutWindow->addWidget(pwi);
	m_pwindow = pwi;

	defenp fdata = FenpIni::GetData();
	if (fdata.count(winid - 1) > 0)
	{
		int findex = m_listwindow.at(fdata.at(winid - 1).viewnum).toInt();
		auto iterpwi = pwi->m_mapview.begin();
		while (iterpwi != pwi->m_mapview.end())
		{
			QLineEdit *p = (QLineEdit*)iterpwi->second;
			p->setText(QString::number(findex++));
			iterpwi++;
		}
	}

	//读取配置
	//1窗口id

	if (fdata.count(winid) < 1)
		return;

	if (index != fdata.at(winid).viewnum)
		return;

	//2窗口对应
	QLineEdit* plab;
	auto iter = fdata.at(winid).dfw.begin();
	while (iter != fdata.at(winid).dfw.end())
	{
		if (iter->first < pwi->m_mapview.size())
		{
			plab = (QLineEdit*)pwi->m_mapview.at(iter->first);
			if (iter->second < 0)
				plab->setText(QString(""));
			else
				plab->setText(QString::number(iter->second));

		}
		iter++;
	}
	ui.checkAutoLoad->setChecked(FenpIni::IsOpen());
}

int SystemConfig::GetBitStatu(int num, int pos)
{
	if (num & (1 << pos)) 
	{
		//按位与之后的结果为1
		return 1;
	}
	else
	{
		//按位与之后的结果为0
		return 0;
	}
}

void SystemConfig::OnReadBaseParam() 
{
	QString path = QCoreApplication::applicationDirPath() + '/' + QString(ININAME_SYSTEM);
	QSettings configiniSystem(path, QSettings::IniFormat);
	configiniSystem.setIniCodec("UTF-8");
	configiniSystem.beginGroup("Baseinfo");
	QString str;
	int num;

	str = configiniSystem.value("bBadProductRateAlarm").toString();
	if (str == "true")
	{
		ui.bBadProductRateAlarm->setChecked(true);
		baseparam::set_badproductratealarm(true);
	}
	else
	{
		ui.bBadProductRateAlarm->setChecked(false);
		baseparam::set_badproductratealarm(false);
	}
	num = configiniSystem.value("BadProductRateThr").toInt();
	ui.BadProductRateThr->setValue(num);
	baseparam::set_badproductratethr(num);

	str = configiniSystem.value("bProductBlockedAlarm").toString();
	if (str == "true")
	{
		ui.bProductBlockedAlarm->setChecked(true);
		baseparam::set_productblockedalarm(true);
	}
	else
	{
		ui.bProductBlockedAlarm->setChecked(false);
		baseparam::set_productblockedalarm(false);
	}
	num = configiniSystem.value("ProductBlockedTime").toInt();
	ui.ProductBlockedTime->setValue(num);
	baseparam::set_productblockedalarmtime(num);

	str = configiniSystem.value("bAutoSave").toString();
	if (str == "true")
	{
		ui.bAutoSave->setChecked(true);
		baseparam::set_autosave(true);
	}
	else
	{
		ui.bAutoSave->setChecked(false);
		baseparam::set_autosave(false);
	}
	num = configiniSystem.value("AutoSaveNum").toInt();
	ui.AutoSaveNum->setValue(num);
	baseparam::set_autosavenum(num);
	str = configiniSystem.value("AutoSaveType").toString();
	ui.AutoSaveType->setCurrentText(str);
	if (0 == ui.AutoSaveType->currentIndex()) {
		baseparam::set_autosavetype(BULIANGPIN);
	}
	else {
		baseparam::set_autosavetype(LIANGPIN);
	}

	str = configiniSystem.value("bBadProductRateAlarmStop").toString();
	if (str == "true")
	{
		ui.bBadProductRateAlarmStop->setChecked(true);
		baseparam::set_badproductratealarmstop(true);
	}
	else
	{
		ui.bBadProductRateAlarmStop->setChecked(false);
		baseparam::set_badproductratealarmstop(false);
	}

	str = configiniSystem.value("bProductBlockedAlarmStop").toString();
	if (str == "true")
	{
		ui.bProductBlockedAlarmStop->setChecked(true);
		baseparam::set_productblockedalarmstop(true);
	}
	else
	{
		ui.bProductBlockedAlarmStop->setChecked(false);
		baseparam::set_productblockedalarmstop(false);
	}
    //自动加载最近一次检测方案
    str = configiniSystem.value("bUserLastSolution").toString();
    if (str == "true")
    {
        ui.bUserLastSolution->setChecked(true);
    }
    else
    {
        ui.bUserLastSolution->setChecked(false);
    }
    //开机自动登入为操作员
    str = configiniSystem.value("bLoginAsOperator").toString();
    if (str == "true")
    {
        ui.bLoginAsOperator->setChecked(true);
    }
    else
    {
        ui.bLoginAsOperator->setChecked(false);
    }

	configiniSystem.endGroup();
}

void SystemConfig::OnTabChange(int index)
{
	if (m_istabinit[index])
		return;

	switch (index)
	{
	case 0:
		InitTab0();
		break;
	case 1:
		InitTab2();
		break;
	case 2:
		InitTab3();
		break;
	case 3:
		InitTab4();
		break;
	case 4:
		InitTab5();
		break;
	case 5:
		break;
	case 6:
		InitTab8();
		break;
	default:
		break;
	}

	update();
	m_istabinit[index] = true;

	/*InitTab1();
InitTab2();
InitTab3();
InitTab4();
InitTab5();*/

}

void  SystemConfig::OnSaveBaseParam() 
{
	QString pathSystem = QCoreApplication::applicationDirPath() + '/' + QString(ININAME_SYSTEM);
	QSettings configiniSystem(pathSystem, QSettings::IniFormat);
	configiniSystem.setIniCodec("UTF8");
	configiniSystem.beginGroup("Baseinfo");
	QString str;
	if (ui.bBadProductRateAlarm->isChecked())
	{
		configiniSystem.setValue("bBadProductRateAlarm", true);
		baseparam::set_badproductratealarm(true);
	}
	else
	{
		configiniSystem.setValue("bBadProductRateAlarm", false);
		baseparam::set_badproductratealarm(false);
	}
	str = QString::number(ui.BadProductRateThr->value());
	configiniSystem.setValue("BadProductRateThr", str);
	baseparam::set_badproductratethr(str.toInt());

	if (ui.bProductBlockedAlarm->isChecked())
	{
		configiniSystem.setValue("bProductBlockedAlarm", true);
		baseparam::set_productblockedalarm(true);
	}
	else
	{
		configiniSystem.setValue("bProductBlockedAlarm", false);
		baseparam::set_productblockedalarm(false);
	}
	str = QString::number(ui.ProductBlockedTime->value());
	configiniSystem.setValue("ProductBlockedTime", str);
	baseparam::set_productblockedalarmtime(str.toInt());

	if (ui.bAutoSave->isChecked())
	{
		configiniSystem.setValue("bAutoSave", true);
		baseparam::set_autosave(true);
	}
	else
	{
		configiniSystem.setValue("bAutoSave", false);
		baseparam::set_autosave(false);
	}
	str = QString::number(ui.AutoSaveNum->value());
	configiniSystem.setValue("AutoSaveNum", str);
	baseparam::set_autosavenum(str.toInt());
	str = ui.AutoSaveType->currentText();
	configiniSystem.setValue("AutoSaveType", str);
	if (0 == ui.AutoSaveType->currentIndex()) {
		baseparam::set_autosavetype(BULIANGPIN);
	} else {
		baseparam::set_autosavetype(LIANGPIN);
	}
	
	if (ui.bBadProductRateAlarmStop->isChecked())
	{
		configiniSystem.setValue("bBadProductRateAlarmStop", true);
		baseparam::set_badproductratealarmstop(true);
	}
	else
	{
		configiniSystem.setValue("bBadProductRateAlarmStop", false);
		baseparam::set_badproductratealarmstop(false);
	}

	if (ui.bProductBlockedAlarmStop->isChecked())
	{
		configiniSystem.setValue("bProductBlockedAlarmStop", true);
		baseparam::set_productblockedalarmstop(true);
	}
	else
	{
		configiniSystem.setValue("bProductBlockedAlarmStop", false);
		baseparam::set_productblockedalarmstop(false);
	}

    //自动加载最近一次检测方案
    bool ischeck = ui.bUserLastSolution->isChecked();
    configiniSystem.setValue("bUserLastSolution", ischeck);
    baseparam::set_uselastsolution(ischeck);

     //开机自动登入为操作员
    ischeck = ui.bLoginAsOperator->isChecked();
    configiniSystem.setValue("bLoginAsOperator", true);
    baseparam::set_loginasoperator(ischeck);
    
    configiniSystem.endGroup();
	CiWaMsgDialog::MessageT("保存成功", "");
	//QMessageBox::information(NULL, tr(""), tr("保存成功"));
}





void SystemConfig::InitTab0()
{
	OnReadBaseParam();
}