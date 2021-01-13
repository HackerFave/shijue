
#include "qstring.h"
#include "messageboxgui.h"
class  MessageMethod
{
public:
    ~MessageMethod();
private:
    MessageMethod();
public:
    static bool GetReturn();
    static void MessageT(HINT_FLAG flag, QString shead);
	static void MessageOperTips(QString tips);
    static MessageMethod* Instance();
    static MessageBoxGui* GetDialogPoint();
private:
    static MessageMethod* p_messagemethod_;

private:
    MessageBoxGui * m_msgdialog = Q_NULLPTR;

};
