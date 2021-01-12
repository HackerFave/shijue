#pragma once
#include <QWidget>
#include "ui_debugconfig.h"
#include <vector>
#include <map>
class DebugConfig : public QWidget
{
	Q_OBJECT

public:
	DebugConfig(QWidget *parent, QRect rect);
	~DebugConfig();
private:
	Ui::DebugConfig ui;

	void InitTab();
	//„–÷µ…Ë÷√
	void InitTab1();
	//proc…Ë÷√
	void InitTab2();


private:
	int m_station;
	void *m_pwindow;
	QString m_inipathname;
	QStringList m_gearlist;
	QStringList m_listwindow;
	QIntValidator *m_intValidatornew;
public slots:
	void on_btnSaveThreshold_clicked();
	void on_btnRefresh_clicked();
	void on_btnPROCRefresh_clicked();
};
