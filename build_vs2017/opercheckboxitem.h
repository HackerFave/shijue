#pragma once

#include <QWidget>
#include "ui_opercheckboxitem.h"

class operCheckBoxItem : public QWidget
{
	Q_OBJECT

public:
	operCheckBoxItem(QWidget *parent = Q_NULLPTR);
	~operCheckBoxItem();
	void set_checkBox_concession_checked(bool checked)
	{
		ui.checkBox_concession->setChecked(checked);
	}
	void set_checkBox_outprint_checked(bool checked)
	{
		ui.checkBox_outprint->setChecked(checked);
	}
private slots:
	void on_checkBox_concession_clicked(bool checked);
	void on_checkBox_outprint_clicked(bool checked);
	
signals:
	void  signalConcessionCheckedStatus(bool);
	void  signalOutprintCheckedStatus(bool);
private:
	Ui::operCheckBoxItem ui;
};
