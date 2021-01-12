#pragma once

#include <QDialog>
#include "ui_excelexport.h"

class ExcelExport : public QDialog
{
	Q_OBJECT

public:
	ExcelExport(QWidget *parent = Q_NULLPTR);
	~ExcelExport();

private:
	Ui::ExcelExport ui;
	QString OpenSaveExcelFile(QString str);
public:
	QString OpenSaveExcelFile(QString str, QString title, QString filter);
	QString OpenOpenExcelFile(QString str, QString title, QString filter);
private slots:
	void on_btnOk_clicked();
	void on_btnCancel_clicked();
	void on_btnExit_clicked();

public:
signals:    //添加自定义的信号
	void SendMsgProduct(QString path);
signals:
	void SendMsgStation(QString path);
signals:
	void SendMsgBatch(QString path);
};
