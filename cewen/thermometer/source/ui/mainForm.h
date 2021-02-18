#pragma once

#include <QWidget>
#include "ui_mainForm.h"

class mainForm : public QWidget
{
	Q_OBJECT

public:
	mainForm(QWidget *parent = Q_NULLPTR);
	~mainForm();

private:
	Ui::mainForm ui;
};
