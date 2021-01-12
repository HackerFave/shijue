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
	LONGLONG m_totalMem;				//总内存
	LONGLONG m_freeMem;					//可用内存	
	QString m_memDescribe;
	QString m_osdescirbe;				//操作系统
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

	//获取计算机CPU信息
	const QString& CPUType();
	//获取计算机名
	const QString LocalMachineName();
	//获取IP地址
	const QString IP();
	//获取公网IP
	const QString PublicIp();
	//获取mac地址
	const QString MAC();
	//获取显卡信息
	const QString GraphicsCard();
	//获取内存信息
	const QString& Memory();
	//获取操作系统
	const QString& OSVersion();
	//获取计算机屏幕个数以及像素
	const QString& Screen();
	//获取计算机硬盘信息
	const QString& Disk();
	//获取网卡信息
	ULONG IPCard();
	ULONG IPCardFriendlyName();
	//获取DNS信息
	QStringList GetDns(PIP_ADAPTER_INFO p);
	//网络连接状态
	bool GetAdapterState(PIP_ADAPTER_INFO p);

};
