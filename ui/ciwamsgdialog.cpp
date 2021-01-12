#include "ciwamsgdialog.h"


CiWaMsgDialog * CiWaMsgDialog::m_ciwamsg = NULL;
CiWaMsgDialog::CiWaMsgDialog()
{
	m_msgdialog = new messageDialog();
}

bool CiWaMsgDialog::GetReturn()
{
	messageDialog *msgdialog = Instance()->m_msgdialog;
	return msgdialog->getCurrentIndex();
}

//³É¹¦
void CiWaMsgDialog::MessageT(QString shead)
{
	messageDialog *msgdialog = Instance()->m_msgdialog;
	msgdialog->setMessageHint(shead, QString());
	msgdialog->exec();
}

//Ê§°Ü
void CiWaMsgDialog::MessageT(QString shead,QString msg)
{
	messageDialog *msgdialog = Instance()->m_msgdialog;
	msgdialog->setMessageHint(shead, msg);
	msgdialog->exec();
}

void CiWaMsgDialog::MessageSignal(QString shead, QString msg)
{
	emit Instance()->m_msgdialog->SendMesSignal(shead, msg);
}

CiWaMsgDialog * CiWaMsgDialog::Instance()
{
	if (!m_ciwamsg)
		m_ciwamsg = new CiWaMsgDialog();
	return m_ciwamsg;
}

messageDialog * CiWaMsgDialog::GetDialogPoint()
{
	return Instance()->m_msgdialog;
}


CiWaMsgDialog::~CiWaMsgDialog()
{
}
