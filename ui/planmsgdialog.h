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
signals:    //添加自定义的信号
	void SendMsgOk();
signals:
	void SendMsgAddIni();

private:
	bool AddNewIni(const QString fileName);
	QString m_filename;;

public:
	//获得检查方案名
	QString get_file_name() { return m_filename; }
	//工位数量
	int get_station_num();
	//获得组数量	
	int get_group_num();
	//重写执行
	int exec();

};
