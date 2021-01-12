#pragma once
#include <QTcpServer> //�����׽���
#include <QTcpSocket> //ͨ���׽���//�Է���(�ͻ��˵�)�׽���(ͨ���׽���)
#include <QObject>
#include <qjsonobject.h>
#include <QTimer>
#include "tcp_server.h"
#include "tcp_server_private.h"
enum HostState {SLAVE, MASTE, NORMAL};
enum MSGID {PLG, COUNT, TEST, HEART, SGRADE, GETTABLE, REPORT,SLAVESTART,SQL};
class CiWaSocket : public QObject
{
	Q_OBJECT

public:
	~CiWaSocket();
	
private:
	CiWaSocket();
	bool SocketStart();
	bool SocketStop();
	bool SocketTest();
	bool SocketHeart();
	bool SocketSend(QJsonObject str, MSGID msg);
	bool SocketSetServerParam(QString ip, quint16);
	bool SocketSetLocalParam(QString ip, quint16);
public:
	//���������ļ�
	static bool CiWaSocketLoadIni();
	//���������ļ�
	static void CiWaSocketSetIni();
	//��ʼ������
	static void CiWaSocketIni();
	//��ȡ��������
	static bool ReadNetWorkIni();
	//д����������
	static bool WriteNetWorkIni();
	//�����Ƿ������
	static void SetIsOpenNet(int is);
	//��ȡ�Ƿ������
	static int GetIsOpenNet();
	//���õ�ǰΪ������/�� ��
	static void SetCurrMaster(HostState ismaster);
	//��ȡ��ǰ��/�� ��
	static  HostState GetCurrMaster();
	//����
	static bool NetWorkStart();
	//ֹͣ
	static bool NetWorkStop();
	//����
	static void NetWorkTest();
	//���÷���� IP�˿�
	static bool NetServerWorkParam(QString ip, quint16 port);
	//���ñ���   IP�˿�
	static bool NetLocalWorkParam(QString ip, quint16 port);
	//��������
	static bool NetWorkSendData(QJsonObject str, MSGID msg);
	//��õ�ǰ����
	static CiWaSocket* GetCurrSocket();

	//��ȡ���ز���
	static QString GetServerIp();
	static QString GetLocalIp();
	static quint16 GetServerPort();
	static quint16 GetLocalPort();
	static HostState GetHostState();
	
	//��ȡPLC����
	static QString GetPLCIp();
	static quint16 GetPLCPort();
	//PLC���ò���
	static bool SetPLCip(QString str);
	static bool SetPLCPort(int port);
private:
	TcpServer *m_tcpserver;  //�����׽���	master
	
	QTcpSocket *m_tcpsocket;  //ͨ���׽���
	HostState  m_ismaster;    //��ǰ���
	quint16	   m_serverport;  //����˶˿�
	quint16	   m_localport;	  //���ض˿�
	QString    m_serverip;	  //�����IP
	QString    m_localip;	  //��  ��IP
	bool	   m_socketrunning;//�Ƿ�������
	int		   m_isopennet;	  //�Ƿ������	0�رգ�1��
	int		   readByteCount = 0;
	QString    temp;
	QString	   lastErrorString; //���Ĵ�������  
	int        lastErrorCode;	//�������  
	QString	   m_plcip;			//plcip��ַ
	quint16    m_plcport;        //plc�˿ں�
	QTimer *timer;//��ʱ��

	//static CiWaSocket *g_ciwasocket;
private slots:
	void OnMasterConnection(qintptr handle, QTcpSocket* socket);	//master
	void OnReadClient();		//master
	void OnMasterError(QAbstractSocket::SocketError socketError);
	void OnDisconnected(qintptr handle);

	void OnSlaveConnection();	//slave
	bool ParseData(QJsonDocument &jsonDoc);			//��������
	void OnReadServer();		//slave
	void OnSlaveError(QAbstractSocket::SocketError socketError);
	void OnTimerOut();		//��ʱ��
signals:
	void NewJsonData(const QJsonDocument &doc);//�µ�json����  
signals:
	void ChangeGrade(const QJsonDocument &doc);
signals:
	void NetUpTable();
signals:
	void SendStatusBar(QString str);//����״̬����Ϣ 
signals:
	void JsonError(const int &errorCode, const QString &errorString);//json��ʽ���� 	
signals:
	void SendReport();//�����ͻ�ȡ���α����Ϣ
signals:
	void SetReport(const QJsonDocument &doc);//��ȡ���α��
signals:
	void SendStartRun(const QJsonDocument & doc);//���Ϳ�ʼ����
signals:
	void SendProduct(const QJsonDocument & doc);//���Ͳ�Ʒ��Ϣ

};

