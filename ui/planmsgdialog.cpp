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

	ui.editStationNum->setValidator(new QIntValidator(0, SATATIONNUM, this)); //0, 1000为输入的数字值范围
	ui.editStationNum->setToolTip(QString("工位数，最大 %1 个").arg(SATATIONNUM));
	ui.editStationNum->setContextMenuPolicy(Qt::NoContextMenu);//设置无右键菜单
	ui.editStationNum->setMaxLength(6);//设置最大长度16位
	

	ui.editGroupNum->setValidator(new QIntValidator(0, SATATIONNUM, this)); //0, 1000为输入的数字值范围
	ui.editGroupNum->setToolTip(QString("工位组数，最大 %1 个").arg(SATATIONNUM));
	ui.editGroupNum->setContextMenuPolicy(Qt::NoContextMenu);//设置无右键菜单
	ui.editGroupNum->setMaxLength(6);//设置最大长度16位

	ui.editFileName->setContextMenuPolicy(Qt::NoContextMenu);//设置无右键菜单
	ui.editFileName->setMaxLength(16);//设置最大长度16位
		//隐藏标题栏
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
		CiWaMsgDialog::MessageT("创建解决方案", "该检查方案已存在");
		//QMessageBox::about(NULL, "", QString("该检查方案已存在"));
		return false;
	}
	//创建目录
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


