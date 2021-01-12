#pragma once

#include <QWidget>
#include "ui_runframenewui.h"
#include "runtoprightdialog.h"
#include "windowing.h"
#include "runframetablewidget.h"
#include "qmutex.h"
#include"common.h"
class RunFrameNewUi : public QWidget
{
	Q_OBJECT

public:
	RunFrameNewUi(QWidget *parent = Q_NULLPTR, QRect rect = QRect());
	~RunFrameNewUi();
    QString getModelName();


private:
	Ui::RunFrameNewUi ui;
	QMutex mutex;
	Windowing *m_windowing;
	//分屏类 列表为额外的屏幕
	std::vector<Windowing*> m_windowing_list;
	RunFrameTableWidget *m_runframetablewidget;
	bool m_isfpshow;
	QList<int> m_listwindow;
public:
	RunTopRightDialog *m_runtoprightdialog;
private slots:
	void OnSendUpdataWindow();
	//自定义屏幕
	void OnSendCustomWindow();
	//分屏按钮	
	void OnSendMultiSignal();
	void OnSendSingleSignal();
private:
	void ShowMdisplay(int iscustom =1);


public:
	void ShowCameraPic(const imginfo& img);
	void ShowCameraPicMain(const imginfo & img);
	void show();
};
