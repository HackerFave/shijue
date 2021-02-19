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
//初始化menubar
void menuBar::initmenu()
{
	QMenuBar * menuBar = new QMenuBar;
	QMenu *menuFile = menuBar->addMenu(tr("&文件"));
	QMenu *menuForm = menuBar->addMenu(tr("&窗口"));
	menuBar->setStyleSheet("QMenuBar{background-color:transparent;color:white;}");/*设置背景色，跟随背景色*/
	 //   "QMenuBar::selected{background-color:transparent;}"/*设置菜单栏选中背景色*/
	   // "QMenuBar::item{font-size:12px;font-family:Microsoft YaHei;color:rgba(255,255,255,1);}"/*设置菜单栏字体为白色，透明度为1（取值范围0.0-255）*/
	actionOpen = new QAction(tr("&打开工程"));
	//actionOpen->setShortcut(QKeySequence::Open);

	actionNew = new QAction(tr("&新建工程"));
	// actionNew->setShortcuts(QKeySequence::New);

	actionAdd = new QAction(tr("&添加图片"));
	// actionAdd->setStatusTip(tr("添加一个新的文件"));

	actionSave = new QAction(tr("&保存工程"));
	// actionSave->setShortcuts(QKeySequence::Save);

	actionSet = new QAction(tr("&添加标注"));
	//actionSet->setShortcuts(QKeySequence("ctrl+e"));

	actionAddOther = new QAction(tr("&添加OK图"));
	// actionAddOther->setShortcuts(QKeySequence::Save);
	actionCallout = new QAction(tr("&参数配置"));
	actionChart = new QAction(tr("&统计分析"));
	actionTool = new QAction(tr("&标注工具"));
	actionSysSetting = new QAction(tr("&系统设置"));
	// actionAddOther->setShortcuts(QKeySequence::Save);
	actionSysSetting->setEnabled(false);
	connect(actionOpen, SIGNAL(triggered()), this, SLOT(slotOpenPro())); //打开
	connect(actionNew, SIGNAL(triggered()), this, SLOT(slotNewPro())); //新建
	connect(actionAdd, SIGNAL(triggered()), this, SLOT(slotAddImage())); //添加图片
	connect(actionSave, SIGNAL(triggered()), this, SLOT(slotSave())); //保存
	connect(actionSet, SIGNAL(triggered()), this, SLOT(slotLoadLabel())); //加载标注
	connect(actionAddOther, SIGNAL(triggered()), this, SLOT(slotAddOkImage())); //添加ok图
	connect(actionCallout, SIGNAL(triggered()), this, SLOT(slotActionCalloutShow())); //New点击事件调用test
	connect(actionChart, SIGNAL(triggered()), this, SLOT(slotActionChartShow())); //New点击事件调用test
	connect(actionTool, SIGNAL(triggered()), this, SLOT(slotActionToolShow()));//工具条
	connect(actionSysSetting, SIGNAL(triggered()), this, SLOT(slotActionSysSetting()));//系统设置界面
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
/*工程操作*/

//“打开”按钮处理函数
void menuBar::slotOpenPro()
{
	emit signalActionObject("Open");
}

//“新建”按钮处理函数
void menuBar::slotNewPro()
{
	emit signalActionObject("New");
}

//“增加”按钮处理函数
void menuBar::slotAddImage()
{
	emit signalActionObject("AddImage");
}

//“保存”按钮处理函数
void menuBar::slotSave()
{
	emit signalActionObject("Save");
}

//“加载标注”按钮处理函数
void menuBar::slotLoadLabel()
{
	emit signalActionObject("LoadLabel");
}
//“增加无缺陷图”按钮处理函数
void menuBar::slotAddOkImage()
{
	emit signalActionObject("AddOkImage");
}
//参数配置
void menuBar::slotActionCalloutShow()
{
	emit signalActionObject("CalloutShow");
}
//统计表
void menuBar::slotActionChartShow()
{
	emit signalActionObject("ChartShow");
}
//工具条
void menuBar::slotActionToolShow()
{
	emit signalActionObject("ToolShow");
}
//系统设置
void menuBar::slotActionSysSetting()
{
	//系统设置界面
	//_sysSettingform->show();
	emit signalActionObject("sysSettingShow");
}