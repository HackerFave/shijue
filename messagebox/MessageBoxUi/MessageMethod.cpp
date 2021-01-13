#include "MessageMethod.h"
MessageMethod * MessageMethod::p_messagemethod_ = NULL;
MessageMethod::MessageMethod()
{
    m_msgdialog = new MessageBoxGui();
}

bool MessageMethod::GetReturn()
{
    MessageBoxGui *msgdialog = Instance()->m_msgdialog;
    return msgdialog->GetCurrentIndex();
}

//³É¹¦
void MessageMethod::MessageT(HINT_FLAG flag, QString shead)
{
    MessageBoxGui *msgdialog = Instance()->m_msgdialog;
    msgdialog->SetMessageHint(flag, shead);
    msgdialog->exec();
}
void MessageMethod::MessageOperTips(QString tips)
{
	MessageBoxGui *msgdialog = Instance()->m_msgdialog;
	msgdialog->MessageOperT(tips);
	msgdialog->exec();
}
MessageMethod * MessageMethod::Instance()
{
    if (!p_messagemethod_)
        p_messagemethod_ = new MessageMethod();
    return p_messagemethod_;
}

MessageBoxGui * MessageMethod::GetDialogPoint()
{
    return Instance()->m_msgdialog;
}
MessageMethod::~MessageMethod()
{
}