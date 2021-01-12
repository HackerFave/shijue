#pragma once
#include <map>
#include <QDialog>
#include "ui_windowing.h"

class Windowing : public QDialog
{
	Q_OBJECT

public:
	//window 窗口数量，idwindow 屏幕数量
	Windowing(QWidget *parent = Q_NULLPTR, int window = 0, int idwindow = 0, int type = 0, bool ismaste = false);
	~Windowing();
	void show();
private:
	//动态分配窗口
	void DistributeView(int row, int col);
	//处理自定义的分屏(map转化)
	void SetCustomeWin();

public:
	void Init(int winodw);
	Ui::Windowing ui;
	int m_window;
	int m_idwindow;
	int m_windowtype;	//0 widget, 1 lable
	bool m_ismaste;
	std::map<int, void*> m_mapview;
	std::map<int, int> m_stationfenp;
	std::map<int, void*> m_stationmap;

};
