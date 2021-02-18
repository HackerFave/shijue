#include "MainTitleBar.h"
#include "TitleBar.h"

MainTitleBar::MainTitleBar(QWidget *parent)
	: TitleBar(parent)
{
	ui.setupUi(this);
	ui.setupUi(this);
	ui.pushButtonNormalMax->setStyleSheet("QPushButton{border-image: url(:/res/res/image/normal_normal.svg);}"
		"QPushButton:hover{border-image: url(:/res/res/image/normal_hover.svg);}");
}

MainTitleBar::~MainTitleBar()
{
}
void MainTitleBar::on_pushButtonClose_clicked()
{
	if (parentWidget)
	{
		parentWidget->close();
	}
}

void MainTitleBar::on_pushButtonMin_clicked()
{
	if (parentWidget)
	{
		parentWidget->showMinimized();
	}
}


void MainTitleBar::on_pushButtonNormalMax_clicked()
{
	if (parentWidget == nullptr) return;
	if (parentWidget->isMaximized())
	{
		parentWidget->showNormal();
		ui.pushButtonNormalMax->setStyleSheet("QPushButton{border-image: url(:/res/res/image/normal_normal.svg);}"
			"QPushButton:hover{border-image: url(:/res/res/image/normal_hover.svg);}");
	}
	else
	{
		parentWidget->showMaximized();
		ui.pushButtonNormalMax->setStyleSheet("QPushButton{border-image: url(:/res/res/image/max_normal.svg);}"
			"QPushButton:hover{border-image: url(:/res/res/image/max_hover.svg);}");
	}
}