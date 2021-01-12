#include "excelexport.h"
#include "qfiledialog.h"
#include "uicommon.h"

ExcelExport::ExcelExport(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
	ui.btnHelp->hide();
}

ExcelExport::~ExcelExport()
{
}

void ExcelExport::on_btnCancel_clicked()
{
	this->close();
}

void ExcelExport::on_btnExit_clicked()
{
	this->close();
}

void ExcelExport::on_btnOk_clicked()
{
	
	this->close();
	//���ι�ѡ
	if (ui.checkIsBatch->isChecked())
	{
		emit SendMsgBatch(OpenSaveExcelFile(BATCHTABLE));
	}
	//��λ��ѡ
	if (ui.checkIsStation->isChecked())
	{
		emit SendMsgStation(OpenSaveExcelFile(STATIONTABLE));
	}
	//��Ʒ��ѡ
	if (ui.checkIsProduct->isChecked())
	{
		emit SendMsgProduct(OpenSaveExcelFile(PRODUCTTABLE));
	}

}

QString ExcelExport::OpenSaveExcelFile(QString str)
{

	QString directory = QFileDialog::getSaveFileName(
		NULL,
		tr("����excel"),
		str,
		"*.xlsx");

	if (!directory.isEmpty())
		return directory;
	return QString();
}


QString ExcelExport::OpenSaveExcelFile(QString str,QString title,QString filter )
{

	QString directory = QFileDialog::getSaveFileName(
		NULL,
		title,
		str,
		filter);

	if (!directory.isEmpty())
		return directory;
	return QString();
}

QString ExcelExport::OpenOpenExcelFile(QString str, QString title, QString filter)
{

	QString directory = QFileDialog::getOpenFileName(
		NULL,
		title,
		str,
		filter);

	if (!directory.isEmpty())
		return directory;
	return QString();
}