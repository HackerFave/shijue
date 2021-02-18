#pragma once

#include <QWidget>
#include "ui_labelImageForm.h"

class labelImageForm : public QWidget, public Ui::labelImageForm
{
	Q_OBJECT

public:
	labelImageForm(QWidget *parent = Q_NULLPTR);
	~labelImageForm();
};
