#include "systeminfo.h"
#pragma comment(lib,"Iphlpapi.lib") //需要添加Iphlpapi.lib库
SystemInfo::SystemInfo(QObject *parent)
	: QObject(parent)
{
	m_gb = 1024 * 1024 * 1024;
	m_maxfreedisk = 0;
	pAdapter = NULL;
	pAddresses = NULL;
}

SystemInfo::~SystemInfo()
{
	if (pAdapter != NULL)
	{
		free(pAdapter);
		pAdapter = NULL;
	}
	if (pAddresses != NULL)
	{
		free(pAddresses);
		pAddresses = NULL;
	}
}


const QString SystemInfo::LocalMachineName()
{
	QString machineName = QHostInfo::localHostName();
	return machineName;
}

const QString SystemInfo::PublicIp()
{
	{
		QString strIp;
		QString webCode = GetHtml("http://whois.pconline.com.cn/");

		if (!webCode.isEmpty())
		{
			QString web = webCode.replace(" ", "");
			web = web.replace("\r", "");
			web = web.replace("\n", "");
			QStringList list = web.split("<br/>");
			QString tar = list[3];
			QStringList ip = tar.split("=");
			strIp = ip[1];
		}
		else
			strIp = QString::fromLocal8Bit("无法获取公网ip");


		return strIp;
	}
}


const QString SystemInfo::GraphicsCard()
{

	QString dcard;
	QSettings *DCard = new QSettings("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\nvlddmkm\\Video", QSettings::NativeFormat);
	QString type = DCard->value("DeviceDesc").toString();
	delete DCard;

	QString dType = type;
	dType.trimmed();
	if (!dType.isEmpty())
		dcard = dType;

	DCard = new QSettings("HKEY_LOCAL_MACHINE\\SYSTEM\\ControlSet001\\Services\\igfx\\Device0", QSettings::NativeFormat);
	type = DCard->value("DeviceDesc").toString();
	delete DCard;

	dType = type;
	dType.trimmed();
	if (!dType.isEmpty())
		dcard = dcard + "\n" + dType;

	DCard = new QSettings("HKEY_LOCAL_MACHINE\\SYSTEM\\ControlSet001\\Services\\amdkmdap\\Device0", QSettings::NativeFormat);
	type = DCard->value("Device Description").toString();
	delete DCard;

	dType = type;
	dType.trimmed();
	if (!dType.isEmpty())
		dcard = dcard + "\n" + dType;

	dcard.trimmed();
	return dcard;
}

const QString & SystemInfo::Memory()
{
	
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	m_totalMem = statex.ullTotalPhys  * 1.0 / m_gb;
	m_freeMem = statex.ullAvailPhys * 1.0 / m_gb;

	m_memDescribe = QString::fromLocal8Bit("可用 %1 GB / 共 %2 GB").
		arg(QString::asprintf("%.2f", m_freeMem)).arg(QString::asprintf("%.2f", m_totalMem));

	return m_memDescribe;
}

const QString & SystemInfo::OSVersion()
{
	typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
	LPFN_ISWOW64PROCESS fnIsWow64Process;
	BOOL bIsWow64 = FALSE;
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");
	if (NULL != fnIsWow64Process)
	{
		fnIsWow64Process(GetCurrentProcess(), &bIsWow64);
	}
	QString sysBit = "unknown";
	if (bIsWow64)
		sysBit = "64bit";
	else
		sysBit = "32bit";

	m_osdescirbe = QSysInfo::prettyProductName() + " " + sysBit;
	return m_osdescirbe;
}

const QString & SystemInfo::Screen()
{
	m_screendescribe = "";
	QList<QSize> screenSizeList;
	QList <int> screenCountList;
	
	for (int i = 0; i < QApplication::desktop()->screenCount(); i++)
	{
		QRect screenRect = QApplication::desktop()->screenGeometry(i);
		QSize size(screenRect.width(), screenRect.height());

		bool bExist = false;
		for (int j = 0; j < screenSizeList.length(); j++)
		{
			QSize existSize = screenSizeList.at(j);
			if (size == existSize)
			{
				screenCountList[j]++;
				bExist = true;
				break;
			}
		}

		if (!bExist)
		{
			screenSizeList.append(size);
			screenCountList.append(1);
		}
	}

	m_maxscreensize = screenSizeList.at(0);
	for (int i = 0; i < screenSizeList.length(); i++)
	{
		int width = screenSizeList.at(i).width();
		int height = screenSizeList.at(i).height();

		if (width > m_maxscreensize.width() && height > m_maxscreensize.height())
			m_maxscreensize = screenSizeList.at(i);

		m_screendescribe += QString("(%1pixel x %2pixel) x %3").arg(width).arg(height).arg(screenCountList.at(i));
		if (i != screenSizeList.length() - 1)
			m_screendescribe += "、 ";
	}
	return m_screendescribe;
}

ULONG SystemInfo::IPCard()
{
	if (pAdapter != NULL)
	{
		free(pAdapter);
		pAdapter = NULL;
	}
	PIP_ADAPTER_INFO pAdapterInfo;
	DWORD errValue = 0;
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
	
	if (!pAdapterInfo)//malloc失败
		return 0;
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) //空间不够，重新分配
	{
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);
		if (!pAdapterInfo) //malloc失败
			return 0;
	}

	
	if ((errValue = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
	{
		pAdapter = pAdapterInfo;
	}
	int a = sizeof(*pAdapter);
	return ulOutBufLen;
}


ULONG SystemInfo::IPCardFriendlyName()
{
	if (pAddresses != NULL)
	{
		free(pAddresses);
		pAddresses = NULL;
	}
	ULONG outBufLen = 0;
	DWORD dwRetVal = 0;
	PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
	ULONG flags = GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_INCLUDE_GATEWAYS;//包括 IPV4 ，IPV6 网关
	ULONG family = AF_UNSPEC;										  //返回包括 IPV4 和 IPV6 地址
	do
	{
		pCurrAddresses = (IP_ADAPTER_ADDRESSES *)malloc(outBufLen);
		if (pCurrAddresses == NULL)
			return 0;
		dwRetVal = GetAdaptersAddresses(family, flags, NULL, pCurrAddresses, &outBufLen);
		if (dwRetVal == ERROR_BUFFER_OVERFLOW)
		{
			free(pCurrAddresses);
			pCurrAddresses = 0;
		}
		else
		{
			pAddresses = pCurrAddresses;
			break;
		}
	} while (dwRetVal == ERROR_BUFFER_OVERFLOW);

	return outBufLen;
}



QStringList SystemInfo::GetDns(PIP_ADAPTER_INFO p)
{
	if (!p)
		return QStringList();
	IP_PER_ADAPTER_INFO* pPerAdapt = NULL;
	ULONG ulLen = 0;
	QStringList str;
	int err = GetPerAdapterInfo(p->Index, pPerAdapt, &ulLen);
	if (err == ERROR_BUFFER_OVERFLOW)
	{
		pPerAdapt = (IP_PER_ADAPTER_INFO*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, ulLen);
		err = GetPerAdapterInfo(p->Index, pPerAdapt, &ulLen);
		if (err == ERROR_SUCCESS)
		{
			IP_ADDR_STRING* pNext = &(pPerAdapt->DnsServerList);
			while(pNext)
			{ 
				if (pNext && strcmp(pNext->IpAddress.String, "") != 0)//手动DNS
				{
					str.append(pNext->IpAddress.String);
				}
				pNext = pNext->Next;
			}
		}
		HeapFree(GetProcessHeap(), HEAP_ZERO_MEMORY, pPerAdapt);
	}

	return str;
}

bool SystemInfo::GetAdapterState(PIP_ADAPTER_INFO p)
{
	PIP_ADAPTER_INFO pAdapterInfo;
	
	
	if (!p)
		return false;
	MIB_IFROW Info; 
	

	memset(&Info, 0, sizeof(MIB_IFROW));
	Info.dwIndex = p->Index;
	if (GetIfEntry(&Info) != NOERROR)
	{
		printf("ErrorCode = %d\n", GetLastError());
		return false;
	}
	if (Info.dwOperStatus == IF_OPER_STATUS_NON_OPERATIONAL
		|| Info.dwOperStatus == IF_OPER_STATUS_UNREACHABLE
		|| Info.dwOperStatus == 2
		|| Info.dwOperStatus == IF_OPER_STATUS_CONNECTING)
		return false;
	else if (Info.dwOperStatus == IF_OPER_STATUS_OPERATIONAL
		|| Info.dwOperStatus == IF_OPER_STATUS_CONNECTED)
		return true;
	return false;
}



const QString & SystemInfo::Disk()
{
	m_diskdescribe = "";
	QFileInfoList list = QDir::drives();
	foreach(QFileInfo dir, list)
	{
		QString dirName = dir.absolutePath();
		dirName.remove("/");
		LPCWSTR lpcwstrDriver = (LPCWSTR)dirName.utf16();
		ULARGE_INTEGER liFreeBytesAvailable, liTotalBytes, liTotalFreeBytes;
		if (GetDiskFreeSpaceEx(lpcwstrDriver, &liFreeBytesAvailable, &liTotalBytes, &liTotalFreeBytes))
		{
			QString free = QString::number((double)liTotalFreeBytes.QuadPart / m_gb, 'f', 1);
			free += "G";
			QString all = QString::number((double)liTotalBytes.QuadPart / m_gb, 'f', 1);
			all += "G";

			QString str = QString("%1 %2/%3       ").arg(dirName, free, all);
			m_diskdescribe += str;

			double freeMem = (double)liTotalFreeBytes.QuadPart / m_gb;

			if (freeMem > m_maxfreedisk)
				m_maxfreedisk = freeMem;
		}
	}

	return m_diskdescribe;
}

const QString SystemInfo::MAC()
{
	QString strMac;

	QList<QNetworkInterface> netList = QNetworkInterface::allInterfaces();
	foreach(QNetworkInterface item, netList)
	{
		if ((QNetworkInterface::IsUp & item.flags()) && (QNetworkInterface::IsRunning & item.flags()))
		{
			if (strMac.isEmpty() || strMac < item.hardwareAddress())
			{
				strMac = item.hardwareAddress();
			}
		}
	}
	return strMac;
}

const QString SystemInfo::IP()
{
	QString ip = "";

	QList<QNetworkInterface> interFaceList = QNetworkInterface::allInterfaces();
	for (int i = 0; i < interFaceList.size(); i++)
	{
		QNetworkInterface interfacea= interFaceList.at(i);
		if (interfacea.flags().testFlag(QNetworkInterface::IsRunning))
		{
			QList<QNetworkAddressEntry> entryList = interfacea.addressEntries();
			foreach(QNetworkAddressEntry entry, entryList)
			{
				if (QAbstractSocket::IPv4Protocol == entry.ip().protocol() &&
					entry.ip() != QHostAddress::LocalHost && entry.ip().toString().startsWith("192.168."))
				{
					ip = entry.ip().toString();
					break;
				}
			}
		}
	}
	return ip;
}



const QString& SystemInfo::CPUType()
{
	QSettings *CPU = new QSettings("HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", QSettings::NativeFormat);
	m_cpudescribe = CPU->value("ProcessorNameString").toString();
	delete CPU;
	return m_cpudescribe;
}



//获取网页所有源代码
QString SystemInfo::GetHtml(QString url)
{
	QNetworkAccessManager *manager = new QNetworkAccessManager();
	QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(url)));
	QByteArray responseData;
	QEventLoop eventLoop;
	QTimer *timer = new QTimer(this);
	timer->setSingleShot(true);

	connect(manager, SIGNAL(finished(QNetworkReply *)), &eventLoop, SLOT(quit()));
	connect(timer, SIGNAL(timeout()), &eventLoop, SLOT(quit()));

	timer->start(300);
	eventLoop.exec();
	responseData = reply->readAll();

	delete timer;

	return QString(responseData);
}
