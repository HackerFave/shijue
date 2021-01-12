#include "importandexport.h"
#include "databasesqlite.h"
#include "uicommon.h"
#include "excelexport.h"
#include "..\excel\qexcelengine.h"
#include "qprocess.h"
#include "qfileinfo.h"
#include "ciwamsgdialog.h"
#include "..\socket\ciwasocket.h"
ImportAndExport::ImportAndExport(QWidget *parent, QRect rect)
	: QWidget(parent)
{
	ui.setupUi(this);
	//���ر�����
	setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

	//���ô�С
	this->resize(rect.width(), rect.height());
	
	QCoreApplication::processEvents();//ǿ��ˢ��һ��,��Ȼ���н��治�����
	InitSqlTable();

	//�����Ի����ʼ��
	ExcelExport * mpoint = new ExcelExport();
	m_excelexport = (void*)mpoint;
	connect(mpoint, &ExcelExport::SendMsgBatch, this, &ImportAndExport::OnSendMsgBatch);
	connect(mpoint, &ExcelExport::SendMsgProduct, this, &ImportAndExport::OnSendMsgProduct);
	connect(mpoint, &ExcelExport::SendMsgStation, this, &ImportAndExport::OnSendMsgStation);
	//connect(this,&ImportAndExport::SendProductQueryRequest,c)
	m_currenrow = -1;
	m_currentable = -1;

	//ui.btnDatabaseBack->setDisabled(true);
	//ui.btnDatabaseRest->setDisabled(true);
	
}

ImportAndExport::~ImportAndExport()
{
	ExcelExport * mpoint = (ExcelExport*)m_excelexport;
	if (mpoint != NULL)
	{
		delete mpoint;
		mpoint = NULL;
		m_excelexport = NULL;
	}

}


void ImportAndExport::InitSqlTable()
{
	ui.tableBatches->setColumnCount(4); //��������
	ui.tableBatches->setHorizontalHeaderLabels(QStringList() << "����"
		<< "����"
		<< "��Ʒ"
		<< "��Ʒ");

	ui.tableStation->setColumnCount(6); //��������
	ui.tableStation->setHorizontalHeaderLabels(QStringList() << "��λ"
		<< "����"
		<< "����"
		<< "��Ʒ"
		<< "��Ʒ"
		<< "���ǿ��");

	ui.tableProducts->setColumnCount(6); //��������
	ui.tableProducts->setHorizontalHeaderLabels(QStringList() << "��Ʒ���"
		<< "����"
		<< "���ڹ�λ"
		<< "�Ƿ���Ʒ"
		<< "����ʱ��"
		<< "ͼ���ַ");

	update();
	connect(ui.tableBatches, SIGNAL(cellClicked(int, int)), this, SLOT(on_tableBatch_cellClicked(int, int)));
	//connect(ui.tableStation, SIGNAL(cellClicked(int, int)), this, SLOT(on_tableStation_cellClicked(int, int)));
	connect(ui.tableProducts, SIGNAL(cellClicked(int, int)), this, SLOT(on_tableProducts_cellClicked(int, int)));
	connect(CiWaSocket::GetCurrSocket(), &CiWaSocket::SendProduct, this, &ImportAndExport::OnSendProduct);
}

//��λ��񵥻��¼�
void ImportAndExport::on_tableStation_cellClicked(int row, int col)
{
	if (!ui.tableStation->item(row, 0))
		return;
	ProductQuery(ui.tableStation->item(row, 1)->text(),ui.tableStation->item(row, 0)->text(), 0);
	m_currentable = 2;
	m_currenrow = row;

	ui.btnIndex->setValue(0);
}

void ImportAndExport::on_tableProducts_cellClicked(int row, int col)
{
	m_currentable = 3;
	m_currenrow = row;
}

//���α�񵥻��¼�
void ImportAndExport::on_tableBatch_cellClicked(int row, int col)
{
	QString pici;
	pici = ui.tableBatches->item(row, 0)->text();
	//��ѯ��λ��
	//���
	ui.tableStation->clearContents();
	ui.tableProducts->clearContents();

	QTableWidgetItem *newItem;
	QSqlQuery *ret;
	std::list<QString> key;
	std::list<std::list<QString>> drow;
	std::map<QString, QString> dwhere;
	dwhere[BATCHES] = pici;
	key.push_back(STATIONID);
	key.push_back(BATCHES);
	key.push_back(SQLCOUNT);
	key.push_back(SQLGOOD);
	key.push_back(SQLBAD);
	key.push_back(STATIONGRADE);

	DatabaseSqlite::get_db()->Find(STATIONTABLE, key, dwhere, &drow, &ret);

	ui.tableStation->setRowCount(drow.size());
	int i = 0;
	while (drow.size() > 0)
	{
		int rowlen = drow.back().size();
		for (int j = 0; j < rowlen; j++)
		{
			QString src = drow.front().front();
			newItem = new QTableWidgetItem(src);
			ui.tableStation->setItem(i, j, newItem);
			drow.front().pop_front();
		}
		i++;
		drow.pop_front();
	}
	m_currentable = 1;
	m_currenrow = row;

}

//���β�ѯtableBatches
void ImportAndExport::on_btnSelectSQL_clicked()
{
	//���
	ui.tableBatches->clearContents();
	ui.tableProducts->clearContents();
	ui.tableStation->clearContents();
	
	ui.tableProducts->setRowCount(0);
	ui.tableStation->setRowCount(0);
	QTableWidgetItem *newItem;
	QSqlQuery *ret;
	std::list<QString> key;
	std::list<std::list<QString>> row;
	
	key.push_back(BATCHES);
	key.push_back(SQLCOUNT);
	key.push_back(SQLGOOD);
	key.push_back(SQLBAD);
	DatabaseSqlite::get_db()->Find(BATCHTABLE, key, &row, &ret);
	ui.tableBatches->setRowCount(row.size());

	int i = 0;
	while (row.size() > 0)
	{
		int rowlen = row.back().size();
		for (int j = 0; j < rowlen; j++)
		{
			QString src = row.back().front();
			newItem = new QTableWidgetItem(src);
			ui.tableBatches->setItem(i, j, newItem);
			row.back().pop_front();
		}
		i++;
		row.pop_back();
	}
}
//����
void ImportAndExport::on_btnDatabaseBack_clicked()
{
	ExcelExport * mpoint = (ExcelExport*)m_excelexport;
	QString path = mpoint->OpenSaveExcelFile("backup.sql", "����","*.sql");
	if (path.isEmpty())
		return;

	QString cmd = QString("mysqldump.exe --add-drop-table -u%1 -p%2 cicai").arg("root", "123456");
	QProcess poc;
	poc.setStandardOutputFile(path);
	poc.start(cmd);

	QFileInfo fileInfo(path);
	if (fileInfo.isFile())
		CiWaMsgDialog::MessageT("���ݳɹ�");
	else
		CiWaMsgDialog::MessageT("ʧ��", "���ݿⱸ��ʧ��");

	return;
}
//�ָ�
void ImportAndExport::on_btnDatabaseRest_clicked()
{
	ExcelExport * mpoint = (ExcelExport*)m_excelexport;
	QString path = mpoint->OpenOpenExcelFile("backup.sql", "�ָ�", "*.sql");
	if (path.isEmpty())
		return;
	QString cmd = QString("mysql.exe -u%1 -p%2 cicai").arg("root", "123456");
	QProcess poc;
	poc.setStandardOutputFile(path);
	poc.start(cmd);

	QFileInfo fileInfo(path);
	if (fileInfo.isFile())
		CiWaMsgDialog::MessageT("�ָ��ɹ�");
	else
		CiWaMsgDialog::MessageT("ʧ��", "���ݿ�ָ�ʧ��");

	return;
}

void ImportAndExport::on_btnNext_clicked()
{
	if (ui.tableProducts->item(0, 1)&& ui.tableProducts->item(0, 2))
	{ 
		int index = ui.btnIndex->value();
		ui.btnIndex->setValue(++index);
		ProductQuery(ui.tableProducts->item(0, 1)->text(), ui.tableProducts->item(0, 2)->text(), ui.btnIndex->value());
	}
}

void ImportAndExport::on_btnPrevious_clicked()
{
	if (ui.tableProducts->item(0, 1) && ui.tableProducts->item(0, 2))
	{
		int index = ui.btnIndex->value();
		ui.btnIndex->setValue(--index);
		ProductQuery(ui.tableProducts->item(0, 1)->text(), ui.tableProducts->item(0, 2)->text(), ui.btnIndex->value());
	}
}

void ImportAndExport::on_btnIndexOK_clicked()
{

	if (ui.tableProducts->item(0, 1) && ui.tableProducts->item(0, 2))
	{
		int index = ui.btnIndex->value();
		ProductQuery(ui.tableProducts->item(0, 1)->text(), ui.tableProducts->item(0, 2)->text(), index);
	}

}

void ImportAndExport::on_btnEnd_clicked()
{
    if (ui.tableProducts->item(0, 1) && ui.tableProducts->item(0, 2)) {



        //���õȴ����
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        QString sql = QString("select count(`ProductID`) from `cicai`.`products` where `StationID`=%1 and `Batch`=%2;")
            .arg(ui.tableProducts->item(0, 2)->text())
            .arg(ui.tableProducts->item(0, 1)->text());
        QSqlQuery *ret;
        std::list<std::list<QString>> drow;
        DatabaseSqlite::get_db()->Find(sql, &drow, &ret, 1);
        QApplication::restoreOverrideCursor();

        if (drow.size() > 0)
        {

            QString src = drow.front().front();
            int count = ceil(src.toInt() / 50) -1;
            ui.btnIndex->setMaximum(count);
            ui.btnIndex->setValue(count);

            if (ui.tableProducts->item(0, 1) && ui.tableProducts->item(0, 2))
                ProductQuery(ui.tableProducts->item(0, 1)->text(), ui.tableProducts->item(0, 2)->text(), count);
        }
    }
}

//����excel
void ImportAndExport::on_btnExport_clicked()
{
	ExcelExport * mpoint =(ExcelExport*)m_excelexport;
	mpoint->exec();
}


//ɾ��
void ImportAndExport::on_btnDeleteSQL_clicked()
{
	if ((m_currenrow < 0) || (m_currentable < 0))
		return;


	QStringList qlist = QStringList() << BATCHTABLE << STATIONTABLE << PRODUCTTABLE;
	QSqlQuery *ret;
	std::map<QString, QString> dwhere;

	switch (m_currentable)
	{
	case 1:	//����
		if (m_currenrow >= ui.tableBatches->rowCount())
			return;
		dwhere[BATCHES] = ui.tableBatches->item(m_currenrow, 0)->text();
		ui.tableBatches->removeRow(m_currenrow);
		break;
	case 2://��λ
		if (m_currenrow >= ui.tableStation->rowCount())
			return;
		dwhere[STATIONID] = ui.tableStation->item(m_currenrow, 0)->text();
		dwhere[BATCHES] = ui.tableStation->item(m_currenrow, 1)->text();
		ui.tableStation->removeRow(m_currenrow);
		break;
	case 3:
		if (m_currenrow >= ui.tableProducts->rowCount())
			return;
		dwhere[PRODUCTID] = ui.tableProducts->item(m_currenrow, 0)->text();
		dwhere[BATCHES] = ui.tableProducts->item(m_currenrow, 1)->text();
		ui.tableProducts->removeRow(m_currenrow);
		break;
	default:
		return;
	}
	//ɾ��
	DatabaseSqlite::get_db()->Del(qlist.at(m_currentable - 1), dwhere,&ret );
	
}


//���յ�����Ʒ���ź�
void ImportAndExport::OnSendMsgProduct(QString path)
{
	if (ui.tableProducts->rowCount() < 1)
		return;
	IntoExcel(path, ui.tableProducts);
}

//���յ�����λ���ź�
void ImportAndExport::OnSendMsgStation(QString path)
{
	if (ui.tableStation->rowCount() < 1)
		return;
	IntoExcel(path, ui.tableStation);
}
//���յ������α��ź�
void ImportAndExport::OnSendMsgBatch(QString path)
{
	if (ui.tableBatches->rowCount() < 1)
		return;

	IntoExcel(path, ui.tableBatches);
}

//������excel
bool ImportAndExport::IntoExcel(QString filename, QTableWidget* tableinto)
{
	QApplication::setOverrideCursor(Qt::WaitCursor);
	QCoreApplication::processEvents();

	if (filename.isEmpty())
		return false;


	QExcelEngine excelEngine ;
	bool b = excelEngine.Open(filename, 1, false);
	if (!b)
	{
		return b;
	}
	excelEngine.ClearAllData("");
	excelEngine.SetIsSaveAlready(false);
	excelEngine.SaveDataFrTable(tableinto);
	excelEngine.Close();
	QApplication::restoreOverrideCursor();
	CiWaMsgDialog::MessageT("�����ɹ�");
	return true;
}

void ImportAndExport::SendProductQueryRequest(QString str)
{
	QJsonObject jsonObject;
	jsonObject["sql"] = str;
	CiWaSocket::NetWorkSendData(jsonObject, MSGID::SLAVESTART);
}

void ImportAndExport::OnSendProduct(const QJsonDocument & doc)
{
	/*
	//����
	QJsonObject rootObject = doc.object();
	//��ù�λ	
	QJsonValue jsonValue;
	jsonValue = rootObject.value("sql");

	//��ѯ���ݿ�
	QSqlQuery *ret;
	std::list<std::list<QString>> drow;
	DatabaseSqlite::get_db()->Find(jsonValue.toString(), &drow, &ret, 6);

	int i = 0;
	while (drow.size() > 0)
	{
		int rowlen = drow.back().size();
		for (int j = 0; j < rowlen; j++)
		{
			QString src = drow.front().front();
			newItem = new QTableWidgetItem(src);
			ui.tableStation->setItem(i, j, newItem);
			drow.front().pop_front();
		}
		i++;
		drow.pop_front();
	}
	*/
}

void ImportAndExport::ProductQuery(QString batch, QString station, int page)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	//��ѯ��λ��
	//���
	ui.tableProducts->clearContents();
	QTableWidgetItem *newItem;
	QSqlQuery *ret;
	std::list<std::list<QString>> drow;

	QString sql = QString("SELECT `ProductID`,`Batch`,`StationID`,`IsGood`,`CreateTime`,`Addr` "
		"FROM `cicai`.`products` "
		"where `StationID`=%1 and `Batch`=%2 and `ProductID`>="
		"(SELECT `ProductID` FROM `cicai`.`products` LIMIT %3, 1)"
		"limit 0, 50 ").arg(station).arg(batch).arg(page*50);

	DatabaseSqlite::get_db()->Find(sql, &drow, &ret, 6);

	if (drow.size() > 0)
	{
		ui.tableProducts->setRowCount(drow.size());
		int i = 0;
		while (drow.size() > 0)
		{
			int rowlen = drow.back().size();
			for (int j = 0; j < rowlen; j++)
			{
				QString src = drow.front().front();
				newItem = new QTableWidgetItem(src);
				ui.tableProducts->setItem(i, j, newItem);
				drow.front().pop_front();
			}
			i++;
			drow.pop_front();
		}

	}
	else
	{
		QApplication::restoreOverrideCursor();
		if (CiWaSocket::GetCurrMaster() != HostState::MASTE)
			return;
		//���������ѯ��Ϣ
		SendProductQueryRequest(DatabaseSqlite::get_db()->get_lastsql());
	}

	QApplication::restoreOverrideCursor();
}
