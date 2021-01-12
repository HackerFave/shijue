#pragma once

#include <QWidget>
#include "ui_settingFormPLC.h"
#include "plc.h"

class settingFormPLC : public QWidget
{
    Q_OBJECT

public:
    settingFormPLC(QWidget *parent = Q_NULLPTR);
    ~settingFormPLC();

private:
    Ui::settingFormPLC ui;

	//初始化串口菜单
	void initcomm();
	void plcmessageprocess(plc::PlcMessageType msg);
private slots:
	void on_btnplcconnect_2_clicked();
	void on_btnplcdisconnect_2_clicked();
	void on_btnplcreadspeed_clicked();
	void on_btnplcreadlength_clicked();
	void on_btnplcreadtotallength_clicked();
	void on_btnplcreadtemp_clicked();
	void on_btnclearini_2_clicked();
	void on_btnloadini_2_clicked();
	void on_btnsaveini_clicked();
};
