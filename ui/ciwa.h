#pragma once
#include <QTime>
//#include <QtWidgets/QMainWindow>
#include <qmessagebox.h>
#include <QKeyEvent>
#include "planmsg.h"
#include "planmsgdialog.h"
#include "ui_ciwa.h"
#include "common.h"
#include "qdir.h"
#include "systemconfig.h"
#include "runframenewui.h"
#include "debugconfig.h"
#include "importandexport.h"
#include "manuvisioninterface.h"
#include "plcinterface.h"
#include "dataMainFormZJ.h"
#include "databaseSqlInset.h"
#include "dataSql.h"
class ciwa : public QWidget, ManuvisionInterface
{
	Q_OBJECT
        Q_PLUGIN_METADATA(IID "org.qt-project.Qt.Examples.ManuvisionInterface" FILE "manuvisionInterface.json")
        Q_INTERFACES(ManuvisionInterface)

	
public:
	ciwa(QWidget *parent = Q_NULLPTR);
	~ciwa();
    void* GetWidgetPoint();

private:
	Ui::ciwaClass ui;
	
public: signals:
	void SendAddDataBase(std::map<QString, QString>);
    void signaModelName(QString&);
public slots:
	//更新检测速度
	void OnSendUpdatePpm(int totalframenum);
	//处理软件报警
	void OnSendSoftwareAlarm(QString str);
	//单屏信号
	//void OnSendSingleSignal();
	//多屏信号
	//void OnSendMultiSignal();
	//方案检测按钮
	void on_btnDetectionScheme_clicked();
	//系统设置按钮
	void on_btnSystemSetting_clicked();
    //中集
    void on_toolButton_zhongji_clicked();
	//调试按钮
	void on_btnDebugConfig_clicked();
	//启动按钮
	void on_btnStart_clicked();
	//停止按钮
	void on_btnStop_clicked();
	//运行按钮
	void on_btnRun_clicked();
	//退出按钮
	void on_btnExit_clicked();
	//网络状态栏
	void on_SendStatusBar(QString msg);
	//导入导出按钮
	void on_btnImportAndExport_clicked();
	//接受添加数据库信号
	void OnSendAddDataBase(std::map<QString, QString>);
	
	void frameDoned(const imginfo& img);

	void resetClicked();

	//ciwa类初始化
	void InitCiwa();
	void InitCiwaUI();
	//接受线程中弹出对话框失败
	void CiWaInitFail(QString shead, QString msg);
    //发送方案名称
    
    void slotGetModeName(bool);
    void slotRunFrameModelName(QString& name);//方案名称
private:
	//方案检测按钮
	PlanMsg *m_planmsg;
	//系统配置
	SystemConfig *m_systemconfig;
	//运行按钮
	//更改ui的界面
	RunFrameNewUi *m_runframenewui;
	//调试按钮
	DebugConfig *m_debugconfig;
	//导入导出按钮
	ImportAndExport *m_importandexport;
    //中集
    dataMainFormZJ *p_dataMainFormZJ = Q_NULLPTR;
	//是正在否运行
	bool m_isrunning;

	QTime m_starttime;
private:
	//创建ini目录
	void CreadteIniPath();
	//隐藏所有子窗口
	void HideChildrenFrame();
    //处理plc消息
    void plcmessageprocess(plc::PlcMessageType msg);
    //加载plc配置文件
    void plcloadini();
	void Start();
	void Stop();
	bool ShutDown();
	//键盘事件
	void keyReleaseEvent(QKeyEvent *ev);
};
