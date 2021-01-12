#include "uicommon.h"
#include <QDateTime>
#include "log.h"
#ifdef USECAMERADLL
#include "..\camera\cameramanage\cameramanage.h"
#endif
#include "alg.h"
#include "plc.h"
#include "ciwa.h"
#include "solution.h"
#include "group.h"
#include "login.h"
#include <plcinterface.h>
#include "../socket/ciwasocket.h"
#include "debugdata.h"
#include "baseparam.h"
#include <QMetaType>
#include "ciwamsgdialog.h"
#ifdef DEBUGTIME
#include "proc.h"
#endif
#include "wantusblight.h"

void * ciwa::GetWidgetPoint()
{
    return (void*)this;
}


ciwa::ciwa(QWidget *parent)
	: QWidget(parent)
{
    InitCiwa();
    InitCiwaUI();
}

ciwa::~ciwa()
{
#ifdef USECAMERADLL
    CCameraManage::Deinitialize();
#endif
	alg::deinitialize();
	group::deinitialize();
    light::deinitialize();
	plcmaster::deinitialize();
	if (m_planmsg !=NULL)
	{
		delete m_planmsg;
		m_planmsg = NULL;
	}

	if (m_systemconfig != NULL)
	{
		m_systemconfig->close();
		delete m_systemconfig;
		m_systemconfig = NULL;
	}
	if (m_importandexport!= NULL)
	{
		m_importandexport->close();
		delete m_importandexport;
		m_importandexport = NULL;
	}
    if (p_dataMainFormZJ != NULL) {
        p_dataMainFormZJ->close();
        delete p_dataMainFormZJ;
        p_dataMainFormZJ = NULL;
    }
	DatabaseSqlite::get_db()->CloseDB();
    databaseSqlInset::get_db()->CloseDB();
}
//网络状态栏
void ciwa::on_SendStatusBar(QString msg)
{
	if (msg.isEmpty())
	{
		if (m_systemconfig != NULL)
		{
			m_systemconfig->SetSocketLab(true);
		}
	}
	return;

	ui.label_8->setText(msg);

}
//导入导出按钮
void ciwa::on_btnImportAndExport_clicked()
{
	HideChildrenFrame();
	if (!m_importandexport)
	{
		m_importandexport = new ImportAndExport(ui.groupBox_3, ui.groupBox_3->rect());
		
	}
	m_importandexport->show();
}
//接受添加数据库信号
void ciwa::OnSendAddDataBase(std::map<QString, QString> data)
{
#ifdef DEBUGTIME
	QTime temp;
	temp.start();
#endif
	QSqlQuery * ret;
	DatabaseSqlite::get_db()->Add(PRODUCTTABLE, data, &ret);
#ifdef DEBUGTIME
	DebugPROC::EndTime(temp, TIMEID::T_ADDDATABASE);
#endif	
}


//系统设置按钮
void ciwa::on_btnSystemSetting_clicked()
{
#ifdef DEBUGTIME
	QTime temp;
	temp.start();
#endif // DEBUGTIME
	HideChildrenFrame();
	if (!m_systemconfig)
	{
		//设置等待鼠标
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		m_systemconfig = new SystemConfig(ui.groupBox_3, ui.groupBox_3->rect());
		QObject::connect(m_systemconfig, SIGNAL(SendCustomFenpSignal()), m_runframenewui->m_runtoprightdialog, SLOT(OnRefreshSolutionUI()));
		QApplication::restoreOverrideCursor();
	}
	m_systemconfig->show();
	ui.btnSystemSetting->setChecked(true);
#ifdef DEBUGTIME
	DebugPROC::EndTime(temp,TIMEID::T_BTNSYSTEM);
#endif // DEBUGTIME
}
//中集新加入界面
void ciwa::on_toolButton_zhongji_clicked()
{
    HideChildrenFrame();
    if (!p_dataMainFormZJ)
    {
        //设置等待鼠标
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        p_dataMainFormZJ = new dataMainFormZJ(ui.groupBox_3, ui.groupBox_3->rect());
        QObject::connect(this, SIGNAL(signaModelName(QString&)),
            p_dataMainFormZJ, SLOT(slotModeName(QString&)));
        QObject::connect(p_dataMainFormZJ, SIGNAL(signalGetModelName(bool)),
            this, SLOT(slotGetModeName(bool)));
        QApplication::restoreOverrideCursor();
    }
    //m_runframenewui->hide();
    p_dataMainFormZJ->showForm(ui.groupBox_3->rect());
    ui.toolButton_zhongji->setChecked(true);
    p_dataMainFormZJ->setFocusPolicy(Qt::StrongFocus);
}
//调试按钮
void ciwa::on_btnDebugConfig_clicked()
{
	HideChildrenFrame();
	if (!m_debugconfig)
	{
		//设置等待鼠标
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		m_debugconfig = new DebugConfig(ui.groupBox_3, ui.groupBox_3->rect());
		//QObject::connect(m_debugconfig, SIGNAL(SendCustomFenpSignal()), m_runframenewui->m_runtoprightdialog, SLOT(OnRefreshSolutionUI()));
		QApplication::restoreOverrideCursor();
	}
	m_debugconfig->show();
	m_debugconfig->on_btnRefresh_clicked();
	ui.btnDebugConfig->setChecked(true);
}
/*
void ciwa::OnSendSingleSignal()
{
	//设置等待鼠标
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	this->Stop();
	m_runframenewui->OnSendSingleSignal();
	if (m_isrunning)
		this->Start();
	QApplication::restoreOverrideCursor();
}

void ciwa::OnSendMultiSignal()
{	
	//设置等待鼠标
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	this->Stop();
	m_runframenewui->OnSendMultiSignal();
	if (m_isrunning)
		this->Start();
	QApplication::restoreOverrideCursor();
}
*/
void ciwa::OnSendUpdatePpm(int totalframenum)
{
	int ppm = totalframenum * 1000 * 60 / m_starttime.elapsed();
	
	QString tr_timeDiff = QString("每分钟：") + QString("%1").arg(ppm) + QString("片");
    ui.ppm->setText(tr_timeDiff);
}

void ciwa::OnSendSoftwareAlarm(QString str) {
	ui.swalarm->setText(str);
	if (str == QString("卡料")) {
	}
	else if (str == QString("不良品率高")) {
	}
}
//方案检测按钮
void ciwa::on_btnDetectionScheme_clicked()
{ 
#ifdef DEBUGTIME
	QTime temp;
	temp.start();
#endif // DEBUGTIME

	HideChildrenFrame();
	if (!m_planmsg)
	{
		m_planmsg = new PlanMsg(ui.groupBox_3, ui.groupBox_3->rect());
		//
		if(m_runframenewui->m_runtoprightdialog)
			QObject::connect(m_planmsg, SIGNAL(RefreshSolution()), m_runframenewui->m_runtoprightdialog, SLOT(OnRefreshSolution()));
	}
	m_planmsg->show();
	ui.btnDetectionScheme->setChecked(true);

#ifdef DEBUGTIME
	DebugPROC::EndTime(temp,TIMEID::T_BTNJIANCHE);
#endif // DEBUGTIME
}

//启动按钮
void ciwa::on_btnStart_clicked()
{
	Start();
	m_isrunning = true;
	m_starttime.restart();
	ui.btnDetectionScheme->setDisabled(true);
	//ui.btnDebugConfig->setDisabled(true);
	ui.btnImportAndExport->setDisabled(true);
	ui.btnSystemSetting->setDisabled(true);
	if (m_runframenewui)
		m_runframenewui->m_runtoprightdialog->DisableSolution(true);

}
//停止按钮
void ciwa::on_btnStop_clicked()
{
	Stop();
	m_isrunning = false;
	ui.btnDetectionScheme->setDisabled(false);
	//ui.btnDebugConfig->setDisabled(false);
	ui.btnImportAndExport->setDisabled(false);
	ui.btnSystemSetting->setDisabled(false);
	if (m_runframenewui)
		m_runframenewui->m_runtoprightdialog->DisableSolution(false);
}


void ciwa::Stop()
{
	plcmaster::stop();
#ifdef USECAMERADLL
    CCameraManage::StopAutoTrigger();
#endif
	
	alg::stop();
	group::stop();
	if (m_runframenewui)
		m_runframenewui->m_runtoprightdialog->StopTime();
}

void ciwa::Start()
{
	sln::resetcnt();
	group::start();
	alg::start();

#ifdef USECAMERADLL
    //获取检测方案的相机
    auto &list = Solution::GetCurrentSolution()->m_camera;
    CCameraManage::LoadUseCameras(&list);
    CCameraManage::StartAutoTrigger();
#endif
	if (m_runframenewui)
		m_runframenewui->m_runtoprightdialog->StarTime(2000);
  
       
	plcmaster::start();
}

bool ciwa::ShutDown()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	//获取进程标志
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return false;
	//获取关机特权的LUID  
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	//获取这个进程的关机特权   
	AdjustTokenPrivileges(hToken, false, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
	if (GetLastError() != ERROR_SUCCESS)
		return false;
	//强制关闭计算机
	if (!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0))
		return false;
	return true;
}


//运行按钮
void ciwa::on_btnRun_clicked()
{
	HideChildrenFrame();

	if (!m_runframenewui)
	{
		m_runframenewui = new RunFrameNewUi(NULL, ui.groupBox_3->rect());
		QObject::connect(m_runframenewui->m_runtoprightdialog, SIGNAL(SendStart()), this, SLOT(on_btnStart_clicked()));
		QObject::connect(m_runframenewui->m_runtoprightdialog, SIGNAL(SendStop()), this, SLOT(on_btnStop_clicked()));
		QObject::connect(m_runframenewui->m_runtoprightdialog, SIGNAL(SendUpdatePpm(int)), this, SLOT(OnSendUpdatePpm(int)));
		QObject::connect(m_runframenewui->m_runtoprightdialog, SIGNAL(SendSoftwareAlarm(QString)), this, SLOT(OnSendSoftwareAlarm(QString)));
        QObject::connect(m_runframenewui->m_runtoprightdialog, SIGNAL(signalRunTopRightModelName(QString&)),
            this, SLOT(slotRunFrameModelName(QString&)));
		qRegisterMetaType<std::map<QString, QString>>("std::map<QString, QString>");
		QObject::connect(this, &ciwa::SendAddDataBase, this, &ciwa::OnSendAddDataBase);
		ui.verticalLayout->addWidget(m_runframenewui);
		m_runframenewui->show();
		m_runframenewui->m_runtoprightdialog->InitAutoFenp();
	}
	//Solution::SetCurrentSolution(m_runframenewui->m_runtoprightdialog->GetCurrText());
	ui.btnRun->setChecked(true);

}

//退出按钮
void ciwa::on_btnExit_clicked()
{
	close();
	qApp->quit();
}




void ciwa::frameDoned(const imginfo& img)
{
#ifdef DEBUGTIME
	QTime temp;
	temp.start();
#endif
	group::push(img);
	m_runframenewui->ShowCameraPic(img);

	
	imginfo tmpimginfo = img;
	QString sqlstationid = QString::number(tmpimginfo.getstationid());
	QString sqlproductid = QString::number(tmpimginfo.getid());
	QString dstdir = QString::fromLocal8Bit(alg::getimgsavedir().c_str()) + "/" + sqlstationid + "/";
	char buf[1024];
	sprintf(buf, "%05d_%05d.png", tmpimginfo.getstationid(), tmpimginfo.getframeid());
	QString sqlpicaddr = QString("%1%2%3").arg("\"")
		.arg(dstdir + buf)
		.arg("\"");
	QString sqlisgood = "\"Yes\"";
	if(tmpimginfo.getNgflag())
		sqlisgood = "\"No\"";

	//插入到数据库中
	std::map<QString, QString> data;
	//data[PRODUCTID] = sqlproductid;
	data[ISGOOD] = sqlisgood;
	data[PICADDR] = sqlpicaddr;
	data[STATIONID] = sqlstationid;
	data[BATCHES] = m_runframenewui->m_runtoprightdialog->get_batch();
#ifdef DEBUGTIME
	DebugPROC::EndTime(temp, TIMEID::T_CALLBACK);
#endif	
	//OnSendAddDataBase(data);
	emit SendAddDataBase(data);

}


void ciwa::CreadteIniPath()
{
	QString runPath = QCoreApplication::applicationDirPath();
	QString path = runPath + QString(SOLUTION);
	QDir dir;
	if (!dir.exists(path))
	{
		dir.mkdir(path);
	}

}
//隐藏所有子窗口
void ciwa::HideChildrenFrame()
{
	if (m_planmsg)
	{ 
		m_planmsg->hide();
		ui.btnDetectionScheme->setChecked(false);
	}
	if (m_systemconfig)
	{
		m_systemconfig->hide();
		ui.btnSystemSetting->setChecked(false);
	}
	if (m_runframenewui)
	{
        //m_runframenewui->hide();
		ui.btnRun->setChecked(false);
	}
	if (m_debugconfig)
	{
		m_debugconfig->hide();
		ui.btnDebugConfig->setChecked(false);
	}
	if (m_importandexport)
	{
		m_importandexport->hide();
		ui.btnImportAndExport->setChecked(false);
	}
    if (p_dataMainFormZJ) {
        p_dataMainFormZJ->hide();
        ui.toolButton_zhongji->setChecked(false);
    }

}

// 其实应用级别不应该看到plc::这么底层的东西的，以后再优化
void ciwa::plcmessageprocess(plc::PlcMessageType msg) {
    switch (msg)
    {
    case plc::PlcMessageType::kStart:
        ui.status->setText(QString("设备启动"));
        on_btnStart_clicked();
        break;
    case plc::PlcMessageType::kStop:
        ui.status->setText(QString("设备停止"));
        on_btnStop_clicked();
        break;
    case plc::PlcMessageType::kPowerOff:
        ui.status->setText(QString("关机"));
        on_btnStop_clicked();
        ShutDown();
        break;
    case plc::PlcMessageType::kFaultAirPressure:
        ui.fault->setText(QString("异常：通讯中断[点击复位]"));
        ui.fault->setStyleSheet("color: #e04040;font-weight: normal;");
        ui.fault->setStyleSheet("background-color: #f06060");
        on_btnStop_clicked();
        break;
/*
    按键主动控制，这里不再使用
    case plc::PlcMessageType::kRollStart:
        ui.status->setText(QString("新卷开始"));
        std::cout << "--------------RollStar--------------\n";
        //后续处理
        p_dataMainFormZJ->setUpdataNewVolumeNum();
        break;
    case plc::PlcMessageType::kRollEnd:
        ui.status->setText(QString("卷结束"));
        std::cout << "++++++++++++RollEnd++++++++++++++\n";
        //后续处理
        //需要调用这个函数
		float lastchecklength = plcmaster::getonlycameralength();
        float l2length = plcmaster::setplcstatusend(); //记录这圈的last_roll_length
        p_dataMainFormZJ->setLastRolllength(QString("%1").arg(lastchecklength),true);
        break;
*/
    }
}

void ciwa::resetClicked()
{
	plcmaster::reset();
}
//初始化算法(进度条)
void ciwa::InitCiwa()
{
#ifdef DEBUGTIME
	DebugPROC::StartTime();
#endif //DebugPROC计时

	log_init("log/" + std::to_string(QDateTime::currentMSecsSinceEpoch()) + ".log", utils::log_priority::detail);
	//配置文件
	Solution::LoadIni();
	baseparam::loadini();

#ifdef DEBUGTIME
	DebugPROC::EndTime(TIMEID::T_INICONFIG);
	DebugPROC::StartTime();
#endif //DebugPROC计时
	//算法
	group::initialize();
    light::initialize();
	////alg::initialize(NULL);
#ifdef USECAMERADLL
    CCameraManage::Initialize(&alg::push);
#endif
	 DatabaseSqlite::get_db()->Init("127.0.0.1", "root", "123456", "cicai");
     databaseSqlInset::get_db()->Init("127.0.0.1", "root", "123456", "zhongji");

#ifdef DEBUGTIME
	DebugPROC::EndTime(TIMEID::T_ALGINIT);
#endif //DebugPROC计时
}

//初始化UI界面
void ciwa::InitCiwaUI()
{
#ifdef DEBUGTIME
	DebugPROC::StartTime();
#endif //DebugPROC计时
	m_planmsg = NULL;
	m_systemconfig = NULL;
	m_runframenewui = NULL;
	m_debugconfig = NULL;
	m_importandexport = NULL;
    p_dataMainFormZJ = NULL;
	m_isrunning = false;

	ui.setupUi(this);
	ui.btnStart->setVisible(false);
	ui.btnStop->setVisible(false);

	//this->showMaximized();
	//this->showFullScreen();
	//创建目录
	CreadteIniPath();
	//隐藏标题栏
    this->setFocusPolicy(Qt::ClickFocus);
	//setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
	QCoreApplication::processEvents();//强制刷新一下,不然运行界面不能最大化
	CiWaSocket::CiWaSocketIni(); 
	QObject::connect(ui.fault, &QPushButton::clicked, this, &ciwa::resetClicked);
	//初始化网络（状态栏）
	QObject::connect(CiWaSocket::GetCurrSocket(), &CiWaSocket::SendStatusBar, this, &ciwa::on_SendStatusBar);
	if (CiWaSocket::GetIsOpenNet() == 1)
		CiWaSocket::NetWorkStart();
	QObject::connect(CiWaMsgDialog::GetDialogPoint(), &messageDialog::SendMesSignal, this, &ciwa::CiWaInitFail, Qt::BlockingQueuedConnection);


#ifdef DEBUGTIME
	DebugPROC::EndTime(TIMEID::T_UI);
	DebugPROC::StartTime();
#endif // DebugPROC计时
	plcmaster::initialize([this](plc::PlcMessageType msg) {plcmessageprocess(msg); });
#ifdef DEBUGTIME
	DebugPROC::EndTime(TIMEID::T_PLC);
#endif // DebugPROC计时
	alg::registercallback([this](const imginfo& img) { frameDoned(img); });
    //on_toolButton_zhongji_clicked();
	//运行按钮
    on_toolButton_zhongji_clicked();
	on_btnRun_clicked();
    p_dataMainFormZJ->setModelName();
	m_starttime.start();
}

//接受线程中弹出对话框失败
void ciwa::CiWaInitFail(QString shead, QString msg)
{
	CiWaMsgDialog::MessageT(shead, msg);
	
}


void ciwa::keyReleaseEvent(QKeyEvent * ev)
{
	
	//qDebug() << "start" << ev->key()<< QKeySequence(Qt::Key_F1)<<"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
	// red
	//if (ev->key() == Qt::Key_Q) {
	//}
	// white
	//else if (ev->key() == Qt::Key_W) {
//	}
	// green
	//else if (ev->key() == Qt::Key_E) {
//	}
     if (ev->key() == Qt::Key_F8)
    {
        qDebug() << "stop" << "";
		//后续处理
		//需要调用这个函数	
		endlengthinfo_s lastlength = plcmaster::setplcstatusend(); //记录这圈的last_roll_length
		ui.status->setText(QString("卷结束"));
		std::cout << "++++++++++++RollEnd++++++++++++++  Camera length " << lastlength.last_check_length << "l2 length " 
			<< lastlength.last_roll_length - lastlength.last_check_length <<"\n";
        QStringList listData;
        listData << QString("%1").arg(lastlength.last_roll_length)
            << QString("%1").arg(lastlength.last_check_length)
            << QString("%1").arg(lastlength.last_roll_length - lastlength.last_check_length);
		QString endTime = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
		listData << endTime;
		p_dataMainFormZJ->setLastRolllength(listData, true);
        return;
    }
    else if (ev->key() == Qt::Key_F6)
    {
        qDebug() << "start" << "";
		float l3start = 0;
		l3start = plcmaster::setplcstatusstart();
		ui.status->setText(QString("新卷开始"));
		std::cout << "--------------RollStar--------------" << l3start <<"\n";
		//后续处理
		p_dataMainFormZJ->setUpdataNewVolumeNum();
        return;
    }
    else if (ev->key() == Qt::Key_F7)
    {
        qDebug() << "reboot" << "";
        return;
    }

    QWidget::keyPressEvent(ev);
}
void ciwa::slotGetModeName(bool flag)
{
  QString name =  m_runframenewui->getModelName();
    if (flag) {
        emit signaModelName(name);
    }
}
void ciwa::slotRunFrameModelName(QString& name)
{
    emit signaModelName(name);
}
