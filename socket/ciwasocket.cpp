#include "ciwasocket.h"
#include "qapplication.h"
#include "qsettings.h"
#include "uicommon.h"
#include <qmessagebox.h>
#include <QTcpServer> //�����׽���
#include <QTcpSocket> //ͨ���׽���
#include <QJsonDocument>  

CiWaSocket *g_ciwasocket = NULL;

CiWaSocket::CiWaSocket()
{
	m_ismaster	= NORMAL;
	m_tcpserver = NULL;
	m_tcpsocket = NULL;
	m_serverport = 8888;
	m_localport = 8888;
	m_serverip = "127.0.0.1";
	m_localip = "127.0.0.1";
	m_plcport = 503;
	m_plcip = "192.168.2.1";

	m_socketrunning = false;
	m_isopennet = 0;
	readByteCount = 0;
	timer = new QTimer();
	timer->setInterval(3000);
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(OnTimerOut()));
}

bool CiWaSocket::SocketStart()
{
	switch (m_ismaster)
	{
		case MASTE:
			if (m_tcpserver != NULL)
			{
				return false;
			}
			m_tcpserver = new TcpServer();
			QObject::connect(m_tcpserver, &TcpServer::ClientConnected, this, &CiWaSocket::OnMasterConnection);
			QObject::connect(m_tcpserver, &QTcpServer::acceptError, this, &CiWaSocket::OnMasterError);
			//���Ͽ�����ʱ�������ź�
			if (!m_tcpserver->listen(QHostAddress(m_serverip), m_serverport))
			{
				m_socketrunning = false;
				QMessageBox::warning(NULL, "", "���������ʼ������,�������ý��������ã����ֶ���������");
				return false;
			}
			else
			{
				m_socketrunning = true;
			}

			break;
		case SLAVE:
			if (m_tcpsocket != NULL)
				return false;
			m_tcpsocket = new QTcpSocket(); 
			if (!(m_tcpsocket->bind(QHostAddress(m_localip), 0, QAbstractSocket::ReuseAddressHint)))
			{
				QMessageBox::warning(NULL, "", "�󶨴���");
				return false;
			}
			m_tcpsocket->setSocketOption(QAbstractSocket::LowDelayOption, "1"); //����nagle���ӳ��㷨  
			m_tcpsocket->setSocketOption(QAbstractSocket::KeepAliveOption, "1"); //���ñ�������״̬

			//2. �����ӡ����ݿɶ���ʧ���ź�����
			QObject::connect(m_tcpsocket, &QTcpSocket::connected, this, &CiWaSocket::OnSlaveConnection);
			QObject::connect(m_tcpsocket, &QIODevice::readyRead, this, &CiWaSocket::OnReadServer);
			QObject::connect(m_tcpserver, &TcpServer::ClientDisconnected, this, &CiWaSocket::OnDisconnected);
			typedef void (QAbstractSocket::*QAbstractSocketErrorSignal)(QAbstractSocket::SocketError);
			QObject::connect(m_tcpsocket, static_cast<QAbstractSocketErrorSignal>(&QTcpSocket::error), this, &CiWaSocket::OnSlaveError);
			//3. ��������˽�������
			m_tcpsocket->connectToHost(m_serverip, m_serverport);
			m_socketrunning = false;
			//�ڴ˷�������
			timer->start();

			break;
		case NORMAL:
			break;
		default:
			return false;
			break; 
	}
	return true;
}

bool CiWaSocket::SocketStop()
{
	timer->stop();
	m_socketrunning = false;
	switch (m_ismaster)
	{
	case MASTE:
		
		if (m_tcpserver != NULL)
		{
			m_tcpserver->close();
			delete m_tcpserver;
			m_tcpserver = NULL;
		}


		break;
	case SLAVE:
		//�����ͶԷ��Ͽ�����
		if(m_tcpsocket!=NULL)
		{ 
			m_tcpsocket->disconnectFromHost();
			m_tcpsocket->close();
			delete m_tcpsocket;
			m_tcpsocket = NULL;
		}
		break;
	case NORMAL:
		break;
	default:
		break;
	}

	return true;
}

bool CiWaSocket::SocketTest()
{
	//return this->SocketSend(QString("{\"test\":\"1\"}"), MSGID::TEST);
	QJsonObject jsonObject;
	jsonObject["test"] = 1;
	return this->SocketSend(jsonObject, MSGID::TEST);
}

bool CiWaSocket::SocketHeart()
{
	QJsonObject jsonObject;
	jsonObject["heart"] = 1;
	return this->SocketSend(jsonObject, MSGID::HEART);
}

bool CiWaSocket::SocketSend(QJsonObject str, MSGID msg)
{
	QTcpSocket *socket;

	if (m_ismaster == HostState::SLAVE)
	{
		if (m_tcpsocket == NULL)
			return false;
		socket = m_tcpsocket;
	}

	if (m_ismaster == HostState::MASTE)
	{
		if (m_tcpserver == NULL)
			return false;
		else
		{
			if (m_tcpserver->get_clientlist()->clients.count()<1)
			{
				return false;
			}
			socket = m_tcpserver->get_clientlist()->clients.begin().value();
		}
	}
	int bytes = -1;

	//����json����
	QByteArray array;
	array.prepend((QJsonDocument(str)).toJson(QJsonDocument::Compact));
	int len = array.count();
	array.prepend(QByteArray::number(len), READ_RAW_LEN);
	
	switch (msg)
	{
	case PLG:
		array.prepend('p');
		break;
	case COUNT:
		array.prepend('c');
		break;
	case TEST:
		array.prepend('t');
		break;
	case HEART:
		array.prepend('h');
		break;
	case SGRADE:
		array.prepend('g');
		break;
	case GETTABLE:
		array.prepend('u');
		break;
	case REPORT:
		array.prepend('r');
		break;
	case SLAVESTART:
		array.prepend('s');
	case SQL:
		array.prepend('q');
	}
	
	bytes = socket->write(array);
	socket = NULL;
	return bytes != -1;
}

bool CiWaSocket::SocketSetServerParam(QString ip, quint16 port)
{
	m_serverip = ip;
	m_serverport = port;
	return true;
}

bool CiWaSocket::SocketSetLocalParam(QString ip, quint16 port)
{
	m_localip = ip;
	m_localport = port;
	return true;
}

bool CiWaSocket::CiWaSocketLoadIni()
{
	if (g_ciwasocket == NULL)
		return false;
	QString path = QCoreApplication::applicationDirPath() + '/' + QString(CIWASOCKETININAME);
	QSettings configini(path, QSettings::IniFormat);
	int temp;
	
	configini.beginGroup(COMMUNICATION);
	//��ȡ�Ƿ�Ϊ������ӻ�
	temp = configini.value(ISMASTER).toInt();
	switch (temp)
	{
	case 0:
		g_ciwasocket->m_ismaster = HostState::SLAVE;
		break;
	case 1:
		g_ciwasocket->m_ismaster = HostState::MASTE;
		break;
	default:
		g_ciwasocket->m_ismaster = HostState::NORMAL;
		break;
	}
	//��ȡ����˵�ַ
	g_ciwasocket->m_serverip = configini.value(SERVERIP).toString();
	
	//��ȡ����˶˿�
	g_ciwasocket->m_serverport = configini.value(SERVERPORT).toUInt();

	//��ȡ���ص�ַ
	g_ciwasocket->m_localip = configini.value(LOCALIP).toString();

	//��ȡ���ض˿�
	g_ciwasocket->m_localport = configini.value(LOCALPROT).toUInt();

	//��ȡ�Ƿ��
	g_ciwasocket->m_isopennet = configini.value(OPENNET).toInt();
	
	//��ȡPLC��IP��ַ
	g_ciwasocket->m_plcip = configini.value(PLCIP).toString();
	//���PLC�Ķ˿ں�
	g_ciwasocket->m_plcport = configini.value(PLCPORT).toUInt();
	configini.endGroup();

	return true;
}

void CiWaSocket::CiWaSocketSetIni()
{
	QString path = QCoreApplication::applicationDirPath() + '/' + QString(CIWASOCKETININAME);
	QSettings configini(path, QSettings::IniFormat);
	configini.beginGroup(COMMUNICATION);
	configini.setValue(ISMASTER, g_ciwasocket->m_ismaster);
	configini.setValue(SERVERIP, g_ciwasocket->m_serverip);
	configini.setValue(SERVERPORT, g_ciwasocket->m_serverport);
	configini.setValue(LOCALIP, g_ciwasocket->m_localip);
	configini.setValue(LOCALPROT, g_ciwasocket->m_localport);
	configini.setValue(OPENNET, g_ciwasocket->m_isopennet);
	configini.setValue(PLCIP, g_ciwasocket->m_plcip);
	configini.setValue(PLCPORT, g_ciwasocket->m_plcport);
	configini.endGroup();
	
}

CiWaSocket::~CiWaSocket()
{
	if (m_tcpserver != NULL)
	{
		delete m_tcpserver;
		m_tcpserver = NULL;
	}

	if (m_tcpsocket != NULL)
	{
		delete m_tcpsocket;
		m_tcpsocket = NULL;
	}
	if (timer != NULL)
	{
		timer->stop();
		delete timer;
	}
}

void CiWaSocket::CiWaSocketIni()
{
	if (g_ciwasocket == NULL)
	{
		g_ciwasocket = new CiWaSocket();
		CiWaSocketLoadIni();
	}
}

bool CiWaSocket::ReadNetWorkIni()
{
	if(!g_ciwasocket)
		return false;

	return true;
}

bool CiWaSocket::WriteNetWorkIni()
{
	if (!g_ciwasocket)
		return false;

	return true;
}

void CiWaSocket::SetIsOpenNet(int is)
{
	if (!g_ciwasocket)
		return;
	g_ciwasocket->m_isopennet = is;
}

int CiWaSocket::GetIsOpenNet()
{
	if (!g_ciwasocket)
		return 0;
	return g_ciwasocket->m_isopennet;
}

void CiWaSocket::SetCurrMaster(HostState ismaster)
{
	if (!g_ciwasocket)
		return;
	g_ciwasocket->m_ismaster = ismaster;
}
HostState CiWaSocket::GetCurrMaster()
{
	if (!g_ciwasocket)
		return HostState();
	return g_ciwasocket->m_ismaster;
}


bool CiWaSocket::NetWorkStart()
{
	if (!g_ciwasocket)
		return false;
	g_ciwasocket->SocketStart();
	return true;
}

bool CiWaSocket::NetWorkStop()
{
	if (!g_ciwasocket)
		return false;
	g_ciwasocket->SocketStop();
	return true;
}

void CiWaSocket::NetWorkTest()
{
	//if (g_ciwasocket->m_ismaster != SLAVE)
	//	return;
	g_ciwasocket->SocketTest();
}

bool CiWaSocket::NetServerWorkParam(QString ip, quint16 port)
{
	if (!g_ciwasocket)
		return false;
	return g_ciwasocket->SocketSetServerParam(ip, port);
}

bool CiWaSocket::NetLocalWorkParam(QString ip, quint16 port)
{
	if (!g_ciwasocket)
		return false;
	return g_ciwasocket->SocketSetLocalParam(ip, port);
}

bool CiWaSocket::NetWorkSendData(QJsonObject str, MSGID msg)
{
	if (!g_ciwasocket)
		return false;
	return g_ciwasocket->SocketSend(str, msg);
}

CiWaSocket * CiWaSocket::GetCurrSocket()
{
	return g_ciwasocket;
}

QString CiWaSocket::GetServerIp()
{
	if (!g_ciwasocket)
		return QString();
	return g_ciwasocket->m_serverip;
}

QString CiWaSocket::GetLocalIp()
{
	if (!g_ciwasocket)
		return QString();
	return g_ciwasocket->m_localip;
}

quint16 CiWaSocket::GetServerPort()
{
	if (!g_ciwasocket)
		return quint16();
	return g_ciwasocket->m_serverport;
}

quint16 CiWaSocket::GetLocalPort()
{
	if (!g_ciwasocket)
		return quint16();
	return g_ciwasocket->m_localport;
}

HostState CiWaSocket::GetHostState()
{
	if (!g_ciwasocket)
		return HostState(2);
	return g_ciwasocket->m_ismaster;
}

QString CiWaSocket::GetPLCIp()
{
	if (!g_ciwasocket)
		return QString();
	return g_ciwasocket->m_plcip;
}

quint16 CiWaSocket::GetPLCPort()
{
	if (!g_ciwasocket)
		return quint16();
	return g_ciwasocket->m_plcport;
}

bool CiWaSocket::SetPLCip(QString str)
{
	if(g_ciwasocket == NULL)
		return false;
	g_ciwasocket->m_plcip = str;
	return true;
}

bool CiWaSocket::SetPLCPort(int port)
{
	if (g_ciwasocket == NULL)
		return false;
	g_ciwasocket->m_plcport = port;
	return true;
}

//�µ�����
void CiWaSocket::OnMasterConnection(qintptr handle, QTcpSocket* socket)
{
	//��ȡ��ͻ���ͨ�ŵ�socket
	QObject::connect(socket, &QTcpSocket::readyRead, this, &CiWaSocket::OnReadClient);
	m_tcpsocket = socket;
	//����һ�����Ի�Ӧ
	this->SocketTest();
	m_tcpsocket = NULL;
	m_socketrunning = true;
}

//�ӿͻ��˶�ȡ
void CiWaSocket::OnReadClient()
{
	QTcpSocket * socket = dynamic_cast<QTcpSocket*>(sender());
	if (socket == NULL)
		return;
	//��ȡ�Է����͵�����
	if (this->readByteCount == 0)
	{
		char peek;
		socket->read(&peek, sizeof(peek));

		if (peek == 'c')			//��ȡͳ����Ϣ	//�鿴�Ƿ������ݰ�����ʼλ��  
		{
			QByteArray len = socket->read(READ_RAW_LEN);
			this->readByteCount = len.toInt();

			if (this->readByteCount > socket->bytesAvailable())
			{
				this->readByteCount = 0;
				return;

			}

			QByteArray json;
			json = socket->read(this->readByteCount);  //��ȡjson����  
			
			QJsonParseError parseError;
			QJsonDocument jsonDoc(QJsonDocument::fromJson(json, &parseError));
			if (parseError.error == QJsonParseError::NoError)  //json��ʽ�Ƿ���ȷ  
			{
				// qDebug()<<"read json data:"<<jsonDoc.toJson();  
				emit NewJsonData(jsonDoc); //����json�����ź�  
			}
			else
			{
				this->lastErrorCode = parseError.error;
				this->lastErrorString = parseError.errorString();
				emit JsonError(parseError.error, parseError.errorString()); //���ʹ������  
				qDebug() << "json format error:" << json;
			}
			this->readByteCount = 0;

		}
		else if (peek == 'p')		//����PLC
		{
			socket->readAll();
			
			return;
		}
		else if (peek == 't')		//ͨ�Ų���
		{
			//��ȡ�Է���IP�Ͷ˿�
			socket->readAll();
			qint16 port = socket->peerPort();
			QString ip = socket->peerAddress().toString();
			temp = QString("[%1:%2]:�ɹ�����").arg(ip).arg(port);
			//���ͻ�Ӧ
			m_tcpsocket = socket;
			//����һ�����Ի�Ӧ
			this->SocketTest();
			m_tcpsocket = NULL;

		//	QMessageBox::about(NULL, "", temp);
			
			return;
		}
		else if (peek == 'r')
		{
			socket->readAll();
			emit SendReport();
			return;
		}

		else //����Ͷ���ȫ��������  
		{
			socket->readAll();
			//qDebug()<<"read all data:"<<this->readAll();  
			return;
		}
	}
}

void CiWaSocket::OnMasterError(QAbstractSocket::SocketError socketError)
{
}

void CiWaSocket::OnDisconnected(qintptr handle)
{

}

void CiWaSocket::OnSlaveConnection()
{
	if (m_tcpsocket == NULL)
		return;
	//��ȡ�Է����͵�����
	QByteArray array = m_tcpsocket->readAll();
	m_socketrunning = true;

}
//��������
bool CiWaSocket::ParseData(QJsonDocument &jsonDoc)
{
	QByteArray len = m_tcpsocket->read(READ_RAW_LEN);
	this->readByteCount = len.toInt();

	if (this->readByteCount > m_tcpsocket->bytesAvailable())
	{
		this->readByteCount = 0;
		return false;
	}
	QByteArray json;
	json = m_tcpsocket->read(this->readByteCount);  //��ȡjson����  

	QJsonParseError parseError;
	jsonDoc = QJsonDocument(QJsonDocument::fromJson(json, &parseError));
	this->readByteCount = 0;

	if (parseError.error != QJsonParseError::NoError)  //json��ʽ�Ƿ���ȷ  
	{
		this->lastErrorCode = parseError.error;
		this->lastErrorString = parseError.errorString();
		emit JsonError(parseError.error, parseError.errorString()); //���ʹ������  
		qDebug() << "json format error:" << json;
		return false;
	}
	return true;
}


//�ӷ���˶�ȡ
void CiWaSocket::OnReadServer()
{
	QJsonDocument jsonDoc;
	if (m_tcpsocket == NULL)
		return;
	//��ȡ�Է����͵�����
	if (this->readByteCount == 0)
	{
		char peek;
		m_socketrunning = true;
		m_tcpsocket->read(&peek, sizeof(peek));
		if (peek == 't')
		{
			temp = QString("���������ɹ�");
			//�����źŸ�ciwa������
			emit SendStatusBar(temp); //����״̬����Ϣ  

		}
		else if (peek == 'g')
		{
			ParseData(jsonDoc);
			emit ChangeGrade(jsonDoc);
		}
		else if (peek == 'u')
		{
			emit NetUpTable();
		}
		else if (peek == 'r')
		{
			ParseData(jsonDoc);
			emit SetReport(jsonDoc);
		}
		else if (peek == 's')
		{
			ParseData(jsonDoc);
			emit SendStartRun(jsonDoc);

		}
		else if (peek == 'q')
		{
			ParseData(jsonDoc);
			emit SendProduct(jsonDoc);
		}
		else //����Ͷ���ȫ��������  
		{
			m_tcpsocket->readAll();
			//qDebug()<<"read all data:"<<this->readAll();  
			return;
		}
		readByteCount = 0;
	}
	m_tcpsocket->readAll();
}


void CiWaSocket::OnSlaveError(QAbstractSocket::SocketError socketError)
{
	//����ʧ�ܣ����Ӵ���Ϊ
	QString msg = QString("����ʧ��%1").arg(socketError);
	//QMessageBox::warning(NULL, "", msg);
	emit SendStatusBar(msg); //����״̬����Ϣ  
	//�����źŸ�ciwa������ �������
	m_socketrunning = false;

}

void CiWaSocket::OnTimerOut()
{
	if (m_tcpsocket == NULL)
		return;
	//��������
	if(m_socketrunning)
		this->SocketHeart();
	//��������
	else
	{
		//�����źŸ�ciwa������ ������������
		m_tcpsocket->connectToHost(m_serverip, m_serverport);
		QString msg = QString("��������..");
		emit SendStatusBar(msg); //����״̬����Ϣ  
	}
}
