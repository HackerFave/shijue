#pragma once

#include "TitleBar.h"
#include "ui_MainTitleBar.h"
#include <qobject.h>
class MainTitleBar : public TitleBar
{
	Q_OBJECT

public:
	MainTitleBar(QWidget *parent = Q_NULLPTR);
	~MainTitleBar();

private slots:
	void on_pushButtonClose_clicked();
	void on_pushButtonMin_clicked();
	void on_pushButtonNormalMax_clicked();


private:
	Ui::MainTitleBar ui;
};
