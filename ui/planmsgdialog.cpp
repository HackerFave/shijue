#include "planmsgdialog.h"
#include "uicommon.h"
#include "qdir.h"
#include "ciwamsgdialog.h"
//#include "qmessagebox.h"
PlanMsgDialog::PlanMsgDialog(QWidget *parent, DetectionScheme *detectionscheme )
	: QDialog(parent)
{

	m_detectionscheme = detectionscheme;

	ui.setupUi(this);

	ui.editStationNum->setValidator(new QIntValidator(0, SATATIONNUM, this)); //0, 1000Ϊ���������ֵ��Χ
	ui.editStationNum->setToolTip(QString("��λ������� %1 ��").arg(SATATIONNUM));
	ui.editStationNum->setContextMenuPolicy(Qt::NoContextMenu);//�������Ҽ��˵�
	ui.editStationNum->setMaxLength(6);//������󳤶�16λ
	

	ui.editGroupNum->setValidator(new QIntValidator(0, SATATIONNUM, this)); //0, 1000Ϊ���������ֵ��Χ
	ui.editGroupNum->setToolTip(QString("��λ��������� %1 ��").arg(SATATIONNUM));
	ui.editGroupNum->setContextMenuPolicy(Qt::NoContextMenu);//�������Ҽ��˵�
	ui.editGroupNum->setMaxLength(6);//������󳤶�16λ

	ui.editFileName->setContextMenuPolicy(Qt::NoContextMenu);//�������Ҽ��˵�
	ui.editFileName->setMaxLength(16);//������󳤶�16λ
		//���ر�����
	setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
	ui.btnHelp->hide();
}

PlanMsgDialog::~PlanMsgDialog()
{
	
}

void PlanMsgDialog::on_ok_clicked()
{
	int cnum = 0;
	int cgro = 0;
	if (m_detectionscheme)
	{
		cnum = ui.editStationNum->text().toInt();
		cgro = ui.editGroupNum->text().toInt();

		m_filename = ui.editFileName->text();
		if (m_filename.isEmpty())
			return;
		QString m_inipath = QCoreApplication::applicationDirPath() + QString(SOLUTION) + m_filename + '/';

		if(cnum && cgro)
		{
			if (!AddNewIni(m_inipath))
				return;
			m_detectionscheme->setStationNum(cnum);
			m_detectionscheme->setGroupNum(cgro);
			emit SendMsgOk();
			emit SendMsgAddIni();
		}

	}
	this->close();
}

void PlanMsgDialog::on_cancel_clicked()
{
	this->close();
}
void PlanMsgDialog::on_btnExit_clicked()
{
	this->close();
}


bool PlanMsgDialog::AddNewIni(const QString filename)
{
	QDir m_dir(filename);
	if (m_dir.exists())
	{
		CiWaMsgDialog::MessageT("�����������", "�ü�鷽���Ѵ���");
		//QMessageBox::about(NULL, "", QString("�ü�鷽���Ѵ���"));
		return false;
	}
	//����Ŀ¼
	m_dir.mkdir(filename);
	return true;
}

int PlanMsgDialog::get_station_num()
{
	return m_detectionscheme ? m_detectionscheme->getStationNum() : 0;
}

int PlanMsgDialog::get_group_num()
{
	return m_detectionscheme ? m_detectionscheme->getGroupNum() : 0;
	
}

int PlanMsgDialog::exec()
{
	ui.editFileName->clear();
	ui.editGroupNum->clear();
	ui.editStationNum->clear();
	return QDialog::exec();
}


