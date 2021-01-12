#include "runframetable.h"
#include "qlineedit.h"
#include "solution.h"
#include "../socket/ciwasocket.h"
#include "qcombobox.h"
#include "uicommon.h"
#include "qevent.h"
#include "baseparam.h"
#include "databasesqlite.h"
RunFrameTable::RunFrameTable(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	m_comboxcount = 0;
	m_swidth = 63;
	m_issingle = false;


	QList<int> list;
	m_swidth_list << m_swidth << m_swidth << m_swidth << m_swidth << m_swidth<< m_swidth;
	ResizeTable(SATATIONNUMTABLE);

	QObject::connect(CiWaSocket::GetCurrSocket(), &CiWaSocket::NewJsonData, this, &RunFrameTable::on_NewJsonData,Qt::DirectConnection);
	QObject::connect(CiWaSocket::GetCurrSocket(), &CiWaSocket::ChangeGrade, this, &RunFrameTable::on_ChangeGrade, Qt::DirectConnection);
	QObject::connect(CiWaSocket::GetCurrSocket(), &CiWaSocket::NetUpTable, this, &RunFrameTable::on_NetUpTable, Qt::DirectConnection);
	m_count = 0;
	m_badcount = 0;
	m_goodcount = 0;
	m_count_slave = 0;
	m_goodcount_slave = 0;

	for (int i = 0; i < SATATIONNUM; i++) {
		m_solution_count[i] = 0;
	}
}
//setColumnWidth
void RunFrameTable::SetTableStyle(bool issingle)
{
	//布局
	if (issingle)
	{
		ui.verticalLayout->setContentsMargins(0, 0, 0, 0);
		ui.verticalLayout->setSpacing(0);
	}
	else
	{
		ui.verticalLayout->setContentsMargins(9, 9, 9, 9);
		ui.verticalLayout->setSpacing(6);
	}

	int row = ui.tableWidget->rowCount();
	int col = ui.tableWidget->columnCount();
	int width = ui.tableWidget->columnWidth(0);
	QLineEdit *lineEdit;
	for (size_t i = 0; i < row; i++)
	{
		if(issingle)
			ui.tableWidget->setRowHeight(i, 43);
		else
			ui.tableWidget->setRowHeight(i, 33);
		for (size_t j = 0; j < col; j++)
		{
			lineEdit =(QLineEdit*) ui.tableWidget->cellWidget(i, j);
			if (issingle)
			{
				if (m_width.count() > 1)
				{
					lineEdit->setMaximumWidth(m_width.at(j) - 15);
				}
				else
					lineEdit->setMaximumWidth(53);
				lineEdit->setMaximumHeight(35);
			}
			else
			{
				lineEdit->setMaximumWidth(53);
				lineEdit->setMaximumHeight(25);
			}
		}
	}
	//隐显示或隐藏第一行
	ui.tableWidget->setRowHidden(0, issingle);
	m_issingle = issingle;
}

//设置列宽度
void RunFrameTable::SetColWidth(QList<int> width)
{

	int col = ui.tableWidget->columnCount();
	
	for (size_t i = 0; i < col-1; i++)
	{
		ui.tableWidget->setColumnWidth(i, width.at(i));
	}
	ui.tableWidget->setColumnWidth(col-1, width.at(col - 1) - 15);
	
	if (width.at(0) == m_swidth)
		return;
	m_width = width;
}
//清空60个工位
void RunFrameTable::ClearAndLoadTable()
{
	QLineEdit* lineEdit;
	QComboBox* combobox;
	//1清空表格
	for (int m = 1; m < SATATIONNUMTABLE; m++)
	{
		for (int j = 0; j < 5; j++)
		{
			lineEdit = (QLineEdit*)ui.tableWidget->cellWidget(m, j);
			if (lineEdit != NULL)
				lineEdit->setText("");
		}
		combobox = (QComboBox*)ui.tableWidget->cellWidget(m, 5);
		if (combobox != NULL)
		{
			QObject::disconnect(combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnGradeChanged(int)));
			combobox->clear();
		}
	}
	//重载挡位
	GradeInit();
	//加载数据
	UpTableData();
	//如果是主机的话，发送给从机一个请求刷新界面
	if (CiWaSocket::GetHostState() == HostState::MASTE)
	{
		QJsonObject json;
		json.insert(GRADE, QString("uptable"));
		CiWaSocket::NetWorkSendData(json, MSGID::GETTABLE);
	}

}


//更新tab 表格
void RunFrameTable::ResizeTable(int newcol)
{
	
	int row = ui.tableWidget->rowCount();
	int col = ui.tableWidget->columnCount();
	for (int d = 1; d < row; d++)
	{
		ui.tableWidget->removeRow(d);
	}
	QStringList str;
	str << "机  位" << "检测数" << "良  品" << "不良品" << "良品率"<<"档  位";
	ui.tableWidget->setRowCount(newcol + 1);//总行数增加1
	
	for (size_t i = 0; i < newcol+1; i++)
	{
		ui.tableWidget->setRowHeight(i, 33);

		for (size_t j = 0; j < col; j++)
		{
			QLineEdit *lineEdit = new QLineEdit();
			lineEdit->setStyleSheet("background-color: rgb(21,21,21);");
			lineEdit->setMaximumWidth(53);
			lineEdit->setMaximumHeight(25);
			ui.tableWidget->setCellWidget(i, j, lineEdit);
			if (i == 0)
			{
				lineEdit->setStyleSheet("background-color: rgb(47,47,47);border: 0px solid;");
				lineEdit->setText(str.at(j));
			}
		}
		if(i!=0)
		{ 
			QComboBox *comEdit = new QComboBox();
			comEdit->setMaximumWidth(53);
			comEdit->setMaximumHeight(25);
			ui.tableWidget->setCellWidget(i, 5, comEdit);
			comEdit->setStyleSheet("QComboBox{ background-color: rgb(21, 21, 21);}QComboBox::down-arrow{image: url(:/resources/icons/ic_arrow_down.png);}");
			//QStringList strlist;
			//strlist << SGEAR0 << SGEAR1 << SGEAR2 << SGEAR3 << SGEAR4;
			//comEdit->addItems(strlist);
			
			
		}
	}
	if (m_issingle)
	{
		this->SetColWidth(m_width);
		this->SetTableStyle(m_issingle);
	}
	else
		this->SetColWidth(m_swidth_list);

	//GradeInit();
	//UpTableData();
	//如果是主机的话，发送给从机一个请求刷新界面
	if (CiWaSocket::GetHostState() == HostState::MASTE)
	{
		QJsonObject json;
		json.insert(GRADE, QString("uptable"));
		CiWaSocket::NetWorkSendData(json, MSGID::GETTABLE);

	}
}


void RunFrameTable::clearJsonObject(QJsonObject &object)
{
	QStringList strList = object.keys();

	for (int i = 0; i < strList.size(); ++i)
	{
		object.remove(strList.at(i));
	}

}

void RunFrameTable::on_ChangeGrade(const QJsonDocument & doc)
{
	//解析
	QJsonObject rootObject = doc.object();
	//获得工位	
	QJsonValue jsonValue;

	jsonValue = rootObject.value("station");
	int row = jsonValue.toString().toInt();
	jsonValue = rootObject.value(GRADE);
	int sgrade = jsonValue.toString().toInt();

	QComboBox* combobox = (QComboBox*)ui.tableWidget->cellWidget(row, 5);
	combobox->setCurrentIndex(sgrade);
}

void RunFrameTable::on_NetUpTable()
{
	UpTableData();
}

void RunFrameTable::OnGradeChanged(int msg)
{
	QComboBox *editor = qobject_cast<QComboBox *>(sender());
	int x = editor->frameGeometry().x();
	int y = editor->frameGeometry().y();

	QModelIndex index = ui.tableWidget->indexAt(QPoint(x, y));

	int row = index.row();
	int column = index.column();

	int sta = Solution::GetCurrentSolution()->get_station();
	if (row > sta)
	{
		QJsonObject json;
		//待处理给从机发送 两个参数1机位，2档位
		//如果是主机
		if (CiWaSocket::GetCurrMaster() == HostState::MASTE)
		{
			json.insert("station", QString::number(row - sta));
			json.insert(GRADE, QString::number(msg));
			CiWaSocket::NetWorkSendData(json, MSGID::SGRADE);
		}
		return;
	}
	Solution::GetCurrentSolution()->getcamera(row - 1).set_grade(msg);
	QString name = Solution::GetCurrentSolution()->get_solutionname();
	Solution::GetCurrentSolution()->WriteIniFile(name);
}

//初始化档位数据
void RunFrameTable::GradeInit()
{
	//检测档位
	QComboBox *combobox;
	int i = 1;
	m_comboxcount = Solution::GetCurrentSolution()->get_station();
	QStringList strlist;
	strlist << SGEAR0 << SGEAR1 << SGEAR2 << SGEAR3 << SGEAR4;
	for (auto it = Solution::GetCurrentSolution()->m_camera.begin(); it != Solution::GetCurrentSolution()->m_camera.end(); it++)
	{
		Solution_Camera &cam = sln::getcamera(it->first);
		combobox = (QComboBox*)ui.tableWidget->cellWidget(i, 5);
		combobox->addItems(strlist);
		combobox->setCurrentIndex(cam.get_grade());
		QObject::connect(combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnGradeChanged(int)));
		i++;
	}
}




//更新表格数据
bool RunFrameTable::UpTableData()
{
	//int m_count = 0;
	//int m_count_bad = 0;
	long ok = 0;
	long ng = 0;
	long count = 0;
	float n = 0.0;
	QString str;
	int row = ui.tableWidget->rowCount();
	int col = ui.tableWidget->columnCount();
	QLineEdit *lineEdit;

	int i = 1;
	QJsonObject json;
	QJsonObject rootjson;
	QJsonArray jarray;
	QComboBox *combobox;
	int comboxindex;
	int tmp_count = 0;
	int tmp_goodcount = 10000;
	int tmp_ngcount = 0;

	bool isblocking = false;
	std::chrono::steady_clock::time_point curtime = std::chrono::steady_clock::now();
	for (auto it = Solution::GetCurrentSolution()->m_camera.begin(); it != Solution::GetCurrentSolution()->m_camera.end(); it++)
	{
		clearJsonObject(json);
		Solution_Camera &cam = sln::getcamera(it->first);
		ok = cam.get_okcnt();
		ng = cam.get_ngcnt();
		count = ok + ng;
		if (count > tmp_count)
		{
			tmp_count = count;
		}
		//次品最大
		if (ng > tmp_ngcount)
		{
			tmp_ngcount = ng;
		}

		//if (ok < tmp_goodcount)
		//{
		//	tmp_goodcount = ok;
		//}

		if (baseparam::get_productblockedalarm()) {
			//int stationid = cam.get_station();
			int stationid = it->first;
			if (count != m_solution_count[stationid]) {
				m_solution_count[stationid] = count;
				m_solution_time[stationid] = curtime;
			}
			if ((m_solution_count[stationid] != 0)
				&& ((curtime - m_solution_time[stationid]).count()/1000000000 > baseparam::get_productblockedalarmtime())) {
				isblocking = true;
			}
		}

		//索引
		lineEdit = (QLineEdit*)ui.tableWidget->cellWidget(i, 0);
		lineEdit->setText(QString::number(cam.get_station()));
		

		//总数
		str = QString::number(count);
		lineEdit = (QLineEdit*)ui.tableWidget->cellWidget(i, 1);
		lineEdit->setText(str);
		
		//良品
		str = QString::number(ok);
		lineEdit = (QLineEdit*)ui.tableWidget->cellWidget(i, 2);
		lineEdit->setText(str);
		
		//劣品
		str = QString::number(ng);
		lineEdit = (QLineEdit*)ui.tableWidget->cellWidget(i, 3);
		lineEdit->setText(str);
	
		//良品率
		str = "";
		lineEdit = (QLineEdit*)ui.tableWidget->cellWidget(i, 4);
		if (count != 0)
		{
			n = (float)ok / count;
			str = QString::number(n, 'f', 2);
			lineEdit->setText(str);

		}
		else
		{
			lineEdit->setText("");
		}

		combobox = (QComboBox*)ui.tableWidget->cellWidget(i, 5);
	
		if (combobox == NULL)
		{
			comboxindex = 0;
		}
		else
		{
			comboxindex = combobox->currentIndex();
		}
		i++;
	
		//m_runframeinfo_map[it->first]->ui.algtime_avg->setText(QString::number(cam.get_algtime_avg()));
		//m_runframeinfo_map[it->first]->ui.algtime_max->setText(QString::number(cam.get_algtime_max()));
		//m_runframeinfo_map[it->first]->ui.algtime_min->setText(QString::number(cam.get_algtime_min()));
		//m_runframeinfo_map[it->first]->ui.lifetime_avg->setText(QString::number(cam.get_lifetime_avg()));
		//m_runframeinfo_map[it->first]->ui.lifetime_max->setText(QString::number(cam.get_lifetime_max()));
		//m_runframeinfo_map[it->first]->ui.lifetime_min->setText(QString::number(cam.get_lifetime_min()));

		//用于网络发送
		json.insert("station", cam.get_station());
		json.insert("cnt", count);
		json.insert("okcnt", ok);
		json.insert("ngcnt", ng);
		json.insert("cntl", str);
		json.insert(GRADE, comboxindex);
		jarray.append(json);

	}
	mutex.lock();

	m_count = tmp_count;
	m_badcount = tmp_ngcount;
	if (CiWaSocket::GetCurrMaster() == HostState::MASTE) {
		if (m_count < m_count_slave) {
			m_count = m_count_slave;
		}
		if (m_badcount < m_goodcount_slave) {
			m_badcount = m_goodcount_slave;
		}
	}
	m_goodcount = m_count - m_badcount;

	mutex.unlock();

	//如果是从机
	if (CiWaSocket::GetCurrMaster() == HostState::SLAVE)
	{
		if (jarray.count() < 1)
			return isblocking;

		//发送消息
		rootjson.insert("num", Solution::GetCurrentSolution()->get_station());
		rootjson.insert("sites", jarray);
		CiWaSocket::NetWorkSendData(rootjson, MSGID::COUNT);
	}


	return isblocking;
}

bool RunFrameTable::UpSqlTableData()
{
	//数据库
	QSqlQuery * ret;
	std::map<QString, QString> data;
	data[BATCHES] = m_batch;
	QLineEdit* item;
	QComboBox* combobox;
	for (int i = 1; i < SATATIONNUM; i++)
	{
		item = (QLineEdit*)ui.tableWidget->cellWidget(i, 0);
		if (item->text().isEmpty())
			continue;
		data[STATIONID] = item->text();

		item = (QLineEdit*)ui.tableWidget->cellWidget(i, 1);
		data[SQLCOUNT] = item->text();

		item = (QLineEdit*)ui.tableWidget->cellWidget(i, 2);
		data[SQLGOOD] = item->text();

		item = (QLineEdit*)ui.tableWidget->cellWidget(i, 3);
		data[SQLBAD] = item->text();

		combobox = (QComboBox*)ui.tableWidget->cellWidget(i, 5);
		data[STATIONGRADE] = QString::number(combobox->currentIndex());

		DatabaseSqlite::get_db()->Add(STATIONTABLE, data, &ret);
		//DatabaseSqlite::get_db()->Updata(STATIONTABLE, dwhere, data, &ret);
	}

	return true;
}

//接受从机的统计信息
void RunFrameTable::on_NewJsonData(const QJsonDocument &doc)
{
	long ok = 0;
	long ng = 0;
	long count = 0;
	int inttemp;
	int comboxindex;
	QString str;
	QLineEdit *lineEdit;
	QComboBox *combobox;
	int currstation = 1;
	if(Solution::GetCurrentSolution()!=NULL)
		currstation = Solution::GetCurrentSolution()->get_station() + 1;

	//解析
	QJsonObject rootObject = doc.object();
	//获得工位	
	QJsonValue jsonValue = rootObject.value("num");
	int stationcount = jsonValue.toInt() + currstation -1;
	//1清空表格
	for (int m = currstation; m < m_comboxcount+1; m++)
	{
		for (int j = 0; j < 5; j++)
		{
			lineEdit = (QLineEdit*)ui.tableWidget->cellWidget(m,j);
			if(lineEdit!=NULL)
				lineEdit->setText("");
		}
		combobox = (QComboBox*)ui.tableWidget->cellWidget(m, 5);
		if(combobox!=NULL)
		{ 
			QObject::disconnect(combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnGradeChanged(int)));
			combobox->clear();
		}
	}


	//动态创建挡位下拉
	if (stationcount > currstation)
	{
		QStringList strlist;
		strlist << SGEAR0 << SGEAR1 << SGEAR2 << SGEAR3 << SGEAR4;
		for (size_t n = 0; n < stationcount - currstation+1; n++)
		{
			combobox = (QComboBox*)ui.tableWidget->cellWidget(n+ currstation, 5);
			if (combobox == NULL)
				return;

			combobox->addItems(strlist);
			//combobox->setCurrentIndex(0);
			QObject::connect(combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnGradeChanged(int)));
		}

	}

	m_comboxcount = stationcount;
	jsonValue = rootObject.value("sites");
	if (!jsonValue.isArray())
	{
		qDebug() << "No target array";
	}
	QJsonArray jsonArray = jsonValue.toArray();
	int tmp_count = 0;
	int tmp_goodcount = 10000;
	int tmp_ngcount = 0;


	for (auto iter = jsonArray.constBegin(); iter != jsonArray.constEnd(); ++iter)
	{
		QJsonObject jsonObject = iter->toObject();
		inttemp = jsonObject.value("station").toInt();

		//索引
		lineEdit = (QLineEdit*)ui.tableWidget->cellWidget(inttemp + currstation, 0);
		lineEdit->setText(QString::number(inttemp + currstation-1));
		
		//总数
		count = jsonObject.value("cnt").toInt();
		str = QString::number(count);
		lineEdit = (QLineEdit*)ui.tableWidget->cellWidget(inttemp + currstation, 1);
		lineEdit->setText(str);

		//良品
		ok = jsonObject.value("okcnt").toInt();
		str = QString::number(ok);
		lineEdit = (QLineEdit*)ui.tableWidget->cellWidget(inttemp + currstation, 2);
		lineEdit->setText(str);

		//劣品
		ng = jsonObject.value("ngcnt").toInt();
		str = QString::number(jsonObject.value("ngcnt").toInt());
		lineEdit = (QLineEdit*)ui.tableWidget->cellWidget(inttemp + currstation, 3);
		lineEdit->setText(str);

		//良品率
		str = jsonObject.value("cntl").toString();
		lineEdit = (QLineEdit*)ui.tableWidget->cellWidget(inttemp + currstation, 4);
		lineEdit->setText(str);

		//挡位
		comboxindex = jsonObject.value(GRADE).toInt();
		combobox = (QComboBox*)ui.tableWidget->cellWidget(inttemp + currstation, 5);
		if (combobox != NULL)
		{
			if(comboxindex!=combobox->currentIndex())
				combobox->setCurrentIndex(comboxindex);
		}

		if (count > tmp_count)
		{
			tmp_count = count;
		}
		//次品最大
		if (ng > tmp_ngcount)
		{
			tmp_ngcount = ng;
		}
		//if (ok < tmp_goodcount)
		//{
		//	tmp_goodcount = ok;
		//}

	}

	mutex.lock();
	this->m_count_slave = tmp_count;
	this->m_goodcount_slave = tmp_count - tmp_ngcount;
	this->m_ngcount_slave = tmp_ngcount;
	mutex.unlock();


}





RunFrameTable::~RunFrameTable()
{
}
