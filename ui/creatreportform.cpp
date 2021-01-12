#include "creatreportform.h"
#include <QStandardItemModel>
#include <qdatetime.h>
#include "..\socket\ciwasocket.h"
CreatReportForm::CreatReportForm(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
	initDate();
	m_isok = false;
	ui.btnWait->hide();
	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(OnReSendBatch()));
	
}

CreatReportForm::~CreatReportForm()
{
	delete m_timer;
}

void CreatReportForm::initDate()
{
	QDateTime qdt1 = QDateTime::currentDateTime();
	QString timeStr = qdt1.toString("yyyy");
	QString timeStr2 = qdt1.toString("MM");
	QString timeStr3 = qdt1.toString("dd");
	
	for (int i = timeStr.toInt(); i < timeStr.toInt() + 10; i++) {
		ui.comboYear->addItem(QString("%1").arg(i));
	}

	for (int j = 0; j < ui.comboYear->count(); j++)
	{
		QString value = ui.comboYear->itemText(j);

		if (value.simplified() != timeStr.simplified())
		{
			continue;
		}
		else
		{
			ui.comboYear->setCurrentIndex(j);
		}
	}
	for (int j1 = 0; j1 < ui.comboMonth->count(); j1++)
	{
		QString value = ui.comboMonth->itemText(j1);

		if (value.simplified().toInt() != timeStr2.simplified().toInt())
		{
			continue;
		}
		else
		{
			ui.comboMonth->setCurrentIndex(j1);
		}
	}
	for (int j2 = 0; j2 < ui.comboDay->count(); j2++)
	{
		QString value = ui.comboDay->itemText(j2);

		if (value.simplified().toInt() != timeStr3.simplified().toInt())
		{
			continue;
		}
		else
		{
			ui.comboDay->setCurrentIndex(j2);
		}
	}
}

void CreatReportForm::initComboxColor(QComboBox * comBox)
{
	QStandardItemModel *pItemModel = qobject_cast<QStandardItemModel*>(comBox->model());
	//修改某项文本
	for (int i = 0; i < comBox->count(); i++)
	{
		pItemModel->item(i)->setForeground(QColor(255, 255, 255));					//修改某项文本颜色
		pItemModel->item(i)->setBackground(QColor(29, 30, 31));						//修改某项背景颜色    
		pItemModel->item(i)->setSizeHint(QSize(180, 40));
		pItemModel->item(i)->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);    //修改某项文本对齐方式
	}
}

void CreatReportForm::comboxSelectDayIndex()
{
}

void CreatReportForm::comboxSelectYearIndex()
{
}

void CreatReportForm::comboxSelectMonthIndex()
{
}

void CreatReportForm::comboxSelectSeasonIndex()
{
}

void CreatReportForm::on_btnCancel_clicked()
{
	StopReSendBatch();
	m_isok = false;
	emit SendbtnClicked();
}

void CreatReportForm::on_btnCreate_clicked()
{
	StopReSendBatch();
	if (ui.editBatchNum->text().isEmpty())
		return;
	m_batchnum = ui.editBatchNum->text();
	m_isok = true;
	emit SendbtnClicked();
}

void CreatReportForm::on_btnWait_clicked()
{
	StopReSendBatch();
	set_slave(false);
	update();
}

void CreatReportForm::OnReSendBatch()
{
	//发送信息给主机
	QJsonObject jsonObject;
	jsonObject["report"] = 1;
	CiWaSocket::NetWorkSendData(jsonObject, MSGID::REPORT);
}

void CreatReportForm::set_slave(bool isslave)
{
	if (isslave)
	{
		ui.btnWait->show();
		ui.btnCreate->hide();
		//启动定时器发送请求
		OnReSendBatch();
		m_timer->start(5000);
	}
	else
	{
		ui.btnCreate->show();
		ui.btnWait->hide();
	}
}

void CreatReportForm::StopReSendBatch()
{
	m_timer->stop();
}

void CreatReportForm::on_DealSelectComboxDate(const QString text)
{

}