#include "sqldatashowdialog.h"

SqlDataShowDialog::SqlDataShowDialog(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    connect(ui.tableView, SIGNAL(signalItemText(const QString &)),this, SIGNAL(signalItemText(const QString &)));
    connect(ui.spinBox_page, SIGNAL(valueChanged(int)), this, SIGNAL(signalChangeValue(int)));
    ui.spinBox_page->setMinimum(1);
}

SqlDataShowDialog::~SqlDataShowDialog()
{
}

void SqlDataShowDialog::setsqldatatomodel(const QMap<QString, QString> &data, const QStringList &keyList)
{
    ui.tableView->addRowItem(data, keyList);
}
void SqlDataShowDialog::initshowform(const QStringList list, const QString &tablename)
{
    ui.tableView->initTableView(list, tablename);
}
void SqlDataShowDialog::cleartabledata()
{
    ui.tableView->tableViewClear();
}
int SqlDataShowDialog::getspinboxvalue()
{
    return ui.spinBox_page->value();
}
void SqlDataShowDialog::setmaxvalue(int value)
{
    ui.spinBox_page->setMaximum(value);
}
void SqlDataShowDialog::setlabelinfo(SQL_INFO flag, const QString &strinfo)
{
	switch (flag)
	{
	case SQL_INFO::Page: {
		ui.label_page->setText(strinfo);
	}
		break;
	case SQL_INFO::Record: {
		ui.label_recordcount->setText(strinfo);
	}
		break;
	case SQL_INFO::Info: {
		ui.label_info->setText(strinfo);
	}
		break;
	default:
		break;
	}
}