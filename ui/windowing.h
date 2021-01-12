#pragma once
#include <map>
#include <QDialog>
#include "ui_windowing.h"

class Windowing : public QDialog
{
	Q_OBJECT

public:
	//window ����������idwindow ��Ļ����
	Windowing(QWidget *parent = Q_NULLPTR, int window = 0, int idwindow = 0, int type = 0, bool ismaste = false);
	~Windowing();
	void show();
private:
	//��̬���䴰��
	void DistributeView(int row, int col);
	//�����Զ���ķ���(mapת��)
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
