#include "mainForm.h"

mainForm::mainForm(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->setWindowFlags(Qt::FramelessWindowHint);
}

mainForm::~mainForm()
{
}
