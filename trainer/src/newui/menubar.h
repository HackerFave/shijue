#pragma once

#include <QWidget>
#include <QMenuBar>
#include <QAction>
#include "ui_menubar.h"

class menuBar : public QWidget
{
	Q_OBJECT

public:
	menuBar(QWidget *parent = Q_NULLPTR);
	~menuBar();
	void initmenu();
private:
	Ui::menuBar ui;
	QAction *actionOpen = Q_NULLPTR;// "&打开工程"
	QAction *actionNew = Q_NULLPTR; //"&新建工程"
	QAction *actionAdd = Q_NULLPTR; //"&添加工程"
	QAction *actionSave = Q_NULLPTR; //"&保存工程";
	QAction *actionSet = Q_NULLPTR; //"&加载标注";
	QAction *actionAddOther = Q_NULLPTR; //"&添加无缺陷图"
	QAction *actionCallout = Q_NULLPTR; //"&参数配置"
	QAction *actionChart = Q_NULLPTR; //"&统计分析"
	QAction *actionTool = Q_NULLPTR; //"&标注工具"
	QAction *actionSysSetting = Q_NULLPTR; //系统设置
private slots:
	void slotOpenPro(); //打开工程
	void slotNewPro(); //新建
	void slotAddImage(); //New点击事件调用test
	void slotSave(); //保存
	void slotLoadLabel(); //加载标注
	void slotAddOkImage(); //添加ok图
	void slotActionCalloutShow(); //New点击事件调用test
	void slotActionChartShow(); //统计表
	void slotActionToolShow();
	void slotActionSysSetting();//系统配置
signals:
	void signalActionObject(const QString &operting);
};
