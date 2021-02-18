#pragma once

#include <QLabel>
#include "ui_labelImage.h"

class labelImage : public QLabel
{
	Q_OBJECT

public:
	labelImage(QWidget *parent = Q_NULLPTR);
	~labelImage();

private:
	Ui::labelImage ui;
};
