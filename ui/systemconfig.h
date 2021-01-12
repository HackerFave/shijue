#pragma once

#include <QWidget>
#include "ui_systemconfig.h"
#include "customtabstyle.h"
#include "systeminfo.h"
#include "cpumemorylabel.h"
#include "gpumemorylabel.h"
#include "writeregistermodel.h"
#include "../socket/ciwasocket.h"
#include <QStandardItemModel>
#include <QElapsedTimer>

class SystemConfig : public QWidget
{
	Q_OBJECT

public:
	SystemConfig(QWidget *parent = Q_NULLPTR, QRect rect = QRect());
	~SystemConfig();
	void SetSocketLab(bool is);
private:
	GpuMemoryLabel *m_gpu;
	int m_plcindex;
	bool m_isnetwork;
	bool m_istabinit[7];
private:
	Ui::SystemConfig ui;
	SystemInfo *m_systeminfo;
	QString m_currcam;
	QStringList m_listwindow;
	void *m_pwindow;
    void *m_cameratable;
    WriteRegisterModel *writeModel;
	std::vector<QString> valuse_alarm_status_names;
	std::vector<QString> valuse_shusongji_low_names;
	std::vector<QString> valuse_shusongji_high_names;
	std::vector<int> valuse_shebeimoshi;

	void InitTab();
	//基本参数
	void InitTab1();
	//用户管理
	void InitTab2();
	//网络配置
	void InitTab3();
	//相机配置
	void InitTab4();
	//信息系统
	void InitTab5();
	//工位显示
	void InitTab8();
	//获得相机参数
	void GetCameraParam(QString camname);
	//设置相机参数
	void SetCameraParam(QString camname);
	//初始化报警状态ID
	void InitAlarmTable();
	//账号管理
	bool UserManage(bool is);
	//清空输入栏
	void ClearUserEdit();

	void InitTab0();

	int GetBitStatu(int num, int pos);
private slots:
	void OnCellClicked(int r, int c);
	//设置相机参数按钮
	void on_btnSet_clicked();
	//设置用户名密码按钮
	void on_modifypwd_clicked();
	//添加用户按钮
	void on_addpwd_clicked();
	//删除用户按钮
	void on_delpwd_clicked();
	//网络测试按钮
	void on_btnNetTest_clicked();
	//开启网络
	void on_ptnNetwork_clicked();
	//保存设置
	void on_btnSocketSave_clicked();
	//设为主机槽
	void on_radioIsMaster_clicked();
	//设为从机槽
	void on_radioIsSlave_clicked();
	//UI测试槽
	void on_btnUITest_clicked();
	//UI测试槽2
	void on_btnUITest_2_clicked();
	//网络设置编辑
	void on_btnEdit_clicked();
	//网络设置取消
	void on_btnSocketChanel_clicked();
	//网络自定义开关
	void on_btnSwitch_change(bool is);
	//相机自定义水平翻转开关
	void on_btnSPFZ_change(bool is);
	//相机配置查看
	void OnCameraConfigClicked();
	//相机配置编辑
	void on_btnCameraEdit_clicked();
	//相机取消参数
	void on_btnReload_clicked();
	//显示工位刷新
	void on_btnShowRefresh_clicked();
	//屏幕选择
	void OnDDisplay(int index);
	//视口选择
	void OnDWindow(int index);
	//视口重置
	void on_btnShowReset_clicked();
	//视口保存
	void on_btnShowSave_clicked();
    //相机断开
    void on_btnCloseDevice_clicked();
    //相机重载
    void on_btnReLoadDevice_clicked();

	void OnSaveBaseParam();
	void OnReadBaseParam();

	void OnTabChange(int index);

public:
	signals:
	void SendCustomFenpSignal();
};
