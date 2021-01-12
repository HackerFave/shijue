#pragma once

#include <QDialog>
#include "ui_creatreportform.h"
#include "qtimer.h"
class CreatReportForm : public QDialog
{
	Q_OBJECT

public:
	CreatReportForm(QWidget *parent = Q_NULLPTR);
	~CreatReportForm();

private:
	Ui::CreatReportForm ui;
	bool m_isok;
	QString m_batchnum;
	QTimer *m_timer;
private:
	void initDate();
	void initComboxColor(QComboBox * comBox);
	void comboxSelectDayIndex();
	void comboxSelectYearIndex();
	void comboxSelectMonthIndex();
	void comboxSelectSeasonIndex();

private slots:
	void on_DealSelectComboxDate(const QString text);
	void on_btnCancel_clicked();
	void on_btnCreate_clicked();
	void on_btnWait_clicked();
	void OnReSendBatch();
public:
	bool get_isok(){ return m_isok; }
	QString get_batchnum() { return m_batchnum; }
	void set_slave(bool isslave);
	void StopReSendBatch();
signals:
	
	void SendbtnClicked();
};
