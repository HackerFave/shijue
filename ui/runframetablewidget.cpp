#include "runframetablewidget.h"

RunFrameTableWidget::RunFrameTableWidget(QWidget *parent, QSize rect)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->resize(rect);
	int i = (rect.width()-40) / 22;
	ui.label_3->setMinimumWidth(i);
	ui.label_3->setMaximumWidth(i);
	ui.label_2->setMinimumWidth(i * 4);
	ui.label_4->setMinimumWidth(i * 4);
	ui.label->setMinimumWidth(i * 4);
	ui.label_5->setMinimumWidth(i * 4);
	ui.label_6->setMinimumWidth(i * 4);

	/*



	list << m_runframetablewidget->ui.label_3->width()
		<< m_runframetablewidget->ui.label_2->width()
		<< m_runframetablewidget->ui.label_4->width()
		<< m_runframetablewidget->ui.label->width()
		<< m_runframetablewidget->ui.label_5->width()
		<< m_runframetablewidget->ui.label_6->width();
*/
}

RunFrameTableWidget::~RunFrameTableWidget()
{
}

void RunFrameTableWidget::SetTable(RunFrameTable *runframetable)
{
	//Òþ²ØµÚÒ»ÐÐ ui->tableWidget->setRowHidden(2, true);
	ui.gridLayout_3->addWidget(runframetable);
	
}

