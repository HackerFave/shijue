#pragma once
#include <QWidget>
#include "qgridlayout.h"
#include "qlabel.h"
class SolutionIcon
{

public:
	SolutionIcon(QWidget *parent = NULL);
	~SolutionIcon();
	//设置第一个背景图
	void SetFristIcon();
	//设置最后一个背景图
	void SetLastIcon();
	//设置中间的背景图
	void SetNormalIcon();
	//已设置
	void Settled();
	//未设置
	void UnSettled();
	//正在设置
	void Setting();
	//已完成 1已设置，0未设置
	bool m_issettled;
	
public:
	QGridLayout* m_pgridlayout;
	QLabel* m_lab;
	QWidget* m_widget;
};
