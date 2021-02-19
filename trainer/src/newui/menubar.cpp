#include "menubar.h"

menuBar::menuBar(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	initmenu();
}

menuBar::~menuBar()
{
}
//��ʼ��menubar
void menuBar::initmenu()
{
	QMenuBar * menuBar = new QMenuBar;
	QMenu *menuFile = menuBar->addMenu(tr("&�ļ�"));
	QMenu *menuForm = menuBar->addMenu(tr("&����"));
	menuBar->setStyleSheet("QMenuBar{background-color:transparent;color:white;}");/*���ñ���ɫ�����汳��ɫ*/
	 //   "QMenuBar::selected{background-color:transparent;}"/*���ò˵���ѡ�б���ɫ*/
	   // "QMenuBar::item{font-size:12px;font-family:Microsoft YaHei;color:rgba(255,255,255,1);}"/*���ò˵�������Ϊ��ɫ��͸����Ϊ1��ȡֵ��Χ0.0-255��*/
	actionOpen = new QAction(tr("&�򿪹���"));
	//actionOpen->setShortcut(QKeySequence::Open);

	actionNew = new QAction(tr("&�½�����"));
	// actionNew->setShortcuts(QKeySequence::New);

	actionAdd = new QAction(tr("&���ͼƬ"));
	// actionAdd->setStatusTip(tr("���һ���µ��ļ�"));

	actionSave = new QAction(tr("&���湤��"));
	// actionSave->setShortcuts(QKeySequence::Save);

	actionSet = new QAction(tr("&��ӱ�ע"));
	//actionSet->setShortcuts(QKeySequence("ctrl+e"));

	actionAddOther = new QAction(tr("&���OKͼ"));
	// actionAddOther->setShortcuts(QKeySequence::Save);
	actionCallout = new QAction(tr("&��������"));
	actionChart = new QAction(tr("&ͳ�Ʒ���"));
	actionTool = new QAction(tr("&��ע����"));
	actionSysSetting = new QAction(tr("&ϵͳ����"));
	// actionAddOther->setShortcuts(QKeySequence::Save);
	actionSysSetting->setEnabled(false);
	connect(actionOpen, SIGNAL(triggered()), this, SLOT(slotOpenPro())); //��
	connect(actionNew, SIGNAL(triggered()), this, SLOT(slotNewPro())); //�½�
	connect(actionAdd, SIGNAL(triggered()), this, SLOT(slotAddImage())); //���ͼƬ
	connect(actionSave, SIGNAL(triggered()), this, SLOT(slotSave())); //����
	connect(actionSet, SIGNAL(triggered()), this, SLOT(slotLoadLabel())); //���ر�ע
	connect(actionAddOther, SIGNAL(triggered()), this, SLOT(slotAddOkImage())); //���okͼ
	connect(actionCallout, SIGNAL(triggered()), this, SLOT(slotActionCalloutShow())); //New����¼�����test
	connect(actionChart, SIGNAL(triggered()), this, SLOT(slotActionChartShow())); //New����¼�����test
	connect(actionTool, SIGNAL(triggered()), this, SLOT(slotActionToolShow()));//������
	connect(actionSysSetting, SIGNAL(triggered()), this, SLOT(slotActionSysSetting()));//ϵͳ���ý���
	menuFile->addAction(actionNew);
	menuFile->addSeparator();
	menuFile->addAction(actionOpen);
	menuFile->addSeparator();
	menuFile->addAction(actionSave);
	menuFile->addSeparator();
	menuFile->addAction(actionAdd);
	menuFile->addSeparator();
	menuFile->addAction(actionSet);
	menuFile->addSeparator();
	menuFile->addAction(actionAddOther);
	menuFile->addSeparator();
	menuForm->addAction(actionCallout);
	actionCallout->setCheckable(true);
	actionCallout->setChecked(false);
	menuForm->addSeparator();
	menuForm->addAction(actionChart);
	actionChart->setCheckable(true);
	actionChart->setChecked(false);
	menuForm->addSeparator();
	menuForm->addAction(actionTool);
	menuForm->addSeparator();
	menuForm->addAction(actionSysSetting);
	actionSysSetting->setCheckable(true);
	actionSysSetting->setChecked(false);
	actionTool->setVisible(false);
	// actionTool->setCheckable(true);
	 //actionTool->setChecked(false);
	ui.horizontalLayout_menu->addWidget(menuBar);
}
/*���̲���*/

//���򿪡���ť������
void menuBar::slotOpenPro()
{
	emit signalActionObject("Open");
}

//���½�����ť������
void menuBar::slotNewPro()
{
	emit signalActionObject("New");
}

//�����ӡ���ť������
void menuBar::slotAddImage()
{
	emit signalActionObject("AddImage");
}

//�����桱��ť������
void menuBar::slotSave()
{
	emit signalActionObject("Save");
}

//�����ر�ע����ť������
void menuBar::slotLoadLabel()
{
	emit signalActionObject("LoadLabel");
}
//��������ȱ��ͼ����ť������
void menuBar::slotAddOkImage()
{
	emit signalActionObject("AddOkImage");
}
//��������
void menuBar::slotActionCalloutShow()
{
	emit signalActionObject("CalloutShow");
}
//ͳ�Ʊ�
void menuBar::slotActionChartShow()
{
	emit signalActionObject("ChartShow");
}
//������
void menuBar::slotActionToolShow()
{
	emit signalActionObject("ToolShow");
}
//ϵͳ����
void menuBar::slotActionSysSetting()
{
	//ϵͳ���ý���
	//_sysSettingform->show();
	emit signalActionObject("sysSettingShow");
}