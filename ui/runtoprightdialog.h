#pragma once
#include <QtCharts/QChart>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieLegendMarker>
#include <QtCharts/qchartview.h>
#include <QWidget>
#include "ui_runtoprightdialog.h"
#include "runframetable.h"




QT_CHARTS_USE_NAMESPACE
class RunTopRightDialog : public QWidget
{
	Q_OBJECT
	
public:
	RunTopRightDialog(QWidget *parent = Q_NULLPTR);
	~RunTopRightDialog();

private:
	Ui::RunTopRightDialog ui;
	void *m_creatreport;
	QPieSlice *m_goodserie;
	QPieSlice *m_badserie;
	QChartView *m_chartview;
	RunFrameTable *m_runframetable;
    //文件检测 true开启状态，false关闭状态
    bool m_isfilewather;
	//false 单, true 多
	bool m_iswindowing;
	//定时器标识
	int m_timeflag;
	//定时器开关
	bool m_isopentimer;
	//数据库批次
	QString m_batch;

private:
	//饼状图
	void InitChart();
	//创建报告对话框
	bool CreateReportDialog();
public:
	//返回table地址
	RunFrameTable * get_table() {  return m_runframetable; }
	//获得当前批次
	QString get_batch() { return m_batch; }

	void StarTime(long n);
	void StopTime();
	void DisableSolution(bool b);
	//将分屏后的表格还原到右下角
	void ShowRightBottomTable();
	//获得当前选择的检查方案名字
	QString GetCurrText();
	//初始化默认分屏
	void InitAutoFenp();
	//开始运行检测
	void StartRun(bool iscreatreport);
    //将最近使用的检测方案保存到系统配置文件中
    void SaveCurrenUseSolutionToIni();
    //读取配置文件中最近使用的检测方案
    void SetLastUseSolutionToIni();
    QString getProgramName();
public :

//signals:
//	void SendCreatReport();
signals:
	void SendUpdatePpm(int);
signals:
	//显示多屏信号
	void SendMultiSignal();
signals:
	//显示单屏信号
	void SendSingleSignal();
signals:
	//开始信号
	void SendStart();
signals:
	//停止信号
	void SendStop();
signals:
	//刷新屏幕
	void SendUpdataWindow();
signals:
	//刷新自定义屏幕
	void SendCustomWindow();
    void signalRunTopRightModelName(QString&);
signals:
	void SendSoftwareAlarm(QString str);
signals:
	//从机开始运行
	void SlaveStarRun();
private slots:
	//分屏按钮	
	void on_btnWindowing_clicked();
	//开始按钮
	void on_btnStart_clicked();
	//停止按钮
	void on_btnStop_clicked();
	//接受获取主机编号
	void OnSendReport();
	//设置批次编号
	void OnSetReport(const QJsonDocument & doc);
	//处理解决方案选择事件
	void OnSetParam(QString str);
	//处理重新刷新检测方案的界面
	void OnRefreshSolutionUI();
	//处理新建的解决方案
	void OnRefreshSolution();
	//处理报告窗口返回事件
	void OnSendbtnClicked();
	//处理从机开始运行
	void OnSlaveStarRun();
	void OnSendStartRun(const QJsonDocument & doc);
    //重新加载模型
    void on_btnReloadModel_clicked();
    //卸载模型
    void on_btnUnloadModel_clicked();
    //选择测试数据
    void on_btnTestData_clicked();
    //单步执行
    void on_btnStep_clicked();
    //文件检测
    //void on_btnFilewatcher_clicked();
    //文件检测接受
    void OnCommitReconRequest(const QString& strFilePath);

private:
	
	//加载检测方案
	void LoadSolution();
	//更新table列表
	void UpdataTable();

	void timerEvent(QTimerEvent *);
	void UpDataChart(double count);
	void CheckBadProdectRate(double rate);
};