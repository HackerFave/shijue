#include "debugconfig.h"
#include "customtabstyle.h"
#include "solution.h"
#include "qsettings.h"
#include "uicommon.h"
#include "debugdata.h"
#include <map>
#include <vector>
#include "proc.h"
#include "qmutex.h"
#include "qdesktopwidget.h"
#include "windowing.h"
#include "fenpIni.h"
#include "qlineedit.h"
#include "threshold.h"

DebugConfig::DebugConfig(QWidget *parent, QRect rect)
	: QWidget(parent)
{

	//隐藏标题栏
	setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
	ui.setupUi(this);
	//重置大小
	this->resize(rect.width(), rect.height());
	qApp->processEvents();
	m_inipathname = QCoreApplication::applicationDirPath() + QString(SOLUTION) + Solution::GetCurrentSolution()->get_solutionname() + '/' + QString(THRESHOLDNAME);
	//档位
	m_gearlist << GEAR0 << GEAR1 << GEAR2 << GEAR3 << GEAR4;
	m_pwindow = NULL;
	m_intValidatornew = new QIntValidator(0, 1000, this);
	InitTab();
	update();
}

DebugConfig::~DebugConfig()
{
	if (m_intValidatornew != NULL)
	{
		delete m_intValidatornew;
		m_intValidatornew = NULL;
	}
}

void DebugConfig::InitTab()
{
	//ui.tabWidget->tabBar()->setStyle(new CustomTabStyle);//注意，设置上述代码风格 就可以实现QTabBar横向
	InitTab1();
}

//档位调节
void DebugConfig::InitTab1()
{
	ui.tableThreshold->horizontalHeader()->setVisible(true);
	ui.tableThreshold->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	on_btnRefresh_clicked();
}

void DebugConfig::on_btnSaveThreshold_clicked()
{	
    int stationid;
    QString defect_name;
    QString type_name;
    threshold::thresholdval tmpval;
    int totalnum = ui.tableThreshold->rowCount();
    for (int i = 0; i < totalnum; i++) {
        stationid = ui.tableThreshold->item(i, 0)->text().toInt();
        defect_name = ui.tableThreshold->item(i, 1)->text();
        type_name = ui.tableThreshold->item(i, 2)->text();
        tmpval.ok = ui.tableThreshold->item(i, 3)->text().toDouble();
        tmpval.ng = ui.tableThreshold->item(i, 4)->text().toDouble();
        threshold::set(stationid, std::string(defect_name.toLocal8Bit()), 
            std::string(type_name.toLocal8Bit()), &tmpval);
    }
    threshold::writeini();
}

void DebugConfig::on_btnRefresh_clicked()
{
    threshold::readini();
    ui.tableThreshold->clearContents();
    ui.tableThreshold->setRowCount(threshold::gettotalnum());

    threshold::thresholdval tmpval;
    int lineidx = 0;
    int stationnum = threshold::getstationnum();
    for (int i = 0; i < stationnum; i++) {
        int defectnum = threshold::getdefectnum(i);
        for (int j = 0; j < defectnum; j++) {
            int typenum = threshold::gettypenum(i, j);
            for (int k = 0; k < typenum; k++) {
                //工位号
                QTableWidgetItem *qtablewidgetitem0 = new QTableWidgetItem();
                ui.tableThreshold->setItem(lineidx, 0, qtablewidgetitem0);
                qtablewidgetitem0->setText(QString::number(i));
                //缺陷类型
                QTableWidgetItem *qtablewidgetitem1 = new QTableWidgetItem();
                ui.tableThreshold->setItem(lineidx, 1, qtablewidgetitem1);
                QString defect_name = QString::fromLocal8Bit(threshold::getdefectname(i, j).c_str());
                qtablewidgetitem1->setText(defect_name);
                //过滤类型
                QTableWidgetItem *qtablewidgetitem2 = new QTableWidgetItem();
                ui.tableThreshold->setItem(lineidx, 2, qtablewidgetitem2);
                QString type_name = QString::fromLocal8Bit(threshold::gettypename(i, j, k).c_str());
                qtablewidgetitem2->setText(type_name);

                QTableWidgetItem *qtablewidgetitem3 = new QTableWidgetItem();
                ui.tableThreshold->setItem(lineidx, 3, qtablewidgetitem3);
                QTableWidgetItem *qtablewidgetitem4 = new QTableWidgetItem();
                ui.tableThreshold->setItem(lineidx, 4, qtablewidgetitem4);
                int retval = threshold::get(i, j, k, &tmpval);
                if (0 == retval) {
                    //OK阈值
                    qtablewidgetitem3->setText(QString::number(tmpval.ok, 'f', 2));
                    //NG阈值
                    qtablewidgetitem4->setText(QString::number(tmpval.ng, 'f', 2));
                }
                else {
                    //OK阈值
                    qtablewidgetitem3->setText(QString::number(0.0, 'f', 2));
                    //NG阈值
                    qtablewidgetitem4->setText(QString::number(0.0, 'f', 2));
                }
                lineidx++;
            }
        }
    }
}

void DebugConfig::on_btnPROCRefresh_clicked()
{
	QMutex mutex;
	mutex.lock();
	ui.textBrowser->setText(DebugPROC::GetTextBUff());
	mutex.unlock();
}
