#include "mrun_messagebox.h"
#include "MessageMethod.h"
MRUN_DLL void setmessage(HINT_FLAG flag, QString message)
{
    MessageMethod::Instance()->MessageT(flag, message);
}
bool messageopertips(QString message)
{
	MessageMethod::Instance()->MessageOperTips(message);
	return MessageMethod::GetReturn();
}