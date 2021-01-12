#pragma once

#include <QObject>

class LoginThread : public QObject
{
	Q_OBJECT

public:
	LoginThread(QObject *parent = NULL);
	~LoginThread();


public slots:
	void DoWork(void *p);

public:signals:
	   void resultReady();

public:signals:
		void finished();//Íê³ÉÐÅºÅ
};
