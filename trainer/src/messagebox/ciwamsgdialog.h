#pragma once
#pragma execution_character_set("utf-8")
#include "qstring.h"
#include "messageDialog.h"
class CiWaMsgDialog
{
public:
	~CiWaMsgDialog();
private:
	CiWaMsgDialog();

public:
	static bool GetReturn();
	static void MessageT(QString shead);
	static void MessageT(QString shead, QString msg);
	static void MessageSignal(QString shead, QString msg);
	static CiWaMsgDialog* Instance();
	static messageDialog* GetDialogPoint();

private:
	static CiWaMsgDialog* m_ciwamsg;
	
private:
	messageDialog * m_msgdialog;

};

