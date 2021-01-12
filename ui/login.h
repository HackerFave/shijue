#pragma once

#include <QDialog>
#include "ui_login.h"
#include "databasesqlite.h"
class Login : public QDialog
{
	Q_OBJECT

public:
	Login(QWidget *parent = Q_NULLPTR);
	~Login();

private:
	Ui::Login ui;

private slots:
	void on_btnOk_clicked();
	void on_btnCancel_clicked();
	void on_ciwaThread();
	void CiWaInitFail(QString shead, QString msg);
public:
	bool m_isruning;
	bool IsRuning() { return m_isruning; }
	bool SetPCiwa(void*p);

public:signals :
	   void CiwaThread(void* p);
private:
	void InitDatabase();
	bool InitLogin(void *p);
	void *m_pciwa;
	
};
