#pragma once

#include <QDialog>
#include "detectionscheme.h"
#include "ui_planmsgdialog.h"
#include "qsettings.h"
class PlanMsgDialog : public QDialog
{
	Q_OBJECT

public:
	PlanMsgDialog(QWidget *parent = Q_NULLPTR,DetectionScheme *detectionscheme = Q_NULLPTR);
	~PlanMsgDialog();

private:
	Ui::PlanMsgDialog ui;
	DetectionScheme *m_detectionscheme;

private slots:
	void on_ok_clicked();
	void on_cancel_clicked();
	void on_btnExit_clicked();
//
public:
signals:    //����Զ�����ź�
	void SendMsgOk();
signals:
	void SendMsgAddIni();

private:
	bool AddNewIni(const QString fileName);
	QString m_filename;;

public:
	//��ü�鷽����
	QString get_file_name() { return m_filename; }
	//��λ����
	int get_station_num();
	//���������	
	int get_group_num();
	//��дִ��
	int exec();

};
