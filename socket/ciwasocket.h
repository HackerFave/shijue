#pragma once
#include <QTcpServer> //监听套接字
#include <QTcpSocket> //通信套接字//对方的(客户端的)套接字(通信套接字)
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
	//加载配置文件
	static bool CiWaSocketLoadIni();
	//设置配置文件
	static void CiWaSocketSetIni();
	//初始化网络
	static void CiWaSocketIni();
	//读取网络配置
	static bool ReadNetWorkIni();
	//写入网络配置
	static bool WriteNetWorkIni();
	//设置是否打开网络
	static void SetIsOpenNet(int is);
	//读取是否打开网络
	static int GetIsOpenNet();
	//设置当前为服务主/从 机
	static void SetCurrMaster(HostState ismaster);
	//读取当前主/从 机
	static  HostState GetCurrMaster();
	//连接
	static bool NetWorkStart();
	//停止
	static bool NetWorkStop();
	//测试
	static void NetWorkTest();
	//设置服务端 IP端口
	static bool NetServerWorkParam(QString ip, quint16 port);
	//设置本地   IP端口
	static bool NetLocalWorkParam(QString ip, quint16 port);
	//发送数据
	static bool NetWorkSendData(QJsonObject str, MSGID msg);
	//获得当前对象
	static CiWaSocket* GetCurrSocket();

	//获取本地参数
	static QString GetServerIp();
	static QString GetLocalIp();
	static quint16 GetServerPort();
	static quint16 GetLocalPort();
	static HostState GetHostState();
	
	//获取PLC参数
	static QString GetPLCIp();
	static quint16 GetPLCPort();
	//PLC设置参数
	static bool SetPLCip(QString str);
	static bool SetPLCPort(int port);
private:
	TcpServer *m_tcpserver;  //监听套接字	master
	
	QTcpSocket *m_tcpsocket;  //通信套接字
	HostState  m_ismaster;    //当前身份
	quint16	   m_serverport;  //服务端端口
	quint16	   m_localport;	  //本地端口
	QString    m_serverip;	  //服务端IP
	QString    m_localip;	  //本  地IP
	bool	   m_socketrunning;//是否在连接
	int		   m_isopennet;	  //是否打开网络	0关闭，1打开
	int		   readByteCount = 0;
	QString    temp;
	QString	   lastErrorString; //最后的错误文字  
	int        lastErrorCode;	//错误代码  
	QString	   m_plcip;			//plcip地址
	quint16    m_plcport;        //plc端口号
	QTimer *timer;//计时器

	//static CiWaSocket *g_ciwasocket;
private slots:
	void OnMasterConnection(qintptr handle, QTcpSocket* socket);	//master
	void OnReadClient();		//master
	void OnMasterError(QAbstractSocket::SocketError socketError);
	void OnDisconnected(qintptr handle);

	void OnSlaveConnection();	//slave
	bool ParseData(QJsonDocument &jsonDoc);			//解析数据
	void OnReadServer();		//slave
	void OnSlaveError(QAbstractSocket::SocketError socketError);
	void OnTimerOut();		//计时器
signals:
	void NewJsonData(const QJsonDocument &doc);//新的json数据  
signals:
	void ChangeGrade(const QJsonDocument &doc);
signals:
	void NetUpTable();
signals:
	void SendStatusBar(QString str);//发送状态栏信息 
signals:
	void JsonError(const int &errorCode, const QString &errorString);//json格式错误 	
signals:
	void SendReport();//请求发送获取批次编号消息
signals:
	void SetReport(const QJsonDocument &doc);//获取批次标号
signals:
	void SendStartRun(const QJsonDocument & doc);//发送开始运行
signals:
	void SendProduct(const QJsonDocument & doc);//发送产品信息

};

