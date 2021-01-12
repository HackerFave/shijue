#include "loginthread.h"
#include "ciwa.h"
#include "ciwamsgdialog.h"
LoginThread::LoginThread(QObject *parent)
	: QObject(parent)
{
	
}

LoginThread::~LoginThread()
{
}



void LoginThread::DoWork(void *p)
{
	ciwa* pciwa;
	pciwa = (ciwa*)p;
	pciwa->InitCiwa();
	emit resultReady();
	emit finished();
}