#pragma once

#include <QWidget>
#include "ui_runframetablewidget.h"
#include "runframetable.h"
class RunFrameTableWidget : public QWidget
{
	Q_OBJECT

public:
	RunFrameTableWidget(QWidget *parent = Q_NULLPTR,QSize rect = QSize());
	~RunFrameTableWidget();
	

	Ui::RunFrameTableWidget ui;

private:
	RunFrameTable *m_runframetable;
public:
	void SetTable(RunFrameTable *runframetable);
};
