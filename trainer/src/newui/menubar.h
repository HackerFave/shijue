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
	QAction *actionOpen = Q_NULLPTR;// "&�򿪹���"
	QAction *actionNew = Q_NULLPTR; //"&�½�����"
	QAction *actionAdd = Q_NULLPTR; //"&��ӹ���"
	QAction *actionSave = Q_NULLPTR; //"&���湤��";
	QAction *actionSet = Q_NULLPTR; //"&���ر�ע";
	QAction *actionAddOther = Q_NULLPTR; //"&�����ȱ��ͼ"
	QAction *actionCallout = Q_NULLPTR; //"&��������"
	QAction *actionChart = Q_NULLPTR; //"&ͳ�Ʒ���"
	QAction *actionTool = Q_NULLPTR; //"&��ע����"
	QAction *actionSysSetting = Q_NULLPTR; //ϵͳ����
private slots:
	void slotOpenPro(); //�򿪹���
	void slotNewPro(); //�½�
	void slotAddImage(); //New����¼�����test
	void slotSave(); //����
	void slotLoadLabel(); //���ر�ע
	void slotAddOkImage(); //���okͼ
	void slotActionCalloutShow(); //New����¼�����test
	void slotActionChartShow(); //ͳ�Ʊ�
	void slotActionToolShow();
	void slotActionSysSetting();//ϵͳ����
signals:
	void signalActionObject(const QString &operting);
};
