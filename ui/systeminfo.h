#pragma once
#include <WinSock2.h>
#include <QSysInfo>
#include "QSettings"
#include "QDebug"
#include <QDesktopWidget>
#include <QFileInfoList>
#include <QDir>
#include <QLibrary>
#include <QTimer>
#include <QtNetwork/qhostinfo.h>
#include <QtNetwork/qnetworkinterface.h>
#include <QtNetwork/qhostaddress.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkrequest.h>
#include <QtNetwork/qnetworkreply.h>
#include <QObject>
#include <qeventloop.h>
#include <Windows.h>
#include <qapplication.h>
#include <iphlpapi.h>  

class SystemInfo : public QObject
{
	Q_OBJECT
		
private:
	QString m_cpudescribe;				//cpu
	LONGLONG m_totalMem;				//���ڴ�
	LONGLONG m_freeMem;					//�����ڴ�	
	QString m_memDescribe;
	QString m_osdescirbe;				//����ϵͳ
	QSize m_maxscreensize;
	QString m_screendescribe;
	QString m_diskdescribe;
	double m_maxfreedisk;
	LONGLONG m_gb ;
	PIP_ADAPTER_INFO pAdapter;
	PIP_ADAPTER_ADDRESSES pAddresses;
	QString GetHtml(QString url);
public:
	SystemInfo(QObject *parent);
	~SystemInfo();
	PIP_ADAPTER_INFO get_network_info() { return pAdapter; }
	PIP_ADAPTER_ADDRESSES get_network_name() { return pAddresses; }

	//��ȡ�����CPU��Ϣ
	const QString& CPUType();
	//��ȡ�������
	const QString LocalMachineName();
	//��ȡIP��ַ
	const QString IP();
	//��ȡ����IP
	const QString PublicIp();
	//��ȡmac��ַ
	const QString MAC();
	//��ȡ�Կ���Ϣ
	const QString GraphicsCard();
	//��ȡ�ڴ���Ϣ
	const QString& Memory();
	//��ȡ����ϵͳ
	const QString& OSVersion();
	//��ȡ�������Ļ�����Լ�����
	const QString& Screen();
	//��ȡ�����Ӳ����Ϣ
	const QString& Disk();
	//��ȡ������Ϣ
	ULONG IPCard();
	ULONG IPCardFriendlyName();
	//��ȡDNS��Ϣ
	QStringList GetDns(PIP_ADAPTER_INFO p);
	//��������״̬
	bool GetAdapterState(PIP_ADAPTER_INFO p);

};
