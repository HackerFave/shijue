#include "runtoprightdialog.h"
#include "qcolor.h"
#include "solution.h"
#include "uicommon.h"
#include "debugdata.h"
#include "baseparam.h"
#include "qdatetime.h"
#include "qdebug.h"
#include "fenpIni.h"
#include "databasesqlite.h"
#include "creatreportform.h"
#include "../socket/ciwasocket.h"
#include "ciwamsgdialog.h"
#include "qsettings.h"
#ifdef DEBUGTIME
#include "proc.h"
#endif // DEBUGTIME
#include "qfiledialog.h"
#ifdef USECAMERADLL
#include "..\camera\cameramanage\cameramanage.h"
#endif
#include "alg.h"

RunTopRightDialog::RunTopRightDialog(QWidget *parent)
	: QWidget(parent)
{
    m_isfilewather = false;
	m_iswindowing = false;
	m_isopentimer = false;
	ui.setupUi(this);
	m_runframetable = new RunFrameTable();	//SetReport
	CreatReportForm* creatreport = new CreatReportForm();
	m_creatreport = (void*)creatreport;
	QObject::connect(creatreport, &CreatReportForm::SendbtnClicked, this, &RunTopRightDialog::OnSendbtnClicked ,Qt::DirectConnection);
	QObject::connect(CiWaSocket::GetCurrSocket(), &CiWaSocket::SendReport, this, &RunTopRightDialog::OnSendReport, Qt::DirectConnection);
	QObject::connect(CiWaSocket::GetCurrSocket(), &CiWaSocket::SetReport, this, &RunTopRightDialog::OnSetReport, Qt::DirectConnection);
	QObject::connect(this, &RunTopRightDialog::SlaveStarRun, this, &RunTopRightDialog::OnSlaveStarRun);
	QObject::connect(CiWaSocket::GetCurrSocket(), &CiWaSocket::SendStartRun, this, &RunTopRightDialog::OnSendStartRun);
	
	ShowRightBottomTable();
	InitChart();
	ui.verticalLayout_3->addWidget(m_chartview);
	LoadSolution();
	//默认选着解决方案为第一个，后期可以优化到配置文件记录最后一个解决方案
	//OnSetParam(ui.btnSolutionNum->itemText(0));
    //读取基本配置文件选择最后一个检测方案
    SetLastUseSolutionToIni();
   

	ui.btnClearNote->hide();
	ui.btnExport->hide();
	m_batch = QString("");

}

RunTopRightDialog::~RunTopRightDialog()
{
	CreatReportForm* creatreport;
	creatreport = (CreatReportForm*)m_creatreport;
	if (creatreport != NULL)
	{
		delete creatreport;
		creatreport = NULL;
		m_creatreport = NULL;
	}
}
void RunTopRightDialog::InitChart()
{
	//更新数据 m_slice->setValue(m_sliceValue->value());
	QColor color;
	QFont font("Arial", 8);

	QPieSeries *series = new QPieSeries();
	series->setHoleSize(0.5);

	m_goodserie = series->append("良品:", 10);
	m_badserie = series->append("不良品:", 10);


	m_badserie->setLabelVisible();
	color = Qt::red;
	m_badserie->setBrush(color.lighter(115));
	m_badserie->setLabelColor(Qt::white);
	m_badserie->setLabelFont(font);

	m_goodserie->setLabelVisible();
	color = Qt::darkGreen;
	m_goodserie->setBrush(color.lighter(115));
	m_goodserie->setLabelColor(Qt::white);
	m_goodserie->setLabelFont(font);
	

	m_chartview = new QChartView();
	m_chartview->chart()->setAnimationOptions(QChart::AllAnimations);
	m_chartview->setRenderHint(QPainter::Antialiasing);
	m_chartview->chart()->addSeries(series);
	m_chartview->chart()->legend()->setFont(QFont("Arial", 9));
	m_chartview->chart()->legend()->setLabelColor(Qt::white);
	
	m_chartview->chart()->legend()->setAlignment(Qt::AlignBottom );
	m_chartview->chart()->setBackgroundVisible(false);


	m_goodserie->setLabel(QString("%1:%2%").arg("良品").arg(100.00, 0, 'f', 2));
	m_badserie->setLabel(QString("%1:%2%").arg("不良品").arg(100.00, 0, 'f', 2));


}
//创建报告对话框
bool RunTopRightDialog::CreateReportDialog()
{
	
	CreatReportForm* creatreport;
	creatreport = (CreatReportForm*)m_creatreport;
	m_batch = QString("");
	m_runframetable->m_batch = QString();
	//如果是从机向主机索要批次，对话框为从机等待模式(有取消等待按钮->跳转到主机设置模式）
	if (CiWaSocket::GetCurrMaster() == HostState::SLAVE)
	{
		creatreport->set_slave(true);

	}
	else
	{
		//如果是主机，对话框为主机设置模式
		creatreport->set_slave(false);
	}
	creatreport->exec();
	if (!creatreport->get_isok())
		return false;
	return true;
}


//开始按钮
void RunTopRightDialog::on_btnStart_clicked()
{
    if (!Solution::GetCurrentSolution())
        return;

    //弹出批次编辑对话框
	StartRun(false);

    //将最近使用的检测方案保存到系统配置文件中
    SaveCurrenUseSolutionToIni();

	//向从机发送开始运行信号
	emit SlaveStarRun();

}
//停止按钮
void RunTopRightDialog::on_btnStop_clicked()
{
    if (!Solution::GetCurrentSolution())
        return;
	//数据库更新批次
	int gcount = m_runframetable->m_count;
	int ok = m_runframetable->m_goodcount;
	int bad = gcount - ok;
	QSqlQuery * ret;
	std::map<QString, QString> data;
	std::map<QString, QString> dwhere;
	dwhere[BATCHES] = m_batch;
	data[SQLCOUNT] = QString::number(gcount);
	data[SQLGOOD] = QString::number(ok);
	data[SQLBAD] = QString::number(bad);
	DatabaseSqlite::get_db()->Updata(BATCHTABLE, dwhere, data, &ret);
	m_batch = QString("");

	m_runframetable->UpSqlTableData();
	ui.btnStart->setText("运行");
	ui.btnStart->setDisabled(false);
	emit SendStop();
}

void RunTopRightDialog::OnSendReport()
{
	//发送批次给从机
	QJsonObject jsonObject;
	jsonObject["report"] = m_batch;
	CiWaSocket::NetWorkSendData(jsonObject, MSGID::REPORT);
	
}

void RunTopRightDialog::OnSetReport(const QJsonDocument & doc)
{
	//解析
	QJsonObject rootObject = doc.object();
	//获得工位	
	QJsonValue jsonValue;

	jsonValue = rootObject.value("report");
	m_batch = jsonValue.toString();
	m_runframetable->m_batch = m_batch;

	if (m_batch.isEmpty())
		return;
	//关闭对话框
	CreatReportForm* creatreport;
	creatreport = (CreatReportForm*)m_creatreport;
	creatreport->StopReSendBatch();
	creatreport->close();
	StartRun(false);
}

//分屏
void RunTopRightDialog::on_btnWindowing_clicked()
{
	if (!m_iswindowing)	//是单
	{
		//变多屏
		m_iswindowing = true;
		ui.btnWindowing->setText(QString("取消分屏"));
		emit SendMultiSignal();
		
	}
	else//是多屏
	{
		//变单屏
		m_iswindowing = false;
		ui.btnWindowing->setText(QString("分  屏"));
		emit SendSingleSignal();

	}

}
//处理解决方案选择事件
void RunTopRightDialog::OnSetParam(QString str)
{
#ifdef DEBUGTIME
	DebugPROC::StartTime();
#endif //DebugPROC计时

	if (str.isEmpty())
		return;
    emit signalRunTopRightModelName(str);
	Solution::SetCurrentSolution(str);

	//debug配置读取
	DebugData::Init();
	
	//更新列表
	UpdataTable();

	//自定义窗口
	FenpIni::ReadIni();
	if (FenpIni::IsOpen())
	{
		emit SendCustomWindow();
	}
	else
	{
		//发送动态空窗刷新
		emit SendUpdataWindow();
	}
	ui.btnSolutionNum->clearFocus();
	QApplication::restoreOverrideCursor();
	QCoreApplication::processEvents();
#ifdef DEBUGTIME
	DebugPROC::EndTime(TIMEID::T_CHANGE_SOLUTION);
#endif // DEBUGTIME

}


void RunTopRightDialog::OnRefreshSolutionUI()
{
	QString str;
	if (!Solution::GetCurrentSolution())
		return;
	str = Solution::GetCurrentSolution()->get_solutionname();
	OnSetParam(str);
}


void RunTopRightDialog::OnRefreshSolution()
{
	QObject::disconnect(ui.btnSolutionNum, SIGNAL(currentIndexChanged(QString)), this, SLOT(OnSetParam(QString)));
	ui.btnSolutionNum->clear();
	LoadSolution();
}

void RunTopRightDialog::OnSendbtnClicked()
{
	CreatReportForm* creatreport;
	creatreport = (CreatReportForm*)m_creatreport;
	if (creatreport->get_isok())
		m_batch = creatreport->get_batchnum();

	QSqlQuery * ret;
	std::list<QString> key;
	std::map<QString, QString> dbwhere;
	std::list<std::list<QString>> row;
	dbwhere[BATCHES] = m_batch;
	key.push_back("*");
	DatabaseSqlite::get_db()->Find(BATCHTABLE, key, dbwhere, &row, &ret);
	if (row.size() > 0)
	{
		CiWaMsgDialog::MessageT("添加失败", "该批次已存在");
		m_batch = "";
		return;
	}
	
	this->m_runframetable->m_batch = m_batch;
	creatreport->close();


}

void RunTopRightDialog::OnSlaveStarRun()
{
	if (CiWaSocket::GetCurrMaster() != HostState::MASTE)
		return;

	//发送从机网络开启命令
	QJsonObject jsonObject;
	jsonObject["start"] = m_batch;
	CiWaSocket::NetWorkSendData(jsonObject, MSGID::SLAVESTART);
}

void RunTopRightDialog::OnSendStartRun(const QJsonDocument & doc)
{
	if (CiWaSocket::GetCurrMaster() != HostState::SLAVE)
		return;
	//解析
	QJsonObject rootObject = doc.object();
	//获得工位	
	QJsonValue jsonValue;

	jsonValue = rootObject.value("start");
	m_batch = jsonValue.toString();
	m_runframetable->m_batch = m_batch;
	
	StartRun(false);
}


//加载检测方案
void RunTopRightDialog::LoadSolution()
{
	for (size_t i = 0; i < Solution::g_solution.size(); i++)
	{
		ui.btnSolutionNum->addItem(Solution::g_solution[i]);
	}
	QObject::connect(ui.btnSolutionNum, SIGNAL(currentIndexChanged(QString)), this, SLOT(OnSetParam(QString)));
}



//更新table列表
void RunTopRightDialog::UpdataTable()
{
	//int station = Solution::GetCurrentSolution()->get_station();
	//清空表格数据并加载配置文件数据
	m_runframetable->ClearAndLoadTable();
	//m_runframetable->ResizeTable(SATATIONNUM);
	
}

void RunTopRightDialog::StarTime(long n)
{
	m_runframetable->m_count = 0;
	m_runframetable->m_goodcount = 0;
	m_runframetable->m_badcount = 0;
	m_runframetable->UpTableData();
	this->UpDataChart(0.0);
	m_timeflag = startTimer(n);
	m_isopentimer = true;
}

void RunTopRightDialog::StopTime()
{
	if (m_timeflag > 0)
		killTimer(m_timeflag);
	m_isopentimer = false;
}
//QComboBox{\nbackground-color: rgb(21, 21, 21);\n}\nQComboBox::down-arrow { \n image: url(:/resources/icons/ic_arrow_down.png);\n}\n
void RunTopRightDialog::DisableSolution(bool b) {
	if (b)
		ui.btnSolutionNum->setStyleSheet("QComboBox{background-color: rgb(190,190,190);}QComboBox::down-arrow {image: url(:/resources/icons/ic_arrow_down.png);}");
	else
		ui.btnSolutionNum->setStyleSheet("QComboBox{background-color: rgb(21, 21, 21);}QComboBox::down-arrow {image: url(:/resources/icons/ic_arrow_down.png);}");
	ui.btnSolutionNum->setDisabled(b);
}

void RunTopRightDialog::ShowRightBottomTable()
{
	ui.verticalLayout_5->addWidget(m_runframetable);
}

QString RunTopRightDialog::GetCurrText()
{
	return ui.btnSolutionNum->currentText();
}

void RunTopRightDialog::InitAutoFenp()
{
	if (FenpIni::GetAutoOpen())
		on_btnWindowing_clicked();
}

//开始运行检测 true：弹出对话框，fasle：不弹出
void RunTopRightDialog::StartRun(bool iscreatreport)
{
	/*ui.btnStart->*/
	if (iscreatreport)
	{
		if (!CreateReportDialog())
			return;
	}

	QSqlQuery * ret;
	time_t t = time(0);
	char ch[64];
	strftime(ch, sizeof(ch), "%Y%m%d%H%M%S", localtime(&t)); //年-月-日 时-分-秒
	if (QString(m_batch).isEmpty())
	{
		m_batch = QString(ch);
	}
	std::map<QString, QString> data;

	data[BATCHES] = m_batch;
	//数据库添加批次
	DatabaseSqlite::get_db()->Add(BATCHTABLE, data, &ret);
	m_runframetable->m_batch = m_batch;
	ui.btnStart->setText("正在运行");
	ui.btnStart->setDisabled(true);

	//启动从机
	emit SendStart();
}

void RunTopRightDialog::SaveCurrenUseSolutionToIni()
{

    QString pathSystem = QCoreApplication::applicationDirPath() + '/' + QString(ININAME_SYSTEM);
    QSettings configiniSystem(pathSystem, QSettings::IniFormat);
    configiniSystem.setIniCodec("UTF8");
    configiniSystem.beginGroup("Baseinfo");
    QString str = GetCurrText();
    configiniSystem.setValue("CurrenSolution", str);
    configiniSystem.endGroup();
}

void RunTopRightDialog::SetLastUseSolutionToIni()
{
    int index = -1;
    bool ret = false;
    QString str = baseparam::get_uselastsolutionstr();
    if (baseparam::get_uselastsolution() && (!str.isEmpty())) {
        index = ui.btnSolutionNum->findText(str);
        if (index >-1) {
            ret = true;
        }
    }
    if (!ret) {
        str = ui.btnSolutionNum->itemText(0);
    }
    ui.btnSolutionNum->setCurrentText(str);
   
    OnSetParam(str);
   
}
QString RunTopRightDialog::getProgramName()
{
   return ui.btnSolutionNum->currentText().simplified();
}
void RunTopRightDialog::timerEvent(QTimerEvent *)
{
	if (!m_runframetable)
		return;

	bool isblocking = m_runframetable->UpTableData();
	if (isblocking) {
		emit SendSoftwareAlarm(QString("卡料"));
	}
	this->UpDataChart(0.0);
	if (m_isopentimer == false)
	{
		killTimer(m_timeflag);
	}

	// Solution::GetCurrentSolution()->get_ngcnt();
}


//实现统计图数据更新
void RunTopRightDialog::UpDataChart(double count)
{
	int gcount = m_runframetable->m_count;
	int bad = m_runframetable->m_badcount;
	int ok = gcount - bad;
	double num;

	m_goodserie->setValue(ok);
	m_badserie->setValue(bad);
	ui.lcdNumber->display(gcount);
	ui.lcdNumber_2->display(ok);
	ui.lcdNumber_3->display(bad);
	

	if (gcount == 0)
		return;
	num = (double)ok / (double)gcount;
	m_goodserie->setLabel(QString("%1:%2%").arg("良品").arg(num * 100, 0, 'f', 2));
	num = (double)bad / (double)gcount;
	m_badserie->setLabel(QString("%1:%2%").arg("不良品").arg(num * 100, 0, 'f', 2));
	emit SendUpdatePpm(gcount);

	CheckBadProdectRate(num);
}

//不良品率检测
void RunTopRightDialog::CheckBadProdectRate(double rate) {
	//如果不需要检测
	if (!baseparam::get_badproductratealarm()) {
		return;
	}

	//如果超过阈值
	if (rate*1000 > baseparam::get_badproductratethr()) {
		emit SendSoftwareAlarm(QString("不良品率高"));
	}
}

void RunTopRightDialog::on_btnReloadModel_clicked()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    alg::reloadmodel();
    QApplication::restoreOverrideCursor();
}

void RunTopRightDialog::on_btnUnloadModel_clicked()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    alg::unloadmodel();
    QApplication::restoreOverrideCursor();
}

void RunTopRightDialog::on_btnTestData_clicked()
{
    QString dataabspath = QFileDialog::getExistingDirectory(
        nullptr, QString("Open"), "", QFileDialog::ShowDirsOnly);
    //点击取消
    if (dataabspath.isEmpty()) {
        return;
    }
}

void RunTopRightDialog::on_btnStep_clicked()
{
    if (!Solution::GetCurrentSolution())
        return;
#ifdef USECAMERADLL
    CCameraManage::Trigger();
#endif
}

void RunTopRightDialog::OnCommitReconRequest(const QString& strFilePath)
{
    QString str = QString(strFilePath);
    qDebug() << str;
}
