#include "ciwa.h"
#include <QtWidgets/QApplication>
#include "login.h"
#include "dumpfile.h"
int main(int argc, char *argv[])
{
	
	QApplication a(argc, argv);
	SetUnhandledExceptionFilter(exceptionCallback);
	ciwa w;
	Login login;

	if (!login.SetPCiwa(&w))
		return 0;

	login.exec();
	
	if (!login.IsRuning())
		return 0;

	w.show();
	return a.exec();
}
