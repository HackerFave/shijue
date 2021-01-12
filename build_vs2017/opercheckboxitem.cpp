#include "opercheckboxitem.h"
#include <qcheckbox.h>
operCheckBoxItem::operCheckBoxItem(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.checkBox_concession->setChecked(true);
	ui.checkBox_outprint->setChecked(true);
}

operCheckBoxItem::~operCheckBoxItem()
{
}
//�ò�
void operCheckBoxItem::on_checkBox_concession_clicked(bool checked)
{
	emit signalConcessionCheckedStatus(checked);
}
//��ӡ
void operCheckBoxItem::on_checkBox_outprint_clicked(bool checked)
{
	emit signalOutprintCheckedStatus(checked);
}