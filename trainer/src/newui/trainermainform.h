#pragma once

#include <QWidget>
#include "ui_trainermainform.h"

class TrainerMainForm : public QWidget
{
	Q_OBJECT

public:
	TrainerMainForm(QWidget *parent = Q_NULLPTR);
	~TrainerMainForm();

private:
	Ui::TrainerMainForm ui;
};
