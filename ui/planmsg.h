#pragma once

#include <QWidget>
#include "planmsgdialog.h"
#include "ui_planmsg.h"
#include "detectionscheme.h"
#include "qdir.h"
#include "mylistwidget.h"
class PlanMsg : public QWidget
{
	Q_OBJECT

public:
	PlanMsg(QWidget *parent = Q_NULLPTR,QRect rect = QRect() );
	~PlanMsg();

private:
	Ui::PlanMsg ui;
	DetectionScheme *m_detectionscheme;



private slots:
	void showEvent();
	void on_btnNewPlan_clicked();
	void on_btnRename_clicked();
	void on_btnDel_clicked();
	void OnListWidgetDoubleClicked(QListWidgetItem *item);
	//dialog������OK�ź�
	void OnSendMsgOkSignal();
	void OnReNameSignal();
	void OnSendMsgAddSignal();


private:
	
	PlanMsgDialog* m_planmsgdialog;
	//???��??
	int m_renameindex;
	QString m_prevname;
	bool is_rename;
	bool is_doubclick;
	//��鷽��ͼ��
	QIcon m_listico;
	
	QString m_inipath;
	
	void SetIni();
	
	void InitSolution();
	
	bool DeleteFileOrFolder(const QString & strpath);
	QString ReadIni(QSettings *configini, QString node, QString key);

public:
signals:
	void RefreshSolution();
};
