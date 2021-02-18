#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_thermometer.h"

class thermometer : public QMainWindow
{
	Q_OBJECT

public:
	thermometer(QWidget *parent = Q_NULLPTR);

private:
	Ui::thermometerClass ui;
};
