#include "planmsg.h"
#include "uicommon.h"
#include "qdebug.h"
#include "ciwamsgdialog.h"
#include "solution.h"
PlanMsg::PlanMsg(QWidget *parent, QRect rect)
	: QWidget(parent)
{
	m_detectionscheme = new DetectionScheme;
	//���ر�����
	setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
	ui.setupUi(this);

	this->resize(rect.width(), rect.height());
	//�����ļ�·��
	m_inipath = QCoreApplication::applicationDirPath() + QString(SOLUTION);

	//��ʾ�����ļ�ⷽ��������
	InitSolution();
	is_rename = false;
	is_doubclick = false;
	connect(ui.listPlan, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(OnListWidgetDoubleClicked(QListWidgetItem *)));
	connect(ui.listPlan, &MyListWidget::ReNameSignal, this, &PlanMsg::OnReNameSignal);
	m_planmsgdialog = new PlanMsgDialog(NULL, m_detectionscheme);
	connect(m_planmsgdialog, &PlanMsgDialog::SendMsgOk, this, &PlanMsg::OnSendMsgOkSignal);
	connect(m_planmsgdialog, &PlanMsgDialog::SendMsgAddIni, this, &PlanMsg::OnSendMsgAddSignal);
}

PlanMsg::~PlanMsg()
{
	if(m_detectionscheme != NULL)
	{
		delete m_detectionscheme;
		m_detectionscheme = NULL;
	}
	if(m_planmsgdialog != NULL)
	{
		delete m_planmsgdialog;
		m_planmsgdialog = NULL;
	}
}

void PlanMsg::showEvent()
{

}
//�½��������
void PlanMsg::on_btnNewPlan_clicked()
{
	m_planmsgdialog->exec();
}
//������
void PlanMsg::on_btnRename_clicked()
{
	QListWidgetItem *item = ui.listPlan->currentItem();
	if (item == NULL)
		return;

	ui.listPlan->setFocus();
	is_rename = true;
	ui.listPlan->is_rename = true;
	ui.listPlan->setFocus();
	ui.listPlan->openPersistentEditor(item);
	ui.listPlan->editItem(item);
	ui.listPlan->update();
	m_renameindex = ui.listPlan->currentRow();//����������Ҫ�޸����Ƶ�index
	m_prevname = m_inipath + item->text();
	
}
//ɾ��
void PlanMsg::on_btnDel_clicked()
{
	CiWaMsgDialog::MessageT("ȷ��ɾ���������","?");
	if (!CiWaMsgDialog::GetReturn())
		return;
	int index = ui.listPlan->currentRow();
	QListWidgetItem *item = ui.listPlan->takeItem(index);
	if (item == NULL)
		return;
	if (item->text().isEmpty())
		return;

	QString fileName = m_inipath + item->text();
	
	delete item;
	item = NULL;

	bool ok = DeleteFileOrFolder(fileName);
	if (!ok)
	{
		CiWaMsgDialog::MessageT("�ļ�����", "ɾ��ʧ��");
		//QMessageBox::information(NULL, tr("Error"), tr("Delete Failed"));
		return;
	}
	//��鷽�����޸�
	Solution::g_solution.erase(Solution::g_solution.begin() + index);
	emit RefreshSolution();
}





 //˫�����ļ�
 void PlanMsg::OnListWidgetDoubleClicked(QListWidgetItem * item)
 {

	 QString filename = item->text();
	 Solution::SetCurrentSolution(filename);
	 Solution solution = *Solution::GetCurrentSolution();
	 m_detectionscheme->showMaximized();
	 qApp->processEvents();
	 m_detectionscheme->setStationNum(solution.get_station());
	 m_detectionscheme->setGroupNum(solution.get_group());
 }


 QString PlanMsg::ReadIni(QSettings *configini, QString node , QString key)
 {
	 if (configini == NULL)
		 return QString();
	 configini->beginGroup(node);
	 QString value = configini->value(key).toString();
	 configini->endGroup();
	 return value;
 }







void PlanMsg::OnSendMsgOkSignal()
{
	QListWidgetItem *tmp = new QListWidgetItem();
	tmp->setIcon(m_listico);
	tmp->setText(m_planmsgdialog->get_file_name());
	ui.listPlan->addItem(tmp);
}

void PlanMsg::OnReNameSignal()
{
	if (is_rename)
	{
		is_rename = false;

		QString fileName = m_inipath + ui.listPlan->item(m_renameindex)->text();

		if (QString::compare(m_prevname, fileName, Qt::CaseSensitive) == 0)
			return;
		bool ok = QFile::rename(m_prevname, fileName);
		if (!ok)
		{
			CiWaMsgDialog::MessageT("�ļ�����", "������ʧ��");
			//QMessageBox::information(NULL, tr("Error"), tr("Rename Failed"));
			return;
		}
	}
	//��鷽�����޸�
	Solution::g_solution[m_renameindex] = ui.listPlan->item(m_renameindex)->text();
	emit RefreshSolution();
}

//�½�ini�ļ�
void PlanMsg::OnSendMsgAddSignal()
{
	SetIni();
	//�򿪽���
	if(m_detectionscheme)
		m_detectionscheme->showMaximized();
	//ˢ��������������б�
	emit RefreshSolution();
}

void PlanMsg::SetIni()
{
	if (!m_planmsgdialog)
		return;
	int station = m_planmsgdialog->get_station_num();
	int group = m_planmsgdialog->get_group_num();
	QString filename = m_planmsgdialog->get_file_name();
	Solution setini;
	setini.set_group(group);
	setini.set_station(station);
	setini.WriteIniFile(filename);
	//��鷽�����޸�
	Solution::g_solution.push_back(filename);
	
	Solution::SetCurrentSolution(filename);
}





void PlanMsg::InitSolution()
{
	//if (!Solution::LoadIni())
	//	return;
	ui.listPlan->setResizeMode(QListView::Adjust);
	//����б�
	ui.listPlan->clear();
	QListWidgetItem * tmp;
	m_listico = QIcon(":/resources/icons/solution.png");
	for (size_t i = 0; i < Solution::g_solution.size(); i++)
	{
		tmp = new QListWidgetItem();	
		tmp->setIcon(m_listico);
		tmp->setText(Solution::g_solution[i]);
		ui.listPlan->addItem(tmp);
	}
	tmp = NULL;
}


bool PlanMsg::DeleteFileOrFolder(const QString & strpath)
{
	if (strpath.isEmpty() || !QDir().exists(strpath))
		return false;

	QFileInfo fileInfo(strpath);

	if (fileInfo.isFile())
		QFile::remove(strpath);
	else if (fileInfo.isDir())
	{
		QDir qdir(strpath);
		qdir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
		QFileInfoList fileinfolst = qdir.entryInfoList();
		foreach(QFileInfo qfileinfo, fileinfolst)
		{
			if (qfileinfo.isFile())
				qdir.remove(qfileinfo.absoluteFilePath());
			else
			{
				DeleteFileOrFolder(qfileinfo.absoluteFilePath());
				qdir.rmdir(qfileinfo.absoluteFilePath());
			}
		}
		qdir.rmdir(fileInfo.absoluteFilePath());
	}

	return true;
}



