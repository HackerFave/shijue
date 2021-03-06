#include "login.h"
#include "qmessagebox.h"
#include "loginthread.h"
#include "qthread.h"
#include "ciwa.h"
#include "ciwamsgdialog.h"
#pragma execution_character_set("utf-8")

Login::Login(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	//隐藏标题栏
	setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
	//最大化
	this->showFullScreen();

	m_isruning = false;

	ui.lineEdit_2->setEchoMode(QLineEdit::Password);//设置密码隐藏
	ui.lineEdit_2->setMaxLength(16);//设置最大长度16位
	ui.lineEdit->setMaxLength(16);//设置最大长度16位
	ui.progressBar->hide();//设置进度条隐藏
	InitDatabase();//初始化数据库
	
	}

Login::~Login()
{
	DatabaseSqlite::get_db()->CloseDB();//关闭数据库(可以不用关闭，后期要用)
}
bool Login::InitLogin(void * p)
{
	if (p == NULL)
		return false;

	QThread* workerthread = new QThread();
	LoginThread *runciwa = new LoginThread();
	runciwa->moveToThread(workerthread);
	QObject::connect(runciwa, &LoginThread::finished, workerthread, &QThread::quit);
	QObject::connect(workerthread, &QThread::finished, workerthread, &QObject::deleteLater);	//线程结束删除
	QObject::connect(workerthread, &QThread::finished, runciwa, &QObject::deleteLater);			//线程结束删除
	QObject::connect(this, &Login::CiwaThread, runciwa, &LoginThread::DoWork);
	QObject::connect(runciwa, &LoginThread::resultReady, this, &Login::on_ciwaThread);
	QObject::connect(CiWaMsgDialog::GetDialogPoint(), &messageDialog::SendMesSignal, this,&Login::CiWaInitFail, Qt::BlockingQueuedConnection);

	workerthread->start();
	emit CiwaThread(p);
	return true;
}
void Login::CiWaInitFail(QString shead, QString msg)
{
	CiWaMsgDialog::MessageT(shead, msg);

}
//初始化数据库
void Login::InitDatabase()
{
	DatabaseSqlite::get_db()->Init("127.0.0.1", "root", "123456", "cicai");

}

void Login::on_btnCancel_clicked()
{
	//m_isruning = false;
	//this->close();
	//测试时用
	//开启进度条
	ui.groupBox->hide();
	//显示进度条
	ui.progressBar->show();

	InitLogin(this->m_pciwa);

}
//进度条结束
void Login::on_ciwaThread()
{
	m_isruning = true;
	ciwa* p = (ciwa*)m_pciwa;
	p->InitCiwaUI();
	this->close();

}

bool Login::SetPCiwa(void * p)
{
	if (p == NULL)
		return false;

	this->m_pciwa = p;
	return true;
}


void Login::on_btnOk_clicked()
{
	if (DatabaseSqlite::get_db() == NULL)
		return;
	
	QSqlQuery * ret;
	QString user;
	QString pwd;
	user = ui.lineEdit->text();
	pwd = ui.lineEdit_2->text();
	if (user == "")
	{
		CiWaMsgDialog::MessageT("登入失败", "用户名不能为空");
		//QMessageBox::warning(this, "", "用户名不能为空");
	}
	else if (pwd == "")
		CiWaMsgDialog::MessageT("登入失败", "密码不能为空");
		//QMessageBox::warning(this, "", "密码不能为空");
	else
	{
		QSqlQuery * ret;
		std::list<QString> key;
		std::map<QString, QString> dbwhere;
		std::list<std::list<QString>> row;
		dbwhere["name"] = user;
		dbwhere["pwd"] = pwd;
		key.push_back("*");
		DatabaseSqlite::get_db()->Find("user_login", key, dbwhere, &row, &ret);

		if (row.size() > 0)
		{
			m_isruning = true;
			//开启进度条
			ui.groupBox->hide();
			//显示进度条
			ui.progressBar->show();

			InitLogin(this->m_pciwa);
		}
		else
		{
			CiWaMsgDialog::MessageT("登入失败", "密码错误");
			//QMessageBox::warning(this, "", "密码错误");
		}

	}
}